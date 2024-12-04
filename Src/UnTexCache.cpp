// Metallicafan212:	Robin-map requires C++ 17
#define USE_ROBIN_MAP 1 && _MSVC_LANG >= 201703L

// Metallicafan212:	Robin map has a LOT of conflictitory defines in it..... It has to be included first otherwise it throws a shit fit
#if USE_ROBIN_MAP
#include "Robin-Map\robin_map.h"
#endif

#include "ICBINDx11Drv.h"

#if !USE_ROBIN_MAP
// Metallicafan212:	We need to swap to an unordered map for cache consistency issues
#include <unordered_map>
#endif

#define TEX_BUCKET_SIZE 16384

// Metallicafan212:	Child container so we can include different maps _ONLY_ in this file
class FTextureCacheMap
{
	friend class FTextureCache;

public:

	FTextureCacheMap()
	{
		QWORDMap.reserve(TEX_BUCKET_SIZE);
		QWORDMaskedMap.reserve(TEX_BUCKET_SIZE);

		LastNorm	= nullptr;
		LastMasked	= nullptr;
	}

	~FTextureCacheMap()
	{

	}

protected:

#if USE_ROBIN_MAP
	tsl::robin_map<QWORD, FD3DTexture*>		QWORDMap;//FD3DTexture>		QWORDMap;
	tsl::robin_map<QWORD, FD3DTexture*>		QWORDMaskedMap;//FD3DTexture>		QWORDMaskedMap;
#else
	std::unordered_map<QWORD, FD3DTexture*>	QWORDMap;
	std::unordered_map<QWORD, FD3DTexture*>	QWORDMaskedMap;
#endif

	// Metallicafan212:	Last request for each array
	FD3DTexture*							LastNorm;
	FD3DTexture*							LastMasked;
};

// Metallicafan212:	TODO! Setup the cache system
FTextureCache::FTextureCache()
{
	// Metallicafan212:	Currently, nothing goes on here

	// Metallicafan212:	Reserve 2048 buckets per????
#if 1//USE_UNORDERED_MAP
	//QWORDMap.reserve(4096);
	//QWORDMaskedMap.reserve(4096);

	ChildMap = new FTextureCacheMap();
#endif
}

FTextureCache::~FTextureCache()
{
	// Metallicafan212:	Get rid of the container

	delete ChildMap;
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
		/* Metallicafan212:	Now delete*/ \
		delete Tex; \
	} \
	Map.Empty();

	CLEAR_MAP(DWORD_MAP::TIterator, DWORDMap);
	CLEAR_MAP(DWORD_MAP::TIterator, DWORDMaskedMap);

	CLEAR_MAP(QWORD_MAP::TIterator, QWORDMap);
	CLEAR_MAP(QWORD_MAP::TIterator, QWORDMaskedMap);
#elif USE_ROBIN_MAP

#define CLEAR_MAP(Map) \
	for(auto i = Map.begin(); i != Map.end(); i++) \
	{ \
		/* Metallicafan212:	Release all this info */ \
		FD3DTexture*& Tex = i.value(); /*->second;*/ \
		SAFE_RELEASE(Tex->m_View); \
		SAFE_RELEASE(Tex->m_Tex); \
		/* Metallicafan212: UAVs for each mip (unused) */ \
		for (INT i = 0; i < Tex->UAVMips.Num(); i++) \
		{ \
			SAFE_RELEASE(Tex->UAVMips(i)); \
		} \
		SAFE_RELEASE(Tex->P8ConvSRV); \
		SAFE_RELEASE(Tex->P8ConvTex); \
		/* Metallicafan212:	Now delete*/ \
		delete Tex; \
	} \
	Map.clear();

	// Metallicafan212:	Check what the hash size was
	//GLog->Logf(TEXT("DX11: Texture map bucket size %d"), ChildMap->QWORDMap.bucket_count());
	//GLog->Logf(TEXT("DX11: Texture masked map bucket size %d"), ChildMap->QWORDMaskedMap.bucket_count());

	//CLEAR_MAP(DWORDMap);
	//CLEAR_MAP(DWORDMaskedMap);

	CLEAR_MAP(ChildMap->QWORDMap);
	CLEAR_MAP(ChildMap->QWORDMaskedMap);

	ChildMap->QWORDMap.reserve(TEX_BUCKET_SIZE);
	ChildMap->QWORDMaskedMap.reserve(TEX_BUCKET_SIZE);

