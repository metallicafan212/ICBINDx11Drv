#include "ICBINDx11Drv.h"
#if !DX11_HP2
#include <windows.h>
#include <winternl.h>
#include <VersionHelpers.h>

// Metallicafan212:	Define all the stuff needed here
// Metallicafan212:	Bool to mark the currently running application as a wine app
UBOOL						GWineAndDine = 0;

// Metallicafan212:	Bool to say that the current OS is 8.1 or higher
//					Certain DPI options REQUIRE this
UBOOL						GWin81 = 0;

// Metallicafan212:	Same here, but for Windows 10
UBOOL						GWin10 = 0;

#define USE_PRIVATE_PEB 0
#if USE_PRIVATE_PEB
// Metallicafan212: Blank these as they aren't important
typedef void*   PPEB_FREE_BLOCK;
 struct         _ACTIVATION_CONTEXT_DATA;
struct          _ASSEMBLY_STORAGE_MAP;
struct          _FLS_CALLBACK_INFO;

// Metallicafan212:	Private definition for the PEB
//                  M$ doesn't provide this in their headers, but WILL provide it in the VS debugger for some fucking reason
struct PRIVATE_PEB
{
    UCHAR InheritedAddressSpace;
    UCHAR ReadImageFileExecOptions;
    UCHAR BeingDebugged;
    UCHAR BitField;
    ULONG ImageUsesLargePages : 1;
    ULONG IsProtectedProcess : 1;
    ULONG IsLegacyProcess : 1;
    ULONG IsImageDynamicallyRelocated : 1;
    ULONG SpareBits : 4;
    PVOID Mutant;
    PVOID ImageBaseAddress;
    PPEB_LDR_DATA Ldr;
    PRTL_USER_PROCESS_PARAMETERS ProcessParameters;
    PVOID SubSystemData;
    PVOID ProcessHeap;
    PRTL_CRITICAL_SECTION FastPebLock;
    PVOID AtlThunkSListPtr;
    PVOID IFEOKey;
    ULONG CrossProcessFlags;
    ULONG ProcessInJob : 1;
    ULONG ProcessInitializing : 1;
    ULONG ReservedBits0 : 30;
    union
    {
        PVOID KernelCallbackTable;
        PVOID UserSharedInfoPtr;
    };
    ULONG SystemReserved[1];
    ULONG SpareUlong;
    PPEB_FREE_BLOCK FreeList;
    ULONG TlsExpansionCounter;
    PVOID TlsBitmap;
    ULONG TlsBitmapBits[2];
    PVOID ReadOnlySharedMemoryBase;
    PVOID HotpatchInformation;
    VOID** ReadOnlyStaticServerData;
    PVOID AnsiCodePageData;
    PVOID OemCodePageData;
    PVOID UnicodeCaseTableData;
    ULONG NumberOfProcessors;
    ULONG NtGlobalFlag;
    LARGE_INTEGER CriticalSectionTimeout;
    ULONG HeapSegmentReserve;
    ULONG HeapSegmentCommit;
    ULONG HeapDeCommitTotalFreeThreshold;
    ULONG HeapDeCommitFreeBlockThreshold;
    ULONG NumberOfHeaps;
    ULONG MaximumNumberOfHeaps;
    VOID** ProcessHeaps;
    PVOID GdiSharedHandleTable;
    PVOID ProcessStarterHelper;
    ULONG GdiDCAttributeList;
    PRTL_CRITICAL_SECTION LoaderLock;
    ULONG OSMajorVersion;
    ULONG OSMinorVersion;
    WORD OSBuildNumber;
    WORD OSCSDVersion;
    ULONG OSPlatformId;
    ULONG ImageSubsystem;
    ULONG ImageSubsystemMajorVersion;
    ULONG ImageSubsystemMinorVersion;
    ULONG ImageProcessAffinityMask;
    ULONG GdiHandleBuffer[34];
    PVOID PostProcessInitRoutine;
    PVOID TlsExpansionBitmap;
    ULONG TlsExpansionBitmapBits[32];
    ULONG SessionId;
    ULARGE_INTEGER AppCompatFlags;
    ULARGE_INTEGER AppCompatFlagsUser;
    PVOID pShimData;
    PVOID AppCompatInfo;
    UNICODE_STRING CSDVersion;
    _ACTIVATION_CONTEXT_DATA* ActivationContextData;
    _ASSEMBLY_STORAGE_MAP* ProcessAssemblyStorageMap;
    _ACTIVATION_CONTEXT_DATA* SystemDefaultActivationContextData;
    _ASSEMBLY_STORAGE_MAP* SystemAssemblyStorageMap;
    ULONG MinimumStackCommit;
    _FLS_CALLBACK_INFO* FlsCallback;
    LIST_ENTRY FlsListHead;
    PVOID FlsBitmap;
    ULONG FlsBitmapBits[4];
    ULONG FlsHighIndex;
    PVOID WerRegistrationData;
    PVOID WerShipAssertPtr;
};
#endif

