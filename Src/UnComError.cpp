// Metallicafan212:	Make this file optional (if needed)

#include "ICBINDx11Drv.h"

#if USE_COM_ERROR

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