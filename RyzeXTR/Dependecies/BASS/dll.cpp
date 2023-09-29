//#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <winnt.h>
#include <stddef.h>
#include <stdint.h>
#include <tchar.h>
#include <iostream>
#include <fstream>

#ifdef DEBUG_OUTPUT
#include <stdio.h>
#endif

#if _MSC_VER
// Disable warning about data -> function pointer conversion
#pragma warning(disable:4055)
#endif

#define IMAGE_SIZEOF_BASE_RELOCATION (sizeof(IMAGE_BASE_RELOCATION))

#include "dll.h"

#define GET_HEADER_DICTIONARY(module, idx)  &(module)->headers->OptionalHeader.DataDirectory[idx]
#define ALIGN_DOWN(address, alignment)      (LPVOID)((uintptr_t)(address) & ~((alignment) - 1))
#define ALIGN_VALUE_UP(value, alignment)    (((value) + (alignment) - 1) & ~((alignment) - 1))

BOOL
CWin32PE::CheckSize(size_t size, size_t expected) {
	if (size < expected) {
		SetLastError(ERROR_INVALID_DATA);
		return FALSE;
	}

	return TRUE;
}

BOOL CWin32PE::CopySections(const unsigned char *data, size_t size, PIMAGE_NT_HEADERS old_headers, PMEMORYMODULE module)
{
	int i, section_size;
	unsigned char *codeBase = module->codeBase;
	unsigned char *dest;
	PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(module->headers);
	for (i = 0; i < module->headers->FileHeader.NumberOfSections; i++, section++) {
		if (section->SizeOfRawData == 0) {
			// section doesn't contain data in the dll itself, but may define
			// uninitialized data
			section_size = old_headers->OptionalHeader.SectionAlignment;
			if (section_size > 0) {
				dest = (unsigned char *) VirtualAlloc(codeBase + section->VirtualAddress,
													  section_size,
													  MEM_COMMIT,
													  PAGE_READWRITE);
				if (dest == NULL) {
					return FALSE;
				}

				// Always use position from file to support alignments smaller
				// than page size.
				dest = codeBase + section->VirtualAddress;
				section->Misc.PhysicalAddress = (DWORD) (uintptr_t) dest;
				memset(dest, 0, section_size);
			}

			// section is empty
			continue;
		}

		if (!CheckSize(size, section->PointerToRawData + section->SizeOfRawData)) {
			return FALSE;
		}

		// commit memory block and copy data from dll
		dest = (unsigned char *) VirtualAlloc(codeBase + section->VirtualAddress,
											  section->SizeOfRawData,
											  MEM_COMMIT,
											  PAGE_READWRITE);
		if (dest == NULL) {
			return FALSE;
		}

		// Always use position from file to support alignments smaller
		// than page size.
		dest = codeBase + section->VirtualAddress;
		memcpy(dest, data + section->PointerToRawData, section->SizeOfRawData);
		section->Misc.PhysicalAddress = (DWORD) (uintptr_t) dest;
	}

	return TRUE;
}

// Protection flags for memory pages (Executable, Readable, Writeable)
static int ProtectionFlags[2][2][2] = {
	{
		// not executable
		{ PAGE_NOACCESS, PAGE_WRITECOPY },
{ PAGE_READONLY, PAGE_READWRITE },
	},{
		// executable
		{ PAGE_EXECUTE, PAGE_EXECUTE_WRITECOPY },
{ PAGE_EXECUTE_READ, PAGE_EXECUTE_READWRITE },
	},
};

DWORD
CWin32PE::GetRealSectionSize(PMEMORYMODULE module, PIMAGE_SECTION_HEADER section) {
	DWORD size = section->SizeOfRawData;
	if (size == 0) {
		if (section->Characteristics & IMAGE_SCN_CNT_INITIALIZED_DATA) {
			size = module->headers->OptionalHeader.SizeOfInitializedData;
		}
		else if (section->Characteristics & IMAGE_SCN_CNT_UNINITIALIZED_DATA) {
			size = module->headers->OptionalHeader.SizeOfUninitializedData;
		}
	}
	return size;
}

