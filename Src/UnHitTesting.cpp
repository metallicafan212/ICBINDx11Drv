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

	// Metallicafan212:	If it's a forced pop, copy over the last hit to the buffer, with all parent info
	if (bForce)
	{
		// Metallicafan212:	Copy over the single hit we want + HCoords, to support clicking nothing
		BYTE* Data = m_HitData;

		// Metallicafan212:	Only attempt a copy if we have a hit
		//					This is to prevent clicking outside a texture in the texture browser, and it selecting the first texture
		if(PixelHitInfo.Num())
		{
			// Metallicafan212:	Unwind the hit info, from the top
			TArray<FPixHitInfo*> Parents;

			FPixHitInfo* Top = Info;//&PixelHitInfo(PixelHitInfo.Num() - 1);

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
				Data		+= Parents(i)->HitData.Num();
				m_HitCount	+= Parents(i)->HitData.Num();

				// Metallicafan212:	Also set their priority to 0, since we already put them in the buffer
				Parents(i)->Priority = 0;
			}
		}

		// Metallicafan212:	Move the data pointer
		m_HitData = Data;

		PixelTopIndex = -1;
	}

	SetupPixelHitTest();

	unguard;
}

struct ColorHack
{
	union
	{
		DWORD Int4;
		struct
		{
			BYTE R;
			BYTE G;
			BYTE B;
			BYTE A;
		};
	};
};

struct ColorHackRGBA
{
	union
	{
		DWORD Int4;
		struct
		{
			BYTE B;
			BYTE G;
			BYTE R;
			BYTE A;
		};
	};
};

#define USE_RT_SCREENSHOT 1

