// Metallicafan212:	My custom shit here
#include "ICBINDx11Drv.h"

IMPLEMENT_CLASS(UDX11RenderTargetTexture);

// Metallicafan212:	Render target stuff
//					We have to fill this in so the rest of the engine works
void UDX11RenderTargetTexture::Lock(FTextureInfo& Info, FTime InTime, INT LOD, URenderDevice* RenDev)
{
	guard(UDX11RenderTargetTexture::Lock);

	// Metallicafan212:	Copy HP2 specific info
#if DX11_HP2
	Info.MaskedColor		= MaskedColor;
	Info.GranularityColor	= MaskedGranularity;
#endif

	// Metallicafan212:	Setup the common vars
	Info.Texture			= this;
#if DX11_HP2
	Info.UScale				= DrawScale;
	Info.VScale				= DrawScale;
#else
	Info.UScale				= 1.f;
	Info.VScale				= 1.f;
#endif

	// Metallicafan212:	TODO! Probably don't need any of these
	/*
	Info.bRealtime				= bRealtime;
	Info.bParametric			= bParametric;
	Info.bRealtimeChanged		= bRealtimeChanged;

	// Metallicafan212:	Make sure this is reset???
	bRealtimeChanged			= 0;

	Info.bHighColorQuality		= bHighColorQuality;
	Info.bHighTextureQuality	= bHighTextureQuality;
	*/

	// Metallicafan212:	Since it's a RT in GPU mem, there's no mips to read
	Info.NumMips			= 0;

	// Metallicafan212:	Matches the GPU side
#if DX11_UT_469 || DX11_HP2
	Info.Format				= TEXF_BGRA8;
#else
	Info.Format				= TEXF_RGBA8;
#endif

	// Metallicafan212:	TODO! Cache ID is probably not needed!
	Info.CacheID			= MakeCacheID((ECacheIDBase)(CID_RenderTexture), this);
	//Info.CacheID			= 0;
	
	Info.Pan				= FVector(0.0f, 0.0f, 0.0f);
	//Info.MaxColor			= &MaxColor;

	Info.PaletteCacheID		= 0;
	Info.USize				= USize;
	Info.VSize				= VSize;
	Info.UClamp				= USize;
	Info.VClamp				= VSize;

	// Metallicafan212:	TODO! Should we even update???
	if (InTime != 0.f)
	{
#if DX11_HP2
		Update(InTime, RenDev != nullptr ? RenDev->Viewport : nullptr);
#else
		Update(InTime);
#endif
	}

	// Metallicafan212:	Give it the bits
#if DX11_HP2
	Info.UBits				= UBits;
	Info.VBits				= VBits;
#endif

	// Success.
	unguardobj;
}

void UDX11RenderTargetTexture::Destroy()
{
	guard(UDX11RenderTargetTexture::Destroy);

	// Metallicafan212:	Make sure it's not bound?
	if (D3DDev != nullptr)
	{
		for (INT i = 0; i < MAX_TEXTURES; i++)
		{
			if (D3DDev->BoundTextures[i].m_SRV == RTSRView.Get())
			{
				D3DDev->SetTexture(i, nullptr, 0);
			}
		}
	}
	
	*RTTex.ReleaseAndGetAddressOf() = nullptr;

	*DTTex.ReleaseAndGetAddressOf() = nullptr;

	*RTView.ReleaseAndGetAddressOf() = nullptr;

	*DTView.ReleaseAndGetAddressOf() = nullptr;

	*RTSRView.ReleaseAndGetAddressOf() = nullptr;

	*DTSRView.ReleaseAndGetAddressOf() = nullptr;

	*NonMSAATex.ReleaseAndGetAddressOf() = nullptr;

	*RTD2D.ReleaseAndGetAddressOf() = nullptr;

	*RTDXGI.ReleaseAndGetAddressOf() = nullptr;

	*RTTexCopy.ReleaseAndGetAddressOf() = nullptr;

	// Metallicafan212:	Remove us from the list
	if (D3DDev != nullptr)
	{
		D3DDev->RTTextures.RemoveItem(this);
	}

	Super::Destroy();

	unguard;
}

