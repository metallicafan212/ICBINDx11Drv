#include "ICBINDx11Drv.h"

FMipmap* GetBaseMip(FTextureInfo& Info)
{
	// Metallicafan212:	Get the 0th mip of this texture

	if (Info.NumMips == 0)
		return nullptr;

#if DX11_HP2
	return Info.Mips[0];
#elif DX11_UT_469
	UBOOL Compressed = FIsCompressedFormat(Info.Format);
	return Info.Texture != nullptr ? (Compressed ? &Info.Texture->CompMips(0) : &Info.Texture->Mips(0)) : nullptr;
#endif
}

// Metallicafan212:	Texturing related functions (since there's going to be quite a bit)
void UICBINDx11RenderDevice::SetTexture(INT TexNum, FTextureInfo* Info, FPLAG PolyFlags)
{
	guard(UICBINDx11RenderDevice::SetTexture);

	// Metallicafan212:	Support null textures
	if (Info == nullptr)
	{
		// Metallicafan212:	Only end buffering if the slot wasn't null before!!!
		if (BoundTextures[TexNum].TexInfoHash != 0)
			EndBuffering();

		BoundTextures[TexNum].bIsRT			= 0;
		BoundTextures[TexNum].UMult			= 1.0f;
		BoundTextures[TexNum].VMult			= 1.0f;
		BoundTextures[TexNum].TexInfoHash	= 0;
		BoundTextures[TexNum].m_SRV			= BlankResourceView;

		m_RenderContext->PSSetShaderResources(TexNum, 1, &BlankResourceView);
		m_RenderContext->PSSetSamplers(TexNum, 1, &BlankSampler);

		return;
	}

	// Metallicafan212:	TODO! HP2 specific (old hack!!!!)
#if DX11_HP2
	if (Info->Texture != nullptr && Info->Texture->bAlphaToCoverage)
		PolyFlags |= PF_AlphaToCoverage;
#endif



	/*
	// Metallicafan212:	TILE HACK!!!
	//					Load the texture directly ONLY for font characters!!!
	//					Intel and AMD both sample correctly
	if (bIsNV && (TexNum == 0 && PolyFlags & PF_NoSmooth && Info->NumMips == 1))
	{
		GlobalShaderVars.bNVTileHack = 1;
	}
	else
	{
		GlobalShaderVars.bNVTileHack = 0;
	}
	*/

	// Metallicafan212:	Adjust the cache ID to fix masking issues (per the DX9 driver)
	//					Like said before, this was disabled because it slots ALL textures into the first bucket
	QWORD CacheID = Info->CacheID;

	//if ((CacheID & 0xFF) == 0xE0)
	//{
	//	//Alter texture cache id if masked texture hack is enabled and texture is masked
	//	CacheID |= ((PolyFlags & PF_Masked) ? 1 : 0);
	//}

	// Metallicafan212:	End buffering if the input texture doesn't match!!!
	UBOOL bSetTex = 0;
	//DWORD CacheHash = GetCacheHash(Info->CacheID);
	if ((BoundTextures[TexNum].TexInfoHash != 0 && BoundTextures[TexNum].TexInfoHash != Info->CacheID))//CacheHash))
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
#else
	UBOOL bTexChanged = Info->bRealtimeChanged;
#endif

	// Metallicafan212:	Check if we need to upload it to the GPU
	//					I also check masked as we need to rehack the palette when mask changes
	UBOOL bUpload			= DaTex == nullptr || bTexChanged || ( ((PolyFlags & PF_Masked) ^ (DaTex->PolyFlags & PF_Masked)) == PF_Masked);

	//UBOOL bDoSampUpdate = 0;

	if(bUpload)
	{
		CacheTextureInfo(*Info, PolyFlags);

		// Metallicafan212:	Get the new bind, if it's changed
		//CacheHash = GetCacheHash(Info->CacheID);
		DaTex = TextureMap.Find(Info->CacheID, PolyFlags);//TextureMap.Find(Info->CacheID);//CacheHash);

		// Metallicafan212:	Already did the sample update
		//bDoSampUpdate = 0;
	}
	/*
	else if (DaTex)
	{
		bDoSampUpdate = (PolyFlags & (PF_NoSmooth | PF_ClampUVs)) != (DaTex->PolyFlags & (PF_NoSmooth | PF_ClampUVs));
	}
	*/


	BoundTextures[TexNum].TexInfoHash	= Info->CacheID;//CacheHash;
	BoundTextures[TexNum].UPan			= Info->Pan.X;
	BoundTextures[TexNum].VPan			= Info->Pan.Y;
	BoundTextures[TexNum].bIsRT			= DaTex->bIsRT;
	BoundTextures[TexNum].UScale		= Info->UScale;
	BoundTextures[TexNum].VScale		= Info->VScale;
	BoundTextures[TexNum].UMult			= 1.0f / (Info->UScale * Info->USize);//M->USize);//Info->USize);
	BoundTextures[TexNum].VMult			= 1.0f / (Info->VScale * Info->VSize);//M->VSize);//Info->VSize);

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


#if DX11_HP2
	// Metallicafan212:	Check if it's a RT texture!!
	//					Really should use IsA, but this is quicker
	if (Info->Texture != nullptr && Info->Texture->GetClass() == UDX11RenderTargetTexture::StaticClass())
	{
		UDX11RenderTargetTexture* TexTemp = (UDX11RenderTargetTexture*)Info->Texture;

		/*
		if (BoundRT == TexTemp)
		{
			// Metallicafan212:	Bind NO texture!!!!
			SetTexture(TexNum, nullptr, PolyFlags);
			return;
		}
		*/

		/*
		// Metallicafan212:	If we have MSAA we need to RESOLVE!!!!!
		if (NumAASamples > 1)
		{
			m_RenderContext->ResolveSubresource(TexTemp->NonMSAATex.Get(), 0, TexTemp->RTTex.Get(), 0, DXGI_FORMAT_B8G8R8A8_UNORM);
		}
		*/
		
		m_RenderContext->PSSetShaderResources(TexNum, 1, TexTemp->RTSRView.GetAddressOf());

		ID3D11SamplerState* Temp = GetSamplerState((PolyFlags) | (DaTex->bShouldUVClamp ? PF_ClampUVs : 0), DaTex->bSkipMipZero ? 1 : 0, 0);
		m_RenderContext->PSSetSamplers(TexNum, 1, &Temp);
		//m_RenderContext->PSSetSamplers(TexNum, 1, &BlankSampler);

		// Metallicafan212:	So we can find whatever is still bound as the RT when we call OMSetRenderTargets
		BoundTextures[TexNum].m_SRV = TexTemp->RTSRView.Get();

		return;
	}
#endif

	BoundTextures[TexNum].m_SRV = DaTex->m_View;

	/*
	if (bDoSampUpdate)
	{
		MakeTextureSampler(DaTex, PolyFlags);
	}
	*/

	m_RenderContext->PSSetShaderResources(TexNum, 1, &DaTex->m_View);

	ID3D11SamplerState* Temp = GetSamplerState((PolyFlags) | (DaTex->bShouldUVClamp ? PF_ClampUVs : 0), DaTex->bSkipMipZero ? 1 : 0, 0);
	m_RenderContext->PSSetSamplers(TexNum, 1, &Temp);

	unguard;
}

