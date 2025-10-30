#pragma once

class FTextureCacheMap;

class FTextureCache
{
	// Metallicafan212:	TODO! Think about data organization
public:
	FTextureCache();

	~FTextureCache();

	// Metallicafan212:	Interface definition for the cache

	// Metallicafan212:	Flush all textures out of the cache(s)
	void Flush();

	// Metallicafan212:	Find a specific texture
	FD3DTexture* Find(D3DCacheId InID, PFLAG PolyFlags, ETextureFormat Format);

	// Metallicafan212:	Get a new bind
	FD3DTexture* Set(D3DCacheId InID, PFLAG PolyFlags, ETextureFormat Format);

	// Metallicafan212:	TODO! More interfaces

protected:

	FTextureCacheMap* ChildMap;
};