void ReplaceInText(FString& In, const TCHAR* Match, const TCHAR* With)
{
	guard(ReplaceInText);

	INT Bad = In.InStr(Match);

	FString Left, Right;

	while (Bad != INDEX_NONE)
	{
		In.Split(Match, &Left, &Right);

		In = Left + FString(With) + Right;

		Bad = In.InStr(Match);
	}

	unguard;
}

#if DX11_HP2

#define DO_MANUAL_SCALE 0

int UICBINDx11RenderDevice::DrawString(QWORD Flags, UFont* Font, INT& DrawX, INT& DrawY, const TCHAR* Text, const FPlane& Color, FLOAT Scale, FLOAT SpriteScaleX, FLOAT SpriteScaleY)
{
	guard(UICBINDx11RenderDevice::DrawString);

	// Metallicafan212:	Update the color if we're doing selection testing
	FPlane LocalColor = Color;

	if (m_HitData != nullptr)
		LocalColor = CurrentHitColor;

	// Metallicafan212:	TODO! Draw using Direct2D

	FString RealText = FString(Text);

	// Metallicafan212:	TODO! Probably not needed for Direct2D, since it'll have these special characters that are in the Polish files
	// Metallicafan212:	Replace the invalid characters
	ReplaceInText(RealText, TEXT("–"), TEXT("--"));
	ReplaceInText(RealText, TEXT("…"), TEXT("..."));

	// Metallicafan212:	Use a local string to add a zero width character
	FString LocalText = RealText;// + TEXT("‌");

	/*
	// Metallicafan212:	Search for a space
	INT SpaceI = LocalText.InStr(TEXT(" "), 1);

	if (SpaceI == LocalText.Len() - 1)
	{
		// Metallicafan212:	Add a & for sizing
		LocalText.GetCharArray()[SpaceI] = L' ';
	}
	*/

	UCanvas* Canvas = Viewport->Canvas;

	// Metallicafan212:	Hold the scale here
	INT fontScale = Font->FontHeight;

	if (Scale > 1.0f)
		fontScale = Font->FontHeight * Scale;

#if DO_MANUAL_SCALE
	if(BoundRT == nullptr)
		fontScale *= ResolutionScale;
#endif

	FString FontKey = *FString::Printf(TEXT("%s %d"), *Font->FontName, fontScale);

	// Metallicafan212:	See if the font has been created before
#if !USE_UNODERED_MAP_EVERYWHERE
	IDWriteTextFormat* DaFont = FontMap.FindRef(FontKey);
#else
	auto f = FontMap.find(FontKey);

	IDWriteTextFormat* DaFont = f != FontMap.end() ? f->second : nullptr;
#endif

	HRESULT hr = S_OK;

	if (DaFont == nullptr)
	{
		FName lang = UObject::GetLanguage();

		DWRITE_FONT_WEIGHT fontWeight = DWRITE_FONT_WEIGHT_DEMI_BOLD;

		// Metallicafan212:	Catch when the font is specified
		FName tempFontName = *Font->FontName;

		if (Font->FontName == TEXT(""))
		{
			tempFontName = TEXT("Open Sans");

			// Metallicafan212:	Use the same size code as USA
			fontWeight = DWRITE_FONT_WEIGHT_NORMAL;//FW_NORMAL;

			if (fontScale < 10)
			{
				fontWeight = DWRITE_FONT_WEIGHT_LIGHT;
			}
		}

		// Metallicafan212:	Normal weight
		if (lang == TEXT("USA") || lang == TEXT("INT"))
		{
			fontWeight = DWRITE_FONT_WEIGHT_NORMAL;

			if (fontScale < 10)
			{
				fontWeight = DWRITE_FONT_WEIGHT_LIGHT;
			}
		}
		else
		{
			fontWeight = DWRITE_FONT_WEIGHT_NORMAL;
		}


		if (lang == TEXT("JAP"))
		{
			//charSet=SHIFTJIS_CHARSET;

			// Metallicafan212:	Use the JP font
			tempFontName = TEXT("Yu Mincho");
		}

		// Metallicafan212:	If the font name has BOLD in it, then mark it as bold
		FString RealCopy	= *tempFontName;
		FString Copy		= RealCopy.Caps();
		INT Bold			= Copy.InStr(TEXT(" BOLD"));

		if (Bold != INDEX_NONE)
		{
			fontWeight = DWRITE_FONT_WEIGHT_BOLD;

			RealCopy = RealCopy.Left(Bold);
		}

		// Metallicafan212:	In order for DWrite to use custom fonts, we have to provide a collection here...
		//					This is initialized globally in the windowing subsystem, and the separate shared factory here can access it just fine
#if DX11_HP2
		IDWriteFontCollection* FC = (IDWriteFontCollection*)GDWriteFontCollection;

		// Metallicafan212:	Test for the family name!!!!
		UINT Test = 0;
		BOOL Exists = 0;

		if (FC != nullptr && (FAILED(FC->FindFamilyName(*RealCopy, &Test, &Exists)) || Test == UINT_MAX))
		{
			FC = nullptr;
		}
#else
		IDWriteFontCollection* FC = nullptr;
#endif
		// Metallicafan212:	Create it
		hr = m_D2DWriteFact->CreateTextFormat(
			*RealCopy,
			FC,
			fontWeight,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			fontScale,
			TEXT(""),
			&DaFont
		);

		if (SUCCEEDED(hr))
		{
#if !USE_UNODERED_MAP_EVERYWHERE
			FontMap.Set(*FontKey, DaFont);
#else
			FontMap[FontKey] = DaFont;
#endif
			/*
			// Metallicafan212:	Per Scintilla, set the spacing to be consistent
			DaFont->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
			IDWriteTextLayout* pTextLayout = nullptr;
			hr = m_D2DWriteFact->CreateTextLayout(TEXT("X"), 1, DaFont, 100.0f, 100.0f, &pTextLayout);

			if (SUCCEEDED(hr) && pTextLayout != nullptr)
			{
				constexpr int maxLines = 2;
				DWRITE_LINE_METRICS lineMetrics[maxLines]{};
				UINT32 lineCount = 0;
				hr = pTextLayout->GetLineMetrics(lineMetrics, maxLines, &lineCount);

				pTextLayout->Release();
				DaFont->SetLineSpacing(DWRITE_LINE_SPACING_METHOD_UNIFORM, lineMetrics[0].height, lineMetrics[0].baseline);
			}
			*/

			//FontMap.Set(*RealCopy, DaFont);

			DaFont->SetWordWrapping(DWRITE_WORD_WRAPPING_WRAP);
		}
	}

	if (DaFont != nullptr)
	{
		// Metallicafan212:	Optional word wrapping
		if (Flags & PF_BrightCorners)
		{
			DaFont->SetWordWrapping(DWRITE_WORD_WRAPPING_WRAP);
		}
		else
		{
			DaFont->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
		}

		// Metallicafan212:	Create a text format to render it
		IDWriteTextLayout* layout = nullptr;
		hr = m_D2DWriteFact->CreateTextLayout(*LocalText, LocalText.Len(), DaFont, Canvas->ClipX, Canvas->ClipY, &layout);
		
		ThrowIfFailed(hr);

		// Metallicafan212:	Now calculate the rect
		DWRITE_TEXT_METRICS Met;
		layout->GetMetrics(&Met);

		INT OldDrawX = DrawX;
		INT OldDrawY = DrawY;

#if DO_MANUAL_SCALE
		if (BoundRT == nullptr)
		{
			Met.width /= ResolutionScale;
			Met.height /= ResolutionScale;
		}
#endif

		DrawX += Met.widthIncludingTrailingWhitespace; /// ResolutionScale;//Met.width;
		DrawY += Met.height; /// ResolutionScale;

		// Metallicafan212:	PF_Invisible says to just calc the rect
		if (!(Flags & PF_Invisible))
		{

			// Metallicafan212:	IMPORTANT!!!! D2D seems to actually somewhat RESPECT the current shaders, so we need to use a generic shader for this
			FGenShader->Bind(m_RenderContext);

			// Metallicafan212:	Actually draw it now
			m_CurrentD2DRT->BeginDraw();

			ID2D1SolidColorBrush* ColBrush = nullptr;
			m_CurrentD2DRT->CreateSolidColorBrush(D2D1::ColorF(LocalColor.X, LocalColor.Y, LocalColor.Z, 1.f - LocalColor.W), &ColBrush);

			// Metallicafan212:	Now draw it at the right location
			FLOAT X = OldDrawX + Canvas->OrgX;
			FLOAT Y = OldDrawY + Canvas->OrgY;
			FLOAT W = Canvas->ClipX - X;
			FLOAT H = Canvas->ClipY - Y;

			// Metallicafan212:	Center the text if PF_TwoSided
			if (Flags & PF_TwoSided)
			{
				// Center about DrawX.
				X -= Met.widthIncludingTrailingWhitespace / 2.0f;
				//Y -= Met.height / 2.0f;
				W = Canvas->ClipX - X;
				//H = Canvas->ClipY - Y;
				//layout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);//->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
			}
#if DO_MANUAL_SCALE
			if (BoundRT == nullptr)
			{
				X *= ResolutionScale;
				Y *= ResolutionScale;
				W *= ResolutionScale;
				H *= ResolutionScale;
			}
#else
#if !RES_SCALE_IN_PROJ
			if (BoundRT == nullptr && ResolutionScale != 1.0f)
			{
				D2D1::Matrix3x2F s = D2D1::Matrix3x2F::Scale(ResolutionScale, ResolutionScale);

				// Metallicafan212:	Now apply the scale!!!
				m_CurrentD2DRT->SetTransform(s);
			}
			else
			{
				m_CurrentD2DRT->SetTransform(D2D1::Matrix3x2F::Identity());
			}
#endif
#endif

			layout->SetMaxWidth(W);
			layout->SetMaxHeight(H);

			//m_D2DRT->FillRectangle(D2D1::RectF(0, 0, 1920, 1080), ColBrush);
			m_CurrentD2DRT->DrawTextLayout(D2D1::Point2F(X, Y), layout, ColBrush, D2D1_DRAW_TEXT_OPTIONS_NONE); //D2D1_DRAW_TEXT_OPTIONS_CLIP);
			
			// Metallicafan212:	Now end and release
			m_CurrentD2DRT->EndDraw();

			ColBrush->Release();
		}

		// Metallicafan212:	Release it outside the if lmao
		layout->Release();
	}

	return 1;

	unguard;
}
#endif

