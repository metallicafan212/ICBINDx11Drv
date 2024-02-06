#include "ICBINDx11Drv.h"

// Metallicafan212:	Might be too stupid of a name lmao
#define CACHE_FILE SHADER_FOLDER TEXT("Haybale.cache")

EXTERN_C IMAGE_DOS_HEADER __ImageBase;

FShaderManager::FShaderManager(UICBINDx11RenderDevice* InDev)
	: bIsInitalized(0)
	, bShouldSaveCache(0)
	, bCacheInvalid(0)
	, CacheTime(0)
{
	guard(FShaderManager::FShaderManager);

	// Metallicafan212:	Nothing else needed right now, just grab the pointer
	DXDevice = InDev;

	unguard;
}

FArchive& operator<<(FArchive& Ar, FShaderManager* Manager)
{
	guard(FShaderManager::operator<<);

	// Metallicafan212:	Serialize out the version
	INT Ver = SHADER_CACHE_VER;

	Ar << Ver;

	// Metallicafan212:	If the version number doesn't match, ignore it
	if (Ar.IsLoading() && Ver != SHADER_CACHE_VER)
	{
		Manager->bCacheInvalid = 1;
	}

	// Metallicafan212:	Load (or save) the GPU name and driver version
	FString Desc = Manager->DXDevice->GPUDesc;
	Ar << Desc;

	if (Ar.IsLoading() && Desc != Manager->DXDevice->GPUDesc)
	{
		Manager->bCacheInvalid = 1;
	}

	QWORD DriverVer = Manager->DXDevice->GPUDriverVer.QuadPart;

	Ar << DriverVer;

	if (Ar.IsLoading() && DriverVer != Manager->DXDevice->GPUDriverVer.QuadPart)
	{
		Manager->bCacheInvalid = 1;
	}

	// Metallicafan212:	Now save/load the cache itself
	Ar << Manager->Bytecode;

	if (Manager->bCacheInvalid)
	{
		Manager->Bytecode.Empty();
	}

	unguard;
}

void FShaderManager::Init()
{
	guard(FShaderManager::Init);

	// Metallicafan212:	Check if we already initialized
	if (!bIsInitalized)
	{
		bIsInitalized = 1;

		// Metallicafan212:	Load in the file, and see if it needs to be invalidated

		// Metallicafan212:	Find out the modification date on the shader cache, if it's less than this module's date, reload it
		HMODULE CurrModule		= (HMODULE)&__ImageBase;

		TCHAR ModHand[4096]		= TEXT("\0");

		GetModuleFileName(CurrModule, ModHand, sizeof(TCHAR*) * 4096);

		SQWORD ModTime			= GFileManager->GetGlobalTime(ModHand);

		SQWORD CacheFileTime	= GFileManager->GetGlobalTime(CACHE_FILE);

		// Metallicafan212:	Load it
		if (GFileManager->FileSize(CACHE_FILE) >= 0 && CacheFileTime > ModTime)
		{
			FArchive* Ar = GFileManager->CreateFileReader(CACHE_FILE);

			if (Ar != nullptr)
			{
				(*Ar) << this;
			}

			// Metallicafan212: Now the cache should be loaded or in a valid (empty) state
			delete Ar;

			if (!bCacheInvalid)
			{
				CacheTime = CacheFileTime;
			}
			else
			{
				CacheTime = 0;
			}
		}
		else
		{
			// Metallicafan212:	Do nothing
			CacheTime = 0;
		}

		bCacheInvalid = 0;
	}

	unguard;
}

