#include "D3D11Drv.h"

// Metallicafan212:	Texturing related functions (since there's going to be quite a bit)
void UD3D11RenderDevice::SetTexture(INT TexNum, FTextureInfo* Info, QWORD PolyFlags)
{
	guard(UD3D11RenderDevice::SetTexture);

	// Metallicafan212:	Support null textures
	if (Info == nullptr)
	{
		// Metallicafan212:	Only end buffering if the slot wasn't null before!!!
		if (BoundTextures[TexNum].TexInfo != nullptr)
			EndBuffering();

		BoundTextures[TexNum].TexInfo	= nullptr;
		BoundTextures[TexNum].m_SRV		= BlankResourceView;

		m_D3DDeviceContext->PSSetShaderResources(TexNum, 1, &BlankResourceView);
		m_D3DDeviceContext->PSSetSamplers(TexNum, 1, &BlankSampler);

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

	//if ((CacheID & 0xFF) == 0xE0)
	//{
	//	//Alter texture cache id if masked texture hack is enabled and texture is masked
	//	CacheID |= ((PolyFlags & PF_Masked) ? 1 : 0);
	//}

	// Metallicafan212:	End buffering if the input texture doesn't match!!!
	UBOOL bSetTex = 0;
	if ((BoundTextures[TexNum].TexInfo != nullptr && BoundTextures[TexNum].TexInfo->CacheID != CacheID))
	{
		bSetTex = 1;
		EndBuffering();
	}

	// Metallicafan212:	Search for the bind
	FD3DTexture* DaTex		= TextureMap.Find(Info->CacheID);

	// Metallicafan212:	Check if we need to upload it to the GPU
	UBOOL bUpload			= DaTex == nullptr || Info->bRealtimeChanged /* || Info->bRealtime || Info->bParametric*/ || (PolyFlags & PF_Masked) != (DaTex->PolyFlags & PF_Masked);

	UBOOL bDoSampUpdate;

	if(bUpload)
	{
		CacheTextureInfo(*Info, PolyFlags);

		// Metallicafan212:	Get the new bind, if it's changed
		DaTex = TextureMap.Find(CacheID);

		// Metallicafan212:	Already did the sample update
		bDoSampUpdate = 0;
	}
	else if (DaTex)
	{
		bDoSampUpdate = (PolyFlags & (PF_NoSmooth | PF_ClampUVs)) != (DaTex->PolyFlags & (PF_NoSmooth | PF_ClampUVs));
	}

	BoundTextures[TexNum].TexInfo	= DaTex;
	BoundTextures[TexNum].UPan		= Info->Pan.X;
	BoundTextures[TexNum].VPan		= Info->Pan.Y;
	BoundTextures[TexNum].bIsRT		= DaTex->bIsRT;

	// Metallicafan212:	Check if it's a RT texture!!
	//					Really should use IsA, but this is quicker
	if (Info->Texture != nullptr && Info->Texture->GetClass() == UDX11RenderTargetTexture::StaticClass())
	{
		UDX11RenderTargetTexture* TexTemp = (UDX11RenderTargetTexture*)Info->Texture;

		// Metallicafan212:	If we have MSAA we need to RESOLVE!!!!!
		if (NumAASamples > 1)
		{
			m_D3DDeviceContext->ResolveSubresource(TexTemp->NonMSAATex.Get(), 0, TexTemp->RTTex.Get(), 0, DXGI_FORMAT_B8G8R8A8_UNORM);
		}
		
		m_D3DDeviceContext->PSSetShaderResources(TexNum, 1, TexTemp->RTSRView.GetAddressOf());

		ID3D11SamplerState* Temp = GetSamplerState(0);
		m_D3DDeviceContext->PSSetSamplers(TexNum, 1, &Temp);

		// Metallicafan212:	So we can find whatever is still bound as the RT when we call OMSetRenderTargets
		BoundTextures[TexNum].m_SRV = TexTemp->RTSRView.Get();

		return;
	}

	BoundTextures[TexNum].m_SRV = DaTex->m_View;

	if (bDoSampUpdate)
	{
		MakeTextureSampler(DaTex, PolyFlags);
	}

	m_D3DDeviceContext->PSSetShaderResources(TexNum, 1, &DaTex->m_View);

	ID3D11SamplerState* Temp = GetSamplerState((PolyFlags) | (DaTex->bShouldUVClamp ? PF_ClampUVs : 0));
	m_D3DDeviceContext->PSSetSamplers(TexNum, 1, &Temp);

	unguard;
}

// Metallicafan212:	TODO! Since I've redone the way samplers are made, this is pretty redundant now
//					The only check is for UV clamp
void UD3D11RenderDevice::MakeTextureSampler(FD3DTexture* Bind, QWORD PolyFlags)
{
	guard(UD3D11RenderDevice::MakeTextureSampler);

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

static UBOOL GetMipInfo(FTextureInfo& Info, FD3DTexType* Type, INT MipNum, BYTE*& DataPtr, INT& Size, INT& SourcePitch)
{
#if DX11_HP2
	FMipmap* Mip = Info.Mips[MipNum];
	if (Mip->DataPtr == nullptr)
	{
		Mip->DataArray.Load();
		Mip->DataPtr = static_cast<BYTE*>(Mip->DataArray.GetData());
	}
	DataPtr = Mip->DataPtr;
	Size    = Mip->DataArray.Num();

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
	}	
	else
	{	
		// probably a light or fog map	
		FMipmapBase* MipBase = Info.Mips[MipNum];
		DataPtr = MipBase->DataPtr;
		Size    = FTextureBytes(Info.Format, MipBase->USize, MipBase->VSize);
		SourcePitch = FTextureBlockBytes(Info.Format) * FTextureBlockAlignedWidth(Info.Format, MipBase->USize) / FTextureBlockWidth(Info.Format);
	}
#else
#error "Implement Me!"
#endif

	return TRUE;
}

void UD3D11RenderDevice::CacheTextureInfo(FTextureInfo& Info, QWORD PolyFlags, UBOOL bJustSampler)
{
	guard(UD3D11RenderDevice::CacheTextureInfo);

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
	FD3DTexture* DaTex = TextureMap.Find(CacheID);

	if (DaTex == nullptr)
	{
		DaTex = &TextureMap.Set(CacheID, FD3DTexture());
	}

	DaTex->Tex			= Info.Texture;

	// Metallicafan212:	Cache it now!
	DaTex->UClamp		= Info.UClamp;
	DaTex->VClamp		= Info.VClamp;
	DaTex->NumMips		= Info.NumMips;
	DaTex->PolyFlags	= PolyFlags;
	DaTex->USize		= Info.USize;
	DaTex->VSize		= Info.VSize;
	DaTex->UMult		= 1.0f / (Info.UScale * Info.USize);
	DaTex->VMult		= 1.0f / (Info.VScale * Info.VSize);
	DaTex->UScale		= Info.UScale;
	DaTex->VScale		= Info.VScale;
	DaTex->CacheID		= CacheID;

	// Metallicafan212:	More texture information
#if DX11_HP2
	DaTex->MaskedColor			= Info.MaskedColor.Plane();
	DaTex->MaskedGranularity	= Info.GranularityColor.Plane();
#endif

	// Metallicafan212:	This was stupid to leave here lmao
	//DaTex->Format		= Info.Format;
	//DaTex->TexFormat	= DXGI_FORMAT_B8G8R8A8_UNORM;

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

	// Metallicafan212:	TODO! Would it be better to have static memory for this? Or to hold onto a buffer, and only increase its size as the request gets bigger?
	ConversionMemory	= new BYTE[4 * Info.USize * Info.VSize]();

	// Metallicafan212:	Get the format description
	//					DO THIS FROM THE INFO!!!! THE MAPPED VERSION IS NULL
	FD3DTexType* Type = SupportedTextures.Find(Info.Format);

	FD3DTexType::GetPitch P = Type->GetTexturePitch;
	BYTE* MipData = nullptr;
	INT MipSize = 0, MipPitch = 0;

	// Metallicafan212:	Check if we need to make it
	if (DaTex->m_Tex == nullptr)
	{
		// Metallicafan212:	Check for the info
		DaTex->Format		= Info.Format;
		DaTex->TexFormat	= Type->DXFormat;

		if (Type == nullptr)
			appErrorf(TEXT("Metallicafan212 you idiot, you forgot to add a descriptor for %d"), DaTex->Format);

		// Metallicafan212:	Create the texture
		D3D11_TEXTURE2D_DESC Desc;
		appMemzero(&Desc, sizeof(Desc));

		Desc.Format				= Type->DXFormat;
		Desc.Width				= Info.USize;
		Desc.Height				= Info.VSize;
		Desc.MipLevels			= Info.NumMips;
		Desc.Usage				= D3D11_USAGE_DEFAULT;
		Desc.BindFlags			= D3D11_BIND_SHADER_RESOURCE;
		Desc.ArraySize			= 1;
		Desc.CPUAccessFlags		= D3D11_CPU_ACCESS_WRITE;
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

		// Metallicafan212:	Create the view
		D3D11_SHADER_RESOURCE_VIEW_DESC vDesc;
		appMemzero(&vDesc, sizeof(vDesc));

		vDesc.ViewDimension				= D3D11_SRV_DIMENSION_TEXTURE2D;
		vDesc.Texture2D.MipLevels		= Info.NumMips;
		vDesc.Texture2D.MostDetailedMip = 0;
		vDesc.BufferEx.FirstElement		= 0;
		vDesc.Format					= DaTex->TexFormat;

		hr = m_D3DDevice->CreateShaderResourceView(DaTex->m_Tex, &vDesc, &DaTex->m_View);

		ThrowIfFailed(hr);

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
	}
	else
	{
		// Metallicafan212:	Texture update!
		if (Type->bSupported)
		{
		CopyTexture:
			// Metallicafan212:	We need to copy each mip
			for (INT i = 0; i < Info.NumMips; i++)
			{	
				if (GetMipInfo(Info, Type, i, MipData, MipSize, MipPitch))
					Type->TexUploadFunc(MipData, MipSize, MipPitch, ConversionMemory, DaTex, m_D3DDeviceContext, Info.Mips[i]->USize, Info.Mips[i]->VSize, i);
			}
		}
		else
		{
		ConvertTexture:	
			// Metallicafan212:	We need to convert each mip!!!! TODO!
			UBOOL bMaskedHack = (Info.Format == TEXF_P8 && PolyFlags & PF_Masked);

			// Metallicafan212:	TODO! Mask hack it!!
			FColor OldMasked = bMaskedHack ? Info.Palette[0] : FColor(0, 0, 0, 0);

			if (bMaskedHack)
				Info.Palette[0] = FColor(0, 0, 0, 0);

			for (INT i = 0; i < Info.NumMips; i++)
			{
				if (GetMipInfo(Info, Type, i, MipData, MipSize, MipPitch))
				{
					// Metallicafan212:	Sigh.... We need to map dynamic memory to do this!!!!!
					// Metallicafan212:	Do the conversion over
					SIZE_T Size = (MipSize == 0 ? 4 * Info.Mips[i]->USize * Info.Mips[i]->VSize : MipSize);
					checkSlow(Size <= 4 * Info.USize * Info.VSize);

					Type->TexConvFunc(Info.Palette, MipData, Size, (Type->*P)(Info.Mips[i]->USize), ConversionMemory, DaTex, m_D3DDeviceContext, Info.Mips[i]->USize, Info.Mips[i]->VSize, i);
				}

			}

			// Metallicafan212:	Restore
			if (bMaskedHack)
				Info.Palette[0] = OldMasked;
		}
	}

	delete[] ConversionMemory;

	unguard;
}

// Metallicafan212:	Function to register supported texture types
//					The device will query first to see what's supported, while some hand-fed types will be provided
void UD3D11RenderDevice::RegisterTextureFormat(ETextureFormat Format, DXGI_FORMAT DXFormat, UBOOL bRequiresConversion, INT ByteOrBlockSize, FD3DTexType::GetPitch PitchFunc, FD3DTexType::UploadFunc UFunc, FD3DTexType::ConversionFunc UConv)
{
	guard(UD3D11RenderDevice::RegisterTextureFormat);

	// Metallicafan212:	Init the format description
	FD3DTexType Type;

	Type.Format				= Format;
	Type.DXFormat			= DXFormat;
	Type.bSupported			= !bRequiresConversion;
	Type.TexUploadFunc		= UFunc;
	Type.TexConvFunc		= UConv;
	Type.GetTexturePitch	= PitchFunc;
	Type.BytesPerPixel		= ByteOrBlockSize;
	Type.BlockSize			= ByteOrBlockSize;

	SupportedTextures.Set(Format, Type);

	unguard;
}

// Metallicafan212:	Texture uploading functions
void MemcpyTexUpload(void* Source, SIZE_T SourceLength, SIZE_T SourcePitch, void* ConversionMem, FD3DTexture* tex, ID3D11DeviceContext* m_D3DDeviceContext, INT USize, INT VSize, INT Mip)
{
	guard(MemcpyTexUpload);

	// Metallicafan212:	Just copy over, todo!!!!
	m_D3DDeviceContext->UpdateSubresource(tex->m_Tex, Mip, nullptr, Source, SourcePitch, 0);

	unguard;
}

void RGBA7To8(FColor* Palette, void* Source, SIZE_T SourceLength, SIZE_T SourcePitch, void* ConversionMem, FD3DTexture* tex, ID3D11DeviceContext* m_D3DDeviceContext, INT USize, INT VSize, INT Mip)
{
	guard(RGBA7To8);

	DWORD* pTex = (DWORD*)ConversionMem;

	// Metallicafan212:	Get each color
	//					TODO! Support masking????
	SIZE_T Read		= 0;
	BYTE* Bytes		= (BYTE*)Source;
	BYTE* DBytes	= (BYTE*)ConversionMem;

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
	m_D3DDeviceContext->UpdateSubresource(tex->m_Tex, Mip, nullptr, ConversionMem, SourcePitch, 0);
	unguard;
}

void P8ToRGBA(FColor* Palette, void* Source, SIZE_T SourceLength, SIZE_T SourcePitch, void* ConversionMem, FD3DTexture* tex, ID3D11DeviceContext* m_D3DDeviceContext, INT USize, INT VSize, INT Mip)
{
	guard(P8ToRGBA);

	// Metallicafan212:	Update each 4 byte block
	SIZE_T	Read	= 0;
	BYTE*	Bytes	= (BYTE*)Source;
	BYTE*	DBytes	= (BYTE*)ConversionMem;

	// Metallicafan212:	Just read across
	//					There might be a quicker way to do this, but this was the easiest to write lmao
	while (Read < SourceLength)
	{
#if DX11_HP2
		(*(DWORD*)DBytes) = Palette[Bytes[Read]].Int4;
#else
		const FColor& Color = Palette[Bytes[Read]];
		DBytes[0] = Color.R;
		DBytes[1] = Color.G;
		DBytes[2] = Color.B;
		DBytes[3] = Color.A;
#endif

		Read	+= 1;
		DBytes	+= 4;
	}


	// Metallicafan212:	Now update
	m_D3DDeviceContext->UpdateSubresource(tex->m_Tex, Mip, nullptr, ConversionMem, SourcePitch, 0);

	unguard;
}

// Metallicafan212:	Based on the DX9 version, but HEAVILY modified
void UD3D11RenderDevice::SetBlend(QWORD PolyFlags)
{
	guard(UD3D11RenderDevice::SetBlend);

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
		PolyFlags = (PolyFlags & ~(PF_Translucent | PF_Modulated | PF_Highlighted | PF_LumosAffected)) | PF_AlphaBlend;
	}

	// Metallicafan212:	Check if the input blend flags are relevant
#if DX11_HP2
	QWORD blendFlags = PolyFlags & (PF_Translucent | PF_Modulated | PF_Invisible | PF_Occlude | PF_Masked | PF_ColorMask | PF_Highlighted | PF_RenderFog | PF_LumosAffected | PF_AlphaBlend | PF_AlphaToCoverage);
#else
	QWORD blendFlags = PolyFlags & (PF_Translucent | PF_Modulated | PF_Invisible | PF_Occlude | PF_Masked | PF_Highlighted | PF_RenderFog);
#endif

	if (blendFlags != CurrentPolyFlags)
	{
		// Metallicafan212:	Check for changes
		QWORD Xor = CurrentPolyFlags ^ blendFlags;

		// Metallicafan212:	Save the blend flags now
		CurrentPolyFlags = blendFlags;

		// Metallicafan212:	Again, the DX9 driver saves the day
#if DX11_HP2
		const QWORD RELEVANT_BLEND_FLAGS = PF_Translucent | PF_Modulated | PF_Highlighted | PF_LumosAffected | PF_Invisible | PF_AlphaBlend | PF_AlphaToCoverage | PF_Masked | PF_ColorMask;
#else
		const QWORD RELEVANT_BLEND_FLAGS = PF_Translucent | PF_Modulated | PF_Highlighted | PF_Invisible;
#endif
		EndBuffering();

		if (Xor & (RELEVANT_BLEND_FLAGS))
		{
			// Metallicafan212:	Find and set the render state
			if (!(blendFlags & (RELEVANT_BLEND_FLAGS)))
			{
				FindAndSetBlend(0, D3D11_BLEND_ONE, D3D11_BLEND_ZERO, D3D11_COLOR_WRITE_ENABLE_ALL, 0);
			}
			else
			{
				// Metallicafan212:	DX9 allows you to completely turn off color drawing. We achieve the same effect here by using a 0 source blend and a 1 dest blend (since it will keep the dst color)
				if (blendFlags & PF_Invisible)
				{
					FindAndSetBlend(PF_Invisible, D3D11_BLEND_ZERO, D3D11_BLEND_ONE);
				}
				else if (blendFlags == PF_Highlighted)
				{
					FindAndSetBlend(PF_Highlighted, D3D11_BLEND_ONE, D3D11_BLEND_INV_SRC_ALPHA);
				}
#if DX11_HP2
				else if (blendFlags & PF_Translucent && (blendFlags & PF_Highlighted || blendFlags & PF_AlphaBlend))
#else
				else if (blendFlags & PF_Translucent && (blendFlags & PF_Highlighted))
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
		// Metallicafan212:	Set the correct fake fog values
		//					Reset fog if the XOR was Translucent or Modulated
		//					TODO! This can be a bit glitchy in the editor, where it turns all fog into modulated fog
		if ((GlobalShaderVars.bDoDistanceFog || GlobalShaderVars.bFadeFogValues) && (Xor & (PF_Translucent | PF_Modulated | PF_AlphaBlend | PF_Highlighted)))
		{
			QWORD Flags = (blendFlags & RELEVANT_BLEND_FLAGS);

			// Metallicafan212:	Translucent gets combined with a few other flags to set a specific hack
			//					Sigh.... If only they just added a alpha flag instead of reusing flags, it makes it extremely annoying
			if (Flags & PF_Translucent && !(Flags & (PF_AlphaBlend | PF_Highlighted)))
			{
				GlobalShaderVars.DistanceFogColor = GlobalShaderVars.TransFogColor;
			}
			else if (Flags & PF_Modulated)
			{
				GlobalShaderVars.DistanceFogColor = GlobalShaderVars.ModFogColor;
			}
			else
			{
				GlobalShaderVars.DistanceFogColor = GlobalShaderVars.DistanceFogFinal;
			}
		}

		// Metallicafan212:	TODO! Allow the user to specify the alpha reject values
		if (Xor & (PF_Masked | PF_AlphaBlend | PF_LumosAffected | PF_ColorMask))
		{
			if (!(blendFlags & PF_ColorMask))
			{
				// Metallicafan212:	Disable masked
				GlobalShaderVars.bColorMasked = 0;
			}

			if (blendFlags & PF_AlphaBlend)
			{
				GlobalShaderVars.AlphaReject = 1e-6f;
				GlobalShaderVars.bColorMasked = 0;
			}
			else if (blendFlags & PF_ColorMask)
			{
				GlobalShaderVars.AlphaReject = 0.8f;
				GlobalShaderVars.bColorMasked = 1;
			}
			else if (blendFlags & PF_Masked)
			{
				GlobalShaderVars.AlphaReject = 0.8f;
				GlobalShaderVars.bColorMasked = 0;
			}
			else if (blendFlags & PF_LumosAffected)
			{
				GlobalShaderVars.AlphaReject = 1e-6f;
				GlobalShaderVars.bColorMasked = 0;

			}
			else
			{
				GlobalShaderVars.AlphaReject = 1e-6f;
				GlobalShaderVars.bColorMasked = 0;
			}
		}

		// Metallicafan212:	Toggle between the z write and no z write states
		if (Xor & PF_Occlude)
		{
			if ((blendFlags & PF_Occlude))
			{
				m_D3DDeviceContext->OMSetDepthStencilState(m_DefaultZState, 0);
			}
			else
			{
				m_D3DDeviceContext->OMSetDepthStencilState(m_DefaultNoZState, 0);
			}
		}
	}

	unguard;
}

#if DX11_UT_469
UBOOL UD3D11RenderDevice::SupportsTextureFormat(ETextureFormat Format)
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