#pragma once
#include <windows.h>
#define XorStr(str) str

using HCUSTOMMODULE = void*;

using MemLoadLibraryFn = HCUSTOMMODULE( *)(LPCSTR, void*);
using MemGetProcAddressFn = FARPROC( *)(HANDLE, LPCSTR, void*);
using MemFreeLibraryFn = void( *)(HANDLE, void*);

using DllEntryProc = BOOL( WINAPI *)(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved);
using ExeEntryProc = int( WINAPI *)(void);

typedef struct
{
    PIMAGE_NT_HEADERS   headers;
    unsigned char*      codeBase;
    HCUSTOMMODULE*      modules;
    int                 numModules;
    BOOL                initialized;
    BOOL                isDLL;
    BOOL                isRelocated;
    MemLoadLibraryFn    loadLibrary;
    MemGetProcAddressFn getProcAddress;
    MemFreeLibraryFn    freeLibrary;
    void*               userdata;
    ExeEntryProc        exeEntry;
    DWORD               pageSize;
}                       MEMORYMODULE, *PMEMORYMODULE;

typedef struct
{
    LPVOID address;
    LPVOID alignedAddress;
    DWORD  size;
    DWORD  characteristics;
    BOOL   last;
}          SECTIONFINALIZEDATA, *PSECTIONFINALIZEDATA;

class CWin32PE
{
protected:
    int   CheckSize(size_t size, size_t expected);
    DWORD GetRealSectionSize(PMEMORYMODULE module, PIMAGE_SECTION_HEADER section);
    int   CopySections(const unsigned char* data, size_t size, PIMAGE_NT_HEADERS old_headers, PMEMORYMODULE module);
    int   FinalizeSection(PMEMORYMODULE module, PSECTIONFINALIZEDATA sectionData);
    int   FinalizeSections(PMEMORYMODULE module);
    int   ExecuteTLS(PMEMORYMODULE module);
    int   PerformBaseRelocation(PMEMORYMODULE module, ptrdiff_t delta);
    int   BuildImportTable(PMEMORYMODULE module);
};

class CLoad : protected CWin32PE
{
private:
    HANDLE MemLoadLibraryEx(const void*         data, size_t                     size, MemLoadLibraryFn loadLibrary,
                            MemGetProcAddressFn getProcAddress, MemFreeLibraryFn freeLibrary, void*     userdata);

public:
    HANDLE LoadFromMemory(const void*, size_t);
    HANDLE LoadFromResources(int IDD_RESOUCE);
    HANDLE LoadFromFile(LPCSTR filename);

    FARPROC GetProcAddressFromMemory(HANDLE hModule, LPCSTR ProcName);

    int  CallEntryPointFromMemory(HANDLE hModule);
    void FreeLibraryFromMemory(HANDLE hModule);
};

struct stream_sounds_s
{
    DWORD headshot;

    DWORD cod_hitsound;
    DWORD ut_hitsound;
    DWORD q3_hitsound;
    DWORD uff_hitsound;
    DWORD roblox_hitsound;
    DWORD laser;

    DWORD doublekill;
    DWORD triplekill;
    DWORD quadkill;
    DWORD wickedsick;
};

namespace BASS
{
    extern stream_sounds_s stream_sounds;
    extern HANDLE          bass_lib_handle;
    extern CLoad           bass_lib;
    extern DWORD           stream_handle;
    extern DWORD           request_number;
    extern BOOL            bass_init;
    extern char            bass_metadata[MAX_PATH];
    extern char            bass_channelinfo[MAX_PATH];
}

extern const unsigned char bass_dll_image[111772];