// Metallicafan212:	Viewer-based zone fog
void UICBINDx11RenderDevice::SetDistanceFog(UBOOL Enable, FLOAT FogStart, FLOAT FogEnd, FPlane Color, FLOAT FadeRate)
{
	guard(UICBINDx11RenderDevice::SetDistanceFog);

	// Metallicafan212:	Fog is already disabled?
	if (!Enable && !FogShaderVars.bDoDistanceFog)
		return;

	// Metallicafan212:	Update the values if it's enabled
	FogShaderVars.FogFadeRate = FadeRate;

	if (Enable)
	{
		// Metallicafan212:	Check if it's equal
		if(FogShaderVars.bDoDistanceFog && FogShaderVars.TargetFogColor == Color && FogShaderVars.CurrentFogStart == FogStart && FogShaderVars.CurrentFogEnd == FogEnd)
			return;

		// Metallicafan212:	Save the fog settings
		FogShaderVars.LastFogColor		= FogShaderVars.DistanceFogColor;
		FogShaderVars.LastFogSettings	= FogShaderVars.DistanceFogSettings;

		// Metallicafan212:	Set the fog values
		FogShaderVars.TargetFogColor		= Color;

		// Metallicafan212:	Start and end
		FogShaderVars.TargetFogSettings	= FPlane(1.0f / (FogEnd - FogStart), (FogEnd / (FogEnd - FogStart)) - 1.0f, Color.W, 0.0f);

		FogShaderVars.bFadeFogValues		= 1;

		FogShaderVars.CurrentFogStart	= FogStart;
		FogShaderVars.CurrentFogEnd		= FogEnd;
	}
	else
	{
		// Metallicafan212:	Save the fog settings
		FogShaderVars.LastFogColor		= FogShaderVars.DistanceFogColor;
		FogShaderVars.LastFogSettings	= FogShaderVars.DistanceFogSettings;

		// Metallicafan212:	Fade out
		FogShaderVars.TargetFogColor		= FPlane(0.0f, 0.0f, 0.0f, 0.0f);
		FogShaderVars.TargetFogSettings	= FPlane(1.0f / 32768.0f, 0.0f, 0.0f, 0.0f);

		FogShaderVars.bFadeFogValues		= 1;

		FogShaderVars.CurrentFogStart	= FogShaderVars.CurrentFogEnd = 0.0f;
	}


	// Metallicafan212:	Grab the current time
	FogShaderVars.FogSetTime = Viewport->CurrentTime.GetFloat();

	// Metallicafan212:	Keep the val around, so we can selectively set blending
	FogShaderVars.bDoDistanceFog = Enable;

	UpdateFogSettings();


	unguard;
}

