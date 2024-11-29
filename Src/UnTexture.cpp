#include "ICBINDx11Drv.h"

FORCEINLINE FMipmap* GetBaseMip(const FTextureInfo& Info)
{
	// Metallicafan212:	Get the 0th mip of this texture
	if (Info.NumMips == 0)
		return nullptr;

#if DX11_UT_469
	UBOOL Compressed = FIsCompressedFormat(Info.Format);
	return Info.Texture != nullptr ? (Compressed ? &Info.Texture->CompMips(Info.LOD) : &Info.Texture->Mips(Info.LOD)) : nullptr;
#elif DX11_UNREAL_227
	return Info.Texture ? &Info.Texture->Mips(0) : nullptr;
#else
	return Info.Mips[0];
#endif
}

// Metallicafan212:	Texturing related functions (since there's going to be quite a bit)
void UICBINDx11RenderDevice::SetTexture(INT TexNum, const FTextureInfo* Info, PFLAG PolyFlags, UBOOL bNoAF)
{
	guardSlow(UICBINDx11RenderDevice::SetTexture);

	FD3DBoundTex& TX = BoundTextures[TexNum];

	DWORD SlotFlag = (1 << TexNum);

	// Metallicafan212:	Support null textures
	if (Info == nullptr)
	{
		// Metallicafan212:	Only end buffering if the slot wasn't null before!!!
		if (TX.TexInfoHash != 0)
		{
			EndBuffering();
		}
		// Metallicafan212:	Check if the slot is still "sticky"
		//					Somehow in my code, this is becoming a problem.....
		else if (TX.m_SRV == nullptr && !(BoundTexturesInfo.CurrentBoundTextures & SlotFlag))//!= nullptr)
		{
			// Metallicafan212:	It's already been null-d out
			return;
		}

		TX.bIsRT		= 0;
		TX.UMult		= 1.0f;
		TX.VMult		= 1.0f;
		TX.TexInfoHash	= 0;
		TX.m_SRV		= nullptr;//BlankResourceView;
		TX.Flags		= 0;

		m_RenderContext->PSSetShaderResources(TexNum, 0, nullptr);//1, &BlankResourceView);
		m_RenderContext->PSSetSamplers(TexNum, 0, nullptr);//1, &BlankSampler);

		bWriteTexturesBuffer = 1;

		// Metallicafan212:	Unbind the texture
		BoundTexturesInfo.CurrentBoundTextures &= ~SlotFlag;

		return;
	}

	// Metallicafan212:	TODO! HP2 specific (old hack!!!!)
#if DX11_HP2
	if (Info->Texture != nullptr && Info->Texture->bAlphaToCoverage)
		PolyFlags |= PF_AlphaToCoverage;
#endif

	// Metallicafan212:	Adjust the cache ID to fix masking issues (per the DX9 driver)
	//					Like said before, this was disabled because it slots ALL textures into the first bucket
	QWORD CacheID = Info->CacheID;

	// Metallicafan212:	End buffering if the input texture doesn't match!!!
	UBOOL bSetTex = 0;

	// Metallicafan212:	We have to be able to draw without a texture, so we do have to not ignore hash 0
	if ((TX.TexInfoHash != Info->CacheID))
	{
		bSetTex = 1;
		EndBuffering();
	}

	// Metallicafan212:	Search for the bind
	FD3DTexture* DaTex		= TextureMap.Find(Info->CacheID, PolyFlags);//TextureMap.Find(Info->CacheID);//CacheHash);

	// Metallicafan212:	Using Info->NeedsRealtimeUpdate steals 50fps for some reason.... It's incredibly weird
//#if DX11_UT_469 
//	UBOOL bTexChanged = DaTex == nullptr || Info->NeedsRealtimeUpdate(DaTex->RealtimeChangeCount);
//#elif|| DX11_HP2
#if DX11_UT_469 || DX11_HP2
	UBOOL bTexChanged = (Info->Texture != nullptr && DaTex != nullptr ? Info->Texture->RealtimeChangeCount != DaTex->RealtimeChangeCount : Info->bRealtimeChanged);
#elif DX11_UNREAL_227
	UBOOL bTexChanged = (Info->Texture != nullptr && DaTex != nullptr ? Info->RenderTag != DaTex->RealtimeChangeCount : Info->bRealtimeChanged);
#endif

	// Metallicafan212:	Check if we need to upload it to the GPU
	//					Masked textures get placed in a separate map, so they can be toggled on and off without a refresh
	//					This uses more memory, but results in more consistent FPS and behavior
	UBOOL bUpload			= DaTex == nullptr || bTexChanged;

	if(bUpload)
	{
		DaTex = CacheTextureInfo(*Info, PolyFlags);
	}

	// Metallicafan212:	Save the UV clamp state here
	PolyFlags |= (DaTex->bShouldUVClamp ? PF_ClampUVs : 0);

	// Metallicafan212:	Calculate the new values
	FLOAT NewUMult = 1.0f / (Info->UScale * Info->USize);
	FLOAT NewVMult = 1.0f / (Info->VScale * Info->VSize);

	// Metallicafan212:	Fix some lightmap related issues, check UV mult and UV pan
	if (bSetTex || NewUMult != TX.UMult || NewVMult != TX.VMult || TX.UPan != Info->Pan.X || TX.VPan != Info->Pan.Y)
	{
		bSetTex = 1;

		EndBuffering();

		TX.TexInfoHash	= Info->CacheID;
		TX.UPan			= Info->Pan.X;
		TX.VPan			= Info->Pan.Y;
		TX.bIsRT		= DaTex->bIsRT;
		TX.UScale		= Info->UScale;
		TX.VScale		= Info->VScale;
		TX.UMult		= NewUMult;
		TX.VMult		= NewVMult;

		// Metallicafan212:	Get the size from the base mip!!!!
		FMipmap* M = GetBaseMip(*Info);

		// Metallicafan212:	Only try to UV clamp if the base mip is accessable
		if (M != nullptr && (((DaTex->UClamp ^ M->USize) | (DaTex->VClamp ^ M->VSize)) != 0))
		{
			DaTex->bShouldUVClamp = 1;
		}
		else
		{
			DaTex->bShouldUVClamp = 0;
		}
	}


#if DX11_HP2
	// Metallicafan212:	Check if it's a RT texture!!
	//					Really should use IsA, but this is quicker
	if (Info->Texture != nullptr && Info->Texture->GetClass() == UDX11RenderTargetTexture::StaticClass())
	{
		UDX11RenderTargetTexture* TexTemp = (UDX11RenderTargetTexture*)Info->Texture;

		m_RenderContext->PSSetShaderResources(TexNum, 1, TexTemp->RTSRView.GetAddressOf());

		ID3D11SamplerState* Temp = GetSamplerState((PolyFlags) | (DaTex->bShouldUVClamp ? PF_ClampUVs : 0), DaTex->MipSkip, bNoAF);//DaTex->bSkipMipZero ? 1 : 0, 0);

		m_RenderContext->PSSetSamplers(TexNum, 1, &Temp);
		//m_RenderContext->PSSetSamplers(TexNum, 1, &BlankSampler);

		// Metallicafan212:	So we can find whatever is still bound as the RT when we call OMSetRenderTargets
		TX.m_SRV = TexTemp->RTSRView.Get();

		bWriteTexturesBuffer = 1;

		return;
	}
#endif

	// Metallicafan212:	Only actually set the slot if we need to
	//					2024, check only the RELEVANT polyflags. We don't care (here) if blending changed
	if (bSetTex || TX.m_SRV == nullptr || ((TX.Flags & (PF_NoSmooth | PF_ClampUVs)) != (PolyFlags & (PF_NoSmooth | PF_ClampUVs))))
	{
		TX.m_SRV = DaTex->m_View;

		m_RenderContext->PSSetShaderResources(TexNum, 1, &DaTex->m_View);

		ID3D11SamplerState* Temp = GetSamplerState(PolyFlags, DaTex->MipSkip, bNoAF);

		m_RenderContext->PSSetSamplers(TexNum, 1, &Temp);

		// Metallicafan212:	Tell the shader that the texture is bound
		BoundTexturesInfo.CurrentBoundTextures |= SlotFlag;

		TX.Flags = PolyFlags;

		bWriteTexturesBuffer = 1;
	}

	unguardSlow;
}