BOOL
CWin32PE::FinalizeSection(PMEMORYMODULE module, PSECTIONFINALIZEDATA sectionData) {
	DWORD protect, oldProtect;
	BOOL executable;
	BOOL readable;
	BOOL writeable;

	if (sectionData->size == 0) {
		return TRUE;
	}

	if (sectionData->characteristics & IMAGE_SCN_MEM_DISCARDABLE) {
		// section is not needed any more and can safely be freed
		if (sectionData->address == sectionData->alignedAddress &&
			(sectionData->last ||
			 module->headers->OptionalHeader.SectionAlignment == module->pageSize ||
			 (sectionData->size % module->pageSize) == 0)
			) {
			// Only allowed to decommit whole pages
			VirtualFree(sectionData->address, sectionData->size, MEM_DECOMMIT);
		}
		return TRUE;
	}

	// determine protection flags based on characteristics
	executable = (sectionData->characteristics & IMAGE_SCN_MEM_EXECUTE) != 0;
	readable = (sectionData->characteristics & IMAGE_SCN_MEM_READ) != 0;
	writeable = (sectionData->characteristics & IMAGE_SCN_MEM_WRITE) != 0;
	protect = ProtectionFlags[executable][readable][writeable];
	if (sectionData->characteristics & IMAGE_SCN_MEM_NOT_CACHED) {
		protect |= PAGE_NOCACHE;
	}

	// change memory access flags
	if (VirtualProtect(sectionData->address, sectionData->size, protect, &oldProtect) == 0) {
	#ifdef DEBUG_OUTPUT
		OutputLastError("Error protecting memory page")
	#endif
			return FALSE;
	}

	return TRUE;
}

BOOL
CWin32PE::FinalizeSections(PMEMORYMODULE module)
{
	int i;
	PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(module->headers);
#ifdef _WIN64
	uintptr_t imageOffset = (module->headers->OptionalHeader.ImageBase & 0xffffffff00000000);
#else
#define imageOffset 0
#endif
	SECTIONFINALIZEDATA sectionData;
	sectionData.address = (LPVOID) ((uintptr_t) section->Misc.PhysicalAddress | imageOffset);
	sectionData.alignedAddress = ALIGN_DOWN(sectionData.address, module->pageSize);
	sectionData.size = GetRealSectionSize(module, section);
	sectionData.characteristics = section->Characteristics;
	sectionData.last = FALSE;
	section++;

	// loop through all sections and change access flags
	for (i = 1; i < module->headers->FileHeader.NumberOfSections; i++, section++) {
		LPVOID sectionAddress = (LPVOID) ((uintptr_t) section->Misc.PhysicalAddress | imageOffset);
		LPVOID alignedAddress = ALIGN_DOWN(sectionAddress, module->pageSize);
		DWORD sectionSize = GetRealSectionSize(module, section);
		// Combine access flags of all sections that share a page
		// TODO(fancycode): We currently share flags of a trailing large section
		//   with the page of a first small section. This should be optimized.
		if (sectionData.alignedAddress == alignedAddress || (uintptr_t) sectionData.address + sectionData.size > (uintptr_t) alignedAddress) {
			// Section shares page with previous
			if ((section->Characteristics & IMAGE_SCN_MEM_DISCARDABLE) == 0 || (sectionData.characteristics & IMAGE_SCN_MEM_DISCARDABLE) == 0) {
				sectionData.characteristics = (sectionData.characteristics | section->Characteristics) & ~IMAGE_SCN_MEM_DISCARDABLE;
			}
			else {
				sectionData.characteristics |= section->Characteristics;
			}
			sectionData.size = (((uintptr_t) sectionAddress) + sectionSize) - (uintptr_t) sectionData.address;
			continue;
		}

		if (!FinalizeSection(module, &sectionData)) {
			return FALSE;
		}
		sectionData.address = sectionAddress;
		sectionData.alignedAddress = alignedAddress;
		sectionData.size = sectionSize;
		sectionData.characteristics = section->Characteristics;
	}
	sectionData.last = TRUE;
	if (!FinalizeSection(module, &sectionData)) {
		return FALSE;
	}
#ifndef _WIN64
#undef imageOffset
#endif
	return TRUE;
}