// Metallicafan212:	TODO! Since I've redone the way samplers are made, this is pretty redundant now
//					The only check is for UV clamp
void UICBINDx11RenderDevice::MakeTextureSampler(FD3DTexture* Bind, FPLAG PolyFlags)
{
	guard(UICBINDx11RenderDevice::MakeTextureSampler);

	if (((Bind->UClamp ^ Bind->USize) | (Bind->VClamp ^ Bind->VSize)) != 0)
	{
		Bind->bShouldUVClamp = 1;
	}
	else
	{
		Bind->bShouldUVClamp = 0;
	}

	unguard;
}

UBOOL GetMipInfo(FTextureInfo& Info, FD3DTexType* Type, INT MipNum, BYTE*& DataPtr, INT& Size, INT& SourcePitch, INT& MipW, INT& MipH)
{
#if DX11_HP2
	FMipmap* Mip = Info.Mips[MipNum];

	Size = 0;
	SourcePitch = 0;

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

	// Metallicafan212:	The compiler hates if we reference the pointer directly, so we have to use a variable....
	//					Doing (Type->*GetTexturePitch)() should work, but MSVC is being a stickler for some reason....
	FD3DTexType::GetPitch P = Type->GetTexturePitch;

	SourcePitch = (Type->*P)(Mip->USize);
#elif DX11_UT_469
	UBOOL Compressed = FIsCompressedFormat(Info.Format);
	FMipmap* Mip = Info.Texture ? (Compressed ? &Info.Texture->CompMips(MipNum) : &Info.Texture->Mips(MipNum)) : nullptr;
	if (Mip)
	{
		Mip->LoadMip();
		DataPtr = Mip->DataPtr;
		Size    = Mip->DataArray.Num();
		SourcePitch = FTextureBlockBytes(Info.Format) * FTextureBlockAlignedWidth(Info.Format, Mip->USize) / FTextureBlockWidth(Info.Format);

		// Metallicafan212:	Provide out the mip's size
		MipW	= Mip->USize;
		MipH	= Mip->VSize;
	}	
	else
	{	
		// probably a light or fog map	
		FMipmapBase* MipBase = Info.Mips[MipNum];
		DataPtr = MipBase->DataPtr;
		Size    = FTextureBytes(Info.Format, MipBase->USize, MipBase->VSize);
		SourcePitch = FTextureBlockBytes(Info.Format) * FTextureBlockAlignedWidth(Info.Format, MipBase->USize) / FTextureBlockWidth(Info.Format);

		// Metallicafan212:	Provide out the mip's size
		MipW	= MipBase->USize;
		MipH	= MipBase->VSize;
	}
#else
#error "Implement Me!"
#endif

	return TRUE;
}

#if DX11_UT_469
void UICBINDx11RenderDevice::UpdateTextureRect(FTextureInfo& Info, INT U, INT V, INT UL, INT VL)
{
	guard(UICBINDx11RenderDevice::UpdateTextureRect);

	// Metallicafan212:	This is used by the lightmap atlas textures
	QWORD CacheID = Info.CacheID;

	// Metallicafan212:	TODO! Create the texture
	//DWORD CacheHash = GetCacheHash(CacheID);
	FD3DTexture* DaTex = TextureMap.Find(CacheID, 0);//TextureMap.Find(CacheHash);

	if (DaTex == nullptr)
	{
		DaTex = TextureMap.Set(CacheID, 0);//&TextureMap.Set(CacheHash, FD3DTexture());

		if (DaTex == nullptr)
		{
			// Metallicafan212:	Fail here?
			appErrorf(TEXT("Texture map returned nullptr"));
		}
	}

	// Metallicafan212:	All of this is just copied from the full function
	DaTex->Tex			= Info.Texture;

	// Metallicafan212:	Cache it now!
	DaTex->UClamp		= Info.UClamp;
	DaTex->VClamp		= Info.VClamp;
	DaTex->NumMips		= Info.NumMips;
	DaTex->USize		= Info.USize;
	DaTex->VSize		= Info.VSize;

	/*
	if (m_FeatureLevel != D3D_FEATURE_LEVEL_11_1)
	{
		INT MinSize = Info.Format == TEXF_BC1 || (Info.Format >= TEXF_BC2 && Info.Format <= TEXF_BC6H) ? 4 : 0;
		DaTex->USize	= Clamp(DaTex->USize, MinSize, D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION);
		DaTex->VSize	= Clamp(DaTex->VSize, MinSize, D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION);
	}
	*/
	
	DaTex->CacheID		= CacheID;

	// Metallicafan212:	TODO! Add in a partial upload function to the texture type support struct!!!

	unguard;
}
#endif

