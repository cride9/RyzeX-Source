#pragma once
#include <winnt.h>
#include <cstdint>

struct PEB_LDR_DATA {
	uint32_t		Length;
	uint8_t			Initialized;
	uintptr_t		SsHandle;
	LIST_ENTRY		InLoadOrderModuleList;
	LIST_ENTRY		InMemoryOrderModuleList;
	LIST_ENTRY		InInitializationOrderModuleList;
	uintptr_t		EntryInProgress;
	uint8_t			ShutdownInProgress;
	uintptr_t		ShutdownThreadId;
};

struct UNICODE_STRING {
	uint16_t	Length;
	uint16_t	MaximumLength;
	wchar_t* Buffer;
};

struct STRING {
	uint16_t	Length;
	uint16_t	MaximumLength;
	char* Buffer;
};

struct CURDIR {
	UNICODE_STRING	DosPath;
	uintptr_t		Handle;
};

struct RTL_DRIVE_LETTER_CURDIR {
	uint16_t	Flags;
	uint16_t	Length;
	uint32_t	TimeStamp;
	STRING		DosPath;
};

struct RTL_USER_PROCESS_PARAMETERS {
	uint32_t					MaximumLength;
	uint32_t					Length;
	uint32_t					Flags;
	uint32_t					DebugFlags;
	uintptr_t					ConsoleHandle;
	uint32_t					ConsoleFlags;
	uintptr_t					StandardInput;
	uintptr_t					StandardOutput;
	uintptr_t					StandardError;
	CURDIR						CurrentDirectory;
	UNICODE_STRING				DllPath;
	UNICODE_STRING				ImagePathName;
	UNICODE_STRING				CommandLine;
	uintptr_t					Environment;
	uint32_t					StartingX;
	uint32_t					StartingY;
	uint32_t					CountX;
	uint32_t					CountY;
	uint32_t					CountCharsX;
	uint32_t					CountCharsY;
	uint32_t					FillAttribute;
	uint32_t					WindowFlags;
	uint32_t					ShowWindowFlags;
	UNICODE_STRING				WindowTitle;
	UNICODE_STRING				DesktopInfo;
	UNICODE_STRING				ShellInfo;
	UNICODE_STRING				RuntimeData;
	RTL_DRIVE_LETTER_CURDIR		CurrentDirectores[32];
	uintptr_t					EnvironmentSize;
	uintptr_t					EnvironmentVersion;
	uintptr_t					PackageDependencyData;
	uint32_t					ProcessGroupId;
	uint32_t					LoaderThreads;
};

struct RTL_BALANCED_NODE {
	RTL_BALANCED_NODE* Children[2];
	RTL_BALANCED_NODE* Left;
	RTL_BALANCED_NODE* Right;
	uintptr_t			ParentValue;
};