FORCEINLINE UBOOL GetMipInfo(const FTextureInfo& Info, FD3DTexture* Tex, BYTE* ConversionMem, INT MipNum, BYTE*& DataPtr, INT& Size, INT& SourcePitch, INT& MipW, INT& MipH)
{
	// Metallicafan212:	In HP2, compression is fixed and LOD works as expected
#if DX11_HP2
	FMipmap* Mip = Info.Mips[MipNum];

	if (Mip == nullptr)
	{
		return 0;
	}

	if (Mip->DataPtr == nullptr)
	{
		Mip->DataArray.Load();
		Mip->DataPtr = static_cast<BYTE*>(Mip->DataArray.GetData());
	}
	DataPtr = Mip->DataPtr;
	Size    = Mip->DataArray.Num();

	// Metallicafan212:	Provide out the mip's size
	MipW	= Mip->USize;
	MipH	= Mip->VSize;

	SourcePitch = Tex->D3DTexType->GetPitch(Mip->USize);

	if (Size <= 0)
	{
		Size = SourcePitch * Mip->VSize;
	}

#elif DX11_UT_469
	// Metallicafan212:	Add on the texture LOD setting
	if (Info.Texture != nullptr)
		MipNum += Info.LOD;

	UBOOL Compressed	= FIsCompressedFormat(Info.Format);
	FMipmap* Mip		= Info.Texture ? (Compressed ? &Info.Texture->CompMips(MipNum) : &Info.Texture->Mips(MipNum)) : nullptr;
	if (Mip != nullptr)
	{
		// Metallicafan212:	See if it needs to be decompressed!
		if (Tex->bDecompress)
		{
			Mip->LoadMip();
			TArray<BYTE> Decomp = UTexture::DecompressMip(Info.Format, Mip, TEXF_RGBA8_);//TEXF_BGRA8);

			if (Decomp.Num())
			{
				appMemcpy(ConversionMem, &Decomp(0), Decomp.Num());

				DataPtr			= ConversionMem;
				Size			= Decomp.Num();
				SourcePitch		= FTextureBlockBytes(TEXF_BGRA8) * FTextureBlockAlignedWidth(TEXF_BGRA8, Mip->USize) / FTextureBlockWidth(TEXF_BGRA8);
			}
		}
		else
		{
			Mip->LoadMip();
			DataPtr		= Mip->DataPtr;
			Size		= Mip->DataArray.Num();
			SourcePitch = FTextureBlockBytes(Info.Format) * FTextureBlockAlignedWidth(Info.Format, Mip->USize) / FTextureBlockWidth(Info.Format);
		}

		// Metallicafan212:	Provide out the mip's size
		MipW	= Mip->USize;
		MipH	= Mip->VSize;
	}	
	else
	{	
		// probably a light or fog map	
		FMipmapBase* MipBase = Info.Mips[MipNum];
		DataPtr		= MipBase->DataPtr;
		Size		= FTextureBytes(Info.Format, MipBase->USize, MipBase->VSize);
		SourcePitch = FTextureBlockBytes(Info.Format) * FTextureBlockAlignedWidth(Info.Format, MipBase->USize) / FTextureBlockWidth(Info.Format);

		// Metallicafan212:	Provide out the mip's size
		MipW	= MipBase->USize;
		MipH	= MipBase->VSize;
	}
#elif DX11_UNREAL_227
	const_cast<FTextureInfo&>(Info).Load();

	// non-pow2 compressed textures
	FMipmap* Mip = Info.Texture ? &Info.Texture->Mips(MipNum) : nullptr;
	if (Tex->bDecompress)
	{
		TArray<BYTE> Decomp = UTexture::DecompressMip(static_cast<ETextureFormat>(Info.Format), Mip, TEXF_RGBA8_);//TEXF_BGRA8);

		if (Decomp.Num())
		{
			appMemcpy(ConversionMem, &Decomp(0), Decomp.Num());

			DataPtr = ConversionMem;
			Size = Decomp.Num();
			SourcePitch = FTextureBlockBytes(TEXF_BGRA8) * FTextureBlockAlignedWidth(TEXF_BGRA8, Mip->USize) / FTextureBlockWidth(TEXF_BGRA8);
		}
	}
	else
	{
		FMipmapBase* MipBase = Info.Mips[MipNum];
		DataPtr = MipBase->DataPtr;
		Size = FTextureBytes(Info.Format, MipBase->USize, MipBase->VSize);

		// Metallicafan212:	Provide out the mip's size
		MipW = MipBase->USize;
		MipH = MipBase->VSize;

		SourcePitch = FTextureBlockBytes(Info.Format) * FTextureBlockAlignedWidth(Info.Format, MipBase->USize) / FTextureBlockWidth(Info.Format);
	}
#else
//#error "Implement Me!"
	// Metallicafan212:	TODO! Very basic implementation
	FMipmap* Mip = reinterpret_cast<FMipmap*>(Info.Mips[MipNum]);

	if (Mip == nullptr)
	{
		return 0;
	}

	if (Mip->DataPtr == nullptr)
	{
		Mip->DataArray.Load();
		Mip->DataPtr = static_cast<BYTE*>(Mip->DataArray.GetData());
	}
	DataPtr = Mip->DataPtr;
	Size    = Mip->DataArray.Num();

	// Metallicafan212:	Provide out the mip's size
	MipW	= Mip->USize;
	MipH	= Mip->VSize;

	SourcePitch = Tex->D3DTexType->GetPitch(Mip->USize);

	if (Size <= 0)
	{
		Size = SourcePitch * Mip->VSize;
	}
#endif

	return TRUE;
}