inline void CheckShader(HRESULT hr, ID3D10Blob* error)
{
	if (FAILED(hr))
	{
		// Metallicafan212:	Get the error
		if (error != nullptr)
		{
			SIZE_T ErrorSize = error->GetBufferSize();

			// Metallicafan212:	Now alloc memory and log
			ANSICHAR* ErrorStr = new ANSICHAR[ErrorSize]();

			if (ErrorStr != nullptr)
			{
				appMemcpy(ErrorStr, error->GetBufferPointer(), ErrorSize);

				//appUnwindf(TEXT("D3D11: Error compiling shader. Error is %s"), appFromAnsi(ErrorStr));
				appErrorf(TEXT("DX11: Error compiling shader. Error is %s"), appFromAnsi(ErrorStr));
				delete[] ErrorStr;
			}
			error->Release();
		}
		// Metallicafan212:	Don't do anything else?
		return;
	}
	else
	{
		// Metallicafan212:	Print the warning message out
		if (error != nullptr)
		{
			SIZE_T ErrorSize = error->GetBufferSize();

			// Metallicafan212:	Now alloc memory and log
			ANSICHAR* ErrorStr = new ANSICHAR[ErrorSize]();

			if (ErrorStr != nullptr)
			{
				appMemcpy(ErrorStr, error->GetBufferPointer(), ErrorSize);

				GLog->Logf(TEXT("DX11: Warnings when compiling shader. Error is %s"), appFromAnsi(ErrorStr));
				delete[] ErrorStr;
			}
		}
	}

	if (error != nullptr)
		error->Release();
}

TArray<BYTE>* FShaderManager::GetShaderBytes(FString File, FString Func, const ANSICHAR* Lang, D3D_SHADER_MACRO* Macros, DWORD Flags)
{
	guard(FShaderManager::GetShaderBytes);

	// Metallicafan212:	If we're not initted, do it now
	if (!IsInitialized())
	{
		Init();
	}
	
	TArray<BYTE>* ShaderBytes = nullptr;

	// Metallicafan212:	Compare the cache time to the file time
	SQWORD ShaderTime = GFileManager->GetGlobalTime(*File);

	FString Key = FString::Printf(TEXT("%s:%s"), *File, *Func);

	// Metallicafan212:	On a fresh load, CacheTime should be 0
	if (ShaderTime < CacheTime)
	{
		// Metallicafan212:	See if we find the shader bytecode
		ShaderBytes  = Bytecode.Find(Key);
	}

	if (ShaderBytes == nullptr)
	{
		// Metallicafan212:	Not found, so the shader needs to be read
		ShaderBytes				= &Bytecode.Set(*Key, TArray<BYTE>());
		ID3D10Blob* Error		= nullptr;
		ID3D10Blob* ShaderBuff	= nullptr;

		// Metallicafan212:	Compile
		HRESULT hr				= D3DCompileFromFile(*File, Macros, D3D_CMP_STD_INC, appToAnsi(*Func), Lang, Flags, 0, &ShaderBuff, &Error);

		CheckShader(hr, Error);

		// Metallicafan212:	IDK, do something here?
		DXDevice->ThrowIfFailed(hr);

		// Metallicafan212:	Now copy over
		SIZE_T Size = ShaderBuff->GetBufferSize();

		ShaderBytes->AddZeroed(Size);

		// Metallicafan212:	Now copy it
		appMemcpy(&(*ShaderBytes)(0), ShaderBuff->GetBufferPointer(), Size);

		bShouldSaveCache = 1;

		SAFE_RELEASE(ShaderBuff);
	}

	// Metallicafan212:	Return it out
	return ShaderBytes;

	unguard;
}

void FShaderManager::SaveCache()
{
	guard(FShaderManager::SaveCache);

	if (bShouldSaveCache)
	{
		bShouldSaveCache = 0;

		// Metallicafan212:	Now write it off to disk
		FArchive* Ar = GFileManager->CreateFileWriter(CACHE_FILE);

		if (Ar != nullptr)
		{
			(*Ar) << this;
		}

		// Metallicafan212: Now the cache should be loaded or in a valid (empty) state
		delete Ar;

		CacheTime = GFileManager->GetGlobalTime(CACHE_FILE);
	}

	unguard;
}