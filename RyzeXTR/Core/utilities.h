#pragma once
#include <Windows.h>
#include <Psapi.h>
#include <cstddef>
#include <iostream>
#include "SDK/DataTyes/Color.h"
#include <fstream>
#include "SDK/pe.h"
#include "SDK/Memory.h"
#include <comdef.h>

#define RYZEXCOLOR Color(111, 203, 255)
#define INRANGE(x,a,b)   (x >= a && x <= b)
#define GET_BYTE( x )    (GET_BITS(x[0]) << 4 | GET_BITS(x[1]))
#define GET_BITS( x )    (INRANGE((x&(~0x20)),'A','F') ? ((x&(~0x20)) - 'A' + 0xa) : (INRANGE(x,'0','9') ? x - '0' : 0))

namespace util {

	inline void Print(const char* csText) {
		// YES endl is slower than just a \n BUT IT ALSO CLEARS THE BUFFER (learn C ty)
#if _DEBUG
		std::cout << csText << std::endl;
#endif
	}

	inline void Print(const char* csText1, const char* csText2) {
		// YES endl is slower than just a \n BUT IT ALSO CLEARS THE BUFFER (learn C ty)
		std::cout << csText1 << csText2 << std::endl;
	}

	inline void LogConsole(const char* msg, Color color = Color(255, 255, 255)) {

		static constexpr int LS_MESSAGE = 0;

		using find_channel_fn_t = int(__cdecl*)(const char* name);
		static const auto s_channel_id = ((find_channel_fn_t)GetProcAddress(GetModuleHandleA("tier0.dll"), "LoggingSystem_FindChannel"))("Console");

		using log_direct_fn_t = int(__cdecl*)(int id, int severity, Color color, const char* msg);
		static const auto s_log_direct = (log_direct_fn_t)GetProcAddress(GetModuleHandleA("tier0.dll"), "LoggingSystem_LogDirect");

		//static std::string prefix = "[RyzeX] ";
		//s_log_direct(s_channel_id, LS_MESSAGE, RYZEXCOLOR, "[RyzeX] ");
		
		s_log_direct(s_channel_id, LS_MESSAGE, color, msg);
		
	}

	template <typename T, typename ... Args_t>
	inline constexpr T CallVFunc(void* thisptr, std::size_t nIndex, Args_t... argList) {

		using VirtualFn = T(__thiscall*)(void*, decltype(argList)...);
		return (*static_cast<VirtualFn**>(thisptr))[nIndex](thisptr, argList...);
	}

	template <typename T = void*>
	inline constexpr T GetVFunc(void* thisptr, std::size_t nIndex) {

		return (*static_cast<T**>(thisptr))[nIndex];
	}

	inline std::uintptr_t GetAbsoluteAddress(const std::uintptr_t uRelativeAddress) {

		return uRelativeAddress + 0x4 + *reinterpret_cast<std::int32_t*>(uRelativeAddress);
	}

	inline void* GetExportAddress(const void* pModuleBase, const std::string_view szProcedureName)
	{
		const std::uint8_t* pAddress = static_cast<const std::uint8_t*>(pModuleBase);
		const IMAGE_DOS_HEADER* pDosHeader = static_cast<const IMAGE_DOS_HEADER*>(pModuleBase);
		const IMAGE_NT_HEADERS* pNtHeaders = reinterpret_cast<const IMAGE_NT_HEADERS*>(pAddress + pDosHeader->e_lfanew);
		const IMAGE_OPTIONAL_HEADER* pOptionalHeader = &pNtHeaders->OptionalHeader;

		const std::uintptr_t uExportSize = pOptionalHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size;
		const std::uintptr_t uExportAddress = pOptionalHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;

		if (uExportSize <= 0U)
			return nullptr;

		const IMAGE_EXPORT_DIRECTORY* pExportDirectory = reinterpret_cast<const IMAGE_EXPORT_DIRECTORY*>(pAddress + uExportAddress);
		const std::uintptr_t* pNamesRVA = reinterpret_cast<const std::uintptr_t*>(pAddress + pExportDirectory->AddressOfNames);
		const std::uintptr_t* pFunctionsRVA = reinterpret_cast<const std::uintptr_t*>(pAddress + pExportDirectory->AddressOfFunctions);
		const std::uint16_t* pNameOrdinals = reinterpret_cast<const std::uint16_t*>(pAddress + pExportDirectory->AddressOfNameOrdinals);

		// perform binary search
		std::uintptr_t uRight = pExportDirectory->NumberOfNames;
		std::uintptr_t uLeft = 0;

		while (uRight != uLeft)
		{
			const std::uintptr_t uMiddle = uLeft + ((uRight - uLeft) >> 1U);
			const int iResult = szProcedureName.compare(reinterpret_cast<const char*>(pAddress + pNamesRVA[uMiddle]));

			if (iResult == 0)
				return const_cast<void*>(static_cast<const void*>(pAddress + pFunctionsRVA[pNameOrdinals[uMiddle]]));

			if (iResult > 0)
				uLeft = uMiddle;
			else
				uRight = uMiddle;
		}


		return nullptr;
	}

