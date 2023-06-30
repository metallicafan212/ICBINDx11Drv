#include "ICBINDx11Drv.h"

// Metallicafan212:	Functions relating to testing mouse clicks on the render target
//					I've coped and modified all the hit testing stuff from my version of the DX9 driver
void UICBINDx11RenderDevice::PushHit(const BYTE* Data, INT Count)
{
	guard(UICBINDx11RenderDevice::PushHit);

	// Metallicafan212:	End rendering now
	EndBuffering();

	// Metallicafan212:	Copy it
	FPixHitInfo Info;

	// Metallicafan212:	Detect what it is
	HHitProxy* Hit = (HHitProxy*)Data;

	// Metallicafan212:	Ask it for the priority, brushes and gizmos have higher priority
#if DX11_HP2
	Info.Priority = Hit->GetPriority();
#else
	Info.Priority = 1;
#endif

	if (PixelTopIndex != -1)
	{
		Info.Prev = PixelTopIndex;
	}
	else
	{
		Info.Prev = -1;
	}

	Info.Index = PixelHitInfo.Num();

	// Metallicafan212:	Keep this hit info in the hit
	Info.HitData.Add(Count);
	appMemcpy(&Info.HitData(0), Data, Count);

	// Metallicafan212:	Now push it onto our local stack
	PixelHitInfo.AddItem(Info);

	PixelTopIndex = Info.Index;

	SetupPixelHitTest();

	unguard;
}

void UICBINDx11RenderDevice::PopHit(INT Count, UBOOL bForce)
{
	guard(UICBINDx11RenderDevice::PopHit);

	EndBuffering();

	FPixHitInfo* Info = &PixelHitInfo(PixelTopIndex);

	if (Info->Prev != -1)
	{
		PixelTopIndex = Info->Prev;
	}
	else
	{
		PixelTopIndex = -1;
	}

	SetupPixelHitTest();

	unguard;
}

void UICBINDx11RenderDevice::ReadPixels(FColor* Pixels)
{
	// Metallicafan212:	Read the back buffer
	guard(UICBINDx11RenderDevice::ReadPixels);

	ID3D11Texture2D* Resolved	= nullptr;
	ID3D11Texture2D* Stage		= nullptr;
	HRESULT hr = S_OK;

	guard(CopyFromRT);
	// Metallicafan212:	Get a copy of the render target!
	//					We have to copy the whole damn thing when MSAA is enabled!!!!!!!
	//					Because of that, we need to resolve to a temp texture....
	D3D11_TEXTURE2D_DESC ResolveDesc;

	ResolveDesc.Format				= DXGI_FORMAT_B8G8R8A8_UNORM;
	ResolveDesc.Width				= ScaledSizeX;
	ResolveDesc.Height				= ScaledSizeY;
	ResolveDesc.CPUAccessFlags		= D3D11_CPU_ACCESS_READ;
	ResolveDesc.MipLevels			= 1;
	ResolveDesc.ArraySize			= 1;
	ResolveDesc.SampleDesc.Count	= 1;
	ResolveDesc.SampleDesc.Quality	= 0;
	ResolveDesc.Usage				= D3D11_USAGE_DEFAULT;
	ResolveDesc.BindFlags			= 0;
	ResolveDesc.MiscFlags			= 0;

	hr = m_D3DDevice->CreateTexture2D(&ResolveDesc, nullptr, &Resolved);

	ThrowIfFailed(hr);

	// Metallicafan212:	We need a CPU mem copy
	ResolveDesc.Usage				= D3D11_USAGE_STAGING;
	hr = m_D3DDevice->CreateTexture2D(&ResolveDesc, nullptr, &Stage);
	ThrowIfFailed(hr);

	// Metallicafan212:	Now get the screen resource
	ID3D11Resource* RTResource = nullptr;

	m_D3DScreenRTV->GetResource(&RTResource);

	m_D3DDeviceContext->ResolveSubresource(Resolved, 0, RTResource, 0, DXGI_FORMAT_B8G8R8A8_UNORM);

	// Metallicafan212:	Now release that copy
	RTResource->Release();

	// Metallicafan212:	Copy to a staging texture...
	m_D3DDeviceContext->CopySubresourceRegion(Stage, 0, 0, 0, 0, Resolved, 0, nullptr);

	SAFE_RELEASE(Resolved);

	unguard;

	guard(ReadPixels);

	// Metallicafan212:	TODO! Make a new texture and map it to a compute shader to do filtering!!!!
	//					For now, just memcpy if the resolution scale is 1.0
	if (ResolutionScale == 1.0f)
	{
		struct ColorHack
		{
			union
			{
				DWORD P;
				struct
				{
					BYTE R;
					BYTE G;
					BYTE B;
					BYTE A;
				};
			};
		};

		D3D11_MAPPED_SUBRESOURCE Map;
		hr = m_D3DDeviceContext->Map(Stage, 0, D3D11_MAP_READ, 0, &Map);

		ThrowIfFailed(hr);

		//appMemcpy(Pixels, )
		for (INT y = 0; y < ScaledSizeY; y++)
		{
			INT Row			= ScaledSizeX * y;
			ColorHack*	H	= ((ColorHack*)Map.pData) + Row;
			FColor*		P	= Pixels + Row;

			for (INT x = 0; x < ScaledSizeX; x++)
			{
#if DX11_HP2
				P->Int4 = H->P;
#else
				*P = FColor(H->R, H->G, H->B, 255);
#endif
				P++;
				H++;
			}
		}

		m_D3DDeviceContext->Unmap(Stage, 0);

		SAFE_RELEASE(Stage);
	}
	else
	{
		// Metallicafan212:	Do nothing right now, we need a compute shader
	}

	unguard;

	unguard;
}

