#include "ICBINDx11Drv.h"

// Metallicafan212:	TODO!
IMPLEMENT_CLASS(UICBINDx11RenderDevice);
IMPLEMENT_PACKAGE(ICBINDx11Drv);

void UICBINDx11RenderDevice::SetupDevice()
{
	guard(UICBINDx11RenderDevice::SetupDevice);

#if DX11_HP2
	// Metallicafan212:	Release all the fonts
	for (TMap<FString, IDWriteTextFormat*>::TIterator It(FontMap); It; ++It)
	{
		It.Value()->Release();
	}
	FontMap.Empty();
#endif

	// Metallicafan212:	Cleanout the texture cache
	Flush(0);

	// Metallicafan212:	Cleanup the blend states
	for (TMap<FPLAG, ID3D11BlendState*>::TIterator It(BlendMap); It; ++It)
	{
		It.Value()->Release();
	}
	BlendMap.Empty();

	// Metallicafan212:	Cleanup the sampler map
	FlushTextureSamplers();

	ClearRTTextures();

#if DX11_HP2
	// Metallicafan212:	HP2 specific


	SAFE_RELEASE(m_D2DRT);
	SAFE_RELEASE(m_D2DFact);
	SAFE_RELEASE(m_D2DWriteFact);
	SAFE_RELEASE(m_DXGISurf);
	SAFE_RELEASE(m_TextParams);
#endif

	SAFE_RELEASE(m_D3DQuery);
	SAFE_RELEASE(m_D3DDebug);
	SAFE_RELEASE(m_D3DQueue);
	SAFE_RELEASE(m_D3DSwapChain);
	SAFE_RELEASE(VertexBuffer);
	SAFE_RELEASE(IndexBuffer);

	// Metallicafan212:	Render target and back buffer texture
	SAFE_RELEASE(m_BackBuffTex);
	SAFE_RELEASE(m_BackBuffRT);
	SAFE_RELEASE(m_BackBuffUAV);
	SAFE_RELEASE(m_ScreenBuffTex);
	SAFE_RELEASE(m_D3DScreenRTV);
	SAFE_RELEASE(m_ScreenRTSRV);

	//SAFE_RELEASE(m_ScreenOpacityTex);
	//SAFE_RELEASE(m_ScreenOpacityRTSRV);
	//SAFE_RELEASE(m_D3DScreenOpacityRTV);

	// Metallicafan212:	Depth stencil target
	SAFE_RELEASE(m_ScreenDSTex);
	SAFE_RELEASE(m_D3DScreenDSV);
	SAFE_RELEASE(m_ScreenDTSRV);


	// Metallicafan212:	TODO! Do we need to make shaders each time?
	SAFE_DELETE(FTileShader);
	SAFE_DELETE(FGenShader);
	SAFE_DELETE(FResScaleShader);
	SAFE_DELETE(FMeshShader);
	SAFE_DELETE(FSurfShader);
	SAFE_DELETE(FLineShader);
	SAFE_DELETE(FMSAAShader);

	FlushRasterStates();

#if USE_COMPUTE_SHADER
	SAFE_DELETE(FMshLghtCompShader);
#endif

	// Metallicafan212:	No bind texture/sampler
	SAFE_RELEASE(BlankTexture);
	SAFE_RELEASE(BlankResourceView);
	SAFE_RELEASE(BlankSampler);

	// Metallicafan212:	Depth states for PF_Occlude
	SAFE_RELEASE(m_DefaultZState);
	SAFE_RELEASE(m_DefaultNoZState);

	// Metallicafan212:	Clear the mode, if the device context is already existing!
	if (m_D3DDeviceContext != nullptr)
	{
		m_D3DDeviceContext->ClearState();
		m_D3DDeviceContext->Flush();
	}

	SAFE_RELEASE(m_D3DDeviceContext);
	SAFE_RELEASE(m_D3DDevice);

	// Metallicafan212:	Set the raster state to an invalid value
	CurrentRasterState = DXRS_MAX;

	// Metallicafan212:	Init DX11
	//					We want to use feature level 11_1 for compute shaders
	D3D_FEATURE_LEVEL FLList[7] = 
	{ 
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1,
	};

	INT FLCount = ARRAY_COUNT(FLList);

	D3D_FEATURE_LEVEL* FLPtr = &FLList[0];

	// Metallicafan212:	Make it single threaded for more performance?
	UINT Flags =	D3D11_CREATE_DEVICE_BGRA_SUPPORT
				//|	D3D11_CREATE_DEVICE_SINGLETHREADED
				|	D3D11_CREATE_DEVICE_DEBUG
				;
	
	GLog->Logf(TEXT("DX11: Creating device with the maximum feature level"));

MAKE_DEVICE:
	HRESULT hr = S_OK;

	// Metallicafan212:	TODO! May not even support this lol
	//					It probably wouldn't even run any better, and you need a ID3D12Device
	/*
	if (bUseD3D11On12)
	{
		hr = D3D11On12CreateDevice(nullptr, Flags, FLPtr, FLCount, nullptr, 0, 0, &m_D3DDevice, &m_D3DDeviceContext, &m_FeatureLevel);

		if (FAILED(hr))
		{
			// Metallicafan212:	Not supported? Jump down
			goto NORMAL_DX11;
		}
		else
		{
			GLog->Logf(TEXT("DX11: Created device using D3D11On12"));
		}
	}
	else
	*/
	{
	NORMAL_DX11:
		hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, Flags, FLPtr, FLCount, D3D11_SDK_VERSION, &m_D3DDevice, &m_FeatureLevel, &m_D3DDeviceContext);
	}

	// Metallicafan212:	Check if it failed due to the debug layer
	if (FAILED(hr) && (Flags & D3D11_CREATE_DEVICE_DEBUG))
	{
		GLog->Logf(TEXT("DX11: Removing the debug layer from the device flags"));
		Flags &= ~D3D11_CREATE_DEVICE_DEBUG;

		goto MAKE_DEVICE;
	}
	else if (FAILED(hr))
	{
		// Metallicafan212:	Try a lower feature level???
		FLCount--;

		if (FLCount <= 0)
		{
			// Metallicafan212:	We failed, so throw us out
			ThrowIfFailed(hr);
		}

		// Metallicafan212:	Update the ptr
		FLPtr++;

		goto MAKE_DEVICE;
	}

	ThrowIfFailed(hr);

	// Metallicafan212:	Log the feature level
	const TCHAR* FLStr = nullptr;

	switch (m_FeatureLevel)
	{
		case D3D_FEATURE_LEVEL_11_1:
		{
			FLStr = TEXT("11.1");
			break;
		}

		case D3D_FEATURE_LEVEL_11_0:
		{
			FLStr = TEXT("11.0");
			break;
		}

		case D3D_FEATURE_LEVEL_10_1:
		{
			FLStr = TEXT("10.1");
			break;
		}

		case D3D_FEATURE_LEVEL_10_0:
		{
			FLStr = TEXT("10.0");
			break;
		}

		case D3D_FEATURE_LEVEL_9_3:
		{
			FLStr = TEXT("9.3");
			break;
		}

		case D3D_FEATURE_LEVEL_9_2:
		{
			FLStr = TEXT("9.2");
			break;
		}

		case D3D_FEATURE_LEVEL_9_1:
		{
			FLStr = TEXT("9.1");
			break;
		}
	}

	GLog->Logf(TEXT("DX11: Using feature level %s"), FLStr);

	// Metallicafan212:	Make the query
	D3D11_QUERY_DESC qDesc = { D3D11_QUERY_EVENT, 0 };
	m_D3DDevice->CreateQuery(&qDesc, &m_D3DQuery);

	GLog->Logf(TEXT("DX11: Creating shaders"));

	// Metallicafan212:	Make the shader
	FTileShader			= new FD3DTileShader(this);

	FGenShader			= new FD3DGenericShader(this);

	FResScaleShader		= new FD3DResScalingShader(this);

#if USE_COMPUTE_SHADER
	FMshLghtCompShader	= new FD3DLghtMshCompShader(this);
#endif

	FMeshShader			= new FD3DMeshShader(this);

	FSurfShader			= new FD3DSurfShader(this);

	FLineShader			= new FD3DLineShader(this);

	FMSAAShader			= new FD3DMSAAShader(this);

	// Metallicafan212:	Setup the debug info
#if 1//_DEBUG

	if ((Flags & D3D11_CREATE_DEVICE_DEBUG))
	{
		GLog->Logf(TEXT("DX11: Grabbing debug interface"));

		hr = m_D3DDevice->QueryInterface(__uuidof(ID3D11Debug), (void**)&m_D3DDebug);

		ThrowIfFailed(hr);

		hr = m_D3DDebug->QueryInterface(__uuidof(ID3D11InfoQueue), (void**)&m_D3DQueue);

		ThrowIfFailed(hr);

		// Metallicafan212:	To catch issues, will be removed when the renderer... works...
		if (!bDisableDebugInterface)
		{
			m_D3DQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, 1);
			m_D3DQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, 1);
		}

		D3D11_MESSAGE_ID hide[] =
		{
			D3D11_MESSAGE_ID_SETPRIVATEDATA_CHANGINGPARAMS,
			// TODO: Add more message IDs here as needed.
		};
		D3D11_INFO_QUEUE_FILTER filter = {};
		filter.DenyList.NumIDs = static_cast<UINT>(ARRAYSIZE(hide));
		filter.DenyList.pIDList = hide;
		hr = m_D3DQueue->AddStorageFilterEntries(&filter);

		ThrowIfFailed(hr);
	}
#endif

	// Metallicafan212:	Setup vertex buffers here
	m_VertexBuffSize			= sizeof(FD3DVert)	* VBUFF_SIZE;
	m_IndexBuffSize				= sizeof(_WORD)		* IBUFF_SIZE;
	D3D11_BUFFER_DESC VertDesc	= { m_VertexBuffSize, D3D11_USAGE_DYNAMIC, D3D11_BIND_VERTEX_BUFFER, D3D11_CPU_ACCESS_WRITE, NULL, 0 };
	D3D11_BUFFER_DESC IndexDesc = { m_IndexBuffSize, D3D11_USAGE_DYNAMIC, D3D11_BIND_INDEX_BUFFER, D3D11_CPU_ACCESS_WRITE, NULL, 0 };

	GLog->Logf(TEXT("DX11: Creating vertex buffer"));

	hr = m_D3DDevice->CreateBuffer(&VertDesc, nullptr, &VertexBuffer);

	ThrowIfFailed(hr);

	GLog->Logf(TEXT("DX11: Creating index buffer"));

	// Metallicafan212:	Index buffer
	hr = m_D3DDevice->CreateBuffer(&IndexDesc, nullptr, &IndexBuffer);

	ThrowIfFailed(hr);

#if DX11_HP2
	GLog->Logf(TEXT("DX11: Creating D2D1 factory1"));

	// Metallicafan212:	D2D manager
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, IID_PPV_ARGS(&m_D2DFact));

	ThrowIfFailed(hr);

	GLog->Logf(TEXT("DX11: Creating DWrite factory"));

	// Metallicafan212:	Text related factory
	hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory1),  (IUnknown**)&m_D2DWriteFact);

	ThrowIfFailed(hr);