void UICBINDx11RenderDevice::CacheTextureInfo(FTextureInfo& Info, FPLAG PolyFlags, UBOOL bJustSampler)
{
	guard(UICBINDx11RenderDevice::CacheTextureInfo);

	HRESULT hr = S_OK;

	// Metallicafan212:	Adjust the cache ID to fix masking issues (per the DX9 driver)
	//					Disabled for now, as it slots ALL textures into one bucket in the TMap
	//					I just re-init the texture when the flag changes....
	//					Probably not great, but whatever
	QWORD CacheID = Info.CacheID;

	//if ((CacheID & 0xFF) == 0xE0)
	//{
	//	//Alter texture cache id if masked texture hack is enabled and texture is masked
	//	CacheID |= ((PolyFlags & PF_Masked) ? 1 : 0);
	//}

	// Metallicafan212:	TODO! Create the texture
	//DWORD CacheHash = GetCacheHash(CacheID);
	FD3DTexture* DaTex = TextureMap.Find(CacheID, PolyFlags);//TextureMap.Find(CacheHash);

	if (DaTex == nullptr)
	{
		DaTex = TextureMap.Set(CacheID, PolyFlags);//&TextureMap.Set(CacheHash, FD3DTexture());

		if (DaTex == nullptr)
		{
			// Metallicafan212:	Fail here?
			appErrorf(TEXT("Texture map returned nullptr"));
		}
	}

	// Metallicafan212:	Get the base mip
	FMipmap* M = GetBaseMip(Info);

	DaTex->Tex			= Info.Texture;

	// Metallicafan212:	Cache it now!
	DaTex->UClamp		= Info.UClamp;
	DaTex->VClamp		= Info.VClamp;
	DaTex->NumMips		= Info.NumMips;
	DaTex->PolyFlags	= PolyFlags;

	// Metallicafan212:	Fix a crash relating to RT Textures
	if (M != nullptr)
	{
		DaTex->USize	= M->USize;//Info.USize;
		DaTex->VSize	= M->VSize;//Info.VSize;
	}
	else
	{
		DaTex->USize	= Info.USize;
		DaTex->VSize	= Info.VSize;
	}

	DaTex->bSkipMipZero	= 0;

	// Metallicafan212:	Implement checking against the new UT469 realtime changed count
	//					Using the function in Info causes a 50fps loss for some reason... It makes no sense....
#if DX11_UT_469 || DX11_HP2
	DaTex->RealtimeChangeCount = Info.Texture != nullptr ? Info.Texture->RealtimeChangeCount : 0;
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

		return;
	}
#endif

	// Metallicafan212:	Get the format description
	//					DO THIS FROM THE INFO!!!! THE MAPPED VERSION IS NULL
	FD3DTexType* Type = SupportedTextures.Find(Info.Format);

	if (Type == nullptr)
		appErrorf(TEXT("Metallicafan212 you idiot, you forgot to add a descriptor for %d"), DaTex->Format);

	INT MinSize = 0;

	if (1)//m_FeatureLevel < D3D_FEATURE_LEVEL_11_1)
	{
		// Metallicafan212:	Natively embed the check
		MinSize		= Type->bIsCompressed ? 4 : 0;
	}

	// Metallicafan212:	If we're trying to fix a texture, we need to do an extra mip...
	if(Type->bIsCompressed && (DaTex->USize < MinSize || DaTex->VSize < MinSize))
		DaTex->bSkipMipZero = 1;

	if (DaTex->bSkipMipZero)
	{
		// Metallicafan212:	Increase the size....
		//					Exploit powers of 2
		DaTex->USize	<<= 1;
		DaTex->VSize	<<= 1;
		//DaTex->UClamp	<<= 1;
		//DaTex->VClamp	<<= 1;

		// Metallicafan212:	Remove UV clamping...
		//DaTex->bShouldUVClamp = 0;
	}

	DaTex->USize = Clamp(DaTex->USize, MinSize, D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION);
	DaTex->VSize = Clamp(DaTex->VSize, MinSize, D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION);

	FD3DTexType::GetPitch P = Type->GetTexturePitch;
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
		guard(CreateTexture);
		// Metallicafan212:	Check for the info
		DaTex->Format		= Info.Format;
		DaTex->TexFormat	= Type->DXFormat;

		// Metallicafan212:	Create the texture
		D3D11_TEXTURE2D_DESC Desc;
		appMemzero(&Desc, sizeof(Desc));

		Desc.Format				= Type->DXFormat;
		Desc.Width				= DaTex->USize;
		Desc.Height				= DaTex->VSize;
		Desc.MipLevels			= DaTex->bSkipMipZero ? Info.NumMips + 1 : Info.NumMips;
		Desc.Usage				= D3D11_USAGE_DEFAULT;
		Desc.BindFlags			= D3D11_BIND_SHADER_RESOURCE; //| (Info.Format == TEXF_P8 ? D3D11_BIND_UNORDERED_ACCESS : 0);
		Desc.ArraySize			= 1;
		Desc.CPUAccessFlags		= 0;//D3D11_CPU_ACCESS_WRITE;
		Desc.SampleDesc.Count	= 1;

		// Metallicafan212:	TODO! Find some way to force DX11 to directly read the data, we need pitch info though...
		//					This would save us doing it for each mip ourselfs
		//					Might not even save performance though...
		/*
		// Metallicafan212:	Assemble the data for it as well
		D3D11_SUBRESOURCE_DATA Mips[MAX_MIPS];

		appMemzero(Mips, sizeof(Mips));

		if(Type->bSupported)
		{
			// Metallicafan212:	Rather than create then lock, we'll provide it when we create it
			for (INT i = 0; i < Info.NumMips; i++)
			{
				D3D11_SUBRESOURCE_DATA& ThisMip = Mips[i];

				Info.Mips[i]->DataArray.Load();

				ThisMip.pSysMem				= Info.Mips[i]->DataArray.GetData();

				// Metallicafan212:	TODO! Do we need to calcuate this???
				FD3DTexType::GetPitch P		= Type->GetTexturePitch;
				ThisMip.SysMemPitch			= (Type->*P)(Info.Mips[i]->USize);
				ThisMip.SysMemSlicePitch	= 0;
			}
		}
		*/
		
		hr = m_D3DDevice->CreateTexture2D(&Desc, nullptr, &DaTex->m_Tex);

		ThrowIfFailed(hr);

		guard(CreateSRV);

		// Metallicafan212:	Create the view
		D3D11_SHADER_RESOURCE_VIEW_DESC vDesc;
		appMemzero(&vDesc, sizeof(vDesc));

		vDesc.ViewDimension				= D3D11_SRV_DIMENSION_TEXTURE2D;
		vDesc.Texture2D.MipLevels		= -1;//Info.NumMips;
		vDesc.Texture2D.MostDetailedMip = DaTex->bSkipMipZero ? 1 : 0;
		vDesc.BufferEx.FirstElement		= 0;
		vDesc.Format					= DaTex->TexFormat;

		hr = m_D3DDevice->CreateShaderResourceView(DaTex->m_Tex, &vDesc, &DaTex->m_View);

		ThrowIfFailed(hr);

		unguard;