void UICBINDx11RenderDevice::TickDistanceFog()
{
	guard(UICBINDx11RenderDevice::TickDistanceFog);

	// Metallicafan212:	If fog is disabled but we don't have 0 alpha, fade out
	if (FogShaderVars.bFadeFogValues)
	{
		// Metallicafan212:	fade the current fog setting
		FLOAT CurrPos = FogShaderVars.FogFadeRate <= 0.0f ? 1.0f : ((Viewport->CurrentTime.GetFloat() - FogShaderVars.FogSetTime) / FogShaderVars.FogFadeRate);

		if (CurrPos >= 1.0f)
		{
			FogShaderVars.DistanceFogColor		= FogShaderVars.TargetFogColor;
			FogShaderVars.DistanceFogSettings	= FogShaderVars.TargetFogSettings;

			FogShaderVars.bFadeFogValues = 0;
		}
		else
		{
			// Metallicafan212:	Lerp all the values
#define LERP_FPLANE(F, A, B) \
			F.X = Lerp(A.X, B.X, CurrPos); \
			F.Y = Lerp(A.Y, B.Y, CurrPos); \
			F.Z = Lerp(A.Z, B.Z, CurrPos); \
			F.W = Lerp(A.W, B.W, CurrPos);

			LERP_FPLANE(FogShaderVars.DistanceFogColor,		FogShaderVars.LastFogColor,		FogShaderVars.TargetFogColor);
			LERP_FPLANE(FogShaderVars.DistanceFogSettings,	FogShaderVars.LastFogSettings,	FogShaderVars.TargetFogSettings);
		}
	}

	// Metallicafan212:	Set the alpha on the other arrays
	FogShaderVars.ModFogColor.W		= FogShaderVars.DistanceFogColor.W;
	FogShaderVars.TransFogColor.W	= FogShaderVars.DistanceFogColor.W;

	// Metallicafan212:	Keep a copy so we can swap it for the modulated and translucent hacks
	FogShaderVars.DistanceFogFinal	= FogShaderVars.DistanceFogColor;

	UpdateFogSettings();

	unguard;
}