#endif

	// Metallicafan212:	Query for a supported depth stencil format
	UINT bSupportsFormat = 0;
	hr = m_D3DDevice->CheckFormatSupport(DXGI_FORMAT_D32_FLOAT_S8X24_UINT, &bSupportsFormat);

	if (FAILED(hr) || !bSupportsFormat)
	{
		GLog->Logf(TEXT("DX11: Using 24 bit depth buffer"));
		DSTFormat		= DXGI_FORMAT_D24_UNORM_S8_UINT;
		DSTTexFormat	= DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		DSTSRVFormat	= DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	}
	else
	{
		GLog->Logf(TEXT("DX11: Using 32bit depth buffer"));
		DSTFormat		= DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
		DSTTexFormat	= DXGI_FORMAT_R32G8X24_TYPELESS;
		DSTSRVFormat	= DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;

	}

	SetRasterState(DXRS_Normal);

	// Metallicafan212:	Reset the shader state
	GlobalShaderVars = FGlobalShaderVars();

	unguard;
}

// Metallicafan212:	UGH, I'm a fucking idiot. You can't mix MSAA and non-MSAA rendering in DX11..... So I'm going to have to come up with some alternative method to render font tiles correctly
//					Per this page: https://learn.microsoft.com/en-us/windows/win32/api/d3d11/ns-d3d11-d3d11_rasterizer_desc
//					"Note  For feature levels 9.1, 9.2, 9.3, and 10.0, if you set MultisampleEnable to FALSE, 
//					 the runtime renders all points, lines, and triangles without anti-aliasing even for render targets with a sample count greater than 1. 
//					 For feature levels 10.1 and higher, the setting of MultisampleEnable has no effect on points and triangles with regard to MSAA and impacts only the selection of the line-rendering algorithm"
void UICBINDx11RenderDevice::SetRasterState(DWORD State)
{
	guard(UICBINDx11RenderDevice::SetRasterState);

	// Metallicafan212:	See if the raster state differs
	//					TODO! Add more flags
	State &= (DXRS_Wireframe | DXRS_NoAA);

	// Metallicafan212:	Add on the extra raster flags
	State |= ExtraRasterFlags;

	if (State != CurrentRasterState)
	{
		// Metallicafan212:	End whatever rendering we're doing right now!
		EndBuffering();

		// Metallicafan212:	Find what needs to be added on to make it, if it doesn't exist yet
		ID3D11RasterizerState* m_s = RasterMap.FindRef(State);

		if (m_s == nullptr)
		{
			D3D11_RASTERIZER_DESC Desc;

			// Metallicafan212:	Default vars
			Desc.CullMode					= D3D11_CULL_NONE;
			Desc.DepthBias					= 0;
			Desc.DepthBiasClamp				= 0.0f;
			Desc.DepthClipEnable			= TRUE;
			Desc.FrontCounterClockwise		= FALSE;
			Desc.ScissorEnable				= FALSE;

			// Metallicafan212:	Now check the flags
			if (State & DXRS_Wireframe)
			{
				Desc.FillMode = D3D11_FILL_WIREFRAME;
			}
			else
			{
				Desc.FillMode = D3D11_FILL_SOLID;
			}

			// Metallicafan212:	TODO!!!! This does NOTHING in real DX11 modes!!!
			if (State & DXRS_NoAA)
			{
				Desc.AntialiasedLineEnable	= FALSE;
				Desc.MultisampleEnable		= FALSE;
			}
			else
			{
				Desc.AntialiasedLineEnable	= TRUE;
				Desc.MultisampleEnable		= TRUE;
			}

			HRESULT hr = m_D3DDevice->CreateRasterizerState(&Desc, &m_s);

			ThrowIfFailed(hr);

			// Metallicafan212:	Now set it on the map
			RasterMap.Set(State, m_s);
		}

		// Metallicafan212:	Set it
		if(m_s != nullptr)
			m_D3DDeviceContext->RSSetState(m_s);

		CurrentRasterState = State;
	}

	unguard;
}

// Metallicafan212:	Generalized functions for render device stuff (Init, Lock, Unlock, etc.)
UBOOL UICBINDx11RenderDevice::Init(UViewport* InViewport, INT NewX, INT NewY, INT NewColorBytes, UBOOL Fullscreen)
{
	guard(UICBINDx11RenderDevice::Init);

	// Metallicafan212:	Init pointers
	m_D3DDevice			= nullptr;
	m_D3DSwapChain		= nullptr;
	m_D3DDebug			= nullptr;
	m_D3DQueue			= nullptr;

	// Metallicafan212:	Render target stuff
	m_ScreenBuffTex		= nullptr;
	m_D3DScreenRTV		= nullptr;
	m_ScreenRTSRV		= nullptr;

	//m_ScreenOpacityTex		= nullptr;
	//m_D3DScreenOpacityRTV	= nullptr;
	//m_ScreenOpacityRTSRV	= nullptr;

	// Metallicafan212:	Depth stencil target stuff
	m_ScreenDSTex		= nullptr;
	m_ScreenDTSRV		= nullptr;
	m_D3DScreenDSV		= nullptr;

#if DX11_HP2
	// Metallicafan212:	HP2 specific on-screen string drawing
	m_D2DRT				= nullptr;
	m_D2DFact			= nullptr;
	m_DXGISurf			= nullptr;
	m_TextParams		= nullptr;
#endif

	// Metallicafan212:	Shader pointers
	FMeshShader			= nullptr;
	FSurfShader			= nullptr;
	FLineShader			= nullptr;
	FTileShader			= nullptr;
	FGenShader			= nullptr;
	FResScaleShader		= nullptr;
	FMSAAShader			= nullptr;
#if USE_COMPUTE_SHADER
	FMshLghtCompShader	= nullptr;
#endif

	// Metallicafan212:	Blank tex, resource, and samplers for defaults
	BlankTexture		= nullptr;
	BlankResourceView	= nullptr;
	BlankSampler		= nullptr;

	// Metallicafan212:	Raster states for turning on and off occlusion
	m_DefaultZState		= nullptr;
	m_DefaultNoZState	= nullptr;

	ScreenSamp			= nullptr;

	CurrentRasterState	= DXRS_MAX;

	bLastFullscreen = bFullscreen = 0;

	appMemzero(BoundTextures, sizeof(BoundTextures));

	CurrentPolyFlags	= 0;

	Viewport			= InViewport;

	// Metallicafan212:	Get the style now!!!
	ViewExtendedStyle = GetWindowLongPtr((HWND)Viewport->GetWindow(), GWL_EXSTYLE);

	// Metallicafan212:	Save the values
	//					We also have to clamp here since unreal could pass bad values (the editor opening, for example)
	SizeX		= Max(NewX, 2);
	SizeY		= Max(NewY, 2);

	bFullscreen	= Fullscreen;

	GLog->Logf(TEXT("DX11: Starting"));

	SetupDevice();

	// Metallicafan212:	Set res???
	SetRes(SizeX, SizeY, NewColorBytes, bFullscreen);

	// Metallicafan212:	Get all display modes
	//					Copied and modified from the DX9 driver (since I'm a lazy bastard)
	for (INT i = 0; ; i++)
	{
		DEVMODE Tmp;
		appMemzero(&Tmp, sizeof(Tmp));
		Tmp.dmSize = sizeof(Tmp);

		if (!EnumDisplaySettings(NULL, i, &Tmp))
		{
			break;
		}
		Modes.AddUniqueItem(FPlane(Tmp.dmPelsWidth, Tmp.dmPelsHeight, Tmp.dmBitsPerPel, Tmp.dmDisplayFrequency));
	}

	GLog->Logf(TEXT("DX11: Registering supported texture formats"));

	// Metallicafan212:	Assemble the supported texture types
	RegisterTextureFormat(TEXF_P8, DXGI_FORMAT_R8G8B8A8_UNORM, 1, 4, &FD3DTexType::RawPitch, nullptr, P8ToRGBA);

	RegisterTextureFormat(TEXF_RGBA7, DXGI_FORMAT_B8G8R8A8_UNORM, 1, 4, &FD3DTexType::RawPitch, nullptr, RGBA7To8);
	
	// Metallicafan212:	I have standardized the TEXF enum between UT469 and HP2 (since BC6H is currently unused and RGBA8 was already called ARGB in the editor/implemented as BGRA8)

	// Metallicafan212:	Raw BGRA texture
#if DX11_UT_469 || DX11_HP2
	RegisterTextureFormat(TEXF_BGRA8, DXGI_FORMAT_B8G8R8A8_UNORM, 0, 4, &FD3DTexType::RawPitch);
#else
	RegisterTextureFormat(TEXF_RGBA8, DXGI_FORMAT_B8G8R8A8_UNORM, 0, 4, &FD3DTexType::RawPitch);
#endif

	// Metallicafan212:	These are all supported by DX11.1
	//					In the future, I will query for support (or use the DX feature level???)
#if DX11_UT_469
	RegisterTextureFormat(TEXF_DXT1, DXGI_FORMAT_BC1_UNORM_SRGB, 0, 8, &FD3DTexType::BlockCompressionPitch);
#else
	// Metallicafan212:	HP2 uses non-SRGB DXT1, so we need to break that off, otherwise the color will be half'd
	RegisterTextureFormat(TEXF_DXT1, DXGI_FORMAT_BC1_UNORM, 0, 8, &FD3DTexType::BlockCompressionPitch);
#endif

	RegisterTextureFormat(TEXF_DXT3, DXGI_FORMAT_BC2_UNORM, 0, 16, &FD3DTexType::BlockCompressionPitch);

	RegisterTextureFormat(TEXF_DXT5, DXGI_FORMAT_BC3_UNORM, 0, 16, &FD3DTexType::BlockCompressionPitch);

	RegisterTextureFormat(TEXF_BC4, DXGI_FORMAT_BC4_UNORM, 0, 8, &FD3DTexType::BlockCompressionPitch);

	RegisterTextureFormat(TEXF_BC5, DXGI_FORMAT_BC5_UNORM, 0, 16, &FD3DTexType::BlockCompressionPitch);

	// Metallicafan212:	I'll take whatever need to be done for BC6, as it's currently unimplemented in HP2
	//					So, if the format needs to be changed, just change it and I'll make it match when I finally finish implementing it
	//					AMD compressonator wasn't producing this correctly (which is why I disabled it)
#if DX11_UT_469 || DX11_HP2
	RegisterTextureFormat(TEXF_BC6H, DXGI_FORMAT_BC6H_UF16, 0, 16, &FD3DTexType::BlockCompressionPitch);
//#else
//	RegisterTextureFormat(TEXF_BC6H, DXGI_FORMAT_BC6H_UF16, 0, 16, &FD3DTexType::BlockCompressionPitch);
#endif

	RegisterTextureFormat(TEXF_BC7, DXGI_FORMAT_BC7_UNORM, 0, 16, &FD3DTexType::BlockCompressionPitch);

	return 1;

	unguard;
}

