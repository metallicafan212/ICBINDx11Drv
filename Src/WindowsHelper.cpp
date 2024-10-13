#include "ICBINDx11Drv.h"
#if !DX11_HP2
#include <windows.h>
#include <VersionHelpers.h>

// Metallicafan212:	Define all the stuff needed here
// Metallicafan212:	Bool to mark the currently running application as a wine app
UBOOL						GWineAndDine = 0;

// Metallicafan212:	Bool to say that the current OS is 8.1 or higher
//					Certain DPI options REQUIRE this
UBOOL						GWin81 = 0;

// Metallicafan212:	Same here, but for Windows 10
UBOOL						GWin10 = 0;

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

	unguard;
}

#endif