	inline void* GetModuleBaseHandle(const std::string_view szModuleName)
	{
		const _PEB32* pPEB = reinterpret_cast<_PEB32*>(__readfsdword(0x30));

		if (szModuleName.empty())
			return pPEB->ImageBaseAddress;

		const std::wstring wszModuleName(szModuleName.begin(), szModuleName.end());

		for (LIST_ENTRY* pListEntry = pPEB->Ldr->InLoadOrderModuleList.Flink; pListEntry != &pPEB->Ldr->InLoadOrderModuleList; pListEntry = pListEntry->Flink)
		{
			const _LDR_DATA_TABLE_ENTRY* pEntry = CONTAINING_RECORD(pListEntry, _LDR_DATA_TABLE_ENTRY, InLoadOrderLinks);

			if (pEntry->BaseDllName.Buffer && wszModuleName.compare(pEntry->BaseDllName.Buffer) == 0)
				return pEntry->DllBase;
		}


		return nullptr;
	}

	inline BYTE* ReadWavFileIntoMemory( std::string fname ) {
		BYTE* pb = nullptr;
		std::ifstream f( fname, std::ios::binary );

		f.seekg( 0, std::ios::end );
		size_t lim = f.tellg( );

		pb = new BYTE[ lim ];
		f.seekg( 0, std::ios::beg );
		f.read( ( char* )pb, lim );
		f.close( );

		return pb;
	}

	inline DWORD FindHudElement(const char* name) {

		static auto pThis = *reinterpret_cast<DWORD**>(MEM::FindPattern(CLIENT_DLL, XorStr("B9 ? ? ? ? E8 ? ? ? ? 8B 5D 08")) + 1);
		if (!pThis)
			return -1;

		static auto find_hud_element = reinterpret_cast<DWORD(__thiscall*)(void*, const char*)>(MEM::FindPattern(CLIENT_DLL, XorStr("55 8B EC 53 8B 5D 08 56 57 8B F9 33 F6 39 77 28")));
		if (!find_hud_element)
			return -1;

		return find_hud_element(pThis, name);
	}

	inline void SetClan(const char* csTag) {

		static auto pSetClantag = reinterpret_cast<void(__fastcall*)(const char*, const char*)>(MEM::FindPattern(ENGINE_DLL, XorStr("53 56 57 8B DA 8B F9 FF")));
		pSetClantag(csTag, csTag);
	}

	inline void SetSkybox(const char* szSkybox) {

		static auto LoadNamedSky = reinterpret_cast<void(__fastcall*)(const char*)>(MEM::FindPattern(ENGINE_DLL, XorStr("55 8B EC 81 EC ? ? ? ? 56 57 8B F9 C7 45")));
		
		if (LoadNamedSky != nullptr)
			LoadNamedSky(szSkybox);
	}

	template <typename T>
	static constexpr auto GetAbsoluteAddress(uintptr_t address) noexcept
	{
		return (T)(address + 4 + *reinterpret_cast<std::int32_t*>(address));
	}

	inline std::uintptr_t* FindHudElement2( const char* szName )
	{
		// @note: https://www.unknowncheats.me/forum/counterstrike-global-offensive/342743-finding-sigging-chud-pointer-chud-findelement.html

		static auto pHud = *reinterpret_cast<void**>(MEM::FindPattern(CLIENT_DLL, XorStr("B9 ? ? ? ? 68 ? ? ? ? E8 ? ? ? ? 89")) + 0x1);
		// @xref: "CHudWeaponSelection"

		using FindHudElementFn = std::uintptr_t* ( __thiscall* )( void*, const char* );
		static auto oFindHudElement = reinterpret_cast<FindHudElementFn>(MEM::FindPattern(CLIENT_DLL, XorStr("55 8B EC 53 8B 5D 08 56 57 8B F9 33 F6 39 77 28"))); // @xref: "[%d] Could not find Hud Element: %s\n"
		assert( oFindHudElement != nullptr );

		return oFindHudElement( pHud, szName );
	}

