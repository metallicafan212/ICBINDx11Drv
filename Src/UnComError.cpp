// Metallicafan212:	Make this file optional (if needed)

#include "ICBINDx11Drv.h"

// Metallicafan212:	TODO! Figure out why the hell InterlockExchange is undefined in UT....
#if USE_COM_ERROR && DX11_HP2

#if !DX11_HP2
// Metallicafan212:	Force at least windows 7 in the headers
#include <winsdkver.h>
//#ifndef NTDDI_VERSION
#undef NTDDI_VERSION
#undef _WIN32_WINNT
//#define M212_WINVER
#define NTDDI_VERSION NTDDI_WIN8//0x06010000//NTDDI_WIN7//NTDDI_WIN8//7
#define _WIN32_WINNT _WIN32_WINNT_WIN8//0x0601//_WIN32_WINNT_WIN7//_WIN32_WINNT_WIN8//7
#define _WIN7_PLATFORM_UPDATE

#include <sdkddkver.h>
#endif
#include <comdef.h>

// Metallicafan212:	TODO! This is stupid, but you can't really ifdef a linker import in the project file....
//					Actually, the header already does it
//#pragma comment(lib, "comsuppw.lib")

FString GetHRESULTValue(HRESULT hr)
{
	guard(GetHRESULTValue);

	// Metallicafan212:	Use the com error system. This is apparently old windows functionality.
	_com_error Error(hr);

	FString Value = FString::Printf(TEXT("0x%08lx: %s"), hr, Error.ErrorMessage());

	return Value;

	unguard;
}
#else
FString GetHRESULTValue(HRESULT HR)
{
	guard(GetHRESULTValue);

	FString Value = FString::Printf(TEXT("0x%08lx"));

	return Value;

	unguard;
}
#endif