#if DX11_UT_469
void UICBINDx11RenderDevice::UpdateTextureRect(FTextureInfo& Info, INT U, INT V, INT UL, INT VL)
{
	guard(UICBINDx11RenderDevice::UpdateTextureRect);

	// Metallicafan212:	This is HEAVILY TODO!!!!!

	// Metallicafan212:	This is used by the lightmap atlas textures
	QWORD CacheID = Info.CacheID;

	// Metallicafan212:	TODO! Create the texture
	//DWORD CacheHash = GetCacheHash(CacheID);
	FD3DTexture* DaTex = TextureMap.Find(CacheID, 0);//TextureMap.Find(CacheHash);

	// Metallicafan212:	Haven't seen it before?
	if (DaTex == nullptr)
	{
		// Metallicafan212:	Cache the info
		DaTex = CacheTextureInfo(Info, 0);
	}

	/*
	// Metallicafan212:	All of this is just copied from the full function
	DaTex->Tex			= Info.Texture;

	// Metallicafan212:	Cache it now!
	DaTex->UClamp		= Info.UClamp;
	DaTex->VClamp		= Info.VClamp;
	DaTex->NumMips		= Info.NumMips;
	DaTex->USize		= Info.USize;
	DaTex->VSize		= Info.VSize;
	*/

	/*
	if (m_FeatureLevel != D3D_FEATURE_LEVEL_11_1)
	{
		INT MinSize = Info.Format == TEXF_BC1 || (Info.Format >= TEXF_BC2 && Info.Format <= TEXF_BC6H) ? 4 : 0;
		DaTex->USize	= Clamp(DaTex->USize, MinSize, D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION);
		DaTex->VSize	= Clamp(DaTex->VSize, MinSize, D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION);
	}
	*/

	// Metallicafan212:	Ok, copy JUST the data we need
	//					Reimplemented from the GL driver
	INT DataBlock		= FTextureBlockBytes(Info.Format);
	INT DataSize		= FTextureBytes(Info.Format, UL, VL);
	BYTE* Data			= (BYTE*)appMalloc(DataSize, TEXT("TempTexMem"));//new BYTE(DataSize);

	// Metallicafan212:	Get the pitch, so we can copy JUST the blocks we need
	INT Pitch			= DaTex->D3DTexType->GetPitch(Info.USize);
	INT DPitch			= DaTex->D3DTexType->GetPitch(UL);

	INT xEnd = U + UL;
	INT yEnd = V + VL;

	for (INT y = V, yD = 0; y < yEnd; y++, yD++)
	{
		// Metallicafan212:	Get the aligned pointer
		BYTE* Dst = Data + (DPitch * yD);
		BYTE* Src = Info.Mips[0]->DataPtr + (Pitch * y);

		appMemcpy(Dst, Src, sizeof(BYTE) * UL);
	}
	
	// Metallicafan212:	Do something cursed as a test...
	BYTE* OldMip			= Info.Mips[0]->DataPtr;
	Info.Mips[0]->DataPtr	= Data;

	
	//DaTex->CacheID		= CacheID;

	// Metallicafan212:	Partially upload the texture
	//					TODO! The loop isn't needed
	//for (INT i = 0; i < Info.NumMips; i++)
	//{
	(this->*DaTex->D3DTexType->TexUploadFunc)(Info, DaTex, 0, 1, U, V, UL, VL);
	//}

	Info.Mips[0]->DataPtr = OldMip;

	//Mark.Pop();

	//delete Data;
	appFree(Data);

	//DaTex->RealtimeChangedCount = Info.RealtimeChangeCount;
	Info.bRealtimeChanged = 0;

	unguard;
}
#endif