#if P8_COMPUTE_SHADER
		guard(CreateUAVViews);

		if (Info.Format == TEXF_P8)
		{
			// Metallicafan212:	Create the UAV for this mip
			DaTex->UAVMips.Add(Info.NumMips);
			DaTex->UAVBlank.AddZeroed(Info.NumMips);

			CD3D11_UNORDERED_ACCESS_VIEW_DESC UAVDesc = CD3D11_UNORDERED_ACCESS_VIEW_DESC(DaTex->m_Tex, D3D11_UAV_DIMENSION_TEXTURE2D, DXGI_FORMAT_R8G8B8A8_UNORM);
			for (INT i = 0; i < Info.NumMips; i++)
			{
				UAVDesc.Texture2D.MipSlice = i;
				hr = m_D3DDevice->CreateUnorderedAccessView(DaTex->m_Tex, &UAVDesc, &DaTex->UAVMips(i));

				ThrowIfFailed(hr);
			}

			// Metallicafan212:	Now create a texture to hold onto the P8 info when converting it
			//CD3D11_TEXTURE2D_DESC tempDesc = CD3D11_TEXTURE2D_DESC(DXGI_FORMAT_R8_UNORM, Info.USize, Info.VSize, 1U, Info.NumMips);
			Desc.Format				= DXGI_FORMAT_R8_TYPELESS;
			Desc.Width				= DaTex->USize;
			Desc.Height				= DaTex->VSize;
			Desc.MipLevels			= Info.NumMips;
			Desc.Usage				= D3D11_USAGE_DEFAULT;
			Desc.BindFlags			= D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
			Desc.ArraySize			= 1;
			Desc.CPUAccessFlags		= 0;//D3D11_CPU_ACCESS_WRITE;
			Desc.SampleDesc.Count	= 1;

			hr = m_D3DDevice->CreateTexture2D(&Desc, nullptr, &DaTex->P8ConvTex);

			ThrowIfFailed(hr);

			// Metallicafan212:	Now the shader resource view
			D3D11_SHADER_RESOURCE_VIEW_DESC vDesc;
			appMemzero(&vDesc, sizeof(vDesc));

			vDesc.ViewDimension				= D3D11_SRV_DIMENSION_TEXTURE2D;
			vDesc.Texture2D.MipLevels		= Info.NumMips;
			vDesc.Texture2D.MostDetailedMip = 0;
			vDesc.BufferEx.FirstElement		= 0;
			vDesc.Format					= DXGI_FORMAT_R8_UINT;
			hr = m_D3DDevice->CreateShaderResourceView(DaTex->P8ConvTex, &vDesc, &DaTex->P8ConvSRV);

			ThrowIfFailed(hr);
		}

		unguard;