void SetupWindowsVersionCheck()
{
	guard(SetupWindowsVersionCheck);

	// Metallicafan212:	Check for windows 8.1
	GWin81	= IsWindows8Point1OrGreater();
	GWin10	= IsWindows10OrGreater();

	// Metallicafan212:	Check if wine
	typedef const char* (CDECL* pwine_get_version)(void);

	pwine_get_version func = nullptr;

	HMODULE hntDLL = GetModuleHandle(TEXT("ntdll.dll"));

	if (hntDLL != nullptr)
	{
		func = (pwine_get_version)GetProcAddress(hntDLL, "wine_get_version");

		if (func != nullptr)
		{
			GWineAndDine = 1;

			debugf(NAME_Init, TEXT("DX11: Detected, Wine/Proton Windows emulator. WARNING! Wine/Proton emulation is not perfect!"));
			debugf(NAME_Init, TEXT("DX11: Wine/Proton version: %s"), appFromAnsi(func()));
		}
	}

#if 1
	// Metallicafan212:	Check the PEB if this fails
	if (!GWin10 && !GWin81)
	{
		PROCESS_BASIC_INFORMATION  ProcessInfo;

		appMemzero(&ProcessInfo, sizeof(PROCESS_BASIC_INFORMATION));

		NTSTATUS Status = NtQueryInformationProcess(GetCurrentProcess(), ProcessBasicInformation, &ProcessInfo, sizeof(PROCESS_BASIC_INFORMATION), nullptr);

		if (Status == 0)
		{

#if USE_PRIVATE_PEB
            PRIVATE_PEB* PebPrivate = (PRIVATE_PEB*)ProcessInfo.PebBaseAddress;
            ULONG MajorVersion	    = PebPrivate->OSMajorVersion;
            ULONG MinorVersion	    = PebPrivate->OSMinorVersion;
            ULONG BuildNumber	    = PebPrivate->OSBuildNumber;
#else
			// Metallicafan212:	Success, get the value from the offsets
			//					Microsoft doesn't give official documentation on the variables, but they haven't changed ever
			//					The offsets change based on the OS arch though
			BYTE* PEBPtr		= (BYTE*)ProcessInfo.PebBaseAddress;
#if UNREAL64
			ULONG MajorVersion	= *(ULONG*)(PEBPtr + 0x0118);
			ULONG MinorVersion	= *(ULONG*)(PEBPtr + 0x011C);
			ULONG BuildNumber	= *(ULONG*)(PEBPtr + 0x0120);
#else
			ULONG MajorVersion	= *(ULONG*)(PEBPtr + 0xA4);
			ULONG MinorVersion	= *(ULONG*)(PEBPtr + 0xA8);
			ULONG BuildNumber	= *(ULONG*)(PEBPtr + 0xAC);
#endif
#endif

			// Metallicafan212:	8.1 or higher?
			if (MajorVersion > 6 || (MajorVersion == 6 && MinorVersion >= 3))
			{
				GWin81 = 1;
			}

			// Metallicafan212:	10.0 or higher
			if (MajorVersion > 10 || (MajorVersion == 10 && MinorVersion >= 0))
			{
				GWin10 = 1;
			}

			if (GWin10)
			{
				debugf(NAME_Init, TEXT("DX11: Detected that the system is actually (at least) Windows 10 build %d"), BuildNumber);
			}
			else if (GWin81)
			{
				debugf(NAME_Init, TEXT("DX11: Detected that the system is actually Windows 8.1 build %d"), BuildNumber);
			}

		}
	}
#else 
	// Metallicafan212:	HACK! If the OS is saying that it isn't 81 OR 10, just make sure
	//					This utilizes a hard-coded pointer that every app (apparenly) has on Windows
	if (!GWin10 && !GWin81)
	{
		BYTE* SharedAppData	= (BYTE*)0x7FFE0000;

		ULONG MajorVersion	= *(ULONG*)(SharedAppData + 0x26c);
		ULONG MinorVersion	= *(ULONG*)(SharedAppData + 0x270);
		ULONG BuildNumber	= *(ULONG*)(SharedAppData + 0x260);

		// Metallicafan212:	8.1 or higher?
		if (MajorVersion > 6 || (MajorVersion == 6 && MinorVersion >= 3))
		{
			GWin81 = 1;
		}

		// Metallicafan212:	10.0 or higher
		if (MajorVersion > 10 || (MajorVersion == 10 && MinorVersion >= 0))
		{
			GWin10 = 1;
		}
	}
#endif

	unguard;
}

#endif