struct _PEB {
	uint8_t							InheritedAddressSpace;
	uint8_t							ReadImageFileExecOptions;
	uint8_t							BeingDebugged;
	uint8_t							BitField;
	//uchar							Padding0[ 4 ];
	uintptr_t						Mutant;
	uintptr_t						ImageBaseAddress;
	PEB_LDR_DATA* Ldr;
	RTL_USER_PROCESS_PARAMETERS* ProcessParameters;
	uintptr_t						SubSystemData;
	uintptr_t						ProcessHeap;
	RTL_CRITICAL_SECTION* FastPebLock;
	uintptr_t						AtlThunkSListPtr;
	uintptr_t						IFEOKey;
	uint32_t						CrossProcessFlags;
	uint8_t							Padding1[4];
	uintptr_t						KernelCallbackTable;
	uintptr_t						UserSharedInfoPtr;
	uint32_t						SystemReserved[1];
	uint32_t						AtlThunkSListPtr32;
	uintptr_t						ApiSetMap;
	uint32_t						TlsExpansionCounter;
	uint8_t							Padding2[4];
	uintptr_t						TlsBitmap;
	uint32_t						TlsBitmapBits[2];
	uintptr_t						ReadOnlySharedMemoryBase;
	uintptr_t						SparePvoid0;
	uintptr_t						ReadOnlyStaticServerData;
	uintptr_t						AnsiCodePageData;
	uintptr_t						OemCodePageData;
	uintptr_t						UnicodeCaseTableData;
	uint32_t						NumberOfProcessors;
	uint32_t						NtGlobalFlag;
	LARGE_INTEGER					CriticalSectionTimeout;
	uintptr_t						HeapSegmentReserve;
	uintptr_t						HeapSegmentCommit;
	uintptr_t						HeapDeCommitTotalFreeThreshold;
	uintptr_t						HeapDeCommitFreeBlockThreshold;
	uint32_t						NumberOfHeaps;
	uint32_t						MaximumNumberOfHeaps;
	uintptr_t						ProcessHeaps;
	uintptr_t						GdiSharedHandleTable;
	uintptr_t						ProcessStarterHelper;
	uint32_t						GdiDCAttributeList;
	uint8_t							Padding3[4];
	RTL_CRITICAL_SECTION* LoaderLock;
	uint32_t						OSMajorVersion;
	uint32_t						OSMinorVersion;
	uint16_t						OSBuildNumber;
	uint16_t						OSCSDVersion;
	uint32_t						OSPlatformId;
	uint32_t						ImageSubsystem;
	uint32_t						ImageSubsystemMajorVersion;
	uint32_t						ImageSubsystemMinorVersion;
	uint8_t							Padding4[4];
	uintptr_t						ActiveProcessAffinityMask;
#ifdef _WIN32
	uint32_t						GdiHandleBuffer[34];
#else
	uint32_t						GdiHandleBuffer[60];
#endif
	uintptr_t						PostProcessInitRoutine;
	uintptr_t						TlsExpansionBitmap;
	uint32_t						TlsExpansionBitmapBits[32];
	uint32_t						SessionId;
	uint8_t							Padding5[4];
	ULARGE_INTEGER					AppCompatFlags;
	ULARGE_INTEGER					AppCompatFlagsUser;
	uintptr_t						pShimData;
	uintptr_t						AppCompatInfo;
	UNICODE_STRING					CSDVersion;
	uintptr_t						ActivationContextData;
	uintptr_t						ProcessAssemblyStorageMap;
	uintptr_t						SystemDefaultActivationContextData;
	uintptr_t						SystemAssemblyStorageMap;
	uintptr_t						MinimumStackCommit;
	uintptr_t						FlsCallback;
	LIST_ENTRY						FlsListHead;
	uintptr_t						FlsBitmap;
	uint32_t						FlsBitmapBits[4];
	uint32_t						FlsHighIndex;
	uintptr_t						WerRegistrationData;
	uintptr_t						WerShipAssertPtr;
	uintptr_t						pUnused;
	uintptr_t						pImageHeaderHash;
	uint32_t						TracingFlags;
	uint8_t							Padding6[4];
	uint64_t						CsrServerReadOnlySharedMemoryBase;
	uintptr_t						TppWorkerpListLock;
	LIST_ENTRY						TppWorkerpList;
	uintptr_t						WaitOnAddressHashTable[128];
};

struct LDR_DATA_TABLE_ENTRY {
	LIST_ENTRY				InLoadOrderLinks;
	LIST_ENTRY				InMemoryOrderLinks;
	LIST_ENTRY				InInitializationOrderLinks;
	uintptr_t				DllBase;
	uintptr_t				EntryPoint;
	uint32_t				SizeOfImage;
	UNICODE_STRING			FullDllName;
	UNICODE_STRING			BaseDllName;
	uint8_t					FlagGroup[4];
	uint32_t				Flags;
	uint16_t				ObsoleteLoadCount;
	uint16_t				TlsIndex;
	LIST_ENTRY				HashLinks;
	uint32_t				TimeDateStamp;
	uintptr_t				EntryPointActivationContext;
	uintptr_t				Lock;
	uintptr_t				DdagNode;
	LIST_ENTRY				NodeModuleLink;
	uintptr_t				LoadContext;
	uintptr_t				ParentDllBase;
	uintptr_t				SwitchBackContext;
	RTL_BALANCED_NODE		BaseAddressIndexNode;
	RTL_BALANCED_NODE		MappingInfoIndexNode;
	uintptr_t				OriginalBase;
	LARGE_INTEGER			LoadTime;
	uint32_t				BaseNameHashValue;
	uint32_t				LoadReason;
	uint32_t				ImplicitPathOptions;
	uint32_t				ReferenceCount;
};

