#include "ICBINDx11Drv.h"

// Metallicafan212:	TODO! Setup the cache system
FTextureCache::FTextureCache()
{
	// Metallicafan212:	Currently, nothing goes on here
}

void FTextureCache::Flush()
{
	guardSlow(FTextureCache::Flush);

#if 0
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
#else

#define CLEAR_MAP(Map) \
	for(auto i = Map.begin(); i != Map.end(); i++) \
	{ \
		/* Metallicafan212:	Release all this info */ \
		FD3DTexture& Tex = i->second; \
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
	Map.clear();

	CLEAR_MAP(DWORDMap);
	CLEAR_MAP(DWORDMaskedMap);
	CLEAR_MAP(QWORDMap);
	CLEAR_MAP(QWORDMaskedMap);

#endif

	unguardSlow;
}

FD3DTexture* FTextureCache::Find(D3DCacheId InID, QWORD PolyFlags)
{
	guardSlow(FTextureCache::Find);

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
				auto	f = DWORDMaskedMap.find(Truncated);
				return	f != DWORDMaskedMap.end() ? &f->second : nullptr;

				break;
			}

			default:
			{
				// Metallicafan212:	See if it's in the map
				auto	f  = DWORDMap.find(Truncated);
				return	f != DWORDMap.end() ? &f->second : nullptr;

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
				auto	f  = QWORDMaskedMap.find(InID);
				return	f != QWORDMaskedMap.end() ? &f->second : nullptr;

				break;
			}

			default:
			{
				// Metallicafan212:	See if it's in the map
				auto	f  = QWORDMap.find(InID);
				return	f != QWORDMap.end() ? &f->second : nullptr;

				break;
			}
		}
	}

	unguardSlow;
}

FD3DTexture* FTextureCache::Set(D3DCacheId InID, QWORD PolyFlags)
{
	guardSlow(FTextureCache::Set);

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
				//return &DWORDMaskedMap.Set(Truncated, FD3DTexture());
				return &(DWORDMaskedMap[Truncated] = FD3DTexture());

				break;
			}

			default:
			{
				// Metallicafan212:	See if it's in the map
				return &(DWORDMap[Truncated] = FD3DTexture());

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
				return &(QWORDMaskedMap[InID] = FD3DTexture());

				break;
			}

			default:
			{
				// Metallicafan212:	See if it's in the map
				return &(QWORDMap[InID] = FD3DTexture());

				break;
			}
		}
	}

	unguardSlow;
}