void UICBINDx11RenderDevice::SetupResources()
{
	guard(UICBINDx11RenderDevice::SetupResources);

	GLog->Logf(TEXT("DX11: Setting up resources"));

	// Metallicafan212:	Clamp the user options!
	ClampUserOptions();

	// Metallicafan212:	Reset our bound textures
	for (INT i = 0; i < MAX_TEXTURES; i++)
	{
		//SetTexture(0, nullptr, 0);
		m_D3DDeviceContext->PSSetShaderResources(i, 0, nullptr);
		m_D3DDeviceContext->PSSetSamplers(i, 0, nullptr);
	}

	// Metallicafan212:	CATCH RT TEXTURES!!!!
	RestoreRenderTarget();

	// Metallicafan212:	Clear the RT textures
	ClearRTTextures();

	// Metallicafan212:	Clear any set RT/DC
	m_D3DDeviceContext->OMSetRenderTargets(0, nullptr, nullptr);	

	// Metallicafan212:	Reset the raster state
	CurrentRasterState = DXRS_MAX;

	//m_D3DDeviceContext->RSSetState(m_DefaultRasterState);
	SetRasterState(0);

	// Metallicafan212:	Render target and back buffer texture
	SAFE_RELEASE(m_BackBuffTex);
	SAFE_RELEASE(m_BackBuffRT);
	SAFE_RELEASE(m_BackBuffUAV);
	SAFE_RELEASE(m_ScreenBuffTex);
	SAFE_RELEASE(m_D3DScreenRTV);
	SAFE_RELEASE(m_ScreenRTSRV);

	//SAFE_RELEASE(m_ScreenOpacityTex);
	//SAFE_RELEASE(m_ScreenOpacityRTSRV);
	//SAFE_RELEASE(m_D3DScreenOpacityRTV);

	// Metallicafan212:	Depth stencil target
	SAFE_RELEASE(m_ScreenDSTex);
	SAFE_RELEASE(m_D3DScreenDSV);
	SAFE_RELEASE(m_ScreenDTSRV);

#if DX11_HP2
	// Metallicafan212:	TODO! HP2 specific
	SAFE_RELEASE(m_D2DRT);
	SAFE_RELEASE(m_DXGISurf);
	SAFE_RELEASE(m_TextParams);
#endif

	// Metallicafan212:	No bind texture/sampler
	SAFE_RELEASE(BlankTexture);
	SAFE_RELEASE(BlankResourceView);
	SAFE_RELEASE(BlankSampler);

	// Metallicafan212:	Depth stencil states
	SAFE_RELEASE(m_DefaultZState);
	SAFE_RELEASE(m_DefaultNoZState);

	// Metallicafan212:	Recreate the texture samplers
	FlushTextureSamplers();

	// Metallicafan212:	Now flush
	m_D3DDeviceContext->Flush();

	// Metallicafan212:	Create or resize the swap chain
	HRESULT hr = S_OK;

	if (m_D3DSwapChain == nullptr)
	{
	SetupSwap:

		GLog->Logf(TEXT("DX11: Setting up swap chain"));

		DXGI_SWAP_CHAIN_DESC Swap;
		appMemzero(&Swap, sizeof(DXGI_SWAP_CHAIN_DESC));

		// Metallicafan212:	Setup the swap chain info
		//					We need the DXGI device for all this
		//					This is all copied from a M$ sample, sue me
		//					https://github.com/walbourn/directx-vs-templates/blob/main/d3d11game_win32/Game.cpp
		IDXGIDevice1* dxgiDevice;
		hr = m_D3DDevice->QueryInterface(__uuidof(IDXGIDevice1), (void**)&dxgiDevice);

		// Identify the physical adapter (GPU or card) this device is running on.
		IDXGIAdapter1* dxgiAdapter;
		hr = dxgiDevice->GetAdapter((IDXGIAdapter**)&dxgiAdapter);

		ThrowIfFailed(hr);

		// Metallicafan212:	Get the GPU info
		DXGI_ADAPTER_DESC1 AdDesc;

		dxgiAdapter->GetDesc1(&AdDesc);

		debugf(TEXT("Metallicafan212's farm-raised Directx 11 renderer"));
		debugf(TEXT("Version                 : %s"),	D3D_DRIVER_VERSION);
		debugf(TEXT("Compiled                : %s"),	COMPILED_AT);
		//debugf(TEXT("D3D adapter driver      : %s"), appFromAnsi(ident.Driver));
		debugf(TEXT("D3D adapter description : %s"),	AdDesc.Description);
		// Metallicafan212:	TODO! In 32bit mode, use %lu instead
#if UNREAL32
		debugf(TEXT("D3D adapter VRam        : %dGB (%luMB)"), appRound((AdDesc.DedicatedVideoMemory / 1073741824.0)), (AdDesc.DedicatedVideoMemory / 1048576));
#else
		debugf(TEXT("D3D adapter VRam        : %dGB (%lluMB)"), appRound((AdDesc.DedicatedVideoMemory / 1073741824.0)), (AdDesc.DedicatedVideoMemory / 1048576));
#endif
		//debugf(TEXT("D3D adapter name        : %s"), appFromAnsi(ident.DeviceName));
		//debugf(TEXT("D3D adapter id          : 0x%04X:0x%04X"), ident.VendorId, ident.DeviceId);

		// Metallicafan212:	Check and log what vendor it is
		if (AdDesc.VendorId == 0x1002 || AdDesc.VendorId == 0x1022)
		{
			bIsAMD = 1;
			debugf(TEXT("D3D adapter vendor      : AMD"));
		}
		else if (AdDesc.VendorId == 0x10DE)
		{
			debugf(TEXT("D3D adapter vendor      : NVidia"));
			bIsNV = 1;
		}
		// Metallicafan212:	Intel
		else if (AdDesc.VendorId == 0x163C || AdDesc.VendorId == 0x8086 || AdDesc.VendorId == 0x8087)
		{
			debugf(TEXT("D3D adapter vendor      : Intel"));
			bIsIntel = 1;
		}

		// Metallicafan212:	If to use the new Windows 10 modes. I only test if we're actually running on 10
		//					!GIsEditor is here because using the tearing mode does something fucky in DWM, changing the window in such a way that normal non-DX11 renderers can't draw to it
		//					I need to analyse and see what exactly it's modifying about the window and reverse that change
		bAllowTearing = (!GIsEditor
#if DX11_HP2
			&& GWin10
#endif
			);


		IDXGIFactory5* dxgiFactory5 = nullptr;
		hr = dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory5));

		IDXGIFactory2* dxgiFactory;

		if (FAILED(hr))
		{
			// Metallicafan212:	Get the DXGI factory2 only
			bAllowTearing = 0;
		}
		else
		{
			// Metallicafan212:	See if it supports the effect
			BOOL bSupportsTearing = 0;

			hr = dxgiFactory5->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &bSupportsTearing, sizeof(BOOL));

			if (FAILED(hr) || !bSupportsTearing)
			{
				if (FAILED(hr))
				{
					GLog->Logf(TEXT("DX11: Device does not support tearing"));
				}
				bAllowTearing = 0;
			}

		}

		if (bAllowTearing)
		{
			GLog->Logf(TEXT("DX11: Setting up swap chain with tearing support"));
		}

		// And obtain the factory object that created it.

		hr = dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory));

		ThrowIfFailed(hr);
		/*
		if (bAllowTearing)
		{
			HWND h = (HWND)Viewport->GetWindow();
			LONG_PTR s = GetWindowLongPtr(h, GWL_EXSTYLE);

			// Metallicafan212:	Add in WS_EX_NOREDIRECTIONBITMAP, since the freesync mode needs it, if we've removed it
			s |= WS_EX_NOREDIRECTIONBITMAP;

			SetWindowLongPtr(h, GWL_EXSTYLE, s);
			SetWindowPos(h, 0, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_DRAWFRAME | SWP_SHOWWINDOW);
		}
		*/

		bForceRGBA = 0;

		// Metallicafan212:	Describe the non-aa swap chain (MSAA is resolved in Unlock)
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.Width					= SizeX;
		swapChainDesc.Height				= SizeY;
		swapChainDesc.Format				= DXGI_FORMAT_B8G8R8A8_UNORM;
		swapChainDesc.SampleDesc.Count		= 1;
		swapChainDesc.SampleDesc.Quality	= 0;
		swapChainDesc.BufferUsage			= DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT | DXGI_USAGE_UNORDERED_ACCESS;
		swapChainDesc.BufferCount			= 2;
		//swapChainDesc.Scaling				= DXGI_SCALING_NONE;
		// Metallicafan212:	If we're on windows 10 or above, use the better DXGI mode
		swapChainDesc.SwapEffect			= (bAllowTearing ? DXGI_SWAP_EFFECT_FLIP_DISCARD : DXGI_SWAP_EFFECT_DISCARD);
		swapChainDesc.Flags					|= (bAllowTearing ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0);

		DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsSwapChainDesc = {};
		fsSwapChainDesc.Windowed			= !bFullscreen;//TRUE;

		GLog->Logf(TEXT("DX11: Creating swap chain for the window"));

		//LONG_PTR es = GetWindowLongPtr((HWND)Viewport->GetWindow(), GWL_EXSTYLE);
		//LONG_PTR s	= GetWindowLongPtr((HWND)Viewport->GetWindow(), GWL_STYLE);

		// Metallicafan212:	Create the swap chain now
		hr = dxgiFactory->CreateSwapChainForHwnd(
			m_D3DDevice,
			(HWND)Viewport->GetWindow(),
			&swapChainDesc,
			&fsSwapChainDesc,
			nullptr,
			&m_D3DSwapChain
		);

		ThrowIfFailed(hr);

		// Metallicafan212:	Make it stop messing with the window itself
		dxgiFactory->MakeWindowAssociation((HWND)Viewport->GetWindow(), DXGI_MWA_NO_WINDOW_CHANGES | DXGI_MWA_NO_ALT_ENTER);

		// Metallicafan212:	Release all the pointers
		dxgiFactory->Release();
		dxgiAdapter->Release();
		dxgiDevice->Release();

		// Metallicafan212:	Log the change
		//LONG_PTR esChange	= es ^ GetWindowLongPtr((HWND)Viewport->GetWindow(), GWL_EXSTYLE);
		//LONG_PTR sChange	= s ^ GetWindowLongPtr((HWND)Viewport->GetWindow(), GWL_STYLE);

		//GLog->Logf(TEXT("Window style changes are %llu and %llu"), esChange, sChange);

		SAFE_RELEASE(dxgiFactory5);
	}
	else
	{
		GLog->Logf(TEXT("DX11: Resizing swap chain"));

		if (bLastFullscreen != bFullscreen)
		{
			GLog->Logf(TEXT("DX11: Toggling fullscreen"));
			hr = m_D3DSwapChain->SetFullscreenState(bFullscreen, nullptr);

			ThrowIfFailed(hr);
		}

		// Metallicafan212:	Resize it
		hr = m_D3DSwapChain->ResizeBuffers(2, SizeX, SizeY, DXGI_FORMAT_B8G8R8A8_UNORM, (bAllowTearing ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0));

		if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
		{
			// Metallicafan212:	TODO! Recreate the device!!!
			// RecreateDevice();
			SetupDevice();
			goto SetupSwap;
		}
		else if(FAILED(hr))
		{
			appErrorf(TEXT("Failed to resize buffers with %lu"), hr);
		}
	}

	bLastFullscreen = bFullscreen;

	// Metallicafan212:	Get the closer value to the DX11 resource limit!!!!
	if (SizeY > SizeX)
	{
		ScaledSizeY = Clamp(SizeY * ResolutionScale, 1.f, static_cast<FLOAT>(D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION));

		// Metallicafan212:	Now get it back
		ResolutionScale = ScaledSizeY / SizeY;

		ScaledSizeX = SizeX * ResolutionScale;
	}
	else
	{
		ScaledSizeX = Clamp(SizeX * ResolutionScale, 1.f, static_cast<FLOAT>(D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION));

		// Metallicafan212:	Now get it back
		ResolutionScale = ScaledSizeX / SizeX;

		ScaledSizeY = SizeY * ResolutionScale;
	}

	if (ResolutionScale != 1.0f)
	{
		GLog->Logf(TEXT("DX11: Using a resolution scaling factor of %f. Effective resolution is %dx%d"), ResolutionScale, (INT)ScaledSizeX, (INT)ScaledSizeY);
	}

	GLog->Logf(TEXT("DX11: Setting up Render targets, Depth/Stencil, and Depth states"));

	// Metallicafan212:	Now obtain the back buffer
	hr = m_D3DSwapChain->GetBuffer(0, IID_PPV_ARGS(&m_BackBuffTex));

	ThrowIfFailed(hr);

	// Metallicafan212:	Get the texture size
	D3D11_TEXTURE2D_DESC d;
	m_BackBuffTex->GetDesc(&d);

	// Metallicafan212:	Now set the thread groups for the MSAA compute shader
	//					Since they're in groups of 32, we need to do an extra group if the screen size isn't % 32
	MSAAThreadX = appCeil(SizeX / 32.0f);
	MSAAThreadY = appCeil(SizeY / 32.0f);

	// Metallicafan212:	Create a render target view for it
	hr = m_D3DDevice->CreateRenderTargetView(m_BackBuffTex, nullptr, &m_BackBuffRT);

	ThrowIfFailed(hr);

	// Metallicafan212:	Now a UAV for a compute shader!!!
	CD3D11_UNORDERED_ACCESS_VIEW_DESC bUAV = CD3D11_UNORDERED_ACCESS_VIEW_DESC(m_BackBuffTex, D3D11_UAV_DIMENSION_TEXTURE2D, DXGI_FORMAT_B8G8R8A8_UNORM);

	hr = m_D3DDevice->CreateUnorderedAccessView(m_BackBuffTex, &bUAV, &m_BackBuffUAV);

	ThrowIfFailed(hr);

	// Metallicafan212:	Now create the MSAA target
	//					ClampUserOptions already checks what levels of MSAA are supported, and clamps to that
	CD3D11_TEXTURE2D_DESC RTMSAA = CD3D11_TEXTURE2D_DESC();
	RTMSAA.Width				= ScaledSizeX;
	RTMSAA.Height				= ScaledSizeY;
	RTMSAA.BindFlags			= D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	RTMSAA.Format				= DXGI_FORMAT_B8G8R8A8_UNORM;
	RTMSAA.MipLevels			= 1;
	RTMSAA.SampleDesc.Count		= NumAASamples;
	RTMSAA.SampleDesc.Quality	= 0;//D3D11_STANDARD_MULTISAMPLE_PATTERN;//0;
	RTMSAA.ArraySize			= 1;

	hr = m_D3DDevice->CreateTexture2D(&RTMSAA, nullptr, &m_ScreenBuffTex);

	ThrowIfFailed(hr);

	/*
	// Metallicafan212:	Now the hacked texture for opacity checking
	//					We only need one color
	RTMSAA.SampleDesc.Count		= 1;
	RTMSAA.Format				= DXGI_FORMAT_R32_FLOAT;

	hr = m_D3DDevice->CreateTexture2D(&RTMSAA, nullptr, &m_ScreenOpacityTex);

	ThrowIfFailed(hr);
	*/

	// Metallicafan212:	Now create the views
	hr = m_D3DDevice->CreateRenderTargetView(m_ScreenBuffTex, nullptr, &m_D3DScreenRTV);

	ThrowIfFailed(hr);

	/*
	hr = m_D3DDevice->CreateRenderTargetView(m_ScreenOpacityTex, nullptr, &m_D3DScreenOpacityRTV);

	ThrowIfFailed(hr);
	*/

	// Metallicafan212:	Create a shader resource view for MSAA resolving
	CD3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = CD3D11_SHADER_RESOURCE_VIEW_DESC();
	srvDesc.Format						= DXGI_FORMAT_B8G8R8A8_UNORM;
	srvDesc.ViewDimension				= NumAASamples > 1 ? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;//D3D11_SRV_DIMENSION_TEXTURE2DMS;
	srvDesc.Texture2D.MostDetailedMip	= 0;
	srvDesc.Texture2D.MipLevels			= 1;

	hr = m_D3DDevice->CreateShaderResourceView(m_ScreenBuffTex, &srvDesc, &m_ScreenRTSRV);

	ThrowIfFailed(hr);

	/*
	// Metallicafan212:	Now the shader resource for the opacity texture
	srvDesc.ViewDimension				= D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Format						= DXGI_FORMAT_R32_FLOAT;

	hr = m_D3DDevice->CreateShaderResourceView(m_ScreenOpacityTex, &srvDesc, &m_ScreenOpacityRTSRV);

	ThrowIfFailed(hr);
	*/


	// Metallicafan212:	Make the depth and stencil buffer
	//					TODO! Possibly use a higher quality format????
	CD3D11_TEXTURE2D_DESC depthStencilDesc = CD3D11_TEXTURE2D_DESC();
	depthStencilDesc.Format				= DSTTexFormat;//DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.ArraySize			= 1;
	depthStencilDesc.Width				= ScaledSizeX;
	depthStencilDesc.Height				= ScaledSizeY;
	depthStencilDesc.MipLevels			= 1;
	depthStencilDesc.SampleDesc.Count	= NumAASamples;
	depthStencilDesc.SampleDesc.Quality = 0;//D3D11_STANDARD_MULTISAMPLE_PATTERN;//0;
	depthStencilDesc.BindFlags			= D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	
	hr = m_D3DDevice->CreateTexture2D(&depthStencilDesc, nullptr, &m_ScreenDSTex);

	ThrowIfFailed(hr);

	// Metallicafan212:	Now we need to declare the view as the right format
	CD3D11_DEPTH_STENCIL_VIEW_DESC dtVDesc = CD3D11_DEPTH_STENCIL_VIEW_DESC();
	dtVDesc.Flags				= 0;
	dtVDesc.Format				= DSTFormat;//DXGI_FORMAT_D24_UNORM_S8_UINT;
	dtVDesc.ViewDimension		= NumAASamples > 1 ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
	dtVDesc.Texture2D.MipSlice	= 0;

	hr = m_D3DDevice->CreateDepthStencilView(m_ScreenDSTex, &dtVDesc, &m_D3DScreenDSV);

	ThrowIfFailed(hr);

	// Metallicafan212:	Now make the depth shader resource
	srvDesc.Format = DSTSRVFormat;
	hr = m_D3DDevice->CreateShaderResourceView(m_ScreenDSTex, &srvDesc, &m_ScreenDTSRV);

	ThrowIfFailed(hr);