#pragma pack(push, 8)
struct _UNICODE_STRING
{
	unsigned short Length; // 0x0
	unsigned short MaximumLength; // 0x2
	wchar_t* Buffer; // 0x4
};
static_assert(sizeof(_UNICODE_STRING) == 0x8);

struct _RTL_BALANCED_NODE
{
	_RTL_BALANCED_NODE* Children[2]; // 0x0
	unsigned long ParentValue; // 0x8
};
static_assert(sizeof(_RTL_BALANCED_NODE) == 0xC);

struct _LDR_DATA_TABLE_ENTRY
{
	_LIST_ENTRY InLoadOrderLinks; // 0x00
	_LIST_ENTRY InMemoryOrderLinks; // 0x08
	_LIST_ENTRY InInitializationOrderLinks; // 0x10
	void* DllBase; // 0x18
	void* EntryPoint; // 0x1C
	unsigned long SizeOfImage; // 0x20
	_UNICODE_STRING FullDllName; // 0x24
	_UNICODE_STRING BaseDllName; // 0x2C
	unsigned long Flags; // 0x34
	unsigned short ObsoleteLoadCount; // 0x38
	unsigned short TlsIndex; // 0x3A - always
	_LIST_ENTRY HashLinks; // 0x3C
	unsigned long TimeDateStamp; // 0x44
	void* EntryPointActivationContext; // 0x48
	void* Lock; // 0x4C
	struct _LDR_DDAG_NODE* DdagNode; // 0x50
	_LIST_ENTRY NodeModuleLink; // 0x54
	struct _LDRP_LOAD_CONTEXT* LoadContext; // 0x5C
	void* ParentDllBase; // 0x60
	void* SwitchBackContext; // 0x64
	_RTL_BALANCED_NODE BaseAddressIndexNode; // 0x68
	_RTL_BALANCED_NODE MappingInfoIndexNode; // 0x74
	unsigned long OriginalBase; // 0x80
	long long LoadTime; // 0x84
	unsigned long BaseNameHashValue; // 0x90
	unsigned long LoadReason; // 0x94
	unsigned long ImplicitPathOptions; // 0x98
	unsigned long ReferenceCount; // 0x9C
};
static_assert(sizeof(_LDR_DATA_TABLE_ENTRY) == 0xA0);

struct _PEB_LDR_DATA
{
	unsigned long Length; // 0x00
	unsigned char Initialized; // 0x04
	void* SsHandle; // 0x08
	_LIST_ENTRY InLoadOrderModuleList; // 0x0C
	_LIST_ENTRY InMemoryOrderModuleList; // 0x14
	_LIST_ENTRY InInitializationOrderModuleList; // 0x1C
	void* EntryInProgress; // 0x24
	unsigned char ShutdownInProgress; // 0x28
	void* ShutdownThreadId; // 0x2C
};
static_assert(sizeof(_PEB_LDR_DATA) == 0x30);

struct _CURDIR
{
	_UNICODE_STRING DosPath; // 0x0
	void* Handle; // 0x8
};
static_assert(sizeof(_CURDIR) == 0xC);

struct _STRING32
{
	unsigned short Length; // 0x0
	unsigned short MaximumLength; // 0x2
	char* Buffer; // 0x4
};
static_assert(sizeof(_STRING32) == 0x8);

struct _RTL_DRIVE_LETTER_CURDIR32
{
	unsigned short Flags; // 0x0
	unsigned short Length; // 0x2
	unsigned long Timestamp; // 0x4
	_STRING32 DosPath; // 0x8
};
static_assert(sizeof(_RTL_DRIVE_LETTER_CURDIR32) == 0x10);