#if DX11_UNREAL_227
void UICBINDx11RenderDevice::ReadPixels(FColor* Pixels, UBOOL bGammaCorrectOutput)
#else
void UICBINDx11RenderDevice::ReadPixels(FColor* Pixels)
#endif
{
	// Metallicafan212:	Read the back buffer
	guard(UICBINDx11RenderDevice::ReadPixels);

	// Metallicafan212:	TODO! For HDR, reverse the white balance!

	// Metallicafan212:	Save the gamma mode
	EGammaMode	OldGamma	= (EGammaMode)FrameShaderVars.GammaMode;
	DWORD		FrameFlgs	= FrameShaderVars.FrameFlags;

#if DX11_UNREAL_227
	if (!bGammaCorrectOutput)
#endif
	{
		FrameShaderVars.GammaMode	= GM_None;
		FrameShaderVars.FrameFlags	= ((ActiveHDR) ? FSF_ReverseHDR : 0);

		UpdateGlobalShaderVars();
	}

	// Metallicafan212:	We're going to do something funky, we're going to render the backbuffer back to a render target, and then output that
	//					That way, it doesn't matter what mode the renderer is in, we're rendering to the lowest common denominator, DXGI_FORMAT_R8G8B8A8_UNORM

	// Metallicafan212:	Grab the back buffer texture
	ID3D11Texture2D* Stage = nullptr;

	D3D11_TEXTURE2D_DESC StageDesc;

	StageDesc.Format				= DXGI_FORMAT_R8G8B8A8_UNORM;
	StageDesc.Width					= SizeX;
	StageDesc.Height				= SizeY;
	StageDesc.CPUAccessFlags		= D3D11_CPU_ACCESS_READ;
	StageDesc.MipLevels				= 1;
	StageDesc.ArraySize				= 1;
	StageDesc.SampleDesc.Count		= 1;
	StageDesc.SampleDesc.Quality	= 0;
	StageDesc.Usage					= D3D11_USAGE_STAGING;
	StageDesc.BindFlags				= 0;
	StageDesc.MiscFlags				= 0;
	
	HRESULT hr = m_D3DDevice->CreateTexture2D(&StageDesc, nullptr, &Stage);
	ThrowIfFailed(hr);

	// Metallicafan212:	Temp render target to stretch over!
	StageDesc.Format				= DXGI_FORMAT_R8G8B8A8_UNORM;
	StageDesc.Usage					= D3D11_USAGE_DEFAULT;
	StageDesc.CPUAccessFlags		= 0;
	StageDesc.BindFlags				= D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;

	ID3D11Texture2D* RTStage = nullptr;

	hr = m_D3DDevice->CreateTexture2D(&StageDesc, nullptr, &RTStage);
	ThrowIfFailed(hr);

	// Metallicafan212:	Create a render target
	CD3D11_RENDER_TARGET_VIEW_DESC rtVDesc = CD3D11_RENDER_TARGET_VIEW_DESC();
	rtVDesc.Format				= DXGI_FORMAT_R8G8B8A8_UNORM;
	rtVDesc.ViewDimension		= D3D11_RTV_DIMENSION_TEXTURE2D;
	rtVDesc.Texture2D.MipSlice	= 0;

	ID3D11RenderTargetView* StageRTV = nullptr;
	hr = m_D3DDevice->CreateRenderTargetView(RTStage, &rtVDesc, &StageRTV);

	ThrowIfFailed(hr);

	// Metallicafan212:	Now set and render
	ID3D11RenderTargetView* OldRTV = nullptr;
	ID3D11DepthStencilView* OldDSV = nullptr;

	m_RenderContext->OMGetRenderTargets(1, &OldRTV, &OldDSV);
	m_RenderContext->OMSetRenderTargets(1, &StageRTV, nullptr);

	// Metallicafan212:	Start buffering now
	StartBuffering(BT_ScreenFlash);

	// Metallicafan212:	Order of operations, make sure the alpha rejection is set
	SetBlend(PF_Occlude);

	SetTexture(0, nullptr, 0);

	m_RenderContext->PSSetSamplers(0, 1, &ScreenSamp);
	// Metallicafan212:	Manually setup the vars...
	m_RenderContext->PSSetShaderResources(0, 1, &m_BackBuffSRV);

	SetSceneNode(nullptr);

	FResScaleShader->Bind(m_RenderContext);

	LockVertAndIndexBuffer(6);

	appMemcpy(m_VertexBuff, ScreenVerts, sizeof(FD3DVert) * 6);

	AdvanceVertPos();

	// Metallicafan212:	Draw
	EndBuffering();

	// Metallicafan212:	Fix the shader holding onto the RT texture
	m_RenderContext->PSSetShaderResources(0, 1, &BlankResourceView);

	// Metallicafan212:	Now go back to the previous render target
	m_RenderContext->OMSetRenderTargets(1, &OldRTV, OldDSV);

	// Metallicafan212:	Now stretch over
	m_RenderContext->CopySubresourceRegion(Stage, 0, 0, 0, 0, RTStage, 0, nullptr);

	// Metallicafan212:	Free the temp render target
	StageRTV->Release();
	StageRTV = nullptr;

	RTStage->Release();
	RTStage = nullptr;

	D3D11_MAPPED_SUBRESOURCE Map;
	hr = m_RenderContext->Map(Stage, 0, D3D11_MAP_READ, 0, &Map);

	ThrowIfFailed(hr);

#define CONVERT_PX(H) *P = FColor(H->R, H->G, H->B, 255); 

	BYTE* MappedSrc = (BYTE*)Map.pData;

	//FLOAT MaxImageLevel = 0.0f;//1.0f;
	//FLOAT AvgImgLevel = 0.0f;

	//DOUBLE TempPixelStorage = 0.0;

	FLOAT AvgImgLevel = 0.0f;
	DOUBLE TempPixelStorage = 0.0;
	
	// Metallicafan212:	Copy over the whole image
	for (INT y = 0; y < SizeY; y++)
	{
		// Metallicafan212:	Define the current row of pixels
		INT				Row	= SizeX * y;

		// Metallicafan212:	Calculate this a bit differently
		//ColorHack*		H	= (ColorHack*)(MappedSrc		+ (y * Map.RowPitch));
		ColorHackRGBA*	HR	= (ColorHackRGBA*)(MappedSrc	+ (y * Map.RowPitch));
		//ColorHackHDR*	HDR	= (ColorHackHDR*)(MappedSrc	+ (y * Map.RowPitch));
		FColor* P			= Pixels + Row;

		for (INT x = 0; x < SizeX; x++)
		{
#if USE_RT_SCREENSHOT
			CONVERT_PX(HR);
			HR++;
#else
			// Metallicafan212:	Check if we're in RGBA mode, not BGRA
			if (bForceRGBA)
			{
				CONVERT_PX(HR);
				HR++;
			}
			else if (ActiveHDR)
			{
				/*
				FLOAT R = Clamp(FLOAT(HDR->R), 0.0f, 1.0f);
				FLOAT G = Clamp(FLOAT(HDR->G), 0.0f, 1.0f);
				FLOAT B = Clamp(FLOAT(HDR->B), 0.0f, 1.0f);
				*/
				FLOAT R = FLOAT(HDR->R) / FrameShaderVars.WhiteLevel;//AvgImgLevel, 0.0f, 1.0f);
				FLOAT G = FLOAT(HDR->G) / FrameShaderVars.WhiteLevel;
				FLOAT B = FLOAT(HDR->B) / FrameShaderVars.WhiteLevel;

				if (R < 0.0f || G < 0.0f || B < 0.0f)
				{
					GWarn->Logf(TEXT("Negative pixel!!! x:%d y%d val r:%f g:%f b:%f"), x, y, R, G, B);
				}
				if (R > 1.0f || G > 1.0f || B > 1.0f)
				{
					GWarn->Logf(TEXT("Overbright pixel!!! x:%d y%d val r:%f g:%f b:%f"), x, y, R, G, B);
				}

				// Metallicafan212:	Convert from half to float, and then store
				*P = FColor(R * 255.0f, G * 255.0f, B * 255.0f, 255);
				HDR++;
			}
			else
			{
				CONVERT_PX(H);
				H++;
			}
#endif

			P++;
		}
	}

	m_RenderContext->Unmap(Stage, 0);

	SAFE_RELEASE(Stage);

	// Metallicafan212:	Revert gamma and the white balance
#if DX11_UNREAL_227
	if (!bGammaCorrectOutput)
#endif
	{
		FrameShaderVars.GammaMode	= OldGamma;
		FrameShaderVars.FrameFlags	= FrameFlgs;

		UpdateGlobalShaderVars();
	}


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

// Metallicafan212:	Specifically for Windows 7 and FL 11.0 (and Buggie in particular 😂)
//					Only used when bForceRGBA
struct FPixelIndexRGBA
{
	union
	{
		DWORD Int4;
		struct
		{
			BYTE R, G, B, A;
		};
	};
};

// Metallicafan212:	For detecting the hit after the scene is rendered
//					TODO! Might be quicker to reimplement this entire block as a compute shader that scans the screen itself
void UICBINDx11RenderDevice::DetectPixelHit()
{
	guard(UICBINDx11RenderDevice::DetectPixelHit);

	// Metallicafan212:	Early break
	if (PixelHitInfo.Num() == 0 || Viewport->HitXL == 0 || Viewport->HitYL == 0)
		return;

	TMap<INT, INT> HitAppear;

	INT BiggestHit		= 0;
	INT BiggestHitCount = 0;

	ID3D11Texture2D* ScreenCopy = nullptr;
	//ID3D11Texture2D* Resolved	= nullptr;
	HRESULT hr = S_OK;

	guard(CopyFromRT);

	D3D11_TEXTURE2D_DESC Desc;

	// Metallicafan212:	Simplify the selection code, only offer selection in one format
	Desc.Format				= DXGI_FORMAT_R8G8B8A8_UNORM;//ScreenFormat;
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

	D3D11_BOX Box;

	appMemzero(&Box, sizeof(Box));

	Box.left				= Viewport->HitX * ResolutionScale;
	Box.right				= Box.left + (Viewport->HitXL * ResolutionScale);
	Box.top					= Viewport->HitY * ResolutionScale;
	Box.bottom				= Box.top + (Viewport->HitYL * ResolutionScale);
	Box.back				= 1;
	
	//m_RenderContext->ResolveSubresource(Resolved, 0, RTResource, 0, ScreenFormat);

	// Metallicafan212:	Now copy
	m_RenderContext->CopySubresourceRegion(ScreenCopy, 0, 0, 0, 0, m_SelectionTex, 0, &Box);//m_BackBuffTex, 0, &Box);//Resolved, 0, &Box);

	//SAFE_RELEASE(Resolved);

	//m_RenderContext->Flush();

	// Metallicafan212:	Now release that copy
	//RTResource->Release();

	unguard;

	guard(ReadPixels);

	// Metallicafan212:	Now lock it
	D3D11_MAPPED_SUBRESOURCE Map;
	hr = m_RenderContext->Map(ScreenCopy, 0, D3D11_MAP_READ, 0, &Map);

	ThrowIfFailed(hr);

	// Metallicafan212:	Converted to a DWORD for ease of use
	DWORD* Ptr		= (DWORD*)Map.pData;
	INT LineSize	= Map.RowPitch / sizeof(DWORD);

	DWORD PHitNum	= PixelHitInfo.Num();

	INT HitYL		= Viewport->HitYL * ResolutionScale;
	INT HitXL		= Viewport->HitXL * ResolutionScale;

	// Metallicafan212:	Declare here
	//FPixelIndex		BGRAIndex;
	FPixelIndexRGBA		RGBAIndex;

#if 0
#define TEST_PIXEL(InStruct) \
	/* Metallicafan212:	Bad pixel*/ \
	if(InStruct.A != 255 && InStruct.Int4 != 0) \
		continue; \
	InStruct.A = 0; \
	/* Metallicafan212:	I moved it out by 100 to detect bad hits*/ \
	if(InStruct.Int4 % 100 != 0) \
		continue; \
	InStruct.Int4 /= 100; \
	/* Metallicafan212:	Make sure it's in range, don't want it to be causing crashes*/ \
	if (InStruct.Int4 < PHitNum) \
	{ \
		INT* Val = HitAppear.Find(InStruct.Int4); \
		if (Val == nullptr) \
		{ \
			HitAppear.Set(InStruct.Int4, 0); \
			Val = HitAppear.Find(InStruct.Int4); \
			/* Metallicafan212:	Spurious, but still safe, check if it got hashed*/ \
			if (Val == nullptr) \
				continue; \
		} \
		/* Metallicafan212:	Priority is done when the hit is pushed*/ \
		HitAppear.Set(InStruct.Int4, *Val + PixelHitInfo(InStruct.Int4).Priority); \
	}
#else

#define TEST_PIXEL(InStruct) \
	/* Metallicafan212:	Bad pixel*/ \
	if(InStruct.A != 255 && InStruct.Int4 != 0) \
		continue; \
	InStruct.A = 0; \
	/* Metallicafan212:	I moved it out by 100 to detect bad hits*/ \
	if(InStruct.Int4 % 100 != 0) \
		continue; \
	InStruct.Int4 /= 100; \
	/* Metallicafan212:	Make sure it's in range, don't want it to be causing crashes*/ \
	if (InStruct.Int4 < PHitNum) \
	{ \
		/* Metallicafan212:	Priority is done when the hit is pushed*/ \
		HitAppear.Set(InStruct.Int4, PixelHitInfo(InStruct.Int4).Priority); \
	}

#endif

	for (INT y = 0; y < HitYL; y++)
	{
		DWORD* RealP = Ptr + (y * LineSize);

		for (INT x = 0; x < HitXL; x++)
		{
			// Metallicafan212:	Check what mode the screen is in!
			//if (bForceRGBA)
			{
				RGBAIndex = *(FPixelIndexRGBA*)&RealP[x];

				TEST_PIXEL(RGBAIndex);
			}
			/*
			else
			{
				BGRAIndex = *(FPixelIndex*)&RealP[x];
				TEST_PIXEL(BGRAIndex);
			}
			*/
		}
	}

	// Metallicafan212:	Free the DX resources
	m_RenderContext->Unmap(ScreenCopy, 0);

	ScreenCopy->Release();

	unguard;


	guard(CountHits);
	// Metallicafan212:	Select the biggest hit
	for (TMap<INT, INT>::TIterator HitItt(HitAppear); HitItt; ++HitItt)
	{
		// Metallicafan212:	Prioritize clicking something over clicking the backdrop
		if (BiggestHit == 0 || (BiggestHitCount < HitItt.Value() && HitItt.Key() != 0)) //|| (BiggestHitCount == HitItt.Value() && BiggestHit < HitItt.Key()))
		{
			BiggestHit		= HitItt.Key();
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
			Data		+= Parents(i)->HitData.Num();
			m_HitCount	+= Parents(i)->HitData.Num();
		}
	}
	else
	{
		m_HitCount = 0;
	}

	// Metallicafan212:	Clean out hit data structures
	PixelTopIndex = -1;

	PixelHitInfo.Empty();
	
	// Metallicafan212:	Update the pointer position
	m_HitData = Data;

	unguard;


	unguard;
}

// Metallicafan212:	For setting up the shit for pixel hitting
void UICBINDx11RenderDevice::SetupPixelHitTest()
{
	guard(UICBINDx11RenderDevice::SetupPixelHitTest);

	// Metallicafan212:	Do pixel selection through the normal shaders
	DWORD Index = (PixelTopIndex >= 0 ? PixelTopIndex : 0);

	// Metallicafan212:	Convert implicitly to BGRA, as that's what will be written to the screen (D3DFMT_ARGB)
	//FPixelIndex			Temp;
	FPixelIndexRGBA		RGBTemp;

	// Metallicafan212:	Simplify the selection code by only using a RGBA render target
	//if (bForceRGBA)
	{
		// Metallicafan212:	To prevent any kind of banding/averaging, all of them will have to be % 100 == 0 in order to be a real click
		RGBTemp.Int4 = Index * 100;

		// Metallicafan212:	Set the global color
		CurrentHitColor = FPlane(RGBTemp.R / 255.0f, RGBTemp.G / 255.0f, RGBTemp.B / 255.0f, 1.0f);
	}
	/*
	else
	{
		// Metallicafan212:	To prevent any kind of banding/averaging, all of them will have to be % 100 == 0 in order to be a real click
		Temp.Int4 = Index * 100;

		// Metallicafan212:	Set the global color
		CurrentHitColor = FPlane(Temp.R / 255.0f, Temp.G / 255.0f, Temp.B / 255.0f, 1.0f);
	}
	*/

	unguard;
}