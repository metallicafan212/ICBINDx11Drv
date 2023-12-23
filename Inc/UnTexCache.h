#pragma once

//#define DWORD_MAP TMap<DWORD, FD3DTexture>
//#define QWORD_MAP TMap<D3DCacheId, FD3DTexture>

// Metallicafan212:	We need to swap to an unordered map for cache consistency issues
#include <unordered_map>

class FTextureCache
{
	// Metallicafan212:	TODO! Think about data organization
public:
	FTextureCache();

	// Metallicafan212:	Interface definition for the cache

	// Metallicafan212:	Flush all textures out of the cache(s)
	void Flush();

	// Metallicafan212:	Find a specific texture
	FD3DTexture* Find(D3DCacheId InID, PFLAG PolyFlags);

	// Metallicafan212:	Get a new bind
	FD3DTexture* Set(D3DCacheId InID, PFLAG PolyFlags);

	// Metallicafan212:	TODO! More interfaces

protected:
	// Metallicafan212:	Our different texture caches
	//					We separate based on masking and DWORD/QWORD
	//					Separation of DWORD and QWORD is for performance reasons, to try and minimize as much cache collisions as possible
	//DWORD_MAP DWORDMap;
	//DWORD_MAP DWORDMaskedMap;
	//QWORD_MAP QWORDMap;
	//QWORD_MAP QWORDMaskedMap;

	std::unordered_map<DWORD, FD3DTexture> DWORDMap;
	std::unordered_map<DWORD, FD3DTexture> DWORDMaskedMap;
	std::unordered_map<QWORD, FD3DTexture> QWORDMap;
	std::unordered_map<QWORD, FD3DTexture> QWORDMaskedMap;
};