struct _RTL_USER_PROCESS_PARAMETERS
{
	unsigned long MaximumLength; // 0x00
	unsigned long Length; // 0x04
	unsigned long Flags; // 0x08
	unsigned long DebugFlags; // 0x0C
	void* ConsoleHandle; // 0x10
	unsigned long ConsoleFlags; // 0x14
	void* StandardInput; // 0x18
	void* StandardOutput; // 0x1C
	void* StandardError; // 0x20
	_CURDIR CurrentDirectory; // 0x24
	_UNICODE_STRING DllPath; // 0x30
	_UNICODE_STRING ImagePathName; // 0x38
	_UNICODE_STRING CommandLine; // 0x40
	void* Environment; // 0x48
	unsigned long StartingX; // 0x4C
	unsigned long StartingY; // 0x50
	unsigned long CountX; // 0x54
	unsigned long CountY; // 0x58
	unsigned long CountCharsX; // 0x5C
	unsigned long CountCharsY; // 0x60
	unsigned long FillAttribute; // 0x64
	unsigned long WindowFlags; // 0x68
	unsigned long ShowWindowFlags; // 0x6C
	_UNICODE_STRING WindowTitle; // 0x70
	_UNICODE_STRING DesktopInfo; // 0x78
	_UNICODE_STRING ShellInfo; // 0x80
	_UNICODE_STRING RuntimeData; // 0x88
	_RTL_DRIVE_LETTER_CURDIR32 CurrentDirectores[32]; // 0x90
	unsigned long EnvironmentSize; // 0x290
	unsigned long EnvironmentVersion; // 0x294
	void* PackageDependencyData; // 0x298
	unsigned long ProcessGroupId; // 0x29C
	unsigned long LoaderThreads; // 0x2A0
};
static_assert(sizeof(_RTL_USER_PROCESS_PARAMETERS) == 0x2A4);