#endif

		MakeTextureSampler(DaTex, PolyFlags);

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

		unguard;
	}
	else
	{
		// Metallicafan212:	Texture update!
		if (Type->bSupported)
		{
		CopyTexture:
			guard(DirectTexCopy);
			// Metallicafan212:	We need to copy each mip
			for (INT i = 0; i < Info.NumMips; i++)
			{	
				INT CurDMip = (DaTex->bSkipMipZero ? i + 1 : i);
				if (GetMipInfo(Info, Type, i, MipData, MipSize, MipPitch, MipW, MipH))
				{
					/*
					// Metallicafan212:	If it's lower than 4, fix it....
					if (Type->bIsCompressed)
					{
						MipW = Clamp(MipW, 4, MipW);
						MipH = Clamp(MipH, 4, MipH);
					}
					*/

					Type->TexUploadFunc(MipData, MipSize, MipPitch, DaTex, this, Info.Mips[i]->USize, Info.Mips[i]->VSize, CurDMip, 0, 0, MipW, MipH);

					/*
					// Metallicafan212:	Handle bad DXT textures
					//					TODO! This STILL doesn't work, as it can't even update lower than 4x4 pixels.....
					if (Type->bIsCompressed && ((MipW == 2 && MipH != 2) || (MipH == 2 && MipW != 2)))
					{
						if (MipW == 2)
						{
							// Metallicafan212:	Stretch to the right
							Type->TexUploadFunc(MipData, MipSize, MipPitch, DaTex, this, Info.Mips[i]->USize, Info.Mips[i]->VSize, i, 2, 0, MipW + 2, MipH);
						}
						else
						{
							// Metallicafan212:	Stretch to the bottom
							Type->TexUploadFunc(MipData, MipSize, MipPitch, DaTex, this, Info.Mips[i]->USize, Info.Mips[i]->VSize, i, 0, 2, MipW, MipH + 2);
						}
					}
					*/
				}
			}

			unguard;
		}
		else
		{
		ConvertTexture:	
			guard(ConvertTexture);

			// Metallicafan212:	Convert the mip (could be P8 or RGBA7)
			UBOOL bMaskedHack	= (Info.Format == TEXF_P8 && PolyFlags & PF_Masked);

			// Metallicafan212:	TODO! Mask hack it!!
			FColor OldMasked	= (Info.Palette != nullptr ? Info.Palette[0] : FColor(0, 0, 0, 0));

			if (bMaskedHack)
				Info.Palette[0] = FColor(0, 0, 0, 0);
			//else if(Info.Palette != nullptr)
			//	Info.Palette[0].A = 255;

#if P8_COMPUTE_SHADER
			// Metallicafan212:	TODO! Move this around to allow the child convert functions to loop
			//					This way, I don't need a specific P8 hack!!!
			if (Info.Format == TEXF_P8)
			{
				for (INT i = 0; i < Info.NumMips; i++)
				{
					MipData		= nullptr;
					MipPitch	= 0;

					if (GetMipInfo(Info, Type, i, MipData, MipSize, MipPitch))
					{
						m_RenderContext->UpdateSubresource(DaTex->P8ConvTex, i, nullptr, MipData, MipPitch, 0);
					}
				}

				INT SavedTexAddress = -1;

				// Metallicafan212:	Figure out if it's set!
				for (INT i = 0; i < ARRAY_COUNT(BoundTextures); i++)
				{
					if (BoundTextures[i].m_SRV == DaTex->m_View)
					{
						SetTexture(i, nullptr, 0);
						SavedTexAddress = i;
						break;
					}
				}


				// Metallicafan212:	Use the shader for this purpose!!!!
				INT X = appCeil(Info.USize / 32.0f);
				INT Y = appCeil(Info.VSize / 32.0f);

				// Metallicafan212:	Update the palette colors
				//					This is probably dangerous to do this way, but it's much much quicker
				for (INT i = 0; i < 256; i++)
				{
					((DWORD*)ConversionMemory)[i] = GET_COLOR_DWORD(Info.Palette[i]);
				}
				//m_RenderContext->UpdateSubresource(PaletteTexture, 0, nullptr, (void*)Info.Palette, 256 * 4, 0);
				m_RenderContext->UpdateSubresource(PaletteTexture, 0, nullptr, ConversionMemory, 256 * 4, 0);

				// Metallicafan212:	Now bind the input
				ID3D11ShaderResourceView* InSRV[2] = { DaTex->P8ConvSRV, PaletteSRV };
				m_RenderContext->CSSetShaderResources(0, 2, InSRV);

				m_RenderContext->CSSetUnorderedAccessViews(0, Info.NumMips, (ID3D11UnorderedAccessView**)DaTex->UAVMips.GetData(), nullptr);

				// Metallicafan212:	Bind the P8 upload shader
				FP8ToRGBAShader->USize	= Info.USize;
				FP8ToRGBAShader->VSize	= Info.VSize;
				FP8ToRGBAShader->Bind();

				// Metallicafan212:	Now execute
				m_RenderContext->Dispatch(X, Y, Info.NumMips);

				// Metallicafan212:	Wait for it to complete...
				m_RenderContext->End(m_D3DQuery);

				// Metallicafan212:	Wait for it
				BOOL bDone = 0;

				while (m_RenderContext->GetData(m_D3DQuery, &bDone, sizeof(BOOL), 0) != S_OK && bDone == 0);

				// Metallicafan212:	Clear the render resources!!!
				constexpr ID3D11ShaderResourceView* SRVTemp[2] = { nullptr, nullptr };
				m_RenderContext->CSSetShaderResources(0, 2, SRVTemp);

				m_RenderContext->CSSetUnorderedAccessViews(0, Info.NumMips, (ID3D11UnorderedAccessView**)DaTex->UAVBlank.GetData(), nullptr);

				Info.bRealtimeChanged = 0;

				if (SavedTexAddress != -1)
				{
					SetTexture(SavedTexAddress, &Info, PolyFlags);
				}
			}
			else
#endif
			{

				for (INT i = 0; i < Info.NumMips; i++)
				{
					if (GetMipInfo(Info, Type, i, MipData, MipSize, MipPitch, MipW, MipH))
					{
						// Metallicafan212:	Sigh.... We need to map dynamic memory to do this!!!!!
						// Metallicafan212:	Do the conversion over
						SIZE_T Size = (MipSize == 0 ? 4 * Info.Mips[i]->USize * Info.Mips[i]->VSize : MipSize);
						checkSlow(Size <= 4 * Info.USize * Info.VSize);

						Type->TexConvFunc(Info.Palette, MipData, Size, (Type->*P)(Info.Mips[i]->USize), DaTex, this, Info.Mips[i]->USize, Info.Mips[i]->VSize, i, 0, 0, MipW, MipH);
					}

				}
			}

			// Metallicafan212:	Restore
			//if (bMaskedHack)
			if(Info.Palette != nullptr)
				Info.Palette[0] = OldMasked;

			unguard;
		}
	}

	unguard;
}

