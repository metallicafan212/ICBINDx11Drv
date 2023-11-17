#include "ICBINDx11Drv.h"

// Metallicafan212:	TODO! Setup the cache system
FTextureCache::FTextureCache()
{
	// Metallicafan212:	Currently, nothing goes on here
}

void FTextureCache::Flush()
{
	guard(FTextureCache::Flush);

	// Metallicafan212:	TODO! Loop our maps and clear the textures

#define CLEAR_MAP(ittType, Map) \
	for (ittType It(Map); It; ++It) \
	{ \
		/* Metallicafan212:	Release all this info */ \
		FD3DTexture& Tex = It.Value(); \
		SAFE_RELEASE(Tex.m_View); \
		SAFE_RELEASE(Tex.m_Tex); \
		/* Metallicafan212: UAVs for each mip (unused) */ \
		for (INT i = 0; i < Tex.UAVMips.Num(); i++) \
		{ \
			SAFE_RELEASE(Tex.UAVMips(i)); \
		} \
		SAFE_RELEASE(Tex.P8ConvSRV); \
		SAFE_RELEASE(Tex.P8ConvTex); \
	} \
	Map.Empty();

	CLEAR_MAP(DWORD_MAP::TIterator, DWORDMap);
	CLEAR_MAP(DWORD_MAP::TIterator, DWORDMaskedMap);

	CLEAR_MAP(QWORD_MAP::TIterator, QWORDMap);
	CLEAR_MAP(QWORD_MAP::TIterator, QWORDMaskedMap);

	unguard;
}

FD3DTexture* FTextureCache::Find(D3DCacheId InID, QWORD PolyFlags)
{
	guard(FTextureCache::Find);

	// Metallicafan212:	Figure out what map this should be in
	DWORD Truncated = (DWORD)InID;

	if (Truncated == InID)
	{
		// Metallicafan212:	DWORD map

		// Metallicafan212:	Use a switch to use jump tables
		switch (PolyFlags & PF_Masked)
		{
			case PF_Masked:
			{
				// Metallicafan212:	See if it's in the map
				return DWORDMaskedMap.Find(Truncated);

				break;
			}

			default:
			{
				// Metallicafan212:	See if it's in the map
				return DWORDMap.Find(Truncated);

				break;
			}
		}
	}
	else
	{
		// Metallicafan212:	QWORD map

		// Metallicafan212:	Use a switch to use jump tables
		switch (PolyFlags & PF_Masked)
		{
			case PF_Masked:
			{
				// Metallicafan212:	See if it's in the map
				return QWORDMaskedMap.Find(InID);

				break;
			}

			default:
			{
				// Metallicafan212:	See if it's in the map
				return QWORDMap.Find(InID);

				break;
			}
		}
	}

	unguard;
}

FD3DTexture* FTextureCache::Set(D3DCacheId InID, QWORD PolyFlags)
{
	guard(FTextureCache::Set);

	// Metallicafan212:	Figure out what map this should be in
	DWORD Truncated = (DWORD)InID;

	if (Truncated == InID)
	{
		// Metallicafan212:	DWORD map

		// Metallicafan212:	Use a switch to use jump tables
		switch (PolyFlags & PF_Masked)
		{
			case PF_Masked:
			{
				// Metallicafan212:	See if it's in the map
				return &DWORDMaskedMap.Set(Truncated, FD3DTexture());

				break;
			}

			default:
			{
				// Metallicafan212:	See if it's in the map
				return &DWORDMap.Set(Truncated, FD3DTexture());

				break;
			}
		}
	}
	else
	{
		// Metallicafan212:	QWORD map

		// Metallicafan212:	Use a switch to use jump tables
		switch (PolyFlags & PF_Masked)
		{
			case PF_Masked:
			{
				// Metallicafan212:	See if it's in the map
				return &QWORDMaskedMap.Set(InID, FD3DTexture());

				break;
			}

			default:
			{
				// Metallicafan212:	See if it's in the map
				return &QWORDMap.Set(InID, FD3DTexture());

				break;
			}
		}
	}

	unguard;
}