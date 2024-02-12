#pragma once
// Metallicafan212:	This defines a manager class for loading pre-compiled HLSL from a file
//					It can also be hard-coded in the future to remove the need of having the shader files entirely

#define SHADER_CACHE_VER 2

class FShaderManager
{
protected:
	// Metallicafan212:	The map used for the binary data
	//					TODO! Might want to use a struct instead for extendability
	TMap<FString, TArray<BYTE>>		Bytecode;

	// Metallicafan212:	The device (if needed)
	class UICBINDx11RenderDevice*	DXDevice;

	UBOOL							bIsInitalized;	

	// Metallicafan212:	The last write time of the cache
	SQWORD							CacheTime;

	// Metallicafan212:	Used when loading the cache to know to disregard the cache
	UBOOL							bCacheInvalid;

	UBOOL							bShouldSaveCache;

public:
	FShaderManager(class UICBINDx11RenderDevice* InDevice);

	// Metallicafan212:	Initialize the map (load from disk and disregard if the GPU, driver changed, or the date on the file doesn't match the binary)
	void Init();

	// Metallicafan212:	Get the requested shader bytes, given the file, function, and language to use
	TArray<BYTE>* GetShaderBytes(FString File, FString Func, const ANSICHAR* Lang, D3D_SHADER_MACRO* Macros = nullptr, DWORD Flags = 0);

	FORCEINLINE UBOOL IsInitialized() {return bIsInitalized;}

	void LoadHardcodedShaders();

	void SaveCache();

	friend FArchive& operator<<(FArchive& Ar, FShaderManager* Manager);
};