// Metallicafan212:	Function to register supported texture types
//					The device will query first to see what's supported, while some hand-fed types will be provided
void UICBINDx11RenderDevice::RegisterTextureFormat(ETextureFormat Format, DXGI_FORMAT DXFormat, UBOOL bRequiresConversion, UBOOL bIsCompressed, INT ByteOrBlockSize, FD3DTexType::GetPitch PitchFunc, FD3DTexType::UploadFunc UFunc, FD3DTexType::ConversionFunc UConv)
{
	guard(UICBINDx11RenderDevice::RegisterTextureFormat);

	// Metallicafan212:	Init the format description
	FD3DTexType Type;

	Type.Format				= Format;
	Type.DXFormat			= DXFormat;
	Type.bSupported			= !bRequiresConversion;
	Type.bIsCompressed		= bIsCompressed;
	Type.TexUploadFunc		= UFunc;
	Type.TexConvFunc		= UConv;
	Type.GetTexturePitch	= PitchFunc;
	Type.BytesPerPixel		= ByteOrBlockSize;
	Type.BlockSize			= ByteOrBlockSize;

	SupportedTextures.Set(Format, Type);

	unguard;
}

// Metallicafan212:	Texture uploading functions
void MemcpyTexUpload(void* Source, SIZE_T SourceLength, SIZE_T SourcePitch, FD3DTexture* tex, UICBINDx11RenderDevice* inDev, INT USize, INT VSize, INT Mip, INT UpdateX, INT UpdateY, INT UpdateW, INT UpdateH)
{
	guard(MemcpyTexUpload);

	// Metallicafan212:	Just copy over, todo!!!!
	//D3D11_BOX B = { UpdateX, UpdateY, 0, UpdateW, UpdateH, 1 };

	inDev->m_RenderContext->UpdateSubresource(tex->m_Tex, Mip, /*&B*/nullptr, Source, SourcePitch, 0);

	unguard;
}

void RGBA7To8(FColor* Palette, void* Source, SIZE_T SourceLength, SIZE_T SourcePitch, FD3DTexture* tex, UICBINDx11RenderDevice* inDev, INT USize, INT VSize, INT Mip, INT UpdateX, INT UpdateY, INT UpdateW, INT UpdateH)
{
	guard(RGBA7To8);

	DWORD* pTex = (DWORD*)inDev->ConversionMemory;

	// Metallicafan212:	Get each color
	//					TODO! Support masking????
	SIZE_T Read		= 0;
	BYTE* Bytes		= (BYTE*)Source;
	BYTE* DBytes	= (BYTE*)inDev->ConversionMemory;

	// Metallicafan212:	Sigh.. We have to clamp!
	//					Took my recoded RGBA7 upload from the DX9 driver
	if (tex->UClamp != tex->USize || tex->VClamp != tex->VSize)
	{
		INT		RUSize = USize - 1;
		INT		RVSize = VSize - 1;
		INT		UClamp = (tex->UClamp >> Mip) - 1;
		INT		VClamp = (tex->VClamp >> Mip) - 1;

		// Metallicafan212:	Loop and get the color
		for (INT y = 0; y < VSize; y++)
		{
			// Metallicafan212:	Current pointer
			DWORD* Base = ((DWORD*)Source) + Min<DWORD>(y & RVSize, VClamp) * USize;

			// Metallicafan212:	Now the X direction
			for (INT x = 0; x < USize; x++)
			{
				// Metallicafan212:	Move it over
				//					We multiply by 2 to expand 7 bits to 8
				*pTex = (Base[Min<DWORD>(x & RUSize, UClamp)]) * 2;

				// Metallicafan212:	P8 is forcibly converted to ARGB, so we don't need to respect pitch if we already know 32bpp
				pTex++;
			}
		}
	}
	else
	{
		while (Read < SourceLength)
		{
			DWORD* Addr			= (DWORD*)&Bytes[Read];
			(*(DWORD*)DBytes)	= (*Addr) * 2;

			Read	+= 4;
			DBytes	+= 4;
		}
	}

	// Metallicafan212:	Now update
	inDev->m_RenderContext->UpdateSubresource(tex->m_Tex, Mip, nullptr, inDev->ConversionMemory, SourcePitch, 0);
	unguard;
}

void P8ToRGBA(FColor* Palette, void* Source, SIZE_T SourceLength, SIZE_T SourcePitch, FD3DTexture* tex, UICBINDx11RenderDevice* inDev, INT USize, INT VSize, INT Mip, INT UpdateX, INT UpdateY, INT UpdateW, INT UpdateH)
{
	guard(P8ToRGBA);

	// Metallicafan212:	Update each 4 byte block
	SIZE_T	Read	= 0;
	BYTE*	Bytes	= (BYTE*)Source;
	DWORD*	DBytes	= (DWORD*)inDev->ConversionMemory;

	// Metallicafan212:	Just read across
	//					There might be a quicker way to do this, but this was the easiest to write lmao
	for(INT i = 0; i < SourceLength; i++)
	{
#if DX11_HP2
		(*DBytes) = Palette[*Bytes].Int4;
#else
		(*DBytes) = GET_COLOR_DWORD(Palette[*Bytes]);
#endif
		// Metallicafan212:	Fix broken palettes on specific textures in UT!!!
		//if (*Bytes != 0)
		//{
		//	// Metallicafan212:	This forces colors that aren't 0 to be full alpha on P8, while the 0th entry is taken care of above
		//	(*DBytes) |= 0xFF000000;
		//}

		Bytes++;
		DBytes++;
	}


	// Metallicafan212:	Now update
	inDev->m_RenderContext->UpdateSubresource(tex->m_Tex, Mip, nullptr, inDev->ConversionMemory, SourcePitch, 0);

	unguard;
}