void UICBINDx11RenderDevice::ForceSetFogColor(FPlane FogColor)
{
	guard(UICBINDx11RenderDevice::ForceSetFogColor);

	// Metallicafan212:	TODO!
	FogShaderVars.DistanceFogColor = FogColor;

	UpdateGlobalShaderVars();

	unguard;
}

// Metallicafan212:	Simple BW shader for doing stupid "old-time" looking shit
void UICBINDx11RenderDevice::SetBWPercent(FLOAT Percent)
{
	guard(UICBINDx11RenderDevice::SetBWPercent);

	Percent = Clamp(Percent, 0.0f, 1.0f);

	if (Percent != GlobalPolyflagVars.BWPercent)//FogShaderVars.BWPercent)
	{
		// Metallicafan212:	End any buffering now
		EndBuffering();

		// Metallicafan212:	Update it
		//FogShaderVars.BWPercent = Percent;
		GlobalPolyflagVars.BWPercent = Percent;

		UpdatePolyflagsVars();
	}

	unguard;
}

// Metallicafan212:	New RT stuff
//					Create an RT texture, this is so the texture can be used in normal stuff
UTexture* UICBINDx11RenderDevice::CreateRenderTargetTexture(INT W, INT H, UBOOL bCreateDepth) 
{
	guard(UICBINDx11RenderDevice::CreateRenderTargetTexture);

	// Metallicafan212:	Create the texture
#if DX11_HP2
	UDX11RenderTargetTexture* Tex = ConstructObject<UDX11RenderTargetTexture>(GetTransientPackage(), NAME_None, RF_Transient);
#else
	UDX11RenderTargetTexture* Tex = ConstructObject<UDX11RenderTargetTexture>(UDX11RenderTargetTexture::StaticClass(), GetTransientPackage(), NAME_None, RF_Transient);
#endif

	if (Tex != nullptr)
	{
		// Metallicafan212:	Set the vars
		Tex->USize		= Tex->UClamp = W;
		Tex->VSize		= Tex->VClamp = H;
#if DX11_HP2
		Tex->DrawScale	= 1.0f;
#endif

		Tex->D3DDev		= this;

		Tex->UBits		= appCeilLogTwo(W);
		Tex->VBits		= appCeilLogTwo(H);

		// Metallicafan212:	Create the texture from this
		HRESULT hr	= S_OK;

		CD3D11_TEXTURE2D_DESC bufferDesc = CD3D11_TEXTURE2D_DESC();
		bufferDesc.ArraySize			= 1;
		bufferDesc.BindFlags			= D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		bufferDesc.CPUAccessFlags		= 0;
		bufferDesc.Format				= DXGI_FORMAT_B8G8R8A8_UNORM;
		bufferDesc.Width				= W;
		bufferDesc.Height				= H;
		bufferDesc.MipLevels			= 1;
		bufferDesc.MiscFlags			= 0;
		bufferDesc.SampleDesc.Count		= NumAASamples;
		bufferDesc.SampleDesc.Quality	= 0;
		bufferDesc.Usage				= D3D11_USAGE_DEFAULT;

		// Metallicafan212:	Now call create on this
		hr = m_D3DDevice->CreateTexture2D(&bufferDesc, nullptr, Tex->RTTex.GetAddressOf());

		ThrowIfFailed(hr);

		// Metallicafan212:	If we're using MSAA, we need to resolve first!!!!!
		if (NumAASamples > 1)
		{
			bufferDesc.SampleDesc.Count = 1;

			//hr = m_D3DDevice->CreateTexture2D(&bufferDesc, nullptr, Tex->NonMSAATex.GetAddressOf());

			//ThrowIfFailed(hr);
		}

		bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

		// Metallicafan212:	Create a duplicate texture for setting on surfaces/meshes
		hr = m_D3DDevice->CreateTexture2D(&bufferDesc, nullptr, Tex->RTTexCopy.GetAddressOf());

		ThrowIfFailed(hr);

		// Metallicafan212:	Now a render target view from this
		CD3D11_RENDER_TARGET_VIEW_DESC rtVDesc = CD3D11_RENDER_TARGET_VIEW_DESC();
		rtVDesc.Format				= DXGI_FORMAT_B8G8R8A8_UNORM;
		rtVDesc.ViewDimension		= (NumAASamples > 1 ? D3D11_RTV_DIMENSION_TEXTURE2DMS : D3D11_RTV_DIMENSION_TEXTURE2D);
		rtVDesc.Texture2D.MipSlice	= 0;

		hr = m_D3DDevice->CreateRenderTargetView(Tex->RTTex.Get(), &rtVDesc, Tex->RTView.GetAddressOf());

		ThrowIfFailed(hr);

		// Metallicafan212:	Now the shader bind
		CD3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = CD3D11_SHADER_RESOURCE_VIEW_DESC();
		srvDesc.Format						= DXGI_FORMAT_B8G8R8A8_UNORM;
		srvDesc.ViewDimension				= D3D11_SRV_DIMENSION_TEXTURE2D;//D3D11_SRV_DIMENSION_TEXTURE2DMS;
		srvDesc.Texture2D.MostDetailedMip	= 0;
		srvDesc.Texture2D.MipLevels			= 1;

		/*
		// Metallicafan212:	We do a resolve to render this texture in
		if (NumAASamples > 1)
		{
			hr = m_D3DDevice->CreateShaderResourceView(Tex->NonMSAATex.Get(), &srvDesc, Tex->RTSRView.GetAddressOf());
		}
		else
		{
			hr = m_D3DDevice->CreateShaderResourceView(Tex->RTTex.Get(), &srvDesc, Tex->RTSRView.GetAddressOf());
		}
		*/

		// Metallicafan212:	Use the copy
		hr = m_D3DDevice->CreateShaderResourceView(Tex->RTTexCopy.Get(), &srvDesc, Tex->RTSRView.GetAddressOf());
		
		ThrowIfFailed(hr);

		// Metallicafan212:	Now create the depth info
		bufferDesc.BindFlags	   &= ~D3D11_BIND_RENDER_TARGET;
		bufferDesc.BindFlags	   |= D3D11_BIND_DEPTH_STENCIL;
		bufferDesc.Format			= DSTTexFormat;//DXGI_FORMAT_R24G8_TYPELESS;
		bufferDesc.SampleDesc.Count = NumAASamples;

		CD3D11_DEPTH_STENCIL_VIEW_DESC dtVDesc = CD3D11_DEPTH_STENCIL_VIEW_DESC();
		dtVDesc.Flags				= 0;
		dtVDesc.Format				= DSTFormat;//DXGI_FORMAT_D24_UNORM_S8_UINT;
		dtVDesc.ViewDimension		= NumAASamples > 1 ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
		dtVDesc.Texture2D.MipSlice	= 0;

		srvDesc.Format				= DSTSRVFormat;//DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		srvDesc.ViewDimension		= (NumAASamples > 1 ? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D);//D3D11_SRV_DIMENSION_TEXTURE2DMS;

		hr = m_D3DDevice->CreateTexture2D(&bufferDesc, nullptr, &Tex->DTTex);

		ThrowIfFailed(hr);

		// Metallicafan212:	Now the rest of the info
		hr = m_D3DDevice->CreateDepthStencilView(Tex->DTTex.Get(), &dtVDesc, Tex->DTView.GetAddressOf());

		ThrowIfFailed(hr);

		// Metallicafan212:	TODO! We don't currently allow binding of a depth texture into shaders!
		hr = m_D3DDevice->CreateShaderResourceView(Tex->DTTex.Get(), &srvDesc, Tex->DTSRView.GetAddressOf());

		ThrowIfFailed(hr);

#if DX11_HP2
		// Metallicafan212:	Create a RT for the local render target!
		hr = Tex->RTTex->QueryInterface(IID_PPV_ARGS(Tex->RTDXGI.GetAddressOf()));//m_D3DSwapChain->GetBuffer(0, IID_PPV_ARGS(&m_DXGISurf));

		ThrowIfFailed(hr);

		D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_HARDWARE, D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED));

		hr = m_D2DFact->CreateDxgiSurfaceRenderTarget(Tex->RTDXGI.Get(), &props, Tex->RTD2D.GetAddressOf());

		ThrowIfFailed(hr);

		Tex->RTD2D->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
		Tex->RTD2D->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE);