FD3DTexture* UICBINDx11RenderDevice::CacheTextureInfo(const FTextureInfo& Info, PFLAG PolyFlags, UBOOL bJustSampler)
{
	guardSlow(UICBINDx11RenderDevice::CacheTextureInfo)

	HRESULT hr			= S_OK;

	QWORD CacheID		= Info.CacheID;

	// Metallicafan212:	Find the cached texture
	FD3DTexture* DaTex	= TextureMap.Find(CacheID, PolyFlags);

	FD3DTexType* Type	= DaTex != nullptr ? DaTex->D3DTexType : nullptr;

	if (DaTex == nullptr)
	{
		DaTex = TextureMap.Set(CacheID, PolyFlags);

		if (DaTex == nullptr)
		{
			// Metallicafan212:	Fail here?
			appErrorf(TEXT("Texture map returned nullptr"));
		}

		// Metallicafan212:	Find the type now
#if !USE_UNODERED_MAP_EVERYWHERE
		FD3DTexType* Type = SupportedTextures.Find(Info.Format);
#else
		auto f = SupportedTextures.find(static_cast<ETextureFormat>(Info.Format));

		Type = f != SupportedTextures.end() ? &f->second : nullptr;
#endif
		if (Type == nullptr)
		{
			appErrorf(TEXT("Metallicafan212 you idiot, you forgot to add a descriptor for %u"), Info.Format);
		}

		// Metallicafan212:	Test here if we need to conver it to a different format
		if (Type->bIsCompressed)
		{
			// Metallicafan212:	Check if it's not pow2, and if so, if the U and V sizes are different
			UBOOL bNeedsConversion = (__popcnt(Info.USize) != 1 || __popcnt(Info.VSize) != 1) && Info.USize != Info.VSize;

			if (bNeedsConversion)
			{
				// Metallicafan212:	New destination will be RGBA8
#if DX11_HP2 || DX11_UT_469 || DX11_UNREAL_227
				Type = &SupportedTextures.find(TEXF_BGRA8)->second;
#else
				Type = &SupportedTextures.find(TEXF_RGBA8)->second;
#endif

				DaTex->bDecompress = 1;
			}
		}

		DaTex->D3DTexType = Type;

	}

	// Metallicafan212:	Get the base mip
	FMipmap* M			= GetBaseMip(Info);

	DaTex->Tex			= Info.Texture;

	// Metallicafan212:	Cache it now!
	DaTex->UClamp		= Info.UClamp;
	DaTex->VClamp		= Info.VClamp;
	DaTex->NumMips		= Info.NumMips;
	DaTex->PolyFlags	= PolyFlags;

	// Metallicafan212:	Fix a crash relating to RT Textures
	if (M != nullptr)
	{
		DaTex->USize	= M->USize;
		DaTex->VSize	= M->VSize;
	}
	else
	{
		DaTex->USize	= Info.USize;
		DaTex->VSize	= Info.VSize;
	}

	DaTex->MipSkip = 0;

	// Metallicafan212:	Implement checking against the new UT469 realtime changed count
	//					Using the function in Info causes a 50fps loss for some reason... It makes no sense....
#if DX11_UT_469 || DX11_HP2
	DaTex->RealtimeChangeCount = Info.Texture != nullptr ? Info.Texture->RealtimeChangeCount : 0;
#elif DX11_UNREAL_227
	DaTex->RealtimeChangeCount = Info.Texture != nullptr ? Info.RenderTag : 0;
#else
	DaTex->RealtimeChangeCount = 0;
#endif

	DaTex->CacheID		= CacheID;

	// Metallicafan212:	More texture information
#if DX11_HP2
	DaTex->MaskedColor			= Info.MaskedColor.Plane();
	DaTex->MaskedGranularity	= Info.GranularityColor.Plane();

	// Metallicafan212:	If this is a RT texture, we need to not do any of this!!!!
	//					I know I should be using IsA, but this is quicker (for now)
	if (Info.Texture != nullptr && Info.Texture->GetClass() == UDX11RenderTargetTexture::StaticClass())
	{
		DaTex->bIsRT		= 1;
#if DX11_UT_469 || DX11_HP2
		DaTex->Format		= TEXF_BGRA8;
#else
		DaTex->Format		= TEXF_RGBA8;
#endif
		DaTex->TexFormat	= DXGI_FORMAT_B8G8R8A8_UNORM;

		return DaTex;
	}
#endif

	INT MinSize = Type->bIsCompressed ? 4 : 0;

	// Metallicafan212:	If we're trying to fix a texture, we need to do an extra mip...
	if (Type->bIsCompressed && (DaTex->USize < MinSize || DaTex->VSize < MinSize))
	{
		//DaTex->bSkipMipZero = 1;
		DaTex->MipSkip = 1;
	}

	if (DaTex->MipSkip)
	{
		// Metallicafan212:	Increase the size....
		//					Exploit powers of 2
	SHIFT_TEX:
		DaTex->USize	<<= 1;
		DaTex->VSize	<<= 1;

		// Metallicafan212:	Do it as many times as needed
		if (DaTex->USize < MinSize || DaTex->VSize < MinSize)
		{
			DaTex->MipSkip++;
			goto SHIFT_TEX;
		}
	}
	
	// Metallicafan212:	Clamp between the min and max texture size
	DaTex->USize = Clamp(DaTex->USize, MinSize, D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION);
	DaTex->VSize = Clamp(DaTex->VSize, MinSize, D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION);

	BYTE* MipData			= nullptr;
	INT MipSize = 0, MipPitch = 0;
	INT MipW	= 0, MipH = 0;

	// Metallicafan212:	Create the conversion mem
	//					TODO! Hardcoded size!!!!
	SIZE_T MemRequest = DaTex->USize * DaTex->VSize * 4;

	// Metallicafan212:	Resize as needed
	if (MemRequest > ConversionMemSize)
	{
		if (ConversionMemory == nullptr)
		{
			ConversionMemory = (BYTE*)appMalloc(MemRequest, TEXT("DX11ConversionMem"));
		}
		else
		{
			ConversionMemory = (BYTE*)appRealloc(ConversionMemory, MemRequest, TEXT("DX11ConversionMem"));
		}
		ConversionMemSize = MemRequest;
	}

	// Metallicafan212:	Check if we need to make it
	if (DaTex->m_Tex == nullptr)
	{
		// Metallicafan212:	Check for the info
		DaTex->Format		= Type->Format;//Info.Format;
		DaTex->TexFormat	= Type->DXFormat;

		// Metallicafan212:	Create the texture
		D3D11_TEXTURE2D_DESC Desc;
		appMemzero(&Desc, sizeof(Desc));

		Desc.Format				= Type->DXFormat;
		Desc.Width				= DaTex->USize;
		Desc.Height				= DaTex->VSize;
		Desc.MipLevels			= Info.NumMips + DaTex->MipSkip;
		Desc.Usage				= D3D11_USAGE_DEFAULT;
		Desc.BindFlags			= D3D11_BIND_SHADER_RESOURCE; 
		Desc.ArraySize			= 1;
		Desc.CPUAccessFlags		= 0;
		Desc.SampleDesc.Count	= 1;
		
		hr = m_D3DDevice->CreateTexture2D(&Desc, nullptr, &DaTex->m_Tex);

		ThrowIfFailed(hr);

		// Metallicafan212:	Create the view
		D3D11_SHADER_RESOURCE_VIEW_DESC vDesc;
		appMemzero(&vDesc, sizeof(vDesc));

		vDesc.ViewDimension				= D3D11_SRV_DIMENSION_TEXTURE2D;
		vDesc.Texture2D.MipLevels		= -1;
		vDesc.Texture2D.MostDetailedMip = DaTex->MipSkip;
		vDesc.BufferEx.FirstElement		= 0;
		vDesc.Format					= DaTex->TexFormat;

		hr = m_D3DDevice->CreateShaderResourceView(DaTex->m_Tex, &vDesc, &DaTex->m_View);

		ThrowIfFailed(hr);

		// Metallicafan212:	Check if the texture needs UV clamping
		if (((DaTex->UClamp ^ DaTex->USize) | (DaTex->VClamp ^ DaTex->VSize)) != 0)
		{
			DaTex->bShouldUVClamp = 1;
		}
		else
		{
			DaTex->bShouldUVClamp = 0;
		}

		// Metallicafan212:	Now process it
		if (!Type->bSupported)
		{
			// Metallicafan212:	We need to convert each mip!!!! TODO!
			goto ConvertTexture;
		}
		else
		{
			goto CopyTexture;
		}
	}
	else
	{
		// Metallicafan212:	Texture update!
		if (Type->bSupported)
		{
		CopyTexture:
			guardSlow(DirectTexCopy);
			// Metallicafan212:	We need to copy each mip

			for (INT i = 0; i < Info.NumMips; i++)
			{	
				(this->*Type->TexUploadFunc)(Info, DaTex, i, 0, 0, 0, 0, 0);
			}

			unguardSlow;
		}
		else
		{
		ConvertTexture:	
			guardSlow(ConvertTexture);

			// Metallicafan212:	Convert the mip (could be P8 or RGBA7)
			UBOOL bMaskedHack	= (Type->Format == TEXF_P8 && PolyFlags & PF_Masked);

			// Metallicafan212:	TODO! Mask hack it!!
			FColor OldMasked	= (Info.Palette != nullptr ? Info.Palette[0] : FColor(0, 0, 0, 0));

			if (bMaskedHack && Info.Palette != nullptr)
				Info.Palette[0] = FColor(0, 0, 0, 0);

			for (INT i = 0; i < Info.NumMips; i++)
			{
				(this->*Type->TexUploadFunc)(Info, DaTex, i, 0, 0, 0, 0, 0);
			}

			// Metallicafan212:	Restore
			if(Info.Palette != nullptr)
				Info.Palette[0] = OldMasked;

			unguardSlow;
		}
	}

	// Metallicafan212:	Just to prevent another find call
	return DaTex;

	unguardSlow;
}