// Metallicafan212:	Based on the DX9 version, but HEAVILY modified
void UICBINDx11RenderDevice::SetBlend(FPLAG PolyFlags)
{
	guard(UICBINDx11RenderDevice::SetBlend);

	// Metallicafan212:	Cut it down to only specific flags
	if (!(PolyFlags & (PF_Translucent | PF_Modulated | PF_Highlighted | PF_LumosAffected)))
	{
		PolyFlags |= PF_Occlude;
	}
	else if (PolyFlags & PF_Translucent)
	{
		PolyFlags &= ~(PF_Masked | PF_ColorMask);
	}

	// Metallicafan212:	Pixel based selection requires that we have alpha blending and nothing else
	if (GIsEditor && m_HitData != nullptr)
	{
		PolyFlags = (PolyFlags & ~(PF_Translucent | PF_Modulated | PF_Highlighted | PF_LumosAffected));//| PF_AlphaBlend;
	}

	// Metallicafan212:	Check if the input blend flags are relevant
#if DX11_HP2
	FPLAG blendFlags = PolyFlags & (PF_Translucent | PF_Modulated | PF_Invisible | PF_Occlude | PF_Masked | PF_ColorMask | PF_Highlighted | PF_RenderFog | PF_LumosAffected | PF_AlphaBlend | PF_AlphaToCoverage);
#else
	FPLAG blendFlags = PolyFlags & (PF_Translucent | PF_Modulated | PF_Invisible | PF_Occlude | PF_Masked | PF_Highlighted | PF_RenderFog | PF_AlphaBlend);
#endif

	if (blendFlags != CurrentPolyFlags)
	{
		// Metallicafan212:	We need to render since our flags changed
		EndBuffering();

		// Metallicafan212:	Check for changes
		FPLAG Xor = CurrentPolyFlags ^ blendFlags;

		// Metallicafan212:	Save the blend flags now
		CurrentPolyFlags = blendFlags;

		// Metallicafan212:	Again, the DX9 driver saves the day
#if DX11_HP2
		const FPLAG RELEVANT_BLEND_FLAGS = PF_Translucent | PF_Modulated | PF_Highlighted | PF_LumosAffected | PF_Invisible | PF_AlphaBlend | PF_AlphaToCoverage | PF_Masked | PF_ColorMask;
#else
		const FPLAG RELEVANT_BLEND_FLAGS = PF_Translucent | PF_Modulated | PF_Highlighted | PF_Invisible;
#endif

		if (Xor & (RELEVANT_BLEND_FLAGS))
		{
			// Metallicafan212:	Only set when it's actually using it to render (in case of bad flags)
			GlobalPolyflagVars.bModulated = 0;

			// Metallicafan212:	Find and set the render state
			if (!(blendFlags & (RELEVANT_BLEND_FLAGS)))
			{
				FindAndSetBlend(0, D3D11_BLEND_ONE, D3D11_BLEND_ZERO, D3D11_COLOR_WRITE_ENABLE_ALL, 0);
				GlobalPolyflagVars.bAlphaEnabled = 0;
			}
			else
			{
				GlobalPolyflagVars.bAlphaEnabled = 1;

				// Metallicafan212:	DX9 allows you to completely turn off color drawing. We achieve the same effect here by using a 0 source blend and a 1 dest blend (since it will keep the dst color)
				if (blendFlags & PF_Invisible)
				{
					FindAndSetBlend(PF_Invisible, D3D11_BLEND_ZERO, D3D11_BLEND_ONE, D3D11_COLOR_WRITE_ENABLE_ALPHA);
				}
				else if (blendFlags == PF_Highlighted)
				{
					FindAndSetBlend(PF_Highlighted, D3D11_BLEND_ONE, D3D11_BLEND_INV_SRC_ALPHA);
				}
#if DX11_HP2
				else if (blendFlags & PF_Translucent && (blendFlags & PF_Highlighted || blendFlags & PF_AlphaBlend))
#else
				else if ((blendFlags & (PF_Translucent | PF_Highlighted)) == (PF_Translucent | PF_Highlighted))//blendFlags & PF_Translucent && (blendFlags & PF_Highlighted))
#endif
				{
					FindAndSetBlend(PF_Translucent | PF_AlphaBlend, D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_INV_SRC_ALPHA);
				}
				else if (blendFlags & PF_Translucent)
				{
					FindAndSetBlend(PF_Translucent, D3D11_BLEND_ONE, D3D11_BLEND_INV_SRC_COLOR);
				}
				else if (blendFlags & PF_Modulated)
				{
					GlobalPolyflagVars.bModulated = 1;
					FindAndSetBlend(PF_Modulated, D3D11_BLEND_DEST_COLOR, D3D11_BLEND_SRC_COLOR);
				}
#if DX11_HP2
				// Metallicafan212:	New engine QWORD lumos
				else if (blendFlags & PF_LumosAffected)
				{
					// Metallicafan212:	I've reversed the reversed lumos alpha, as it seems that INV_SRC_ALPHA in DX11 doesn't write full alpha for some reason...
					FindAndSetBlend(PF_LumosAffected, D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_INV_SRC_ALPHA);//D3D11_BLEND_INV_SRC_ALPHA, D3D11_BLEND_SRC_ALPHA);
				}
#endif
				else if (blendFlags & PF_Highlighted)
				{
#if DX11_HP2
					if (blendFlags & PF_AlphaBlend)
					{
						FindAndSetBlend(PF_Highlighted | PF_AlphaBlend, D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_INV_SRC_ALPHA);
					}
					else
#endif
					{
						FindAndSetBlend(PF_Highlighted, D3D11_BLEND_ONE, D3D11_BLEND_INV_SRC_ALPHA);
					}
				}
#if DX11_HP2
				// Metallicafan212:	Alpha to coverage is considered alpha blend, since that's what it's usually used for
				//					TODO! Maybe re-evaluate this????
				else if (blendFlags & PF_AlphaToCoverage)
				{
					FindAndSetBlend(PF_AlphaBlend | PF_AlphaToCoverage, D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_INV_SRC_ALPHA, D3D11_COLOR_WRITE_ENABLE_ALL, 1, 1);
				}
				else if (blendFlags & PF_AlphaBlend)
				{
					FindAndSetBlend(PF_AlphaBlend, D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_INV_SRC_ALPHA);
				}
				else if (blendFlags & PF_ColorMask)
				{
					FindAndSetBlend(PF_ColorMask, D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_INV_SRC_ALPHA);
				}
#endif
				else if (blendFlags & PF_Masked)
				{
					FindAndSetBlend(PF_Masked, D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_INV_SRC_ALPHA);
				}
			}
		}

#if DX11_HP2
		// Metallicafan212:	Set the correct fake fog values
		//					Reset fog if the XOR was Translucent or Modulated
		//					TODO! This can be a bit glitchy in the editor, where it turns all fog into modulated fog
		if ((GlobalShaderVars.bDoDistanceFog || GlobalShaderVars.bFadeFogValues) && (Xor & (PF_Translucent | PF_Modulated | PF_AlphaBlend | PF_Highlighted)))
		{
			FPLAG Flags = (blendFlags & RELEVANT_BLEND_FLAGS);

			// Metallicafan212:	Translucent gets combined with a few other flags to set a specific hack
			//					Sigh.... If only they just added a alpha flag instead of reusing flags, it makes it extremely annoying
			if (Flags & PF_Translucent && !(Flags & (PF_AlphaBlend | PF_Highlighted)))
			{
				GlobalShaderVars.DistanceFogColor = GlobalShaderVars.TransFogColor;
				UpdateFogSettings();
			}
			else if (Flags & PF_Modulated)
			{
				GlobalShaderVars.DistanceFogColor = GlobalShaderVars.ModFogColor;
				UpdateFogSettings();
			}
			else
			{
				GlobalShaderVars.DistanceFogColor = GlobalShaderVars.DistanceFogFinal;
				UpdateFogSettings();
			}
		}
#endif

		// Metallicafan212:	TODO! Allow the user to specify the alpha reject values
		if (Xor & (PF_Masked | PF_AlphaBlend | PF_LumosAffected | PF_ColorMask | PF_Invisible))
		{
#if DX11_HP2
			if (!(blendFlags & PF_ColorMask))
			{
				// Metallicafan212:	Disable masked
				GlobalPolyflagVars.bColorMasked = 0;
			}
#endif
			// Metallicafan212:	Set a fair alpha reject
			if (blendFlags & PF_Invisible)
			{
				GlobalPolyflagVars.AlphaReject		= 1e-6f;
				GlobalPolyflagVars.bColorMasked		= 0;
				GlobalPolyflagVars.bAlphaEnabled	= 1;
			}
			else if (blendFlags & PF_AlphaBlend)
			{
				GlobalPolyflagVars.AlphaReject		= 1e-6f;
				GlobalPolyflagVars.bColorMasked		= 0;
				GlobalPolyflagVars.bAlphaEnabled	= 1;
			}
#if DX11_HP2
			else if (blendFlags & PF_ColorMask)
			{
				GlobalPolyflagVars.AlphaReject		= 0.8f;
				GlobalPolyflagVars.bColorMasked		= 1;
				GlobalPolyflagVars.bAlphaEnabled	= 1;
			}
#endif
			else if (blendFlags & PF_Masked)
			{
				GlobalPolyflagVars.AlphaReject		= 0.8f;
				GlobalPolyflagVars.bColorMasked		= 0;
				GlobalPolyflagVars.bAlphaEnabled	= 1;
			}
#if DX11_HP2
			else if (blendFlags & PF_LumosAffected)
			{
				GlobalPolyflagVars.AlphaReject		= 1e-6f;
				GlobalPolyflagVars.bColorMasked		= 0;
				GlobalPolyflagVars.bAlphaEnabled	= 1;
			}
#endif
			else
			{
				GlobalPolyflagVars.AlphaReject		= 1e-6f;
				GlobalPolyflagVars.bColorMasked		= 0;
			}
		}

		// Metallicafan212:	Toggle between the z write and no z write states
		if (Xor & PF_Occlude)
		{
			if ((blendFlags & PF_Occlude))
			{
				m_RenderContext->OMSetDepthStencilState(m_DefaultZState, 0);
			}
			else
			{
				m_RenderContext->OMSetDepthStencilState(m_DefaultNoZState, 0);
			}
		}

		// Metallicafan212:	Flags changed, update them in the constant buffer
		UpdatePolyflagsVars();
	}

	unguard;
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