BOOL
CWin32PE::ExecuteTLS(PMEMORYMODULE module)
{
	unsigned char *codeBase = module->codeBase;
	PIMAGE_TLS_DIRECTORY tls;
	PIMAGE_TLS_CALLBACK* callback;

	PIMAGE_DATA_DIRECTORY directory = GET_HEADER_DICTIONARY(module, IMAGE_DIRECTORY_ENTRY_TLS);
	if (directory->VirtualAddress == 0) {
		return TRUE;
	}

	tls = (PIMAGE_TLS_DIRECTORY) (codeBase + directory->VirtualAddress);
	callback = (PIMAGE_TLS_CALLBACK *) tls->AddressOfCallBacks;
	if (callback) {
		while (*callback) {
			(*callback)((LPVOID) codeBase, DLL_PROCESS_ATTACH, NULL);
			callback++;
		}
	}
	return TRUE;
}

BOOL
CWin32PE::PerformBaseRelocation(PMEMORYMODULE module, ptrdiff_t delta)
{
	unsigned char *codeBase = module->codeBase;
	PIMAGE_BASE_RELOCATION relocation;

	PIMAGE_DATA_DIRECTORY directory = GET_HEADER_DICTIONARY(module, IMAGE_DIRECTORY_ENTRY_BASERELOC);
	if (directory->Size == 0) {
		return (delta == 0);
	}

	relocation = (PIMAGE_BASE_RELOCATION) (codeBase + directory->VirtualAddress);
	for (; relocation->VirtualAddress > 0;) {
		DWORD i;
		unsigned char *dest = codeBase + relocation->VirtualAddress;
		unsigned short *relInfo = (unsigned short *) ((unsigned char *) relocation + IMAGE_SIZEOF_BASE_RELOCATION);
		for (i = 0; i < ((relocation->SizeOfBlock - IMAGE_SIZEOF_BASE_RELOCATION) / 2); i++, relInfo++) {
			DWORD *patchAddrHL;
#ifdef _WIN64
			ULONGLONG *patchAddr64;
#endif
			int type, offset;

			// the upper 4 bits define the type of relocation
			type = *relInfo >> 12;
			// the lower 12 bits define the offset
			offset = *relInfo & 0xfff;

			switch (type)
			{
			case IMAGE_REL_BASED_ABSOLUTE:
				// skip relocation
				break;

			case IMAGE_REL_BASED_HIGHLOW:
				// change complete 32 bit address
				patchAddrHL = (DWORD *) (dest + offset);
				*patchAddrHL += (DWORD) delta;
				break;

#ifdef _WIN64
			case IMAGE_REL_BASED_DIR64:
				patchAddr64 = (ULONGLONG *) (dest + offset);
				*patchAddr64 += (ULONGLONG) delta;
				break;
#endif

			default:
				//printf("Unknown relocation: %d\n", type);
				break;
			}
		}

		// advance to next relocation block
		relocation = (PIMAGE_BASE_RELOCATION) (((char *) relocation) + relocation->SizeOfBlock);
	}
	return TRUE;
}