// Metallicafan212:	Function to register supported texture types
//					The device will query first to see what's supported, while some hand-fed types will be provided
void UICBINDx11RenderDevice::RegisterTextureFormat(ETextureFormat Format, DXGI_FORMAT DXFormat, UBOOL bRequiresConversion, UBOOL bIsCompressed, INT ByteOrBlockSize, FD3DTexType::GetTypePitch PitchFunc, UploadFunc UFunc)//, FD3DTexType::ConversionFunc UConv)
{
	guard(UICBINDx11RenderDevice::RegisterTextureFormat);

	// Metallicafan212:	Init the format description
	FD3DTexType Type;

	Type.Format				= Format;
	Type.DXFormat			= DXFormat;
	Type.bSupported			= !bRequiresConversion;
	Type.bIsCompressed		= bIsCompressed;
	Type.TexUploadFunc		= UFunc;
	//Type.TexConvFunc		= UConv;
	Type.GetTexturePitch	= PitchFunc;
	Type.BytesPerPixel		= ByteOrBlockSize;
	Type.BlockSize			= ByteOrBlockSize;

#if !USE_UNODERED_MAP_EVERYWHERE
	SupportedTextures.Set(Format, Type);
#else
	SupportedTextures[Format] = Type;
#endif

	unguard;
}

void UICBINDx11RenderDevice::DirectCP(const FTextureInfo& Info, FD3DTexture* Tex, INT Mip, UBOOL bPartial, INT UpdateX, INT UpdateY, INT UpdateW, INT UpdateH)
{
	guardSlow(MemcpyTexUpload);

	// Metallicafan212:	Calculate pitch
	BYTE* Data	= nullptr;
	INT Size	= 0;
	INT Pitch	= 0;
	INT W		= 0;
	INT	H		= 0;

	// Metallicafan212:	TODO! Do this more optimized!
	if (GetMipInfo(Info, Tex, ConversionMemory, Mip, Data, Size, Pitch, W, H))
	{
		if (!bPartial)
		{
			m_RenderContext->UpdateSubresource(Tex->m_Tex, Mip + Tex->MipSkip, nullptr, Data, Pitch, 0);
		}
		else
		{
			// Metallicafan212:	Calc the pitch
			Pitch = Tex->D3DTexType->GetPitch(UpdateW);

			// Metallicafan212:	Now upload it using a box
			D3D11_BOX Upd	= CD3D11_BOX();

			Upd.left		= UpdateX;
			Upd.right		= UpdateX + UpdateW;
			Upd.top			= UpdateY;
			Upd.bottom		= UpdateY + UpdateH;

			m_RenderContext->UpdateSubresource(Tex->m_Tex, Mip + Tex->MipSkip, &Upd, Data, Pitch, 0);
		}
	}

	unguardSlow;
}

void UICBINDx11RenderDevice::RGBA7To8(const FTextureInfo& Info, FD3DTexture* Tex, INT Mip, UBOOL bPartial, INT UpdateX, INT UpdateY, INT UpdateW, INT UpdateH)
{
	guardSlow(UICBINDx11RenderDevice::RGBA7To8);

	BYTE* Data	= nullptr;
	INT Size	= 0;
	INT Pitch	= 0;
	INT W		= 0;
	INT	H		= 0;

	// Metallicafan212:	TODO! Do this more optimized!
	if (GetMipInfo(Info, Tex, ConversionMemory, Mip, Data, Size, Pitch, W, H))
	{
		// Metallicafan212:	TODO! Partial uploads
		if (bPartial)
		{
			// Metallicafan212:	Get the update pitch
			Pitch = UpdateW * 4;//Tex->D3DTexType->GetPitch(UpdateW);

			Size = Pitch * UpdateH;

			DWORD* pTex = (DWORD*)ConversionMemory;

			// Metallicafan212:	Get each color
			INT   Read		= 0;
			BYTE* Bytes		= Data;
			BYTE* DBytes	= ConversionMemory;
			
			// Metallicafan212:	TODO! Does it need clamping?
			while (Read < Size)
			{
				DWORD* Addr			= (DWORD*)&Bytes[Read];
				(*(DWORD*)DBytes)	= (*Addr) * 2;
					
				Read	+= 4;
				DBytes	+= 4;
			}

			// Metallicafan212:	Now update
			D3D11_BOX Upd = CD3D11_BOX();

			Upd.left		= UpdateX;
			Upd.right		= UpdateX + UpdateW;
			Upd.top			= UpdateY;
			Upd.bottom		= UpdateY + UpdateH;

			m_RenderContext->UpdateSubresource(Tex->m_Tex, Mip + Tex->MipSkip, &Upd, ConversionMemory, Pitch, 0);
		}
		else
		{
			DWORD* pTex = (DWORD*)ConversionMemory;

			// Metallicafan212:	Get each color
			INT	  Read		= 0;
			BYTE* Bytes		= Data;
			BYTE* DBytes	= ConversionMemory;

			// Metallicafan212:	Sigh.. We have to clamp!
			//					Took my recoded RGBA7 upload from the DX9 driver
			if (Tex->UClamp != Tex->USize || Tex->VClamp != Tex->VSize)
			{
				INT		RUSize = W - 1;
				INT		RVSize = H - 1;
				INT		UClamp = (Tex->UClamp >> Mip) - 1;
				INT		VClamp = (Tex->VClamp >> Mip) - 1;

				// Metallicafan212:	Loop and get the color
				for (INT y = 0; y < H; y++)
				{
					// Metallicafan212:	Current pointer
					DWORD* Base = ((DWORD*)Data) + Min<DWORD>(y & RVSize, VClamp) * W;

					// Metallicafan212:	Now the X direction
					for (INT x = 0; x < W; x++)
					{
						// Metallicafan212:	Move it over
						//					We multiply by 2 to expand 7 bits to 8
						*pTex++ = (Base[Min<DWORD>(x & RUSize, UClamp)]) << 1;//* 2;

						// Metallicafan212:	P8 is forcibly converted to ARGB, so we don't need to respect pitch if we already know 32bpp
						//pTex++;
					}
				}
			}
			else
			{
				while (Read < Size)
				{
					DWORD* Addr			= (DWORD*)&Bytes[Read];
					(*(DWORD*)DBytes)	= (*Addr) << 1;//* 2;

					Read	+= 4;
					DBytes	+= 4;
				}
			}

			// Metallicafan212:	Now update
			m_RenderContext->UpdateSubresource(Tex->m_Tex, Mip + Tex->MipSkip, nullptr, ConversionMemory, Pitch, 0);
		}
	}

	unguardSlow;
}