struct _PEB32
{
	unsigned char InheritedAddressSpace; // 0x00
	unsigned char ReadImageFileExecOptions; // 0x01
	unsigned char BeingDebugged; // 0x02
	unsigned char BitField; // 0x03
	void* Mutant; // 0x04
	void* ImageBaseAddress; // 0x08
	_PEB_LDR_DATA* Ldr; // 0x0C
	_RTL_USER_PROCESS_PARAMETERS* ProcessParameters; // 0x10
	void* SubSystemData; // 0x14
	void* ProcessHeap; // 0x18
	_RTL_CRITICAL_SECTION* FastPebLock; // 0x1c
	void* AtlThunkSListPtr; // 0x20
	void* IFEOKey; // 0x24
	unsigned long CrossProcessFlags; // 0x28
	void* KernelCallbackTable; // 0x2C
	unsigned long SystemReserved[1]; // 0x30
	unsigned long AtlThunkSListPtr32; // 0x34
	void* ApiSetMap; // 0x38
	unsigned long TlsExpansionCounter; // 0x3C
	void* TlsBitmap; // 0x40
	unsigned long TlsBitmapBits[2]; // 0x44
	void* ReadOnlySharedMemoryBase; // 0x4C
	void* SparePvoid0; // 0x50
	void** ReadOnlyStaticServerData; // 0x54
	void* AnsiCodePageData; // 0x58
	void* OemCodePageData; // 0x5C
	void* UnicodeCaseTableData; // 0x60
	unsigned long NumberOfProcessors; // 0x64
	unsigned long NtGlobalFlag; // 0x68
	long long CriticalSectionTimeout; // 0x70
	unsigned long HeapSegmentReserve; // 0x78
	unsigned long HeapSegmentCommit; // 0x7C
	unsigned long HeapDeCommitTotalFreeThreshold; // 0x80
	unsigned long HeapDeCommitFreeBlockThreshold; // 0x84
	unsigned long NumberOfHeaps; // 0x88
	unsigned long MaximumNumberOfHeaps; // 0x8C
	void** ProcessHeaps; // 0x90
	void* GdiSharedHandleTable; // 0x94
	void* ProcessStarterHelper; // 0x98
	unsigned long GdiDCAttributeList; // 0x9C
	_RTL_CRITICAL_SECTION* LoaderLock; // 0xA0
	unsigned long OSMajorVersion; // 0xA4
	unsigned long OSMinorVersion; // 0xA8
	unsigned short OSBuildNumber; // 0xAC
	unsigned short OSCSDVersion; // 0xAE
	unsigned long OSPlatformId; // 0xB0
	unsigned long ImageSubsystem; // 0xB4
	unsigned long ImageSubsystemMajorVersion; // 0xB8
	unsigned long ImageSubsystemMinorVersion; // 0xBC
	unsigned long ActiveProcessAffinityMask; // 0xC0
	unsigned long GdiHandleBuffer[34]; // 0xC4
	void(*PostProcessInitRoutine)(); // 0x14C
	void* TlsExpansionBitmap; // 0x150
	unsigned long TlsExpansionBitmapBits[32]; // 0x154
	unsigned long SessionId; // 0x1D4
	unsigned long long AppCompatFlags; // 0x1D8
	unsigned long long AppCompatFlagsUser; // 0x1E0
	void* pShimData; // 0x1E8
	void* AppCompatInfo; // 0x1EC
	_UNICODE_STRING CSDVersion; // 0x1F0
	struct _ACTIVATION_CONTEXT_DATA* ActivationContextData; // 0x1F8
	struct _ASSEMBLY_STORAGE_MAP* ProcessAssemblyStorageMap; // 0x1FC
	_ACTIVATION_CONTEXT_DATA* SystemDefaultActivationContextData; // 0x200
	_ASSEMBLY_STORAGE_MAP* SystemAssemblyStorageMap; // 0x204
	unsigned long MinimumStackCommit; // 0x208
	struct _FLS_CALLBACK_INFO* FlsCallback; // 0x20C
	_LIST_ENTRY FlsListHead; // 0x210
	void* FlsBitmap; // 0x218
	unsigned long FlsBitmapBits[4]; // 0x21C
	unsigned long FlsHighIndex; // 0x22C
	void* WerRegistrationData; // 0x230
	void* WerShipAssertPtr; // 0x234
	void* pUnused; // 0x238
	void* pImageHeaderHash; // 0x23C
	unsigned long TracingFlags; // 0x240
	unsigned long long CsrServerReadOnlySharedMemoryBase; // 0x248
};
static_assert(sizeof(_PEB32) == 0x250);

struct _CLIENT_ID32
{
	unsigned long UniqueProcess; // 0x0
	unsigned long UniqueThread; // 0x4
};
static_assert(sizeof(_CLIENT_ID32) == 0x8);

struct _GDI_TEB_BATCH32
{
	unsigned long Offset : 31; // 0x0
	unsigned long HasRenderingCommand : 1; // 0x0
	unsigned long HDC; // 0x4
	unsigned long Buffer[310]; // 0x8
};
static_assert(sizeof(_GDI_TEB_BATCH32) == 0x4E0);