BOOL
CWin32PE::BuildImportTable(PMEMORYMODULE module)
{
	unsigned char *codeBase = module->codeBase;
	PIMAGE_IMPORT_DESCRIPTOR importDesc;
	BOOL result = TRUE;

	PIMAGE_DATA_DIRECTORY directory = GET_HEADER_DICTIONARY(module, IMAGE_DIRECTORY_ENTRY_IMPORT);
	if (directory->Size == 0) {
		return TRUE;
	}

	importDesc = (PIMAGE_IMPORT_DESCRIPTOR) (codeBase + directory->VirtualAddress);
	for (; !IsBadReadPtr(importDesc, sizeof(IMAGE_IMPORT_DESCRIPTOR)) && importDesc->Name; importDesc++) {
		uintptr_t *thunkRef;
		FARPROC *funcRef;
		HCUSTOMMODULE *tmp;
		HCUSTOMMODULE handle = module->loadLibrary((LPCSTR) (codeBase + importDesc->Name), module->userdata);
		if (handle == NULL) {
			SetLastError(ERROR_MOD_NOT_FOUND);
			result = FALSE;
			break;
		}

		tmp = (HCUSTOMMODULE *) realloc(module->modules, (module->numModules + 1)*(sizeof(HCUSTOMMODULE)));
		if (tmp == NULL) {
			module->freeLibrary(handle, module->userdata);
			SetLastError(ERROR_OUTOFMEMORY);
			result = FALSE;
			break;
		}
		module->modules = tmp;

		module->modules[module->numModules++] = handle;
		if (importDesc->OriginalFirstThunk) {
			thunkRef = (uintptr_t *) (codeBase + importDesc->OriginalFirstThunk);
			funcRef = (FARPROC *) (codeBase + importDesc->FirstThunk);
		}
		else {
			// no hint table
			thunkRef = (uintptr_t *) (codeBase + importDesc->FirstThunk);
			funcRef = (FARPROC *) (codeBase + importDesc->FirstThunk);
		}
		for (; *thunkRef; thunkRef++, funcRef++) {
			if (IMAGE_SNAP_BY_ORDINAL(*thunkRef)) {
				*funcRef = module->getProcAddress(handle, (LPCSTR) IMAGE_ORDINAL(*thunkRef), module->userdata);
			}
			else {
				PIMAGE_IMPORT_BY_NAME thunkData = (PIMAGE_IMPORT_BY_NAME) (codeBase + (*thunkRef));
				*funcRef = module->getProcAddress(handle, (LPCSTR) &thunkData->Name, module->userdata);
			}
			if (*funcRef == 0) {
				result = FALSE;
				break;
			}
		}

		if (!result) {
			module->freeLibrary(handle, module->userdata);
			SetLastError(ERROR_PROC_NOT_FOUND);
			break;
		}
	}

	return result;
}


HCUSTOMMODULE MemoryDefaultLoadLibrary(LPCSTR filename, void *userdata)
{
	HMODULE result;
	UNREFERENCED_PARAMETER(userdata);
	result = LoadLibraryA(filename);
	if (result == NULL) {
		return NULL;
	}

	return (HCUSTOMMODULE) result;
}

FARPROC MemoryDefaultGetProcAddress(HCUSTOMMODULE module, LPCSTR name, void *userdata)
{
	UNREFERENCED_PARAMETER(userdata);
	return (FARPROC) GetProcAddress((HMODULE) module, name);
}

void MemoryDefaultFreeLibrary(HCUSTOMMODULE module, void *userdata)
{
	UNREFERENCED_PARAMETER(userdata);
	FreeLibrary((HMODULE) module);
}