	inline void ForceFullUpdate( )
	{
		using ClearHudWeaponIconFn = int( __thiscall* )( void*, int );
		static auto oClearHudWeaponIcon = reinterpret_cast<ClearHudWeaponIconFn>(MEM::FindPattern(CLIENT_DLL, XorStr("55 8B EC 51 53 56 8B 75 08 8B D9 57 6B"))); // @xref: "WeaponIcon--itemcount"
		assert( oClearHudWeaponIcon != nullptr );

		// get hud weapons
		if (const auto pHudWeapons = FindHudElement2(XorStr("CCSGO_HudWeaponSelection")) - 0x28; pHudWeapons != nullptr)
		{
			// go through all weapons
			for ( std::size_t i = 0; i < *( pHudWeapons + 0x20 ); i++ )
				i = oClearHudWeaponIcon( pHudWeapons, i );
		}
	}

	inline uintptr_t FindByteSequenceInModule(HMODULE hModule, const BYTE* sequence, size_t sequenceSize)
	{
		MODULEINFO moduleInfo;
		if (!GetModuleInformation(GetCurrentProcess(), hModule, &moduleInfo, sizeof(MODULEINFO)))
		{
			// Failed to get module information
			return 0;
		}

		BYTE* codeBase = reinterpret_cast<BYTE*>(moduleInfo.lpBaseOfDll);
		const size_t codeSize = moduleInfo.SizeOfImage;

		for (size_t i = 0; i < codeSize - sequenceSize; ++i)
		{
			bool found = true;

			for (size_t j = 0; j < sequenceSize; ++j)
			{
				if (codeBase[i + j] != sequence[j])
				{
					found = false;
					break;
				}
			}

			if (found)
			{
				// Found the byte sequence
				return reinterpret_cast<uintptr_t>(&codeBase[i]);
			}
		}

		// Byte sequence not found
		return 0;
	}

	inline void CopyToClipboard(const std::string& text) {

		if (OpenClipboard(nullptr)) {
			EmptyClipboard();
			HGLOBAL hClipboardData;
			const char* charArray = text.c_str();
			const size_t dataSize = (text.length() + 1) * sizeof(char);
			hClipboardData = GlobalAlloc(GMEM_MOVEABLE, dataSize);
			if (hClipboardData != nullptr) {
				void* data = GlobalLock(hClipboardData);
				if (data != nullptr) {
					memcpy(data, charArray, dataSize);
					GlobalUnlock(hClipboardData);
					SetClipboardData(CF_TEXT, hClipboardData);
				}
			}
			CloseClipboard();
		}
	}

	inline std::vector<std::string> AnimateText(const std::string& szInput) {

		std::vector<std::string> vecReturnValue{};

		int iCurrentLine = 0;
		for (size_t i = 0; i < szInput.length(); i++) {
			iCurrentLine++;
			std::string szLineOutput = "";
			for (size_t j = 0; j < iCurrentLine; j++) {
				szLineOutput += szInput[j];
			}

			vecReturnValue.push_back(szLineOutput);

			if (i == szInput.length() - 1)
				for (size_t j = 0; j < 3; j++)
					vecReturnValue.push_back(szLineOutput);
		}

		iCurrentLine = 0;
		for (size_t i = szInput.length(); i > 0; i--) {
			iCurrentLine++;
			std::string szLineOutput = "";
			for (size_t j = szInput.length() - iCurrentLine; j > 0; j--) {

				szLineOutput += szInput[j];
			}
			std::reverse(szLineOutput.begin(), szLineOutput.end());
			vecReturnValue.push_back(szLineOutput);
		}

		return vecReturnValue;
	}

	inline bool IsFontInstalled(const wchar_t* fontName)
	{
		HDC hdc = GetDC(nullptr);
		LOGFONT lf;
		ZeroMemory(&lf, sizeof(LOGFONT));
		lf.lfCharSet = DEFAULT_CHARSET;
		wcsncpy_s(_bstr_t(lf.lfFaceName), LF_FACESIZE, fontName, LF_FACESIZE - 1);

		bool fontFound = false;
		EnumFontFamiliesEx(hdc, &lf, [](const LOGFONT* lplf, const TEXTMETRIC* lptm, DWORD, LPARAM lParam) -> int {
			*(reinterpret_cast<bool*>(lParam)) = true;
			return 0;
			}, reinterpret_cast<LPARAM>(&fontFound), 0);

		ReleaseDC(nullptr, hdc);
		return fontFound;
	}

	inline bool InstallFontFromByteArray(const BYTE* fontData, DWORD dataSize)
	{
		DWORD fileSize = sizeof(fontData);

		// Install the font from memory
		HANDLE hFont = AddFontMemResourceEx((void*)&fontData[0], fileSize, NULL, &fileSize);
		if (hFont == NULL) {
			// Handle the error if the font cannot be installed
			return false;
		}

		// Use the installed font in your application
		// ...

		// Clean up the resources
		RemoveFontMemResourceEx(hFont);

		return true;
	}
}