#if DX11_HP2
	// Metallicafan212:	Get the D2D render target
	hr = m_ScreenBuffTex->QueryInterface(IID_PPV_ARGS(&m_DXGISurf));//m_D3DSwapChain->GetBuffer(0, IID_PPV_ARGS(&m_DXGISurf));

	ThrowIfFailed(hr);

	D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_HARDWARE, D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED));

	hr = m_D2DFact->CreateDxgiSurfaceRenderTarget(m_DXGISurf, &props, &m_D2DRT);

	ThrowIfFailed(hr);

	// Metallicafan212:	Set the main surface
	m_CurrentD2DRT = m_D2DRT;

	// Metallicafan212:	Setup AA now
	m_D2DRT->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
	m_D2DRT->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);

	// Metallicafan212:	Get the defaults
	IDWriteRenderingParams* Def = nullptr;
	m_D2DRT->GetTextRenderingParams(&Def);

	if (Def != nullptr)
	{
		// Metallicafan212:	Create the text rendering parameters from the defaults
		hr = m_D2DWriteFact->CreateCustomRenderingParams(Def->GetGamma(), Def->GetEnhancedContrast(), Def->GetClearTypeLevel(), Def->GetPixelGeometry(), DWRITE_RENDERING_MODE_NATURAL, &m_TextParams);

		ThrowIfFailed(hr);

		Def->Release();
	}
	else
	{
		// Metallicafan212:	Make our own....
		hr = m_D2DWriteFact->CreateCustomRenderingParams(1.0f, 0.0f, 0.0f, DWRITE_PIXEL_GEOMETRY_BGR, DWRITE_RENDERING_MODE_NATURAL, &m_TextParams);

		ThrowIfFailed(hr);
	}

	m_D2DRT->SetTextRenderingParams(m_TextParams);

	/*
	// Metallicafan212:	IMPORTANT!!! If we have AA, turn off AA in D2D!
	if (NumAASamples > 1)
	{
		m_D2DRT->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
		m_D2DRT->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE);
		
		// Metallicafan212:	Get the defaults
		IDWriteRenderingParams* Def = nullptr;
		m_D2DRT->GetTextRenderingParams(&Def);

		if (Def != nullptr)
		{
			// Metallicafan212:	Create the text rendering parameters from the defaults
			hr = m_D2DWriteFact->CreateCustomRenderingParams(Def->GetGamma(), Def->GetEnhancedContrast(), Def->GetClearTypeLevel(), Def->GetPixelGeometry(), DWRITE_RENDERING_MODE_GDI_NATURAL, &m_TextParams);

			ThrowIfFailed(hr);
		}
		else
		{
			// Metallicafan212:	Make our own....
			hr = m_D2DWriteFact->CreateCustomRenderingParams(1.0f, 0.0f, 0.0f, DWRITE_PIXEL_GEOMETRY_RGB, DWRITE_RENDERING_MODE_GDI_NATURAL, &m_TextParams);

			ThrowIfFailed(hr);
		}

		m_D2DRT->SetTextRenderingParams(m_TextParams);
	}
	*/