struct FPixelIndex
{
	union
	{
		DWORD Int4;
		struct
		{
			BYTE B, G, R, A;
		};
	};
};

// Metallicafan212:	For detecting the hit after the scene is rendered
//					TODO! Might be quicker to reimplement this entire block as a compute shader that scans the screen itself
void UICBINDx11RenderDevice::DetectPixelHit()
{
	guard(UICBINDx11RenderDevice::DetectPixelHit);

	// Metallicafan212:	Early break
	if (PixelHitInfo.Num() == 0)
		return;

	TMap<INT, INT> HitAppear;

	INT BiggestHit		= 0;
	INT BiggestHitCount = 0;

	ID3D11Texture2D* ScreenCopy = nullptr;
	ID3D11Texture2D* Resolved	= nullptr;
	HRESULT hr = S_OK;

	guard(CopyFromRT);
	// Metallicafan212:	Get a copy of the render target!
	//					We have to copy the whole damn thing when MSAA is enabled!!!!!!!
	//					Because of that, we need to resolve to a temp texture....
	D3D11_TEXTURE2D_DESC ResolveDesc;

	ResolveDesc.Format				= DXGI_FORMAT_B8G8R8A8_UNORM;
	ResolveDesc.Width				= ScaledSizeX;
	ResolveDesc.Height				= ScaledSizeY;
	ResolveDesc.CPUAccessFlags		= D3D11_CPU_ACCESS_READ;
	ResolveDesc.MipLevels			= 1;
	ResolveDesc.ArraySize			= 1;
	ResolveDesc.SampleDesc.Count	= 1;
	ResolveDesc.SampleDesc.Quality	= 0;
	ResolveDesc.Usage				= D3D11_USAGE_DEFAULT;
	ResolveDesc.BindFlags			= 0;//D3D11_BIND_SHADER_RESOURCE;
	ResolveDesc.MiscFlags			= 0;

	hr = m_D3DDevice->CreateTexture2D(&ResolveDesc, nullptr, &Resolved);

	ThrowIfFailed(hr);

	D3D11_TEXTURE2D_DESC Desc;

	Desc.Format				= DXGI_FORMAT_B8G8R8A8_UNORM;
	Desc.Width				= Viewport->HitXL * ResolutionScale;
	Desc.Height				= Viewport->HitYL * ResolutionScale;
	Desc.CPUAccessFlags		= D3D11_CPU_ACCESS_READ;
	Desc.MipLevels			= 1;
	Desc.ArraySize			= 1;
	Desc.SampleDesc.Count	= 1;
	Desc.SampleDesc.Quality = 0;
	Desc.Usage				= D3D11_USAGE_STAGING;
	Desc.BindFlags			= 0;
	Desc.MiscFlags			= 0;

	hr						= m_D3DDevice->CreateTexture2D(&Desc, nullptr, &ScreenCopy);

	ThrowIfFailed(hr);

	// Metallicafan212:	Now get the screen resource
	ID3D11Resource* RTResource = nullptr;

	// Metallicafan212:	Make sure it's bound!!!!
	ID3D11RenderTargetView* test = nullptr;
	m_D3DDeviceContext->OMGetRenderTargets(1, &test, nullptr);

	// Metallicafan212:	If it's not the current RT, we need to return
	if (test != m_D3DScreenRTV)
	{
		if (test != nullptr)
			test->Release();
		return;
	}

	// Metallicafan212:	This needs to be reset!!!
	SAFE_RELEASE(test);

	m_D3DScreenRTV->GetResource(&RTResource);

	D3D11_BOX Box;

	appMemzero(&Box, sizeof(Box));

	Box.left				= Viewport->HitX * ResolutionScale;
	Box.right				= Box.left + (Viewport->HitXL * ResolutionScale);
	Box.top					= Viewport->HitY * ResolutionScale;
	Box.bottom				= Box.top + (Viewport->HitYL * ResolutionScale);
	Box.back				= 1;
	
	m_D3DDeviceContext->ResolveSubresource(Resolved, 0, RTResource, 0, DXGI_FORMAT_B8G8R8A8_UNORM);

	// Metallicafan212:	Now copy
	m_D3DDeviceContext->CopySubresourceRegion(ScreenCopy, 0, 0, 0, 0, Resolved, 0, &Box);

	SAFE_RELEASE(Resolved);

	//m_D3DDeviceContext->Flush();

	// Metallicafan212:	Now release that copy
	RTResource->Release();

	unguard;

	guard(ReadPixels);

	// Metallicafan212:	Now lock it
	D3D11_MAPPED_SUBRESOURCE Map;
	hr = m_D3DDeviceContext->Map(ScreenCopy, 0, D3D11_MAP_READ, 0, &Map);

	ThrowIfFailed(hr);

	// Metallicafan212:	Converted to a DWORD for ease of use
	DWORD* Ptr		= (DWORD*)Map.pData;
	INT LineSize	= Map.RowPitch / sizeof(DWORD);

	INT PHitNum = PixelHitInfo.Num();

	INT HitYL = Viewport->HitYL * ResolutionScale;//Viewport->HitY + Viewport->HitYL;
	INT HitXL = Viewport->HitXL * ResolutionScale;//Viewport->HitX + Viewport->HitXL;

	for (INT y = 0; y < HitYL; y++)//Viewport->HitY; y < HitYL; y++)
	{
		DWORD* RealP = Ptr + (y * LineSize);

		for (INT x = 0; x < HitXL; x++)//Viewport->HitY; x < HitXL; x++)
		{
			DWORD Pix = RealP[x];

			// Metallicafan212:	Convert back
			FPixelIndex Temp;

			Temp.Int4 = Pix;

			// Metallicafan212:	Bad pixel
			if (Temp.A != 255 && Temp.Int4 != 0)
				continue;

			Temp.A = 0;

			// Metallicafan212:	I moved it out by 100 to detect bad hits
			if (Temp.Int4 % 100 != 0)
				continue;

			Temp.Int4 /= 100;

			// Metallicafan212:	Make sure it's in range, don't want it to be causing crashes
			if (Temp.Int4 < PHitNum)
			{
				INT* Val = HitAppear.Find(Temp.Int4);

				if (Val == nullptr)
				{
					HitAppear.Set(Temp.Int4, 0);
					Val = HitAppear.Find(Temp.Int4);

					// Metallicafan212:	Spurious, but still safe, check if it got hashed
					if (Val == nullptr)
						continue;
				}

				// Metallicafan212:	Priority is done when the hit is pushed
				HitAppear.Set(Temp.Int4, *Val + PixelHitInfo(Temp.Int4).Priority);
			}
		}
	}

	// Metallicafan212:	Free the DX resources
	m_D3DDeviceContext->Unmap(ScreenCopy, 0);

	ScreenCopy->Release();

	unguard;


	guard(CountHits);
	// Metallicafan212:	Select the biggest hit
	for (TMap<INT, INT>::TIterator HitItt(HitAppear); HitItt; ++HitItt)
	{
		// Metallicafan212:	Prioritize clicking something over clicking the backdrop
		if (BiggestHit == 0 || (BiggestHitCount < HitItt.Value() && HitItt.Key() != 0))
		{
			BiggestHit = HitItt.Key();
			BiggestHitCount = HitItt.Value();
		}
	}

	unguard;


	guard(CopyData);

	// Metallicafan212:	Copy over the single hit we want + HCoords, to support clicking nothing
	BYTE* Data = m_HitData;

	// Metallicafan212:	Only attempt a copy if we have a hit
	//					This is to prevent clicking outside a texture in the texture browser, and it selecting the first texture
	if (BiggestHit != -1)
	{
		// Metallicafan212:	Unwind the hit info, from the top
		TArray<FPixHitInfo*> Parents;

		FPixHitInfo* Top = &PixelHitInfo(BiggestHit);

		// Metallicafan212:	Go until we hit the topmost parent
		while (Top != nullptr)
		{
			Parents.Insert(0);
			Parents(0) = Top;

			// Metallicafan212:	Break out if the parent is invalid
			if (Top->Prev == -1)
				Top = nullptr;
			else
				Top = &PixelHitInfo(Top->Prev);
		}

		// Metallicafan212:	Copy in order to preserve heiarchy
		for (INT i = 0; i < Parents.Num(); i++)
		{
			appMemcpy(Data, &(Parents(i)->HitData(0)), Parents(i)->HitData.Num());
			Data += Parents(i)->HitData.Num();
			m_HitCount += Parents(i)->HitData.Num();
		}
	}

	unguard;


	unguard;
}

// Metallicafan212:	For setting up the shit for pixel hitting
void UICBINDx11RenderDevice::SetupPixelHitTest()
{
	guard(UICBINDx11RenderDevice::SetupPixelHitTest);

	// Metallicafan212:	Do pixel selection through the normal shaders

	DWORD Index = PixelHitInfo.Num();

	// Metallicafan212:	Fix for my stupidity
	if (Index)
		Index -= 1;

	// Metallicafan212:	Convert implicitly to BGRA, as that's what will be written to the screen (D3DFMT_ARGB)
	FPixelIndex Temp;

	// Metallicafan212:	To prevent any kind of banding/averaging, all of them will have to be % 100 == 0 in order to be a real click
	Temp.Int4 = Index * 100;

	// Metallicafan212:	Set the global color
	CurrentHitColor = FPlane(Temp.R / 255.0f, Temp.G / 255.0f, Temp.B / 255.0f, 1.0f);

	unguard;
}