#else
#define CLEAR_MAP(Map) \
	for(auto i = Map.begin(); i != Map.end(); i++) \
	{ \
		/* Metallicafan212:	Release all this info */ \
		FD3DTexture*& Tex = i->second; \
		SAFE_RELEASE(Tex->m_View); \
		SAFE_RELEASE(Tex->m_Tex); \
		/* Metallicafan212: UAVs for each mip (unused) */ \
		for (INT i = 0; i < Tex->UAVMips.Num(); i++) \
		{ \
			SAFE_RELEASE(Tex->UAVMips(i)); \
		} \
		SAFE_RELEASE(Tex->P8ConvSRV); \
		SAFE_RELEASE(Tex->P8ConvTex); \
	} \
	Map.clear();

	// Metallicafan212:	Check what the hash size was
	//GLog->Logf(TEXT("DX11: Texture map bucket size %d"), ChildMap->QWORDMap.bucket_count());
	//GLog->Logf(TEXT("DX11: Texture masked map bucket size %d"), ChildMap->QWORDMaskedMap.bucket_count());

	//CLEAR_MAP(DWORDMap);
	//CLEAR_MAP(DWORDMaskedMap);

	CLEAR_MAP(ChildMap->QWORDMap);
	CLEAR_MAP(ChildMap->QWORDMaskedMap);

	ChildMap->QWORDMap.reserve(TEX_BUCKET_SIZE);
	ChildMap->QWORDMaskedMap.reserve(TEX_BUCKET_SIZE);
#endif

	ChildMap->LastMasked	= nullptr;
	ChildMap->LastNorm		= nullptr;

	unguardSlow;
}

FD3DTexture* FTextureCache::Find(D3DCacheId InID, PFLAG PolyFlags)
{
	guardSlow(FTextureCache::Find);

	/*
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
	*/
	{
		// Metallicafan212:	QWORD map

		// Metallicafan212:	Use a switch to use jump tables
		switch (PolyFlags & PF_Masked)
		{
			case PF_Masked:
			{
				if (ChildMap->LastMasked != nullptr && ChildMap->LastMasked->CacheID == InID)
				{
					return ChildMap->LastMasked;
				}
				// Metallicafan212:	See if it's in the map
				auto	f  = ChildMap->QWORDMaskedMap.find(InID);
#if USE_ROBIN_MAP
				FD3DTexture* Lookup = f != ChildMap->QWORDMaskedMap.end() ? f.value() : nullptr;

				//return	f != ChildMap->QWORDMaskedMap.end() ? f.value() : nullptr;
#else
				FD3DTexture* Lookup = f != ChildMap->QWORDMaskedMap.end() ? f->second : nullptr;
				//return	f != ChildMap->QWORDMaskedMap.end() ? f->second : nullptr;
#endif
				if (Lookup != nullptr)
				{
					ChildMap->LastMasked = Lookup;
				}

				return Lookup;

				break;
			}

			default:
			{
				if (ChildMap->LastNorm != nullptr && ChildMap->LastNorm->CacheID == InID)
				{
					return ChildMap->LastNorm;
				}

				// Metallicafan212:	See if it's in the map
				auto	f  = ChildMap->QWORDMap.find(InID);
#if USE_ROBIN_MAP
				FD3DTexture* Lookup = f != ChildMap->QWORDMap.end() ? f.value() : nullptr;
				//return	f != ChildMap->QWORDMap.end() ? f.value() : nullptr;
#else
				FD3DTexture* Lookup = f != ChildMap->QWORDMap.end() ? f->second : nullptr;
				//return	f != ChildMap->QWORDMap.end() ? f->second : nullptr;
#endif
				if (Lookup != nullptr)
				{
					ChildMap->LastNorm = Lookup;
				}

				return Lookup;

				break;
			}
		}
	}

	unguardSlow;
}

FD3DTexture* FTextureCache::Set(D3DCacheId InID, PFLAG PolyFlags)
{
	guardSlow(FTextureCache::Set);

	/*
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
	*/
	{
		// Metallicafan212:	QWORD map

		// Metallicafan212:	Use a switch to use jump tables
		switch (PolyFlags & PF_Masked)
		{
			case PF_Masked:
			{
				// Metallicafan212:	Add a new texture to the map
				FD3DTexture* NewTex				= new FD3DTexture();
				ChildMap->LastMasked			= NewTex;
				ChildMap->QWORDMaskedMap[InID]	= NewTex;
				
				return NewTex;

				break;
			}

			default:
			{
				// Metallicafan212:	Add a new texture to the map
				FD3DTexture* NewTex				= new FD3DTexture();
				ChildMap->LastNorm				= NewTex;
				ChildMap->QWORDMap[InID]		= NewTex;

				return NewTex;

				break;
			}
		}
	}

	unguardSlow;
}