#endif

	// Metallicafan212:	Make a totally blank texture
	D3D11_TEXTURE2D_DESC Desc;
	appMemzero(&Desc, sizeof(Desc));

	Desc.Format				= DXGI_FORMAT_R8_UNORM;
	Desc.Width				= 2;
	Desc.Height				= 2;
	Desc.MipLevels			= 1;
	Desc.Usage				= D3D11_USAGE_DEFAULT;
	Desc.BindFlags			= D3D11_BIND_SHADER_RESOURCE;
	Desc.ArraySize			= 1;
	Desc.CPUAccessFlags		= D3D11_CPU_ACCESS_WRITE;
	Desc.SampleDesc.Count	= 1;
	hr = m_D3DDevice->CreateTexture2D(&Desc, nullptr, &BlankTexture);

	ThrowIfFailed(hr);

	// Metallicafan212:	Now the resource view for that
	D3D11_SHADER_RESOURCE_VIEW_DESC vDesc;
	appMemzero(&vDesc, sizeof(vDesc));

	vDesc.ViewDimension				= D3D11_SRV_DIMENSION_TEXTURE2D;
	vDesc.Texture2D.MipLevels		= 1;
	vDesc.Texture2D.MostDetailedMip = 0;
	vDesc.BufferEx.FirstElement		= 0;
	vDesc.Format					= Desc.Format;

	hr = m_D3DDevice->CreateShaderResourceView(BlankTexture, &vDesc, &BlankResourceView);

	ThrowIfFailed(hr);

	// Metallicafan212:	Now make a sampler
	D3D11_SAMPLER_DESC SDesc;
	appMemzero(&SDesc, sizeof(SDesc));

	SDesc.Filter			= D3D11_FILTER_ANISOTROPIC;
	SDesc.AddressU			= D3D11_TEXTURE_ADDRESS_WRAP;
	SDesc.AddressV			= D3D11_TEXTURE_ADDRESS_WRAP;
	SDesc.AddressW			= D3D11_TEXTURE_ADDRESS_WRAP;
	SDesc.MinLOD			= -FLT_MAX;
	SDesc.MaxLOD			= FLT_MAX;
	SDesc.MaxAnisotropy		= NumAFSamples;
	SDesc.ComparisonFunc	= D3D11_COMPARISON_NEVER;

	hr = m_D3DDevice->CreateSamplerState(&SDesc, &BlankSampler);

	ThrowIfFailed(hr);

	m_CurrentBuff = BT_None;

	// Metallicafan212:	Reset our bound textures
	for (INT i = 0; i < MAX_TEXTURES; i++)
	{
		SetTexture(0, nullptr, 0);
	}

	// Metallicafan212:	Setup the depth stencil state
	//					From a MSDN page https://learn.microsoft.com/en-us/windows/win32/direct3d11/d3d10-graphics-programming-guide-depth-stencil
	D3D11_DEPTH_STENCIL_DESC dsDesc;

	// Depth test parameters
	dsDesc.DepthEnable					= true;
	dsDesc.DepthWriteMask				= D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc					= D3D11_COMPARISON_LESS_EQUAL;

	// Stencil test parameters
	dsDesc.StencilEnable				= true;
	dsDesc.StencilReadMask				= 0xFF;
	dsDesc.StencilWriteMask				= 0xFF;

	// Stencil operations if pixel is front-facing
	dsDesc.FrontFace.StencilFailOp		= D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	dsDesc.FrontFace.StencilPassOp		= D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFunc		= D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing
	dsDesc.BackFace.StencilFailOp		= D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilDepthFailOp	= D3D11_STENCIL_OP_DECR;
	dsDesc.BackFace.StencilPassOp		= D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFunc			= D3D11_COMPARISON_ALWAYS;

	// Create depth stencil state
	hr = m_D3DDevice->CreateDepthStencilState(&dsDesc, &m_DefaultZState);

	ThrowIfFailed(hr);

	// Metallicafan212:	Set it
	m_D3DDeviceContext->OMSetDepthStencilState(m_DefaultZState, 1);

	// Metallicafan212:	And now a version with no z writing
	dsDesc.DepthWriteMask					= D3D11_DEPTH_WRITE_MASK_ZERO;
	dsDesc.DepthFunc						= D3D11_COMPARISON_LESS_EQUAL;
	hr = m_D3DDevice->CreateDepthStencilState(&dsDesc, &m_DefaultNoZState);
	ThrowIfFailed(hr);

	// Metallicafan212:	Set the index and vertex buffers now (since we don't swap them in and out)
	m_D3DDeviceContext->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	UINT Stride = sizeof(FD3DVert);
	UINT Offset = 0;
	m_D3DDeviceContext->IASetVertexBuffers(0, 1, &VertexBuffer, &Stride, &Offset);

	// Metallicafan212:	Keep the lock version updated
	LastAASamples		= NumAASamples;
	LastResolutionScale = ResolutionScale;

	unguard;
}

UBOOL UICBINDx11RenderDevice::SetRes(INT NewX, INT NewY, INT NewColorBytes, UBOOL Fullscreen)
{
	guard(UICBINDx11RenderDevice::SetRes);

	// Metallicafan212:	Save the values
	//					We also have to clamp here since unreal could pass bad values (the editor opening, for example)
	SizeX			= Max(NewX, 2);
	SizeY			= Max(NewY, 2);

	bFullscreen		= Fullscreen;

	if (Viewport != nullptr)
	{
		Viewport->ResizeViewport(BLIT_HardwarePaint | BLIT_Direct3D, SizeX, SizeY, NewColorBytes);
	}

	// Metallicafan212:	Set the viewport now
	D3D11_VIEWPORT viewport = {0.0f, 0.0f, static_cast<FLOAT>(SizeX), static_cast<FLOAT>(SizeY), 0.f, 1.f };
	m_D3DDeviceContext->RSSetViewports(1, &viewport);

	// Metallicafan212:	Resetup resources that need to be sized
	SetupResources();

	return 1;

	unguard;
}

void UICBINDx11RenderDevice::Exit()
{
	guard(UICBINDx11RenderDevice::Exit);

	Flush(0);

	SAFE_RELEASE(VertexBuffer);
	SAFE_RELEASE(IndexBuffer);

	m_D3DDeviceContext->ClearState();

	// Metallicafan212:	Clear the RT textures
	ClearRTTextures();

	// Metallicafan212:	TODO! When supporting textures, clear textures
	//					Also shaders lol
	SAFE_DELETE(FTileShader);
	SAFE_DELETE(FGenShader);
	SAFE_DELETE(FResScaleShader);
	SAFE_DELETE(FMeshShader);
	SAFE_DELETE(FSurfShader);
	SAFE_DELETE(FLineShader);
	SAFE_DELETE(FMSAAShader);
#if USE_COMPUTE_SHADER
	SAFE_DELETE(FMshLghtCompShader);
#endif

#if DX11_HP2
	// Metallicafan212:	Cleanup all the fonts
	for (TMap<FString, IDWriteTextFormat*>::TIterator It(FontMap); It; ++It)
	{
		It.Value()->Release();
	}

	FontMap.Empty();
#endif

	// Metallicafan212:	Cleanup the blend states
	for (TMap<FPLAG, ID3D11BlendState*>::TIterator It(BlendMap); It; ++It)
	{
		It.Value()->Release();
	}
	BlendMap.Empty();

	// Metallicafan212:	Cleanup the sampler map
	FlushTextureSamplers();

	m_D3DSwapChain->SetFullscreenState(0, nullptr);

	// Metallicafan212:	Clear any set RT/DC
	if(m_D3DDeviceContext != nullptr)
		m_D3DDeviceContext->OMSetRenderTargets(0, nullptr, nullptr);

	// Metallicafan212:	Render target + back buffer texture
	SAFE_RELEASE(m_BackBuffTex);
	SAFE_RELEASE(m_BackBuffRT);
	SAFE_RELEASE(m_BackBuffUAV);
	SAFE_RELEASE(m_ScreenBuffTex);
	SAFE_RELEASE(m_D3DScreenRTV);
	SAFE_RELEASE(m_ScreenRTSRV);

	// Metallicafan212:	Depth stencil target
	SAFE_RELEASE(m_ScreenDSTex);
	SAFE_RELEASE(m_D3DScreenDSV);
	SAFE_RELEASE(m_ScreenDTSRV);

	// Metallicafan212:	No bind texture/sampler
	SAFE_RELEASE(BlankTexture);
	SAFE_RELEASE(BlankResourceView);
	SAFE_RELEASE(BlankSampler);


	SAFE_RELEASE(m_DefaultZState);
	SAFE_RELEASE(m_DefaultNoZState);


	FlushRasterStates();

	// Metallicafan212:	Remove the swap chain early, so we can clear
	//					Also all the other resources
	SAFE_RELEASE(m_D3DSwapChain);

	SAFE_RELEASE(m_D3DQuery);

#if DX11_HP2
	// Metallicafan212:	HP2 specific
	SAFE_RELEASE(m_D2DFact);
	SAFE_RELEASE(m_D2DWriteFact);
	SAFE_RELEASE(m_D2DRT);
	SAFE_RELEASE(m_DXGISurf);
	SAFE_RELEASE(m_TextParams);
#endif

	// Metallicafan212:	Now flush
	m_D3DDeviceContext->Flush();

	SAFE_RELEASE(m_D3DDeviceContext);
	SAFE_RELEASE(m_D3DDevice);

	// Metallicafan212:	Fix the window and make it compatible with other renderers
	HWND h = (HWND)Viewport->GetWindow();
	/*
	LONG_PTR s = GetWindowLongPtr(h, GWL_EXSTYLE);

	//GetWindowRect(h, &r);
	//GetClientRect(h, &r);

	// Metallicafan212:	Remove WS_EX_NOREDIRECTIONBITMAP, since the freesync mode adds it
	s &= ~WS_EX_NOREDIRECTIONBITMAP;

	SetWindowLongPtr(h, GWL_EXSTYLE, s);
	*/

	/*
	SetWindowLongPtr(h, GWL_EXSTYLE, ViewExtendedStyle);

	SetWindowPos(h, 0, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_DRAWFRAME | SWP_SHOWWINDOW);
	ShowWindowAsync(h, SW_SHOW);
	//UpdateWindow(h);

	//MoveWindow(h, r.left, r.top, r.right - r.left, r.bottom - r.top, 1);
	*/

	/*
	// Metallicafan212:	Screw it, this is the only way to fix it.... We have to force windows to swap it back to a normal window........
	HWND hwndButton = CreateWindow(
		TEXT("BUTTON"),  // Predefined class; Unicode assumed 
		L"OK",      // Button text 
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
		10,         // x position 
		10,         // y position 
		100,        // Button width
		100,        // Button height
		h,     // Parent window
		NULL,       // No menu.
		(HINSTANCE)GetWindowLongPtr(h, GWLP_HINSTANCE),
		NULL);      // Pointer not needed.

	ShowWindow(hwndButton, SW_SHOW);
	UpdateWindow(hwndButton);
	UpdateWindow(h);

	DestroyWindow(hwndButton);
	*/

	/*
	// Metallicafan212:	Maybe?????????
	HDC hdc;
	PAINTSTRUCT ps;

	hdc = BeginPaint(h, &ps);
	EndPaint(h, &ps);
	*/

	unguard;
}

void UICBINDx11RenderDevice::Flush(UBOOL AllowPrecache)
{
	guard(UICBINDx11RenderDevice::Flush);

	// Metallicafan212:	Loop and flush out the textures
	//					TODO! There might be a more efficient way to do this!
	for (TMap<D3DCacheId, FD3DTexture>::TIterator It(TextureMap); It; ++It)
	{
		// Metallicafan212:	Release all this info
		SAFE_RELEASE(It.Value().m_View);
		SAFE_RELEASE(It.Value().m_Tex);
	}

	TextureMap.Empty();

	appMemzero(BoundTextures, sizeof(BoundTextures));

	unguard;
}

UBOOL UICBINDx11RenderDevice::Exec(const TCHAR* Cmd, FOutputDevice& Ar)
{
	guard(UICBINDx11RenderDevice::Exec);

	if (URenderDevice::Exec(Cmd, Ar))
	{
		return 1;
	}

	// Metallicafan212:	Directly copied from the DX9 driver
	if (ParseCommand(&Cmd, TEXT("GetRes")))
	{
		TArray<FPlane> Relevant;
		INT i;

		// Metallicafan212:	Optimization hint
		INT num = Modes.Num();

		for (i = 0; i < num; i++)
		{
			if (Modes(i).Z == (Viewport->ColorBytes * 8))
				if
					((Modes(i).X != 320 || Modes(i).Y != 200)
						&& (Modes(i).X != 640 || Modes(i).Y != 400))
					Relevant.AddUniqueItem(FPlane(Modes(i).X, Modes(i).Y, 0, 0));
		}
		appQsort(&Relevant(0), Relevant.Num(), sizeof(FPlane), (QSORT_COMPARE)CompareRes);
		FString Str;

		// Metallicafan212:	Optimization hint
		INT rel_num = Relevant.Num();

		for (i = 0; i < rel_num; i++)
		{
			Str += FString::Printf(TEXT("%ix%i "), (INT)Relevant(i).X, (INT)Relevant(i).Y);
		}
		// Metallicafan212:	This causes the HP2 UC to not add on 1920x1080 (aka the desktop res)
		Ar.Log(*Str);//.LeftChop(1));
		return 1;
	}

	return 0;

	unguard;
}