void UICBINDx11RenderDevice::P8ToRGBA(const FTextureInfo& Info, FD3DTexture* Tex, INT Mip, UBOOL bPartial, INT UpdateX, INT UpdateY, INT UpdateW, INT UpdateH)
{
	guardSlow(P8ToRGBA);

	BYTE* Data	= nullptr;
	INT Size	= 0;
	INT Pitch	= 0;
	INT W		= 0;
	INT	H		= 0;

	// Metallicafan212:	TODO! Partial uploads!

	// Metallicafan212:	TODO! Do this more optimized!
	if (GetMipInfo(Info, Tex, ConversionMemory, Mip, Data, Size, Pitch, W, H))
	{
		// Metallicafan212:	Update each 4 byte block
		SIZE_T	Read	= 0;
		BYTE* Bytes		= (BYTE*)Data;
		DWORD* DBytes	= (DWORD*)ConversionMemory;

		// Metallicafan212:	Just read across
		//					There might be a quicker way to do this, but this was the easiest to write lmao
		for (INT i = 0; i < Size; i++)
		{
			(*DBytes) = GET_COLOR_DWORD(Info.Palette[*Bytes]);
			Bytes++;
			DBytes++;
		}


		// Metallicafan212:	Now update
		m_RenderContext->UpdateSubresource(Tex->m_Tex, Mip + Tex->MipSkip, nullptr, ConversionMemory, W * 4, 0);
	}

	unguardSlow;
}