#endif

	}

	// Metallicafan212:	Keep track of it!
	if (Tex != nullptr)
		RTTextures.AddItem(Tex);

	return Tex;

	unguard;
}

void UICBINDx11RenderDevice::SetRenderTargetTexture(UTexture* Tex) 
{
	guard(UICBINDx11RenderDevice::SetRenderTargetTexture);

	UDX11RenderTargetTexture* RT = Cast<UDX11RenderTargetTexture>(Tex);

	// Metallicafan212:	Error check
	if(RT != nullptr && RT == BoundRT)
		return;

	if (RT != nullptr)
	{
		EndBuffering();

		// Metallicafan212:	Set the RT and DT to the ones we want
		m_RenderContext->OMSetRenderTargets(1, RT->RTView.GetAddressOf(), RT->DTView.Get());

		// Metallicafan212:	Clear both!!!
		//m_RenderContext->ClearRenderTargetView(RT->RTView.Get(), DirectX::Colors::Black);
		//m_RenderContext->ClearDepthStencilView(RT->DTView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		// Metallicafan212:	Add it onto the array
		RTStack.AddItem(RT);

		BoundRT = RT;

#if DX11_HP2
		// Metallicafan212:	Set the D2DRT as well
		m_CurrentD2DRT = RT->RTD2D.Get();
#endif
	}
	else
	{
		// Metallicafan212:	Just use the normal function
		RestoreRenderTarget();
	}


	unguard;
}