HANDLE CLoad::MemLoadLibraryEx(const void *data, size_t size,
							   MemLoadLibraryFn loadLibrary,
							   MemGetProcAddressFn getProcAddress,
							   MemFreeLibraryFn freeLibrary,
							   void *userdata)
{
	PMEMORYMODULE result = NULL;
	PIMAGE_DOS_HEADER dos_header;
	PIMAGE_NT_HEADERS old_header;
	unsigned char *code, *headers;
	ptrdiff_t locationDelta;
	SYSTEM_INFO sysInfo;
	PIMAGE_SECTION_HEADER section;
	DWORD i;
	size_t optionalSectionSize;
	size_t lastSectionEnd = 0;
	size_t alignedImageSize;

	if (!CheckSize(size, sizeof(IMAGE_DOS_HEADER))) {
		return NULL;
	}
	dos_header = (PIMAGE_DOS_HEADER) data;
	if (dos_header->e_magic != IMAGE_DOS_SIGNATURE) {
		SetLastError(ERROR_BAD_EXE_FORMAT);
		return NULL;
	}

	if (!CheckSize(size, dos_header->e_lfanew + sizeof(IMAGE_NT_HEADERS))) {
		return NULL;
	}
	old_header = (PIMAGE_NT_HEADERS)&((const unsigned char *) (data))[dos_header->e_lfanew];
	if (old_header->Signature != IMAGE_NT_SIGNATURE) {
		SetLastError(ERROR_BAD_EXE_FORMAT);
		return NULL;
	}

#ifdef _WIN64
	if (old_header->FileHeader.Machine != IMAGE_FILE_MACHINE_AMD64) {
#else
	if (old_header->FileHeader.Machine != IMAGE_FILE_MACHINE_I386) {
#endif
		SetLastError(ERROR_BAD_EXE_FORMAT);
		return NULL;
	}

	if (old_header->OptionalHeader.SectionAlignment & 1) {
		// Only support section alignments that are a multiple of 2
		SetLastError(ERROR_BAD_EXE_FORMAT);
		return NULL;
	}

	section = IMAGE_FIRST_SECTION(old_header);
	optionalSectionSize = old_header->OptionalHeader.SectionAlignment;
	for (i = 0; i < old_header->FileHeader.NumberOfSections; i++, section++) {
		size_t endOfSection;
		if (section->SizeOfRawData == 0) {
			// Section without data in the DLL
			endOfSection = section->VirtualAddress + optionalSectionSize;
		}
		else {
			endOfSection = section->VirtualAddress + section->SizeOfRawData;
		}

		if (endOfSection > lastSectionEnd) {
			lastSectionEnd = endOfSection;
		}
	}

	GetNativeSystemInfo(&sysInfo);
	alignedImageSize = ALIGN_VALUE_UP(old_header->OptionalHeader.SizeOfImage, sysInfo.dwPageSize);
	if (alignedImageSize != ALIGN_VALUE_UP(lastSectionEnd, sysInfo.dwPageSize)) {
		SetLastError(ERROR_BAD_EXE_FORMAT);
		return NULL;
	}

	// reserve memory for image of library
	// XXX: is it correct to commit the complete memory region at once?
	//      calling DllEntry raises an exception if we don't...
	code = (unsigned char *) VirtualAlloc((LPVOID) (old_header->OptionalHeader.ImageBase),
										  alignedImageSize,
										  MEM_RESERVE | MEM_COMMIT,
										  PAGE_READWRITE);

	if (code == NULL) {
		// try to allocate memory at arbitrary position
		code = (unsigned char *) VirtualAlloc(NULL,
											  alignedImageSize,
											  MEM_RESERVE | MEM_COMMIT,
											  PAGE_READWRITE);
		if (code == NULL) {
			SetLastError(ERROR_OUTOFMEMORY);
			return NULL;
		}
	}

	result = (PMEMORYMODULE) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(MEMORYMODULE));
	if (result == NULL) {
		VirtualFree(code, 0, MEM_RELEASE);
		SetLastError(ERROR_OUTOFMEMORY);
		return NULL;
	}

	result->codeBase = code;
	result->isDLL = (old_header->FileHeader.Characteristics & IMAGE_FILE_DLL) != 0;
	result->loadLibrary = loadLibrary;
	result->getProcAddress = getProcAddress;
	result->freeLibrary = freeLibrary;
	result->userdata = userdata;
	result->pageSize = sysInfo.dwPageSize;

	if (!CheckSize(size, old_header->OptionalHeader.SizeOfHeaders)) {
		goto error;
	}

	// commit memory for headers
	headers = (unsigned char *) VirtualAlloc(code,
											 old_header->OptionalHeader.SizeOfHeaders,
											 MEM_COMMIT,
											 PAGE_READWRITE);

	// copy PE header to code
	memcpy(headers, dos_header, old_header->OptionalHeader.SizeOfHeaders);
	result->headers = (PIMAGE_NT_HEADERS)&((const unsigned char *) (headers))[dos_header->e_lfanew];

	// update position
	result->headers->OptionalHeader.ImageBase = (uintptr_t) code;

	// copy sections from DLL file block to new memory location
	if (!CopySections((const unsigned char *) data, size, old_header, result)) {
		goto error;
	}

	// adjust base address of imported data
	locationDelta = (ptrdiff_t) (result->headers->OptionalHeader.ImageBase - old_header->OptionalHeader.ImageBase);
	if (locationDelta != 0) {
		result->isRelocated = PerformBaseRelocation(result, locationDelta);
	}
	else {
		result->isRelocated = TRUE;
	}

	// load required dlls and adjust function table of imports
	if (!BuildImportTable(result)) {
		goto error;
	}

	// mark memory pages depending on section headers and release
	// sections that are marked as "discardable"
	if (!FinalizeSections(result)) {
		goto error;
	}

	// TLS callbacks are executed BEFORE the main loading
	if (!ExecuteTLS(result)) {
		goto error;
	}

	// get entry point of loaded library
	if (result->headers->OptionalHeader.AddressOfEntryPoint != 0) {
		if (result->isDLL) {
			DllEntryProc DllEntry = (DllEntryProc) (LPVOID) (code + result->headers->OptionalHeader.AddressOfEntryPoint);
			// notify library about attaching to process
			BOOL successfull = (*DllEntry)((HINSTANCE) code, DLL_PROCESS_ATTACH, 0);
			if (!successfull) {
				SetLastError(ERROR_DLL_INIT_FAILED);
				goto error;
			}
			result->initialized = TRUE;
		}
		else {
			result->exeEntry = (ExeEntryProc) (LPVOID) (code + result->headers->OptionalHeader.AddressOfEntryPoint);
		}
	}
	else {
		result->exeEntry = NULL;
	}

	return (HANDLE) result;

error:
	// cleanup
	FreeLibraryFromMemory(result);
	return NULL;
}