struct _TEB32
{
	_NT_TIB NtTib; // 0x0
	void* EnvironmentPointer; // 0x1C
	_CLIENT_ID32 ClientId; // 0x20
	void* ActiveRpcHandle; // 0x28
	void* ThreadLocalStoragePointer; // 0x2C
	_PEB32* ProcessEnvironmentBlock; // 0x30
	unsigned long LastErrorValue; // 0x34
	unsigned long CountOfOwnedCriticalSections; // 0x38
	void* CsrClientThread; // 0x3C
	void* Win32ThreadInfo; // 0x40
	unsigned long User32Reserved[26]; // 0x44
	unsigned long UserReserved[5]; // 0xAC
	void* WOW32Reserved; // 0xC0
	unsigned long CurrentLocale; // 0xC4
	unsigned long FpSoftwareStatusRegister; // 0xC8
	void* ReservedForDebuggerInstrumentation[16]; // 0xCC
	void* SystemReserved1[38]; // 0x10C
	long ExceptionCode; // 0x1A4
	struct _ACTIVATION_CONTEXT_STACK* ActivationContextStackPointer; // 0x1A8
	unsigned long InstrumentationCallbackSp; // 0x1AC
	unsigned long InstrumentationCallbackPreviousPc; // 0x1B0
	unsigned long InstrumentationCallbackPreviousSp; // 0x1B4
	unsigned char InstrumentationCallbackDisabled; // 0x1B8
	unsigned char SpareBytes[23]; // 0x1B9
	unsigned long TxFsContext; // 0x1D0
	_GDI_TEB_BATCH32 GdiTebBatch; // 0x1D4
	_CLIENT_ID32 RealClientId; // 0x6B4
	void* GdiCachedProcessHandle; // 0x6BC
	unsigned long GdiClientPID; // 0x6C0
	unsigned long GdiClientTID; // 0x6C4
	void* GdiThreadLocalInfo; // 0x6C8
	unsigned long Win32ClientInfo[62]; // 0x6CC
	void* glDispatchTable[233]; // 0x7C4
	unsigned long glReserved1[29]; // 0xB68
	void* glReserved2; // 0xBDC
	void* glSectionInfo; // 0xBE0
	void* glSection; // 0xBE4
	void* glTable; // 0xBE8
	void* glCurrentRC; // 0xBEC
	void* glContext; // 0xBF0
	unsigned long LastStatusValue; // 0xBF4
	_UNICODE_STRING StaticUnicodeString; // 0xBF8
	wchar_t StaticUnicodeBuffer[261]; // 0xC00
	void* DeallocationStack; // 0xE0C
	void* TlsSlots[64]; // 0xE10
	_LIST_ENTRY TlsLinks; // 0xF10
	void* Vdm; // 0xF18
	void* ReservedForNtRpc; // 0xF1C
	void* DbgSsReserved[2]; // 0xF20
	unsigned long HardErrorMode; // 0xF28
	void* Instrumentation[9]; // 0xF2C
	_GUID ActivityId; // 0xF50
	void* SubProcessTag; // 0xF60
	void* PerflibData; // 0xF64
	void* EtwTraceData; // 0xF68
	void* WinSockData; // 0xF6C
	unsigned long GdiBatchCount; // 0xF70
	_PROCESSOR_NUMBER CurrentIdealProcessor; // 0xF74
	unsigned long GuaranteedStackBytes; // 0xF78
	void* ReservedForPerf; // 0xF7C
	void* ReservedForOle; // 0xF80
	unsigned long WaitingOnLoaderLock; // 0xF84
	void* SavedPriorityState; // 0xF88
	unsigned long ReservedForCodeCoverage; // 0xF8C
	void* ThreadPoolData; // 0xF90
	void** TlsExpansionSlots; // 0xF94
	unsigned long MuiGeneration; // 0xF98
	unsigned long IsImpersonating; // 0xF9C
	void* NlsCache; // 0xFA0
	void* pShimData; // 0xFA4
	unsigned short HeapVirtualAffinity; // 0xFA8
	unsigned short LowFragHeapDataSlot; // 0xFAA
	void* CurrentTransactionHandle; // 0xFAC
	struct _TEB_ACTIVE_FRAME* ActiveFrame; // 0xFB0
	void* FlsData; // 0xFB4
	void* PreferredLanguages; // 0xFB8
	void* UserPrefLanguages; // 0xFBC
	void* MergedPrefLanguages; // 0xFC0
	unsigned long MuiImpersonation; // 0xFC4
	volatile unsigned short CrossTebFlags; //0xFC8
	unsigned short SameTebFlags; //0xFCA
	void* TxnScopeEnterCallback; //0xFCC
	void* TxnScopeExitCallback; //0xFD0
	void* TxnScopeContext; //0xFD4
	unsigned long LockCount; //0xFD8
	long WowTebOffset; //0xFDC
	void* ResourceRetValue; //0xFE0
	void* ReservedForWdf; //0xFE4
	unsigned long long ReservedForCrt; //0xFE8
	_GUID EffectiveContainerId; //0xFF0
};
static_assert(sizeof(_TEB32) == 0x1000);
#pragma pack(pop)