void UICBINDx11RenderDevice::ClearRenderTargetTexture(UTexture* Tex, FPlane ClearColor)
{
	guard(UICBINDx11RenderDevice::ClearRenderTargetTexture);

	// Metallicafan212:	Allow for clearing the RT to a specific color
	UDX11RenderTargetTexture* RT = Cast<UDX11RenderTargetTexture>(Tex);

	// Metallicafan212:	Error check
	if (RT != nullptr)
	{
		m_RenderContext->ClearRenderTargetView(RT->RTView.Get(), &ClearColor.X);
	}

	unguard;
}

void UICBINDx11RenderDevice::RestoreRenderTarget()
{
	guard(UICBINDx11RenderDevice::RestoreRenderTarget);

	EndBuffering();

	// Metallicafan212:	Pop our RT off the stack
	if (RTStack.Num())
	{
		RTStack.Remove(RTStack.Num() - 1);
	}

	// Metallicafan212:	Reset
	if (RTStack.Num() == 0)
	{
		if (FrameShaderVars.bDoSelection)//m_HitData != nullptr)
		{
			m_RenderContext->OMSetRenderTargets(1, &m_BackBuffRT, m_SelectionDSV);
		}
		else
		{
			m_RenderContext->OMSetRenderTargets(1, &m_D3DScreenRTV, m_D3DScreenDSV);
		}

		// Metallicafan212:	Resolve the RT Texture
		if (BoundRT != nullptr)
		{
			//m_RenderContext->Flush();
			if (NumAASamples > 1)
			{
				m_RenderContext->ResolveSubresource(BoundRT->RTTexCopy.Get(), 0, BoundRT->RTTex.Get(), 0, DXGI_FORMAT_B8G8R8A8_UNORM);
			}
			else
			{
				m_RenderContext->CopyResource(BoundRT->RTTexCopy.Get(), BoundRT->RTTex.Get());
			}
		}

		BoundRT = nullptr;

#if DX11_HP2
		// Metallicafan212:	Restore the D2D render target
		m_CurrentD2DRT = m_D2DRT;
#endif
	}
	else
	{
		// Metallicafan212:	Get the next RT
		UDX11RenderTargetTexture* NewRT = RTStack(RTStack.Num() - 1);

		// Metallicafan212:	Set the RT and DT to the ones we want
		m_RenderContext->OMSetRenderTargets(1, NewRT->RTView.GetAddressOf(), NewRT->DTView.Get());

		// Metallicafan212:	Resolve the currently bound RT Texture
		if (BoundRT != nullptr)
		{
			if (NumAASamples > 1)
			{
				m_RenderContext->ResolveSubresource(BoundRT->RTTexCopy.Get(), 0, BoundRT->RTTex.Get(), 0, DXGI_FORMAT_B8G8R8A8_UNORM);
			}
			else
			{
				m_RenderContext->CopyResource(BoundRT->RTTexCopy.Get(), BoundRT->RTTex.Get());
			}
		}

		// Metallicafan212:	Set it to the next RT
		BoundRT = NewRT;

		// Metallicafan212:	Also set the D2D target

#if DX11_HP2
		// Metallicafan212:	Set the D2DRT as well
		m_CurrentD2DRT = BoundRT->RTD2D.Get();
#endif
	}


	unguard;
}