HANDLE CLoad::LoadFromMemory(const void *data, size_t size)
{
	return MemLoadLibraryEx(data, size, MemoryDefaultLoadLibrary, MemoryDefaultGetProcAddress, MemoryDefaultFreeLibrary, NULL);
}

FARPROC CLoad::GetProcAddressFromMemory(HANDLE module, LPCSTR name)
{
	if (!module || !name)
		return 0x0;

	unsigned char *codeBase = ((PMEMORYMODULE) module)->codeBase;
	DWORD idx = 0;
	PIMAGE_EXPORT_DIRECTORY exports;
	PIMAGE_DATA_DIRECTORY directory = GET_HEADER_DICTIONARY((PMEMORYMODULE) module, IMAGE_DIRECTORY_ENTRY_EXPORT);
	if (directory->Size == 0) {
		// no export table found
		SetLastError(ERROR_PROC_NOT_FOUND);
		return NULL;
	}

	exports = (PIMAGE_EXPORT_DIRECTORY) (codeBase + directory->VirtualAddress);
	if (exports->NumberOfNames == 0 || exports->NumberOfFunctions == 0) {
		// DLL doesn't export anything
		SetLastError(ERROR_PROC_NOT_FOUND);
		return NULL;
	}

	if (HIWORD(name) == 0) {
		// load function by ordinal value
		if (LOWORD(name) < exports->Base) {
			SetLastError(ERROR_PROC_NOT_FOUND);
			return NULL;
		}

		idx = LOWORD(name) - exports->Base;
	}
	else {
		// search function name in list of exported names
		DWORD i;
		DWORD *nameRef = (DWORD *) (codeBase + exports->AddressOfNames);
		WORD *ordinal = (WORD *) (codeBase + exports->AddressOfNameOrdinals);
		BOOL found = FALSE;
		for (i = 0; i < exports->NumberOfNames; i++, nameRef++, ordinal++) {
			if (_stricmp(name, (const char *) (codeBase + (*nameRef))) == 0) {
				idx = *ordinal;
				found = TRUE;
				break;
			}
		}

		if (!found) {
			// exported symbol not found
			SetLastError(ERROR_PROC_NOT_FOUND);
			return NULL;
		}
	}

	if (idx > exports->NumberOfFunctions) {
		// name <-> ordinal number don't match
		SetLastError(ERROR_PROC_NOT_FOUND);
		return NULL;
	}

	// AddressOfFunctions contains the RVAs to the "real" functions
	return (FARPROC) (LPVOID) (codeBase + (*(DWORD *) (codeBase + exports->AddressOfFunctions + (idx * 4))));
}