// Metallicafan212:	Based on the DX9 version, but HEAVILY modified
void UICBINDx11RenderDevice::SetBlend(PFLAG PolyFlags)
{
	guardSlow(UICBINDx11RenderDevice::SetBlend);

	ADJUST_PFLAGS(PolyFlags);

	// Metallicafan212:	Check if the input blend flags are relevant
#if DX11_HP2
	PFLAG blendFlags = PolyFlags & (PF_Translucent | PF_Modulated | PF_Invisible | PF_Occlude | PF_Masked | PF_ColorMask | PF_Highlighted | PF_RenderFog | PF_LumosAffected | PF_AlphaBlend | PF_AlphaToCoverage | PF_Opacity | PF_NoFog | PF_Selected | PF_Memorized);
#else
	PFLAG blendFlags = PolyFlags & (PF_Translucent | PF_Modulated | PF_Invisible | PF_Occlude | PF_Masked | PF_Highlighted | PF_AlphaBlend | PF_Selected | PF_Memorized);
#endif

	UBOOL bUpdatePFlagBuff	= 0;
	UBOOL bUpdateFogBuff	= 0;

	if (blendFlags != CurrentPolyFlags)
	{
		// Metallicafan212:	We need to render since our flags changed
		EndBuffering();

		// Metallicafan212:	Check for changes
		PFLAG Xor = CurrentPolyFlags ^ blendFlags;

		// Metallicafan212:	Check for changes to the selection flag
		if (GIsEditor && Xor & PF_Selected)
		{
			GlobalPolyflagVars.bSelected	= (blendFlags & PF_Selected);// == PF_Selected;
			bUpdatePFlagBuff				= 1;
		}

		// Metallicafan212:	Save the blend flags now
		CurrentPolyFlags = blendFlags;

		// Metallicafan212:	Again, the DX9 driver saves the day
#if DX11_HP2
		const PFLAG RELEVANT_BLEND_FLAGS = PF_Translucent | PF_Modulated | PF_Highlighted | PF_LumosAffected | PF_Invisible | PF_AlphaBlend | PF_AlphaToCoverage | PF_Masked | PF_ColorMask | PF_Opacity | PF_NoFog;
#else
		const PFLAG RELEVANT_BLEND_FLAGS = PF_Translucent | PF_Modulated | PF_Highlighted | PF_Invisible | PF_Masked | PF_AlphaBlend | PF_Invisible;
#endif

		if (Xor & (RELEVANT_BLEND_FLAGS))
		{
			// Metallicafan212:	Only set when it's actually using it to render (in case of bad flags)
			GlobalPolyflagVars.ShaderFlags		&= ~SF_Modulated;

			bUpdatePFlagBuff			= 1;

			ID3D11BlendState* bState	= nullptr;

			// Metallicafan212:	Find and set the render state
			if (!(blendFlags & (RELEVANT_BLEND_FLAGS)))
			{
				bState = GetBlendState(0);
				if (bState == nullptr)
				{
					bState = CreateBlend(0, D3D11_BLEND_ONE, D3D11_BLEND_ZERO, D3D11_COLOR_WRITE_ENABLE_ALL, 0);
				}

				GlobalPolyflagVars.ShaderFlags		&= ~SF_AlphaEnabled;
			}
			else
			{
				GlobalPolyflagVars.ShaderFlags		|= SF_AlphaEnabled;

				// Metallicafan212:	See if it's encountered this blend state before!
				//bState = GetBlendState(blendFlags);

#if !USE_UNODERED_MAP_EVERYWHERE
				bState = BlendCache.FindRef(blendFlags);
#else
				auto f = BlendCache.find(blendFlags);

				bState = f != BlendCache.end() ? f->second : nullptr;
#endif
				if (bState == nullptr)
				{
					// Metallicafan212:	DX9 allows you to completely turn off color drawing. We achieve the same effect here by using a 0 source blend and a 1 dest blend (since it will keep the dst color)
					if (blendFlags & PF_Invisible)
					{
						bState = GetBlendState(PF_Invisible);

						if (bState == nullptr)
						{
							bState = CreateBlend(PF_Invisible, D3D11_BLEND_ZERO, D3D11_BLEND_ONE, D3D11_COLOR_WRITE_ENABLE_ALPHA);
						}
					}
#if DX11_HP2 || DX11_HP1
					else if ((blendFlags & (PF_Translucent | PF_Highlighted)) == (PF_Translucent | PF_Highlighted))
					{
						if (blendFlags & PF_AlphaToCoverage)
						{
							bState = GetBlendState(PF_Translucent | PF_Highlighted | PF_AlphaToCoverage);

							if (bState == nullptr)
							{
								bState = CreateBlend(PF_Opacity, D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_INV_SRC_ALPHA, D3D11_COLOR_WRITE_ENABLE_ALL, 1, 1);
							}
						}
						else
						{
							bState = GetBlendState(PF_Translucent | PF_Highlighted);

							if (bState == nullptr)
							{
								bState = CreateBlend(PF_Opacity, D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_INV_SRC_ALPHA);
							}
						}
					}
#endif
					else if (blendFlags & PF_Translucent)
					{
#if DX11_HP2
						if (blendFlags & PF_AlphaToCoverage)
						{
							bState = GetBlendState(PF_Translucent | PF_AlphaToCoverage);

							if (bState == nullptr)
							{
								bState = CreateBlend(PF_Translucent | PF_AlphaToCoverage, D3D11_BLEND_ONE, D3D11_BLEND_INV_SRC_COLOR, D3D11_COLOR_WRITE_ENABLE_ALL, 1, 1);
							}
						}
						else
#endif
						{
							bState = GetBlendState(PF_Translucent);

							if (bState == nullptr)
							{
								bState = CreateBlend(PF_Translucent, D3D11_BLEND_ONE, D3D11_BLEND_INV_SRC_COLOR);
							}
						}
					}
					else if (blendFlags & PF_Modulated)
					{
						GlobalPolyflagVars.ShaderFlags		|= SF_Modulated;

						bState = GetBlendState(PF_Modulated);

						if (bState == nullptr)
						{
							bState = CreateBlend(PF_Modulated, D3D11_BLEND_DEST_COLOR, D3D11_BLEND_SRC_COLOR);
						}
					}
#if DX11_HP2
					// Metallicafan212:	New engine QWORD lumos
					else if (blendFlags & PF_LumosAffected)
					{
						// Metallicafan212:	I've reversed the reversed lumos alpha, as it seems that INV_SRC_ALPHA in DX11 doesn't write full alpha for some reason...
						//FindAndSetBlend(PF_LumosAffected, D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_INV_SRC_ALPHA);//D3D11_BLEND_INV_SRC_ALPHA, D3D11_BLEND_SRC_ALPHA);
						bState = GetBlendState(PF_LumosAffected);

						if (bState == nullptr)
						{
							bState = CreateBlend(PF_LumosAffected, D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_INV_SRC_ALPHA);
						}
					}
#endif
					else if (blendFlags & PF_Highlighted)
					{
#if DX11_HP2 || DX11_UT_469 || DX11_UNREAL_227
						if (blendFlags & PF_AlphaBlend)
						{
							bState = GetBlendState(PF_Highlighted | PF_AlphaBlend);

							if (bState == nullptr)
							{
								bState = CreateBlend(PF_Highlighted | PF_AlphaBlend, D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_INV_SRC_ALPHA);
							}
						}
						else
#endif
						{
							bState = GetBlendState(PF_Highlighted);

							if (bState == nullptr)
							{
								bState = CreateBlend(PF_Highlighted, D3D11_BLEND_ONE, D3D11_BLEND_INV_SRC_ALPHA);
							}
						}
					}
#if DX11_HP2
					// Metallicafan212:	Alpha to coverage is considered alpha blend, since that's what it's usually used for
					//					TODO! Maybe re-evaluate this????
					else if (blendFlags & PF_AlphaToCoverage)
					{
						//FindAndSetBlend(PF_AlphaBlend | PF_AlphaToCoverage, D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_INV_SRC_ALPHA, D3D11_COLOR_WRITE_ENABLE_ALL, 1, 1);
						bState = GetBlendState(PF_AlphaBlend | PF_AlphaToCoverage);

						if (bState == nullptr)
						{
							bState = CreateBlend(PF_AlphaBlend | PF_AlphaToCoverage, D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_INV_SRC_ALPHA, D3D11_COLOR_WRITE_ENABLE_ALL, 1, 1);
						}
					}
#endif
					// Metallicafan212:	Rewrote this to combine them (as they're all the same end result)
#if DX11_HP2
					else if (blendFlags & (PF_AlphaBlend | PF_ColorMask | PF_Masked))
#elif DX11_UT_469 || DX11_UNREAL_227
					else if (blendFlags & (PF_AlphaBlend | PF_Masked))
#else
					else if (blendFlags & (PF_Masked))
#endif
					{
// Metallicafan212:	TODO! This section SUCKS, recode it so it's not so ifdeffy
#if DX11_HP2
						if ((blendFlags & (PF_AlphaBlend | PF_ColorMask)))
#elif DX11_UT_469 || DX11_UNREAL_227
						if ((blendFlags & (PF_AlphaBlend)))
#else
						if(0)
#endif
						{
							bState = GetBlendState(PF_AlphaBlend);

							if (bState == nullptr)
							{
								bState = CreateBlend(PF_AlphaBlend, D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_INV_SRC_ALPHA);
							}
						}
						else
						{
							bState = GetBlendState(PF_Masked);

							if (bState == nullptr)
							{
								bState = CreateBlend(PF_Masked, D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_INV_SRC_ALPHA, D3D11_COLOR_WRITE_ENABLE_ALL, 1, 1);
							}
						}
					}

					// Metallicafan212:	Cache this blend state
					if (bState != nullptr)
					{
#if !USE_UNODERED_MAP_EVERYWHERE
						BlendCache.Set(blendFlags, bState);
#else
						BlendCache[blendFlags] = bState;
#endif
					}
				}

				// Metallicafan212:	Modulated hack
				if (GetBlendState(PF_Modulated) == bState)
				{
					//GlobalPolyflagVars.bModulated = 1;
					GlobalPolyflagVars.ShaderFlags		|= SF_Modulated;
				}
			}

			// Metallicafan212:	If we got a valid blend state, set it
			if (bState != nullptr)
			{
				m_RenderContext->OMSetBlendState(bState, nullptr, 0xFFFFFFFF);
			}
		}

#if DX11_DISTANCE_FOG
		// Metallicafan212:	Set the correct fake fog values
		//					Reset fog if the XOR was Translucent or Modulated
#if DX11_HP2
		if ((FogShaderVars.bDoDistanceFog || FogShaderVars.bFadeFogValues) && (Xor & (PF_Translucent | PF_Modulated | PF_AlphaBlend | PF_Highlighted | PF_NoFog)))
#elif DX11_UNREAL_227
		if ((FogShaderVars.bDoDistanceFog || FogShaderVars.bFadeFogValues) && (Xor & (PF_Translucent | PF_Modulated | PF_AlphaBlend | PF_Highlighted)))
#endif
		{
			// Metallicafan212:	Don't allow for this to work if we're in a stack...
			//					TODO!!!!!
			//if (!DistanceFogStack.Num())
			{
				PFLAG Flags = (blendFlags & RELEVANT_BLEND_FLAGS);

#if DX11_HP2
				if (Flags & PF_NoFog)
				{
					//PushDistanceFogState();

					//FogHackPopCount++;

					// Metallicafan212:	Disable fogging entirely on this surface/actor
					FogShaderVars.bForceFogOff	= 1;
					bUpdateFogBuff				= 1;
				}
				else
#endif
				{
					FogShaderVars.bForceFogOff = 0;
					// Metallicafan212:	Translucent gets combined with a few other flags to set a specific hack
					//					Sigh.... If only they just added a alpha flag instead of reusing flags, it makes it extremely annoying
					if (Flags & PF_Translucent && !(Flags & (PF_AlphaBlend | PF_Highlighted)))
					{
						//PushDistanceFogState();
						//FogHackPopCount++;
						FogShaderVars.DistanceFogColor	= FogShaderVars.TransFogColor;
						//UpdateFogSettings();
						bUpdateFogBuff					= 1;
					}
					else if (Flags & PF_Modulated)
					{
						//PushDistanceFogState();

						//FogHackPopCount++;

						FogShaderVars.DistanceFogColor	= FogShaderVars.ModFogColor;
						//UpdateFogSettings();
						bUpdateFogBuff					= 1;
					}
					else
					{
						/*
						// Metallicafan212:	TODO! While not likely, another render might've come in and messed with us...
						//					Rethink this approach
						while (FogHackPopCount)
						{
							FogHackPopCount--;
							PopDistanceFogState();
						}

						PopDistanceFogState();
						*/
						//PushDistanceFogState();
						FogShaderVars.DistanceFogColor	= FogShaderVars.DistanceFogFinal;
						//UpdateFogSettings();
						bUpdateFogBuff					= 1;
					}
				}
			}
		}
#endif

		// Metallicafan212:	TODO! Allow the user to specify the alpha reject values
#if DX11_HP2
		if (Xor & (PF_Masked | PF_AlphaBlend | PF_LumosAffected | PF_ColorMask | PF_Invisible | PF_Modulated))
#else
		if (Xor & (PF_Masked | PF_AlphaBlend | PF_Invisible | PF_Modulated))
#endif
		{
			bUpdatePFlagBuff					= 1;
#if DX11_HP2
			if (blendFlags & (PF_Invisible | PF_AlphaBlend | PF_LumosAffected | PF_Modulated))
#elif DX11_UT_469 || DX11_UNREAL_227
			if (blendFlags & (PF_Invisible | PF_AlphaBlend | PF_Modulated))
#else
			if (blendFlags & (PF_Invisible | PF_Modulated))
#endif
			{
				GlobalPolyflagVars.AlphaReject		= 0.01f;//1e-6f;
				GlobalPolyflagVars.AltAlphaReject	= 0.01f;
				//GlobalPolyflagVars.bColorMasked		= 0;
				GlobalPolyflagVars.ShaderFlags		|= SF_AlphaEnabled;//bAlphaEnabled	= 1;

			}
#if DX11_HP2
			else if (blendFlags & (PF_ColorMask | PF_Masked))
#else
			else if (blendFlags & PF_Masked)
#endif
			{
				GlobalPolyflagVars.AlphaReject		= MaskedAlphaReject;//0.8f;
				GlobalPolyflagVars.AltAlphaReject	= (bSmoothHudTiles ? SmoothMaskedAlphaReject : MaskedAlphaReject);
				//GlobalPolyflagVars.bColorMasked		= 1;
				GlobalPolyflagVars.ShaderFlags		|= SF_AlphaEnabled;//bAlphaEnabled	= 1;
			}
			else
			{
				// Metallicafan212:	This was a mistake that made PF_Highlighted not work right
				//					Since it needs to write with 0 alpha, we need alpha testing off
				GlobalPolyflagVars.AlphaReject		= 0.0f;//1e-6f;
				GlobalPolyflagVars.AltAlphaReject	= 0.00f;
				//GlobalPolyflagVars.bColorMasked		= 0;
			}
		}

		// Metallicafan212:	Toggle between the z write and no z write states
		if (Xor & (PF_Occlude | PF_Memorized))
		{
			// Metallicafan212:	This is a hack to prevent selection from doing z writing, but it will still do z testing
			if ((blendFlags & PF_Memorized))
			{
				//m_RenderContext->OMSetDepthStencilState(m_DefaultNoZState, 0);
				//SetZTestMode()
				//bNoZWrite = 1;
				DepthStencilFlags |= DS_NoZWrite;

				// Metallicafan212:	Mark the current ZTest value as invalid
				CurrentZTestIndex = -1;

				SetZTestMode(CurrentZTestMode);
			}
			else if ((blendFlags & PF_Occlude))
			{
				//m_RenderContext->OMSetDepthStencilState(m_DefaultZState, 0);
				//bNoZWrite = 0;
				DepthStencilFlags &= ~DS_NoZWrite;

				// Metallicafan212:	Mark the current ZTest value as invalid
				CurrentZTestIndex = -1;

				SetZTestMode(CurrentZTestMode);
			}
			else
			{
				//m_RenderContext->OMSetDepthStencilState(m_DefaultNoZState, 0);

				//bNoZWrite = 1;
				DepthStencilFlags |= DS_NoZWrite;

				// Metallicafan212:	Mark the current ZTest value as invalid
				CurrentZTestIndex = -1;

				SetZTestMode(CurrentZTestMode);
			}
		}

		// Metallicafan212:	Flags changed, update them in the constant buffer
		if (bUpdatePFlagBuff)
		{
			UpdatePolyflagsVars();
		}

#if DX11_DISTANCE_FOG
		if (bUpdateFogBuff)
		{
			UpdateFogSettings();
		}
#endif
	}

	unguardSlow;
}

#if DX11_UT_469
UBOOL UICBINDx11RenderDevice::SupportsTextureFormat(ETextureFormat Format)
{
	switch (Format)
	{
		case TEXF_P8:
		case TEXF_RGBA7:
		case TEXF_RGB8:
		case TEXF_BGRA8:
			return true;
		case TEXF_BC1:
		case TEXF_BC2:
		case TEXF_BC3:
		case TEXF_BC4:
		case TEXF_BC4_S:
		case TEXF_BC5:
		case TEXF_BC5_S:
		case TEXF_BC6H_S:
		case TEXF_BC6H:
		case TEXF_BC7:
			return SupportsTC;
		default:
			return false;
	}
}
#endif