void UICBINDx11RenderDevice::Lock(FPlane InFlashScale, FPlane InFlashFog, FPlane ScreenClear, DWORD RenderLockFlags, BYTE* HitData, INT* HitSize)
{
	guard(UICBINDx11RenderDevice::Lock);

	// Metallicafan212:	Check if our lock flags changed
	if (LastAASamples != NumAASamples || LastAFSamples != NumAFSamples || LastResolutionScale != ResolutionScale)
	{
		if (LastAASamples != NumAASamples || LastResolutionScale != ResolutionScale)
		{
			// Metallicafan212:	Doing this will also flush the sampler state
			SetupResources();
		}
		else
		{
			FlushTextureSamplers();
		}
	}

#if DX11_HP2
	// Metallicafan212:	Check for wireframe
	if (Viewport->IsWire())
	{
		ExtraRasterFlags = DXRS_Wireframe;
	}
	else
	{
		ExtraRasterFlags = 0;
	}
#endif

	// Metallicafan212:	Only clear if we're in the editor
	if (1)//GIsEditor || (RenderLockFlags & LOCKR_ClearScreen))
	{
		m_D3DDeviceContext->ClearRenderTargetView(m_D3DScreenRTV, &ScreenClear.X);
	}
	// Metallicafan212:	TODO! Only do this in the editor?
	m_D3DDeviceContext->ClearDepthStencilView(m_D3DScreenDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// Metallicafan212:	Clear to max Z
	//m_D3DDeviceContext->ClearRenderTargetView(m_D3DScreenOpacityRTV, DirectX::Colors::White);

	// Metallicafan212:	Make sure we're always using the right RT
	//RestoreRenderTarget();
	//m_D3DDeviceContext->OMSetRenderTargets(1, &m_D3DScreenRTV, m_D3DScreenDSV);
	//BoundRT = nullptr;

	// Metallicafan212:	Hold onto the flash fog for future render
	FlashScale	= InFlashScale;
	FlashFog	= InFlashFog;

	// Metallicafan212:	Hold onto the hit related info
	m_HitData		= HitData;
	m_HitSize		= HitSize;
	m_HitCount		= 0;

	if (m_HitData != nullptr)
	{
		// Metallicafan212:	Reset the pixel hit state
		PixelHitInfo.AddItem(FPixHitInfo());
		PixelTopIndex = -1;

		m_HitBufSize	= *m_HitSize;

		// Metallicafan212:	Tell unreal there was no hits (so far)
		*m_HitSize		= 0;
	}

	// Metallicafan212:	Setup the buffers
	LockVertexBuffer(0, 0);
	LockIndexBuffer(0, 0);
	UnlockVertexBuffer();
	UnlockIndexBuffer();

	// Metallicafan212:	Reset the buffered state
	EndBuffering();

	if (GlobalShaderVars.bDoDistanceFog || GlobalShaderVars.bFadeFogValues)
		TickDistanceFog();

	unguard;
}

void UICBINDx11RenderDevice::Unlock(UBOOL Blit)
{
	guard(UICBINDx11RenderDevice::Unlock);

	// Metallicafan212:	Reset the buffered state
	EndBuffering();

	// Metallicafan212:	Restore our render target (as it may have been changed due to RT textures)
	RestoreRenderTarget();

	// Metallicafan212:	Get the selection
	if (m_HitData != nullptr)
	{
		// Metallicafan212:	Check the hits now
		DetectPixelHit();

		// Metallicafan212:	Clear them now
		PixelHitInfo.Empty();

		//ClearPixelHits();
		*m_HitSize = m_HitCount;

		// Metallicafan212:	TODO! Add this as a debug option
		if(bDebugSelection)
			Blit = 1;
	}

	// Metallicafan212:	TODO! Sample code

	// The first argument instructs DXGI to block until VSync, putting the application
	// to sleep until the next VSync. This ensures we don't waste any cycles rendering
	// frames that will never be displayed to the screen.

	if (Blit)
	{
		// Metallicafan212:	Copy to the screen
		if (NumAASamples > 1)
		{
#if DX11_USE_MSAA_SHADER

#if USE_MSAA_COMPUTE
			if (bUseMSAAComputeShader)
			{
				// Metallicafan212:	Use a compute shader instead!!!
				SetTexture(0, nullptr, 0);
				//SetTexture(1, nullptr, 0);

				m_D3DDeviceContext->OMSetRenderTargets(0, nullptr, nullptr);

				m_D3DDeviceContext->CSSetShaderResources(0, 1, &m_ScreenRTSRV);
				//m_D3DDeviceContext->CSSetShaderResources(1, 1, &m_ScreenDTSRV);

				// Metallicafan212:	Now bind the output
				m_D3DDeviceContext->CSSetUnorderedAccessViews(0, 1, &m_BackBuffUAV, nullptr);

				// Metallicafan212:	Bind the shader
				FMSAAShader->Bind();

				// Metallicafan212:	Now execute!
				m_D3DDeviceContext->Dispatch(MSAAThreadX, MSAAThreadY, 1);

				// Metallicafan212:	Wait for it to complete...
				m_D3DDeviceContext->End(m_D3DQuery);

				// Metallicafan212:	Wait for it
				BOOL bDone = 0;

				while (m_D3DDeviceContext->GetData(m_D3DQuery, &bDone, sizeof(BOOL), 0) != S_OK && bDone == 0);

				// Metallicafan212:	Clear the render resources!!!
				constexpr ID3D11ShaderResourceView* SRVTemp[2] = { nullptr, nullptr };
				m_D3DDeviceContext->CSSetShaderResources(0, 2, SRVTemp);


				constexpr ID3D11UnorderedAccessView* Temp[1] = { nullptr };
				m_D3DDeviceContext->CSSetUnorderedAccessViews(0, 1, Temp, nullptr);

				RestoreRenderTarget();
#else

				// Metallicafan212:	We have to do geometry now...
				EndBuffering();

				// Metallicafan212:	Order of operations, make sure the alpha rejection is set
				SetBlend(0);

				SetTexture(0, nullptr, 0);
				SetTexture(1, nullptr, 0);

				// Metallicafan212:	Manually setup the vars...
				m_D3DDeviceContext->OMSetRenderTargets(1, &m_BackBuffRT, nullptr);
				m_D3DDeviceContext->PSSetShaderResources(0, 1, &m_ScreenRTSRV);
				m_D3DDeviceContext->PSSetShaderResources(1, 1, &m_ScreenDTSRV);

				FMSAAShader->Bind();

				FLOAT Z		= 1.0f;
				FLOAT X		= 0.0f;
				FLOAT Y		= 0.0f;
				FLOAT XL	= ScaledSceneNodeX;//m_sceneNodeX;
				FLOAT YL	= ScaledSceneNodeY;//m_sceneNodeY;

				// Metallicafan212:	Now make 2 triangles
				//					I copied this all from the tile rendering, since I'm such a lazy fucking bastard
				FLOAT PX1	= X - (XL * 0.5f);
				FLOAT PX2	= PX1 + XL;
				FLOAT PY1	= Y - (YL * 0.5f);
				FLOAT PY2	= PY1 + YL;

				FLOAT RPX1	= m_RFX2 * PX1;
				FLOAT RPX2	= m_RFX2 * PX2;
				FLOAT RPY1	= m_RFY2 * PY1;
				FLOAT RPY2	= m_RFY2 * PY2;

				FLOAT SU1 = 0.0f;
				FLOAT SU2 = 1.0f;
				FLOAT SV1 = 0.0f;
				FLOAT SV2 = 1.0f;

				RPX1 *= Z;
				RPX2 *= Z;
				RPY1 *= Z;
				RPY2 *= Z;


				// Metallicafan212:	Disable depth lmao
				//ID3D11DepthStencilState* CurState	= nullptr;
				//UINT Sten							= 0;
				//m_D3DDeviceContext->OMGetDepthStencilState(&CurState, &Sten);
				//m_D3DDeviceContext->OMSetDepthStencilState(m_DefaultNoZState, 0);

				LockVertexBuffer(6 * sizeof(FD3DVert));

				//m_D3DDeviceContext->IASetInputLayout(nullptr);

				// Metallicafan212:	Start buffering now
				StartBuffering(BT_ScreenFlash);

				m_VertexBuff[0].X		= RPX1;
				m_VertexBuff[0].Y		= RPY1;
				m_VertexBuff[0].Z		= Z;
				m_VertexBuff[0].U		= SU1;
				m_VertexBuff[0].V		= SV1;

				m_VertexBuff[1].X		= RPX2;
				m_VertexBuff[1].Y		= RPY1;
				m_VertexBuff[1].Z		= Z;
				m_VertexBuff[1].U		= SU2;
				m_VertexBuff[1].V		= SV1;

				m_VertexBuff[2].X		= RPX2;
				m_VertexBuff[2].Y		= RPY2;
				m_VertexBuff[2].Z		= Z;
				m_VertexBuff[2].U		= SU2;
				m_VertexBuff[2].V		= SV2;

				m_VertexBuff[3].X		= RPX1;
				m_VertexBuff[3].Y		= RPY1;
				m_VertexBuff[3].Z		= Z;
				m_VertexBuff[3].U		= SU1;
				m_VertexBuff[3].V		= SV1;

				m_VertexBuff[4].X		= RPX2;
				m_VertexBuff[4].Y		= RPY2;
				m_VertexBuff[4].Z		= Z;
				m_VertexBuff[4].U		= SU2;
				m_VertexBuff[4].V		= SV2;

				m_VertexBuff[5].X		= RPX1;
				m_VertexBuff[5].Y		= RPY2;
				m_VertexBuff[5].Z		= Z;
				m_VertexBuff[5].U		= SU1;
				m_VertexBuff[5].V		= SV2;

				UnlockVertexBuffer();

				m_D3DDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				AdvanceVertPos(6);

				// Metallicafan212:	Draw
				EndBuffering();

				// Metallicafan212:	Reset Z state
				//m_D3DDeviceContext->OMSetDepthStencilState(CurState, Sten);

				SetTexture(0, nullptr, 0);
				SetTexture(1, nullptr, 0);

				RestoreRenderTarget();
#endif
			}
			else
#endif
			{
				m_D3DDeviceContext->ResolveSubresource(m_BackBuffTex, 0, m_ScreenBuffTex, 0, DXGI_FORMAT_B8G8R8A8_UNORM);
			}
		}
		// Metallicafan212:	Always use the resolution scaling shader, so we can do final effects on the screen
		else
		//else if (ResolutionScale != 1.0f)
		{
#if USE_RES_COMPUTE
			// Metallicafan212:	Use a compute shader instead!!!
			SetTexture(0, nullptr, 0);

			m_D3DDeviceContext->OMSetRenderTargets(0, nullptr, nullptr);

			m_D3DDeviceContext->CSSetShaderResources(0, 1, &m_ScreenRTSRV);
			//m_D3DDeviceContext->CSSetShaderResources(1, 1, &m_ScreenDTSRV);

			// Metallicafan212:	Now bind the output
			m_D3DDeviceContext->CSSetUnorderedAccessViews(0, 1, &m_BackBuffUAV, nullptr);

			// Metallicafan212:	Check if we need to create the sampler
			if (ScreenSamp == nullptr)
			{
				// Metallicafan212:	Create it
				CD3D11_SAMPLER_DESC SDesc = CD3D11_SAMPLER_DESC(D3D11_DEFAULT);
			
				// Metallicafan212:	Bilinear filtering
				SDesc.Filter			= D3D11_FILTER_MIN_MAG_MIP_LINEAR;//D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR;//D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;//D3D11_FILTER_MIN_MAG_MIP_LINEAR;
				SDesc.AddressU			= D3D11_TEXTURE_ADDRESS_CLAMP;
				SDesc.AddressV			= SDesc.AddressU;
				SDesc.AddressW			= D3D11_TEXTURE_ADDRESS_WRAP;//SDesc.AddressU;
				SDesc.MinLOD			= -D3D11_FLOAT32_MAX;
				SDesc.MaxLOD			= D3D11_FLOAT32_MAX;
				SDesc.MipLODBias		= 0.0f;
				SDesc.MaxAnisotropy		= 1;//16;//16;
				SDesc.ComparisonFunc	= D3D11_COMPARISON_NEVER;

				HRESULT hr = m_D3DDevice->CreateSamplerState(&SDesc, &ScreenSamp);

				ThrowIfFailed(hr);
			}

			m_D3DDeviceContext->CSSetSamplers(0, 1, &ScreenSamp);

			// Metallicafan212:	Bind the shader
			FResScaleShader->Bind();

			// Metallicafan212:	Now execute!
			m_D3DDeviceContext->Dispatch(MSAAThreadX, MSAAThreadY, 1);

			// Metallicafan212:	Wait for it to complete...
			m_D3DDeviceContext->End(m_D3DQuery);

			// Metallicafan212:	Wait for it
			BOOL bDone = 0;

			while (m_D3DDeviceContext->GetData(m_D3DQuery, &bDone, sizeof(BOOL), 0) != S_OK && bDone == 0);

			// Metallicafan212:	Clear the render resources!!!
			constexpr ID3D11ShaderResourceView* SRVTemp[2] = { nullptr, nullptr };
			m_D3DDeviceContext->CSSetShaderResources(0, 2, SRVTemp);


			constexpr ID3D11UnorderedAccessView* Temp[1] = { nullptr };
			m_D3DDeviceContext->CSSetUnorderedAccessViews(0, 1, Temp, nullptr);

			RestoreRenderTarget();
#else
			// Metallicafan212:	Check if we need to create the sampler
			if (ScreenSamp == nullptr)
			{
				// Metallicafan212:	Create it
				CD3D11_SAMPLER_DESC SDesc = CD3D11_SAMPLER_DESC(D3D11_DEFAULT);
			
				// Metallicafan212:	Bilinear filtering
				SDesc.Filter			= D3D11_FILTER_MIN_MAG_MIP_LINEAR;//D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR;//D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;//D3D11_FILTER_MIN_MAG_MIP_LINEAR;
				SDesc.AddressU			= D3D11_TEXTURE_ADDRESS_CLAMP;
				SDesc.AddressV			= SDesc.AddressU;
				SDesc.AddressW			= D3D11_TEXTURE_ADDRESS_WRAP;//SDesc.AddressU;
				SDesc.MinLOD			= -D3D11_FLOAT32_MAX;
				SDesc.MaxLOD			= D3D11_FLOAT32_MAX;
				SDesc.MipLODBias		= 0.0f;
				SDesc.MaxAnisotropy		= 1;//16;//16;
				SDesc.ComparisonFunc	= D3D11_COMPARISON_NEVER;

				HRESULT hr = m_D3DDevice->CreateSamplerState(&SDesc, &ScreenSamp);

				ThrowIfFailed(hr);
			}

			// Metallicafan212:	Render as a quad 🙃🙃🙃🙃🙃🙃🙃
			EndBuffering();

			// Metallicafan212:	Order of operations, make sure the alpha rejection is set
			SetBlend(0);

			SetTexture(0, nullptr, 0);

			m_D3DDeviceContext->PSSetSamplers(0, 1, &ScreenSamp);

			// Metallicafan212:	Manually setup the vars...
			m_D3DDeviceContext->OMSetRenderTargets(1, &m_BackBuffRT, nullptr);
			m_D3DDeviceContext->PSSetShaderResources(0, 1, &m_ScreenRTSRV);

			SetSceneNode(nullptr);

			FResScaleShader->Bind();

			FLOAT Z = 1.0f;
			FLOAT X = 0.0f;
			FLOAT Y = 0.0f;
			FLOAT XL = SizeX;
			FLOAT YL = SizeY;

			// Metallicafan212:	Now make 2 triangles
			//					I copied this all from the tile rendering, since I'm such a lazy fucking bastard
			FLOAT PX1 = X - (XL * 0.5f);
			FLOAT PX2 = PX1 + XL;
			FLOAT PY1 = Y - (YL * 0.5f);
			FLOAT PY2 = PY1 + YL;

			FLOAT RPX1 = m_RFX2 * PX1;
			FLOAT RPX2 = m_RFX2 * PX2;
			FLOAT RPY1 = m_RFY2 * PY1;
			FLOAT RPY2 = m_RFY2 * PY2;

			FLOAT SU1 = 0.0f;
			FLOAT SU2 = 1.0f;
			FLOAT SV1 = 0.0f;
			FLOAT SV2 = 1.0f;

			RPX1 *= Z;
			RPX2 *= Z;
			RPY1 *= Z;
			RPY2 *= Z;

			LockVertexBuffer(6 * sizeof(FD3DVert));

			// Metallicafan212:	Start buffering now
			StartBuffering(BT_ScreenFlash);

			m_VertexBuff[0].X		= RPX1;
			m_VertexBuff[0].Y		= RPY1;
			m_VertexBuff[0].Z		= Z;
			m_VertexBuff[0].U		= SU1;
			m_VertexBuff[0].V		= SV1;
			m_VertexBuff[0].Color	= FPlane(1.f, 1.f, 1.f, 1.f);

			m_VertexBuff[1].X		= RPX2;
			m_VertexBuff[1].Y		= RPY1;
			m_VertexBuff[1].Z		= Z;
			m_VertexBuff[1].U		= SU2;
			m_VertexBuff[1].V		= SV1;
			m_VertexBuff[1].Color	= FPlane(1.f, 1.f, 1.f, 1.f);

			m_VertexBuff[2].X		= RPX2;
			m_VertexBuff[2].Y		= RPY2;
			m_VertexBuff[2].Z		= Z;
			m_VertexBuff[2].U		= SU2;
			m_VertexBuff[2].V		= SV2;
			m_VertexBuff[2].Color	= FPlane(1.f, 1.f, 1.f, 1.f);

			m_VertexBuff[3].X		= RPX1;
			m_VertexBuff[3].Y		= RPY1;
			m_VertexBuff[3].Z		= Z;
			m_VertexBuff[3].U		= SU1;
			m_VertexBuff[3].V		= SV1;
			m_VertexBuff[3].Color	= FPlane(1.f, 1.f, 1.f, 1.f);

			m_VertexBuff[4].X		= RPX2;
			m_VertexBuff[4].Y		= RPY2;
			m_VertexBuff[4].Z		= Z;
			m_VertexBuff[4].U		= SU2;
			m_VertexBuff[4].V		= SV2;
			m_VertexBuff[4].Color	= FPlane(1.f, 1.f, 1.f, 1.f);

			m_VertexBuff[5].X		= RPX1;
			m_VertexBuff[5].Y		= RPY2;
			m_VertexBuff[5].Z		= Z;
			m_VertexBuff[5].U		= SU1;
			m_VertexBuff[5].V		= SV2;
			m_VertexBuff[5].Color	= FPlane(1.f, 1.f, 1.f, 1.f);

			UnlockVertexBuffer();

			m_D3DDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			AdvanceVertPos(6);

			// Metallicafan212:	Draw
			EndBuffering();

			SetTexture(0, nullptr, 0);

			RestoreRenderTarget();
#endif
		}
		/*
		else
		{
			m_D3DDeviceContext->CopyResource(m_BackBuffTex, m_ScreenBuffTex);
		}
		*/

		constexpr DXGI_PRESENT_PARAMETERS Parm{ 0, nullptr, nullptr, nullptr };
		HRESULT hr = m_D3DSwapChain->Present1(bVSync ? 1 : 0, (bAllowTearing && !bFullscreen && !bVSync ? DXGI_PRESENT_ALLOW_TEARING : 0), &Parm);//m_D3DSwapChain->Present(0, 0);

		// Metallicafan212:	Check if DXGI needs a resize (alt+tab in fullscreen for example)
		if (hr == DXGI_ERROR_INVALID_CALL)
		{
			SetupResources();
		}
		else if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
		{
			SetupDevice();
			SetupResources();
		}
		else if(!SUCCEEDED(hr))//hr != DXGI_STATUS_OCCLUDED)
		{
			ThrowIfFailed(hr);
		}
	}

	if (bDebugSelection && m_HitData != nullptr)
	{
		appSleep(1.0f);
	}

	unguard;
}

void UICBINDx11RenderDevice::ClearZ(FSceneNode* Frame)
{
	guard(UICBINDx11RenderDevice::ClearZ);

	EndBuffering();

	m_D3DDeviceContext->ClearDepthStencilView(m_D3DScreenDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	unguard;
}

void UICBINDx11RenderDevice::EndFlash()
{
	guard(UICBINDx11RenderDevice::EndFlash);

#if DX11_HP2
	/*
	// Metallicafan212:	Test for a very small, but not 0 float, since the code seems to keep running for some reason
	if ((1.0f - Min(FlashScale.W * 2.0f, 1.0f)) <= 0.0001f)
	{
		return;
	}
	*/
#endif

	// Metallicafan212:	Draw it as a tile, but using the generic shader
#if DX11_HP2
	if ((FlashScale != FPlane(0.5f, 0.5f, 0.5f, 0.5f)) || (FlashFog != FPlane(0.0f, 0.0f, 0.0f, 0.0f)))
#else
	if ((FlashScale != FPlane(0.5f, 0.5f, 0.5f, 0.0f)) || (FlashFog != FPlane(0.0f, 0.0f, 0.0f, 0.0f)))
#endif
	{
		EndBuffering();

		// Metallicafan212:	Order of operations, make sure the alpha rejection is set
		SetBlend(PF_Highlighted);

		SetTexture(0, nullptr, 0);

		FGenShader->Bind();

#if DX11_HP2
		FPlane Color = FPlane(FlashFog.X, FlashFog.Y, FlashFog.Z, 1.0f - Min(FlashScale.W * 2.0f, 1.0f));
#else
		FPlane Color = FPlane(FlashFog.X, FlashFog.Y, FlashFog.Z, 1.0f - Min(FlashScale.X * 2.0f, 1.0f));
#endif

		FLOAT Z		= 0.5f;
		FLOAT X		= 0.0f;
		FLOAT Y		= 0.0f;
		FLOAT XL	= ScaledSceneNodeX;//m_sceneNodeX;
		FLOAT YL	= ScaledSceneNodeY;//m_sceneNodeY;

		// Metallicafan212:	Now make 2 triangles
		//					I copied this all from the tile rendering, since I'm such a lazy fucking bastard
		FLOAT PX1	= X - (XL * 0.5f);//(m_sceneNodeX * 0.5f);
		FLOAT PX2	= PX1 + XL;
		FLOAT PY1	= Y - (YL * 0.5f);
		FLOAT PY2	= PY1 + YL;

		FLOAT RPX1	= m_RFX2 * PX1;
		FLOAT RPX2	= m_RFX2 * PX2;
		FLOAT RPY1	= m_RFY2 * PY1;
		FLOAT RPY2	= m_RFY2 * PY2;

		RPX1 *= Z;
		RPX2 *= Z;
		RPY1 *= Z;
		RPY2 *= Z;


		// Metallicafan212:	Disable depth lmao
		ID3D11DepthStencilState* CurState = nullptr;
		UINT Sten = 0;
		m_D3DDeviceContext->OMGetDepthStencilState(&CurState, &Sten);
		m_D3DDeviceContext->OMSetDepthStencilState(m_DefaultNoZState, 0);

		LockVertexBuffer(6 * sizeof(FD3DVert));

		// Metallicafan212:	Start buffering now
		StartBuffering(BT_ScreenFlash);

		m_VertexBuff[0].Color	= Color;
		m_VertexBuff[0].X		= RPX1;
		m_VertexBuff[0].Y		= RPY1;
		m_VertexBuff[0].Z		= Z;

		m_VertexBuff[1].Color	= Color;
		m_VertexBuff[1].X		= RPX2;
		m_VertexBuff[1].Y		= RPY1;
		m_VertexBuff[1].Z		= Z;

		m_VertexBuff[2].Color	= Color;
		m_VertexBuff[2].X		= RPX2;
		m_VertexBuff[2].Y		= RPY2;
		m_VertexBuff[2].Z		= Z;

		m_VertexBuff[3].Color	= Color;
		m_VertexBuff[3].X		= RPX1;
		m_VertexBuff[3].Y		= RPY1;
		m_VertexBuff[3].Z		= Z;

		m_VertexBuff[4].Color	= Color;
		m_VertexBuff[4].X		= RPX2;
		m_VertexBuff[4].Y		= RPY2;
		m_VertexBuff[4].Z		= Z;

		m_VertexBuff[5].Color	= Color;
		m_VertexBuff[5].X		= RPX1;
		m_VertexBuff[5].Y		= RPY2;
		m_VertexBuff[5].Z		= Z;

		UnlockVertexBuffer();

		m_D3DDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		AdvanceVertPos(6);

		// Metallicafan212:	Draw
		EndBuffering();

		// Metallicafan212:	Reset Z state
		m_D3DDeviceContext->OMSetDepthStencilState(CurState, Sten);
	}

	unguard;
}

void UICBINDx11RenderDevice::GetStats(TCHAR* Result)
{

}

void UICBINDx11RenderDevice::DrawStats(FSceneNode* Frame)
{

}

void UICBINDx11RenderDevice::SetSceneNode(FSceneNode* Frame)
{
	guard(UICBINDx11RenderDevice::SetSceneNode);

	// Metallicafan212:	End any buffering that was requested???
	EndBuffering();

	// Metallicafan212:	Check if nullptr (special hack!!!!)
	if (Frame == nullptr)
	{
		// Metallicafan212:	Scale the resolution! TODO!!!!
		FLOAT NewX = SizeX;
		FLOAT NewY = SizeY;

		// Set the viewport.
		D3D11_VIEWPORT viewport =
		{
			0.0f, 0.0f,
			NewX, NewY, 0.f, 1.f
		};

		m_D3DDeviceContext->RSSetViewports(1, &viewport);

		// Metallicafan212:	All of this is copied from the DX9 driver
		// Precompute stuff.
		FLOAT rcpFrameFX = 1.0f / NewX;//Frame->FX;
		m_Aspect = NewY * rcpFrameFX;//Frame->FY * rcpFrameFX;

		// Metallicafan212:	This is HP2 specific! Since I have a viewport FOV that is calcuated to be a hor+ FOV, so 90 @ 16x9 is 109
#if DX11_HP2
		m_RProjZ = appTan(Viewport->FOVAngle * PI / 360.0);//Viewport->Actor->FovAngle * PI / 360.0);
#else
		m_RProjZ = appTan(Viewport->Actor->FovAngle * PI / 360.0);
#endif
		m_RFX2 = 2.0f * m_RProjZ * rcpFrameFX;
		m_RFY2 = 2.0f * m_RProjZ * rcpFrameFX;

		//Remember Frame->X and Frame->Y
		m_sceneNodeX = SizeX;
		m_sceneNodeY = SizeY;

		// Metallicafan212:	Remember the scaled values!
		ScaledSceneNodeX = NewX;
		ScaledSceneNodeY = NewY;

		// Metallicafan212:	Scaled FX2 and FY2
		ScaledFX2		= NewX * 0.5f;
		ScaledFY2		= NewY * 0.5f;
	}
	else
	{
		// Metallicafan212:	Scale the resolution! TODO!!!!
		FLOAT NewX = Frame->FX;
		FLOAT NewY = Frame->FY;

		if (BoundRT == nullptr)
		{
			NewX *= ResolutionScale;
			NewY *= ResolutionScale;

			// Metallicafan212:	Scaled FX2 and FY2
#if !RES_SCALE_IN_PROJ
			ScaledFX2 = Frame->FX2 * ResolutionScale;
			ScaledFY2 = Frame->FY2 * ResolutionScale;
#else
			ScaledFX2 = Frame->FX2;
			ScaledFY2 = Frame->FY2;
#endif
		}
		else
		{
			// Metallicafan212:	Scaled FX2 and FY2
			ScaledFX2 = Frame->FX2;
			ScaledFY2 = Frame->FY2;
		}

		// Set the viewport.
		D3D11_VIEWPORT viewport =
		{
			static_cast<FLOAT>(Frame->XB), static_cast<FLOAT>(Frame->YB),
			NewX, NewY, 0.f, 1.f
		};

		m_D3DDeviceContext->RSSetViewports(1, &viewport);

		SizeX = Frame->X;
		SizeY = Frame->Y;

		// Metallicafan212:	All of this is copied from the DX9 driver
		// Precompute stuff.
		FLOAT rcpFrameFX = 1.0f / NewX;//Frame->FX;
		m_Aspect = NewY * rcpFrameFX;//Frame->FY * rcpFrameFX;

		// Metallicafan212:	This is HP2 specific! Since I have a viewport FOV that is calcuated to be a hor+ FOV, so 90 @ 16x9 is 109
#if DX11_HP2
		m_RProjZ = appTan(Viewport->FOVAngle * PI / 360.0);//Viewport->Actor->FovAngle * PI / 360.0);
#else
		m_RProjZ = appTan(Viewport->Actor->FovAngle * PI / 360.0);
#endif
		m_RFX2 = 2.0f * m_RProjZ * rcpFrameFX;
		m_RFY2 = 2.0f * m_RProjZ * rcpFrameFX;

		//Remember Frame->X and Frame->Y
		m_sceneNodeX = Frame->X;
		m_sceneNodeY = Frame->Y;

		// Metallicafan212:	Remember the scaled values!
		ScaledSceneNodeX = NewX;
		ScaledSceneNodeY = NewY;

		
	}

	SetProjectionStateNoCheck(0, 1);

	unguard;
}

// Metallicafan212:	Shamfully copied from the DX9 renderer
void UICBINDx11RenderDevice::SetProjectionStateNoCheck(UBOOL bRequestingNearRangeHack, UBOOL bForceUpdate)
{
	float left, right, bottom, top, zNear, zFar;
	float invRightMinusLeft, invTopMinusBottom, invNearMinusFar;

	if (m_nearZRangeHackProjectionActive != bRequestingNearRangeHack)
		EndBuffering();
	// Metallicafan212:	Check if it was set at least once!
	//else if(!bForceUpdate && m_CurrentBuff != BT_None)
	//	return;

	//Save new Z range hack projection state
	m_nearZRangeHackProjectionActive = bRequestingNearRangeHack;

	//Set default zNearVal
	FLOAT zNearVal = 0.5f;

	FLOAT zScaleVal = 1.0f;
	if (bRequestingNearRangeHack)
	{
		zScaleVal = 0.125f;

		// Metallicafan212:	Attempt to fix some near-z clipping issues
		zNearVal = 0.5f;
	}

	left	= -m_RProjZ * zNearVal;
	right	= +m_RProjZ * zNearVal;
	bottom	= -m_Aspect * m_RProjZ * zNearVal;
	top		= +m_Aspect * m_RProjZ * zNearVal;
	zNear	= 1.0f		* zNearVal;

	//Set zFar
#if DX11_HP2 || DX11_UNREAL_227
	zFar = 49152.0f;
#else
	zFar = 32768.0f;
#endif

	if (bRequestingNearRangeHack)
	{
		zFar *= zScaleVal;
	}

	invRightMinusLeft = 1.0f / (right - left);
	invTopMinusBottom = 1.0f / (top - bottom);
	invNearMinusFar = 1.0f / (zNear - zFar);

	Proj.m[0][0] = 2.0f * zNear * invRightMinusLeft;
	Proj.m[0][1] = 0.0f;
	Proj.m[0][2] = 0.0f;
	Proj.m[0][3] = 0.0f;

	Proj.m[1][0] = 0.0f;
	Proj.m[1][1] = 2.0f * zNear * invTopMinusBottom;
	Proj.m[1][2] = 0.0f;
	Proj.m[1][3] = 0.0f;

#if RES_SCALE_IN_PROJ
	Proj.m[2][0] = 1.0f		/ m_sceneNodeX;//(FLOAT)m_sceneNodeX;
	Proj.m[2][1] = -1.0f	/ m_sceneNodeY;//(FLOAT)m_sceneNodeY;
#else
	Proj.m[2][0] = 1.0f		/ ScaledSceneNodeX;//(FLOAT)m_sceneNodeX;
	Proj.m[2][1] = -1.0f	/ ScaledSceneNodeY;//(FLOAT)m_sceneNodeY;
#endif
	Proj.m[2][2] = zScaleVal * (zFar * invNearMinusFar);
	Proj.m[2][3] = -1.0f;

	Proj.m[3][0] = 0.0f;
	Proj.m[3][1] = 0.0f;
	Proj.m[3][2] = zScaleVal * zScaleVal * (zNear * zFar * invNearMinusFar);
	Proj.m[3][3] = 0.0f;

#if RES_SCALE_IN_PROJ
	//Proj *= DirectX::XMMatrixScaling(1.0f / ResolutionScale, 1.0f / ResolutionScale, 1.0f);
#endif

	//Proj = DirectX::XMMatrixPerspectiveFovLH(m_RProjZ, ((FLOAT)m_sceneNodeX) / ((FLOAT)m_sceneNodeY), zScaleVal * zNear, zScaleVal * zFar);

	//Proj = DirectX::XMMatrixPerspectiveOffCenterLH(0.0f, m_sceneNodeX, m_sceneNodeY, 0.0f, zNear * zScaleVal, zFar * zScaleVal);//DirectX::XMMatrixPerspectiveFovLH(m_RProjZ, m_Aspect, zNear * zScaleVal, zFar * zScaleVal);
	// Metallicafan212:	Transpose???
	FLOAT Temp[4][4];
	Temp[0][0]	= Proj.m[0][0];
	Temp[0][1]	= -Proj.m[1][0];
	Temp[0][2]	= -Proj.m[2][0];
	Temp[0][3]	= Proj.m[3][0];
	Temp[1][0]	= Proj.m[0][1];
	Temp[1][1]	= -Proj.m[1][1];
	Temp[1][2]	= -Proj.m[2][1];
	Temp[1][3]	= Proj.m[3][1];
	Temp[2][0]	= Proj.m[0][2];
	Temp[2][1]	= -Proj.m[1][2];
	Temp[2][2]  = -Proj.m[2][2];
	Temp[2][3]  = Proj.m[3][2];
	Temp[3][0]  = Proj.m[0][3];
	Temp[3][1]  = -Proj.m[1][3];
	Temp[3][2]  = -Proj.m[2][3];
	Temp[3][3]  = Proj.m[3][3];

	// Metallicafan212:	Now back????
	appMemcpy(&Proj.m[0][0], &Temp[0][0], sizeof(FLOAT[4][4]));
}

void UICBINDx11RenderDevice::PrecacheTexture(FTextureInfo& Info, FPLAG PolyFlags)
{
	guard(UICBINDx11RenderDevice::PrecacheTexture);

	// Metallicafan212:	Upload it now
	CacheTextureInfo(Info, PolyFlags);

	unguard;
}