void CLoad::FreeLibraryFromMemory(HANDLE mod)
{
	PMEMORYMODULE module = (PMEMORYMODULE) mod;

	if (module == NULL) {
		return;
	}
	if (module->initialized) {
		// notify library about detaching from process
		DllEntryProc DllEntry = (DllEntryProc) (LPVOID) (module->codeBase + module->headers->OptionalHeader.AddressOfEntryPoint);
		(*DllEntry)((HINSTANCE) module->codeBase, DLL_PROCESS_DETACH, 0);
	}

	if (module->modules != NULL) {
		// free previously opened libraries
		int i;
		for (i = 0; i < module->numModules; i++) {
			if (module->modules[i] != NULL) {
				module->freeLibrary(module->modules[i], module->userdata);
			}
		}

		free(module->modules);
	}

	if (module->codeBase != NULL) {
		// release memory of library
		VirtualFree(module->codeBase, 0, MEM_RELEASE);
	}

	HeapFree(GetProcessHeap(), 0, module);
}

int CLoad::CallEntryPointFromMemory(HANDLE mod)
{
	PMEMORYMODULE module = (PMEMORYMODULE) mod;

	if (module == NULL || module->isDLL || module->exeEntry == NULL || !module->isRelocated) {
		return -1;
	}

	return module->exeEntry();
}

HANDLE CLoad::LoadFromFile(LPCSTR filename)
{
	HANDLE Module;
	size_t size;
	char * memblock;
	std::fstream file(filename, std::ios::in | std::ios::binary | std::ios::ate);
	if (file.is_open())
	{
		size = size_t(file.tellg());
		memblock = new char[size];
		file.seekg(0, std::ios::beg);
		file.read(memblock, size);
		file.close();
		Module = LoadFromMemory(memblock, size);
		delete[] memblock;
		return Module;
	}
	else {
		return 0;
	}
}

HANDLE CLoad::LoadFromResources(int IDD_RESOUCE)
{
	/*
	HGLOBAL hResData;
	HRSRC   hResInfo;
	void    *pvRes;
	DWORD dwSize;
	void* lpMemory;
	HMODULE hModule = GetModuleHandle( NULL );

	if ( ( ( hResInfo = FindResource( hModule, MAKEINTRESOURCE( IDD_RESOUCE ), "DLL" ) ) != NULL ) && ( ( hResData = LoadResource( hModule, hResInfo ) ) != NULL ) && ( ( pvRes = LockResource( hResData ) ) != NULL ) )
	{
		dwSize = SizeofResource( hModule, hResInfo );
		lpMemory = (char*)malloc( dwSize );
		memset( lpMemory, 0, dwSize );
		memcpy( lpMemory, pvRes, dwSize );
		return lpMemory;
	}
	*/

	return nullptr;
}

namespace BASS
{
	stream_sounds_s stream_sounds;
	HANDLE bass_lib_handle;
	CLoad bass_lib;
	DWORD stream_handle;
	DWORD request_number;
	BOOL bass_init;
	char bass_metadata[MAX_PATH];
	char bass_channelinfo[MAX_PATH];
}