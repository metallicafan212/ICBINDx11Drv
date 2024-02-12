#include "ICBINDx11Drv.h"

// Metallicafan212:	TODO!
IMPLEMENT_CLASS(UICBINDx11RenderDevice);
IMPLEMENT_PACKAGE(ICBINDx11Drv);

UBOOL UICBINDx11RenderDevice::bSetupIValArray = 0;
INT	  UICBINDx11RenderDevice::IndexValueArray[IBUFF_SIZE];

void UICBINDx11RenderDevice::SetupDevice()
{
	guard(UICBINDx11RenderDevice::SetupDevice);

	// Metallicafan212: Detect windows version and wine on non-HP2 engines
#if !DX11_HP2
	SetupWindowsVersionCheck();
#endif


#if DX11_HP2
#if !USE_UNODERED_MAP_EVERYWHERE
	// Metallicafan212:	Cleanup all the fonts
	for (TMap<FString, IDWriteTextFormat*>::TIterator It(FontMap); It; ++It)
	{
		It.Value()->Release();
	}
	FontMap.Empty();
#else
	for (auto i = FontMap.begin(); i != FontMap.end(); i++)
	{
		i->second->Release();
	}
	FontMap.clear();
#endif
#endif

	// Metallicafan212:	Cleanout the texture cache
	Flush(0);

	// Metallicafan212:	Cleanup the blend states
#if !USE_UNODERED_MAP_EVERYWHERE
	for (TMap<PFLAG, ID3D11BlendState*>::TIterator It(BlendMap); It; ++It)
	{
		It.Value()->Release();
	}
	BlendMap.Empty();
#else
	for (auto i = BlendMap.begin(); i != BlendMap.end(); i++)
	{
		i->second->Release();
	}
	BlendMap.clear();
#endif

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
	SAFE_RELEASE(SecondaryVertexBuffer);

	// Metallicafan212:	Render target and back buffer texture
	SAFE_RELEASE(m_BackBuffTex);
	SAFE_RELEASE(m_BackBuffRT);
	SAFE_RELEASE(m_BackBuffUAV);
	SAFE_RELEASE(m_ScreenBuffTex);
	SAFE_RELEASE(m_D3DScreenRTV);
	SAFE_RELEASE(m_ScreenRTSRV);

	SAFE_RELEASE(m_MSAAResolveSRV);
	SAFE_RELEASE(m_MSAAResolveTex);


	// Metallicafan212:	Depth stencil target
	SAFE_RELEASE(m_ScreenDSTex);
	SAFE_RELEASE(m_D3DScreenDSV);
	SAFE_RELEASE(m_ScreenDTSRV);

	// Metallicafan212:	Selection version
	SAFE_RELEASE(m_SelectionDSTex);
	SAFE_RELEASE(m_SelectionDSV);


	// Metallicafan212:	TODO! Do we need to make shaders each time?
	SAFE_DELETE(FTileShader);
	SAFE_DELETE(FGenShader);
	SAFE_DELETE(FResScaleShader);
	SAFE_DELETE(FMeshShader);
	SAFE_DELETE(FSurfShader);
	SAFE_DELETE(FLineShader);
	//SAFE_DELETE(FMSAAShader);
	SAFE_DELETE(ShaderManager);
#if P8_COMPUTE_SHADER
	SAFE_DELETE(FP8ToRGBAShader);
#endif

	CurrentShader = nullptr;

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
	SAFE_RELEASE(m_D3DDeferredContext);
	SAFE_RELEASE(m_D3DCommandList);

	SAFE_RELEASE(FrameConstantsBuffer);
	SAFE_RELEASE(GlobalDistFogBuffer);
	SAFE_RELEASE(GlobalPolyflagsBuffer);

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

	if (!GWin81)
	{
		// Metallicafan212:	Start with 11_0
		FLPtr++;
		FLCount--;
	}

	// Metallicafan212:	On my system, the multithreaded supported device runs ever so slightly faster, for no reason
	UINT Flags =	D3D11_CREATE_DEVICE_BGRA_SUPPORT
				| (bUseMultiThreadedDevice ? 0 : D3D11_CREATE_DEVICE_SINGLETHREADED)
				//|	D3D11_CREATE_DEVICE_SINGLETHREADED
				|	(!bDisableSDKLayers ? D3D11_CREATE_DEVICE_DEBUG : 0)
				;
	
	GLog->Logf(TEXT("DX11: Creating device with the maximum feature level"));

	// Metallicafan212:	Don't allow for deferred rendering if we're not using multi-threading
	if(!bUseMultiThreadedDevice)
		bUseDeferredRendering = 0;

MAKE_DEVICE:
	HRESULT hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, Flags, FLPtr, FLCount, D3D11_SDK_VERSION, &m_D3DDevice, &m_FeatureLevel, &m_D3DDeviceContext);

	// Metallicafan212:	Check if it failed due to the debug layer
	if (FAILED(hr) && (Flags & D3D11_CREATE_DEVICE_DEBUG))
	{
		GLog->Logf(TEXT("DX11: Removing the debug layer from the device flags"));
		Flags &= ~D3D11_CREATE_DEVICE_DEBUG;

		// Metallicafan212:	Don't try again on the next run
		bDisableSDKLayers = 1;

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

	// Metallicafan212:	Initialize the geo shader bool
	//					Wine/proton does not like my geo shader, so we ignore it (for now)
	bUseGeoShaders = !GWineAndDine;

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

	// Metallicafan212:	Figure out the vertex/pixel/geo shader versions
	//					TODO! Could combine with the previous switch, but then I would have to reformat this
	switch (m_FeatureLevel)
	{
		case D3D_FEATURE_LEVEL_11_1:
		case D3D_FEATURE_LEVEL_11_0:
		{
			MaxVSLevel		= "vs_5_0";
			MaxPSLevel		= "ps_5_0";
			MaxGSLevel		= "gs_5_0";
			break;
		}

		case D3D_FEATURE_LEVEL_10_1:
		{
			MaxVSLevel		= "vs_4_1";
			MaxPSLevel		= "ps_4_1";
			MaxGSLevel		= "gs_4_1";
			break;
		}

		case D3D_FEATURE_LEVEL_10_0:
		{
			MaxVSLevel		= "vs_4_0";
			MaxPSLevel		= "ps_4_0";
			MaxGSLevel		= "gs_4_0";
			break;
		}

		// Metallicafan212:	9.3 doesn't allow for pixel shader 3.0 but allows for 4.0?????
		//					Nevermind, turns out my shaders can't compile for this target since integer inputs aren't allowed
		/*
		case D3D_FEATURE_LEVEL_9_3:
		{
			MaxVSLevel		= "vs_4_0_level_9_3";
			MaxPSLevel		= "ps_4_0_level_9_3";
			MaxGSLevel		= "";
			bUseGeoShaders	= 0;
			break;
		}
		*/

		// Metallicafan212:	This doesn't work at all, and only indicates support of 2_x NOT 3_0
		/*
		case D3D_FEATURE_LEVEL_9_3:
		{
			MaxVSLevel		= "vs_3_0";
			MaxPSLevel		= "ps_3_0";
			MaxGSLevel		= "";
			bUseGeoShaders	= 0;
			break;
		}
		*/

		case D3D_FEATURE_LEVEL_9_3:
		case D3D_FEATURE_LEVEL_9_2:
		case D3D_FEATURE_LEVEL_9_1:
		default:
		{
			appErrorf(TEXT("Unsupported feature level %s"), FLStr);
		}
	}

	m_RenderContext = m_D3DDeviceContext;

	if (bUseDeferredRendering && m_FeatureLevel >= D3D_FEATURE_LEVEL_11_0)
	{
		// Metallicafan212:	Create a deferred context (if the user has it enabled)
		hr = m_D3DDevice->CreateDeferredContext(0, &m_D3DDeferredContext);

		if (SUCCEEDED(hr))
		{
			GLog->Logf(TEXT("DX11: Using deferred rendering! This is expirimental!"));
		}
		else
		{
			bUseDeferredRendering = 0;
		}
	}

	GLog->Logf(TEXT("DX11: Using feature level %s"), FLStr);

	// Metallicafan212:	Make the query
	D3D11_QUERY_DESC qDesc = { D3D11_QUERY_EVENT, 0 };
	m_D3DDevice->CreateQuery(&qDesc, &m_D3DQuery);

	// Metallicafan212:	Setup the debug info
#if 1//_DEBUG

	if ((Flags & D3D11_CREATE_DEVICE_DEBUG))
	{
		GLog->Logf(TEXT("DX11: Grabbing debug interface"));

		hr = m_D3DDevice->QueryInterface(__uuidof(ID3D11Debug), (void**)&m_D3DDebug);

		// Metallicafan212:	WINE doesn't return a debug interface....
		if (SUCCEEDED(hr))
		{
			hr = m_D3DDebug->QueryInterface(__uuidof(ID3D11InfoQueue), (void**)&m_D3DQueue);

			if (SUCCEEDED(hr))
			{
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
		}
	}
#endif

	// Metallicafan212:	Setup vertex buffers here
	m_VertexBuffSize				= sizeof(FD3DVert)			* VBUFF_SIZE;
	m_IndexBuffSize					= sizeof(INDEX)				* IBUFF_SIZE;
	m_SecVertexBuffSize				= sizeof(FD3DSecondaryVert) * VBUFF_SIZE;
	D3D11_BUFFER_DESC VertDesc		= { m_VertexBuffSize,		D3D11_USAGE_DYNAMIC, D3D11_BIND_VERTEX_BUFFER, D3D11_CPU_ACCESS_WRITE, NULL, 0 };
	D3D11_BUFFER_DESC SecVertDesc	= { m_SecVertexBuffSize,	D3D11_USAGE_DYNAMIC, D3D11_BIND_VERTEX_BUFFER, D3D11_CPU_ACCESS_WRITE, NULL, 0 };
	D3D11_BUFFER_DESC IndexDesc		= { m_IndexBuffSize,		D3D11_USAGE_DYNAMIC, D3D11_BIND_INDEX_BUFFER, D3D11_CPU_ACCESS_WRITE, NULL, 0 };

	GLog->Logf(TEXT("DX11: Creating vertex buffer"));

	hr = m_D3DDevice->CreateBuffer(&VertDesc, nullptr, &VertexBuffer);

	ThrowIfFailed(hr);

	m_BufferedVerts = 0;
	m_DrawnVerts	= 0;

	GLog->Logf(TEXT("DX11: Creating index buffer"));

	// Metallicafan212:	Index buffer
	hr = m_D3DDevice->CreateBuffer(&IndexDesc, nullptr, &IndexBuffer);

	ThrowIfFailed(hr);

	m_BufferedIndices	= 0;
	m_DrawnIndices		= 0;

#if EXTRA_VERT_INFO
	GLog->Logf(TEXT("DX11: Creating secondary vertex buffer"));

	// Metallicafan212:	Secondary vertex buffer
	hr = m_D3DDevice->CreateBuffer(&SecVertDesc, nullptr, &SecondaryVertexBuffer);

	ThrowIfFailed(hr);

	m_SecVertexBuff			= nullptr;
	m_SecVertexBuffPos		= 0;
#endif

#if DX11_HP2
	GLog->Logf(TEXT("DX11: Creating D2D1 factory1"));

	// Metallicafan212:	D2D manager
	hr = D2D1CreateFactory(/*D2D1_FACTORY_TYPE_MULTI_THREADED*/D2D1_FACTORY_TYPE_SINGLE_THREADED, IID_PPV_ARGS(&m_D2DFact));

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
		DSTTexFormat	= DXGI_FORMAT_R24G8_TYPELESS;
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
	FogShaderVars	= FFogShaderVars();
	FrameShaderVars		= FFrameShaderVars();

	// Metallicafan212:	Recreate the constant buffer as well
	//D3D11_BUFFER_DESC ConstDesc = { sizeof(FFrameShaderVars), D3D11_USAGE_DEFAULT, D3D11_BIND_CONSTANT_BUFFER, 0, 0, 0 };
	D3D11_BUFFER_DESC ConstDesc = { sizeof(FFrameShaderVars), D3D11_USAGE_DYNAMIC, D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE, 0, 0 };

	hr = m_D3DDevice->CreateBuffer(&ConstDesc, nullptr, &FrameConstantsBuffer);

	// Metallicafan212:	IDK, do something here?
	ThrowIfFailed(hr);

	m_RenderContext->VSSetConstantBuffers(0, 1, &FrameConstantsBuffer);
	m_RenderContext->GSSetConstantBuffers(0, 1, &FrameConstantsBuffer);
	m_RenderContext->PSSetConstantBuffers(0, 1, &FrameConstantsBuffer);
	m_RenderContext->CSSetConstantBuffers(0, 1, &FrameConstantsBuffer);

	// Metallicafan212:	Now create one for the distance fog settings
	//D3D11_BUFFER_DESC DistConst = { sizeof(FDistFogVars), D3D11_USAGE_DEFAULT, D3D11_BIND_CONSTANT_BUFFER, 0, 0, 0 };
	D3D11_BUFFER_DESC DistConst = { sizeof(FDistFogVars), D3D11_USAGE_DYNAMIC, D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE, 0, 0 };
	hr = m_D3DDevice->CreateBuffer(&DistConst, nullptr, &GlobalDistFogBuffer);

	ThrowIfFailed(hr);

	m_RenderContext->VSSetConstantBuffers(1, 1, &GlobalDistFogBuffer);
	m_RenderContext->GSSetConstantBuffers(1, 1, &GlobalDistFogBuffer);
	m_RenderContext->PSSetConstantBuffers(1, 1, &GlobalDistFogBuffer);
	m_RenderContext->CSSetConstantBuffers(1, 1, &GlobalDistFogBuffer);

	// Metallicafan212:	And now the dynamic polyflags buffer
	D3D11_BUFFER_DESC PolyConst = { sizeof(FPolyflagVars), D3D11_USAGE_DYNAMIC, D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE, 0, 0 };
	hr = m_D3DDevice->CreateBuffer(&PolyConst, nullptr, &GlobalPolyflagsBuffer);

	ThrowIfFailed(hr);

	m_RenderContext->VSSetConstantBuffers(2, 1, &GlobalPolyflagsBuffer);
	m_RenderContext->GSSetConstantBuffers(2, 1, &GlobalPolyflagsBuffer);
	m_RenderContext->PSSetConstantBuffers(2, 1, &GlobalPolyflagsBuffer);
	m_RenderContext->CSSetConstantBuffers(2, 1, &GlobalPolyflagsBuffer);

	// Metallicafan212:	Setup the pre-defined index values array
	if (!bSetupIValArray)
	{
		for (INT i = 0; i < IBUFF_SIZE; i++)
		{
			IndexValueArray[i] = i;
		}

		// Metallicafan212:	Mark it as initialized
		//					TODO! Find some way to generate this as a static array via preprocessor
		bSetupIValArray = 1;
	}

	// Metallicafan212:	If we're using deferred rendering, we HAVE to map with discard first!!!!
	if (m_D3DDeferredContext != nullptr)
	{
		LockVertAndIndexBuffer(0, 0, 0);
		UnlockBuffers();
	}

	unguard;
}

void UICBINDx11RenderDevice::InitShaders()
{
	guard(UICBINDx11RenderDevice::InitShaders);

#define MAKE_SHADER(Var, cls) \
		if(Var == nullptr) \
		{	\
			GLog->Logf(TEXT("DX11: Making %s"), TEXT(#Var)); \
			Var = new cls(this); \
		}

	// Metallicafan212:	Should probably rename this macro
	MAKE_SHADER(ShaderManager,		FShaderManager);

	MAKE_SHADER(FTileShader,		FD3DTileShader);
	MAKE_SHADER(FGenShader,			FD3DGenericShader);
	MAKE_SHADER(FResScaleShader,	FD3DResScalingShader);
#if USE_COMPUTE_SHADER
	MAKE_SHADER(FMshLghtCompShader,	FD3DLghtMshCompShader);
#endif
	MAKE_SHADER(FMeshShader,		FD3DMeshShader);
	MAKE_SHADER(FSurfShader,		FD3DSurfShader);
	MAKE_SHADER(FLineShader,		FD3DLineShader);
	//MAKE_SHADER(FMSAAShader,		FD3DMSAAShader);
#if P8_COMPUTE_SHADER
	MAKE_SHADER(FP8ToRGBAShader,	FD3DP8ToRGBAShader);
#endif

#undef MAKE_SHADER

	// Metallicafan212:	See if the cache needs to be written
	ShaderManager->SaveCache();

	unguard;
}

// Metallicafan212:	UGH, I'm a fucking idiot. You can't mix MSAA and non-MSAA rendering in DX11..... So I'm going to have to come up with some alternative method to render font tiles correctly
//					Per this page: https://learn.microsoft.com/en-us/windows/win32/api/d3d11/ns-d3d11-d3d11_rasterizer_desc
//					"Note  For feature levels 9.1, 9.2, 9.3, and 10.0, if you set MultisampleEnable to FALSE, 
//					 the runtime renders all points, lines, and triangles without anti-aliasing even for render targets with a sample count greater than 1. 
//					 For feature levels 10.1 and higher, the setting of MultisampleEnable has no effect on points and triangles with regard to MSAA and impacts only the selection of the line-rendering algorithm"
void UICBINDx11RenderDevice::SetRasterState(DWORD State)
{
	guardSlow(UICBINDx11RenderDevice::SetRasterState);

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
#if !USE_UNODERED_MAP_EVERYWHERE
		ID3D11RasterizerState* m_s = RasterMap.FindRef(State);
#else
		auto f = RasterMap.find(State);

		ID3D11RasterizerState* m_s = f != RasterMap.end() ? f->second : nullptr;
#endif

		if (m_s == nullptr)
		{
			CD3D11_RASTERIZER_DESC Desc(D3D11_DEFAULT);

			// Metallicafan212: We want no backface culling
			Desc.CullMode					= D3D11_CULL_NONE;

			// Metallicafan212:	These are defaults
			//Desc.DepthBias					= 0;
			//Desc.DepthBiasClamp				= 0.0f;
			//Desc.DepthClipEnable			= TRUE;
			//Desc.FrontCounterClockwise		= FALSE;
			//Desc.ScissorEnable				= FALSE;

			// Metallicafan212:	Now check the flags
			if (State & DXRS_Wireframe)
			{
				Desc.FillMode = D3D11_FILL_WIREFRAME;
			}
			// Metallicafan212:	This is default
			/*
			else
			{
				Desc.FillMode = D3D11_FILL_SOLID;
			}
			*/

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
#if !USE_UNODERED_MAP_EVERYWHERE
			RasterMap.Set(State, m_s);
#else
			RasterMap[State] = m_s;
#endif
		}

		// Metallicafan212:	Set it
		if(m_s != nullptr)
			m_RenderContext->RSSetState(m_s);

		CurrentRasterState = State;
	}

	unguardSlow;
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

	// Metallicafan212:	MSAA resolving
	m_MSAAResolveSRV	= nullptr;
	m_MSAAResolveTex	= nullptr;

	// Metallicafan212:	Depth stencil target stuff
	m_ScreenDSTex		= nullptr;
	m_ScreenDTSRV		= nullptr;
	m_D3DScreenDSV		= nullptr;
	m_SelectionDSTex	= nullptr;
	m_SelectionDSV		= nullptr;

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
	//FMSAAShader			= nullptr;
	ShaderManager		= nullptr;
#if P8_COMPUTE_SHADER
	FP8ToRGBAShader		= nullptr;
#endif
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

	ConversionMemory	= nullptr;
	ConversionMemSize	= 0;

	m_DrawnIndices		= 0;
	m_DrawnVerts		= 0;
	m_BufferedIndices	= 0;
	m_BufferedVerts		= 0;

	FrameConstantsBuffer	= nullptr;
	GlobalDistFogBuffer		= nullptr;
	GlobalPolyflagsBuffer	= nullptr;
	

	Viewport			= InViewport;

	// Metallicafan212:	Get the style now!!!
	//ViewExtendedStyle = GetWindowLongPtr((HWND)Viewport->GetWindow(), GWL_EXSTYLE);

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
#if P8_COMPUTE_SHADER
	RegisterTextureFormat(TEXF_P8, DXGI_FORMAT_R8G8B8A8_UNORM, 1, 0, 1, &FD3DTexType::RawPitch, nullptr, P8ToRGBA);
#else
	RegisterTextureFormat(TEXF_P8, DXGI_FORMAT_R8G8B8A8_UNORM, 1, 0, 4, &FD3DTexType::RawPitch, &UICBINDx11RenderDevice::P8ToRGBA);
#endif

	RegisterTextureFormat(TEXF_RGBA7, DXGI_FORMAT_B8G8R8A8_UNORM, 1, 0, 4, &FD3DTexType::RawPitch, &UICBINDx11RenderDevice::RGBA7To8);
	
	// Metallicafan212:	I have standardized the TEXF enum between UT469 and HP2 (since BC6H is currently unused and RGBA8 was already called ARGB in the editor/implemented as BGRA8)

	// Metallicafan212:	Raw BGRA texture
#if DX11_UT_469 || DX11_HP2
	RegisterTextureFormat(TEXF_BGRA8, DXGI_FORMAT_B8G8R8A8_UNORM, 0, 0, 4, &FD3DTexType::RawPitch);
#else
	RegisterTextureFormat(TEXF_RGBA8, DXGI_FORMAT_B8G8R8A8_UNORM, 0, 0, 4, &FD3DTexType::RawPitch);
#endif

	// Metallicafan212:	These are all supported by DX11.1
	//					In the future, I will query for support (or use the DX feature level???)
#if 0 // DX11_UT_469 // Turn off for fix issue with dark UT HD textures
	RegisterTextureFormat(TEXF_DXT1, DXGI_FORMAT_BC1_UNORM_SRGB, 0, 1, 8, &FD3DTexType::BlockCompressionPitch);
#else
	// Metallicafan212:	HP2 uses non-SRGB DXT1, so we need to break that off, otherwise the color will be half'd
	RegisterTextureFormat(TEXF_DXT1, DXGI_FORMAT_BC1_UNORM, 0, 1, 8, &FD3DTexType::BlockCompressionPitch);
#endif

	RegisterTextureFormat(TEXF_DXT3, DXGI_FORMAT_BC2_UNORM, 0, 1, 16, &FD3DTexType::BlockCompressionPitch);

	RegisterTextureFormat(TEXF_DXT5, DXGI_FORMAT_BC3_UNORM, 0, 1, 16, &FD3DTexType::BlockCompressionPitch);

	RegisterTextureFormat(TEXF_BC4, DXGI_FORMAT_BC4_UNORM, 0, 1, 8, &FD3DTexType::BlockCompressionPitch);

	RegisterTextureFormat(TEXF_BC5, DXGI_FORMAT_BC5_UNORM, 0, 1, 16, &FD3DTexType::BlockCompressionPitch);

	// Metallicafan212:	I'll take whatever need to be done for BC6, as it's currently unimplemented in HP2
	//					So, if the format needs to be changed, just change it and I'll make it match when I finally finish implementing it
	//					AMD compressonator wasn't producing this correctly (which is why I disabled it)
#if DX11_UT_469 || DX11_HP2
	RegisterTextureFormat(TEXF_BC6H, DXGI_FORMAT_BC6H_UF16, 0, 1, 16, &FD3DTexType::BlockCompressionPitch);
//#else
//	RegisterTextureFormat(TEXF_BC6H, DXGI_FORMAT_BC6H_UF16, 0, 16, &FD3DTexType::BlockCompressionPitch);
#endif

	RegisterTextureFormat(TEXF_BC7, DXGI_FORMAT_BC7_UNORM, 0, 1, 16, &FD3DTexType::BlockCompressionPitch);

	return 1;

	unguard;
}

void UICBINDx11RenderDevice::AutodetectWhiteBalance()
{
	guard(UICBINDx11RenderDevice::AutodetectWhiteBalance);

	GLog->Logf(TEXT("DX11: Autodetecting the display's white balance (in nits)"));

	// Metallicafan212:	Default to a white balance level of 1.0
	FrameShaderVars.WhiteLevel	= 1.0f;

	// Metallicafan212:	Get the current display that the swap chain will display to
	IDXGIOutput* Out			= nullptr;
	HRESULT hr					= m_D3DSwapChain->GetContainingOutput(&Out);

	ThrowIfFailed(hr);

	// Metallicafan212:	Get the description, so we can see where the output is going
	DXGI_OUTPUT_DESC OutDesc;
	Out->GetDesc(&OutDesc);

	// Metallicafan212:	We don't need the output now (we have the info we need)
	//					So release it
	Out->Release();

	// Metallicafan212:	Get the path to the current monitor
	MONITORINFOEX MInfo;
	MInfo.cbSize = sizeof(MInfo);

	GetMonitorInfo(OutDesc.Monitor, &MInfo);

	GLog->Logf(TEXT("DX11: Querying windows for monitor information, to get the HDR white balance level"));

	// Metallicafan212:	This FUCKING sucks, why THE FUCK do I have to do all this EXTERNAL fucking querying, why isn't there a fucking
	//					D3DFuckAdapter->GetCurrentWhiteLevel() or a fucking IDXGIOutput->GetCurrentStupidWhiteLevelMyGod()
	//					What fucking insane programmers are Muskysoft employing these days???????
	//					You NEED the white level for this, BUT THEY MAKE IT VEYR HARD TO OBTAIN!!!!

	// Metallicafan212:	Code adopted from https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-querydisplayconfig#examples
	UINT32 pathCount, modeCount;
	LONG result = GetDisplayConfigBufferSizes(QDC_ONLY_ACTIVE_PATHS, &pathCount, &modeCount);

	if (result != ERROR_SUCCESS)
	{
		GLog->Logf(TEXT("DX11: Failed to query for active path sizes. Error was %d"), result);

		pathCount = 0;
		modeCount = 0;
	}

	if (pathCount == 0)
	{
		GLog->Logf(TEXT("DX11: Returned path count was 0."));
	}

	GLog->Logf(TEXT("DX11: Current monitor path is %s"), MInfo.szDevice);

	// Metallicafan212:	Create buffers to hold all the dumb information
	TArray<DISPLAYCONFIG_PATH_INFO> Paths;
	TArray<DISPLAYCONFIG_MODE_INFO> Modes;

	Paths.AddZeroed(pathCount);
	Modes.AddZeroed(modeCount);

	// Metallicafan212:	Now get it all, holy shit
	result = QueryDisplayConfig(QDC_ONLY_ACTIVE_PATHS, &pathCount, &Paths(0), &modeCount, &Modes(0), nullptr);

	if (result != ERROR_SUCCESS)
	{
		GLog->Logf(TEXT("DX11: Failed to query for active paths. Error was %d"), result);
	}
	else
	{

		// Metallicafan212:	Now loop the paths
		DISPLAYCONFIG_PATH_INFO* CurrentPath = nullptr;

		for (INT i = 0; i < Paths.Num(); i++)
		{
			DISPLAYCONFIG_PATH_INFO& P = Paths(i);

			// Metallicafan212:	Get device path name from this path
			DISPLAYCONFIG_SOURCE_DEVICE_NAME  deviceName ={};
			deviceName.header.adapterId			= P.sourceInfo.adapterId;
			deviceName.header.type				= DISPLAYCONFIG_DEVICE_INFO_GET_SOURCE_NAME;
			deviceName.header.size				= sizeof(deviceName);
			deviceName.header.id				= P.sourceInfo.id;

			result = DisplayConfigGetDeviceInfo(&deviceName.header);

			if (result == ERROR_SUCCESS)
			{
				// Metallicafan212:	See if the path matches
				if (appStrcmp(deviceName.viewGdiDeviceName, MInfo.szDevice) == 0)
				{
					GLog->Logf(TEXT("DX11: Found the current monitor"));

					// Metallicafan212:	Found it, use it
					CurrentPath = &Paths(i);
					break;
				}
			}
			else
			{
				GLog->Logf(TEXT("DX11: Failed to get device path name for path"));
			}
		}

		if (CurrentPath != nullptr)
		{
			// Metallicafan212:	Get the current white level
			DISPLAYCONFIG_SDR_WHITE_LEVEL White ={};
			White.header.type		= DISPLAYCONFIG_DEVICE_INFO_GET_SDR_WHITE_LEVEL;
			White.header.size		= sizeof(White);
			White.header.adapterId	= CurrentPath->targetInfo.adapterId;//AdDesc.AdapterLuid;
			White.header.id			= CurrentPath->targetInfo.id;

			result					= DisplayConfigGetDeviceInfo(&White.header);

			if (result != ERROR_SUCCESS)
			{
				GLog->Logf(TEXT("DX11: Failed to query for white balance level. Error is %d"), result);
				// Metallicafan212:	Sigh....
				FrameShaderVars.WhiteLevel = 1.0f;
			}
			else
			{
				// Metallicafan212:	Get the value back from windows
				HDRWhiteBalanceNits = (White.SDRWhiteLevel / 1000) * 80;
				GLog->Logf(TEXT("DX11: Windows says that the current display has %d nits. Using %f as our white balance multiplier."), HDRWhiteBalanceNits, HDRWhiteBalanceNits / 80.f);
			}
		}
		else
		{
			GLog->Logf(TEXT("DX11: Failed to find display corresponding to the current swap chain ouput"));
		}
	}

	// Metallicafan212:	Set it if we found a value
	if (HDRWhiteBalanceNits > 0)
	{
		FrameShaderVars.WhiteLevel = HDRWhiteBalanceNits / 80.0f;
	}

	LastHDRWhiteBalanceNits = HDRWhiteBalanceNits;

	//GConfig->Flush(0);
	SaveConfig();

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
		m_RenderContext->PSSetShaderResources(i, 0, nullptr);
		m_RenderContext->PSSetSamplers(i, 0, nullptr);
	}

	// Metallicafan212:	CATCH RT TEXTURES!!!!
	RestoreRenderTarget();

	// Metallicafan212:	Clear the RT textures
	ClearRTTextures();

	// Metallicafan212:	Clear any set RT/DC
	m_RenderContext->OMSetRenderTargets(0, nullptr, nullptr);	

	// Metallicafan212:	Reset the raster state
	CurrentRasterState = DXRS_MAX;

	//m_RenderContext->RSSetState(m_DefaultRasterState);
	SetRasterState(DXRS_Normal);

	// Metallicafan212:	Render target and back buffer texture
	SAFE_RELEASE(m_BackBuffTex);
	SAFE_RELEASE(m_BackBuffRT);
	SAFE_RELEASE(m_BackBuffUAV);
	SAFE_RELEASE(m_ScreenBuffTex);
	SAFE_RELEASE(m_D3DScreenRTV);
	SAFE_RELEASE(m_ScreenRTSRV);

	// Metallicafan212:	MSAA resolving textures
	SAFE_RELEASE(m_MSAAResolveSRV);
	SAFE_RELEASE(m_MSAAResolveTex);

	//SAFE_RELEASE(m_ScreenOpacityTex);
	//SAFE_RELEASE(m_ScreenOpacityRTSRV);
	//SAFE_RELEASE(m_D3DScreenOpacityRTV);

	// Metallicafan212:	Depth stencil target
	SAFE_RELEASE(m_ScreenDSTex);
	SAFE_RELEASE(m_D3DScreenDSV);
	SAFE_RELEASE(m_ScreenDTSRV);

	// Metallicafan212:	Selection versions
	SAFE_RELEASE(m_SelectionDSTex);
	SAFE_RELEASE(m_SelectionDSV);

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

		// Metallicafan212:	Copy the description
		GPUDesc = AdDesc.Description;

		// Metallicafan212:	Now get the version number of the driver
		hr = dxgiAdapter->CheckInterfaceSupport(__uuidof(IDXGIDevice), &GPUDriverVer);

		ThrowIfFailed(hr);
		
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
			debugf(TEXT("D3D adapter vendor      : AMD"));
			bIsAMD = 1;
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
		else
		{
			debugf(TEXT("D3D adapter vendor      : Unknown (%0x10)"), AdDesc.VendorId);
		}

		// Metallicafan212:	If to use the new Windows 10 modes. I only test if we're actually running on 10
		//					!GIsEditor is here because using the tearing mode does something fucky in DWM, changing the window in such a way that normal non-DX11 renderers can't draw to it
		//					I need to analyse and see what exactly it's modifying about the window and reverse that change
		bAllowTearing = (!GIsEditor
		
		// Metallicafan212:	2024, I added simple windows version checking to the driver for non-HP2 targets
#if 1//DX11_HP2
			&& GWin10
#endif
			);

		// Metallicafan212:	Don't even test it if it's not able to be grabbed
		IDXGIFactory2* dxgiFactory = nullptr;
		if (bAllowTearing)
		{
			IDXGIFactory5* dxgiFactory5 = nullptr;
			hr = dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory5));

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

				SAFE_RELEASE(dxgiFactory5);
			}

			if (bAllowTearing)
			{
				GLog->Logf(TEXT("DX11: Setting up swap chain with tearing support"));
			}
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

		// Metallicafan212:	See if we should use the HDR compatible mode
	TESTHDR:
		ScreenFormat = (!GIsEditor && UseHDR ? DXGI_FORMAT_R16G16B16A16_FLOAT : DXGI_FORMAT_B8G8R8A8_UNORM);

		// Metallicafan212:	Base this on the feature level
		if (m_FeatureLevel < D3D_FEATURE_LEVEL_11_1)
		{
			bForceRGBA = 1;
			ScreenFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		}
		

		// Metallicafan212:	Describe the non-aa swap chain (MSAA is resolved in Unlock)
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.Width					= SizeX;
		swapChainDesc.Height				= SizeY;
		swapChainDesc.Format				= ScreenFormat;
		swapChainDesc.SampleDesc.Count		= 1;
		swapChainDesc.SampleDesc.Quality	= 0;
		swapChainDesc.BufferUsage			= DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT | DXGI_USAGE_UNORDERED_ACCESS;
		swapChainDesc.BufferCount			= 2 + NumAdditionalBuffers;
		//swapChainDesc.Scaling				= DXGI_SCALING_NONE;
		// Metallicafan212:	If we're on windows 10 or above, use the better DXGI mode
		swapChainDesc.SwapEffect			= (bAllowTearing ? DXGI_SWAP_EFFECT_FLIP_DISCARD : DXGI_SWAP_EFFECT_DISCARD);
		swapChainDesc.Flags					|= (bAllowTearing ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0);

		DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsSwapChainDesc = {};
		fsSwapChainDesc.Windowed			= !bFullscreen;//TRUE;

		GLog->Logf(TEXT("DX11: Creating swap chain for the window"));

	RETRY_SWAP:
		// Metallicafan212:	Create the swap chain now
		hr = dxgiFactory->CreateSwapChainForHwnd(
			m_D3DDevice,
			(HWND)Viewport->GetWindow(),
			&swapChainDesc,
			&fsSwapChainDesc,
			nullptr,
			&m_D3DSwapChain
		);

		if (FAILED(hr) && UseHDR)
		{
			// Metallicafan212:	Swap
			GLog->Logf(TEXT("DX11: Failed to use HDR screen format, swapping back to SDR"));
			UseHDR = 0;
			goto TESTHDR;
		}

		if (FAILED(hr) && !bForceRGBA)
		{
			GLog->Logf(TEXT("DX11: Failed to set default screen format, trying RGBA8"));
			// Metallicafan212:	Test with RGBA8
			ScreenFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

			swapChainDesc.Format = ScreenFormat;

			bForceRGBA = 1;

			goto RETRY_SWAP;
		}
		else if(SUCCEEDED(hr) && !bForceRGBA && UseHDR)
		{
			GLog->Logf(TEXT("DX11: HDR mode active"));

			ActiveHDR = 1;
		}
		else if(FAILED(hr))
		{
			ThrowIfFailed(hr);
		}

		// Metallicafan212:	Make it stop messing with the window itself
		dxgiFactory->MakeWindowAssociation((HWND)Viewport->GetWindow(), DXGI_MWA_NO_WINDOW_CHANGES | DXGI_MWA_NO_ALT_ENTER);

		// Metallicafan212:	Release all the pointers
		dxgiFactory->Release();
		dxgiAdapter->Release();
		dxgiDevice->Release();

		LastAdditionalBuffers = NumAdditionalBuffers;

		// Metallicafan212:	Log the change
		//LONG_PTR esChange	= es ^ GetWindowLongPtr((HWND)Viewport->GetWindow(), GWL_EXSTYLE);
		//LONG_PTR sChange	= s ^ GetWindowLongPtr((HWND)Viewport->GetWindow(), GWL_STYLE);

		//GLog->Logf(TEXT("Window style changes are %llu and %llu"), esChange, sChange);
	}
	else
	{
		// Metallicafan212:	If the buffer count doesn't match, resetup the device
		if (LastAdditionalBuffers != NumAdditionalBuffers)
		{
			SetupDevice();
			goto SetupSwap;
		}

		GLog->Logf(TEXT("DX11: Resizing swap chain"));

		if (bLastFullscreen != bFullscreen)
		{
			GLog->Logf(TEXT("DX11: Toggling fullscreen"));
			hr = m_D3DSwapChain->SetFullscreenState(bFullscreen, nullptr);

			ThrowIfFailed(hr);
		}

		// Metallicafan212:	Resize it
		hr = m_D3DSwapChain->ResizeBuffers(2, SizeX, SizeY, ScreenFormat, (bAllowTearing ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0));

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

	// Metallicafan212:	Initalize shaders, if needed
	InitShaders();

	// Metallicafan212:	Make sure the correct color space is always set
	if (ActiveHDR)
	{
		IDXGISwapChain3* sp3 = nullptr;

		m_D3DSwapChain->QueryInterface(IID_PPV_ARGS(&sp3));

		if (sp3 != nullptr)
		{
			GLog->Logf(TEXT("DX11: Setting HDR colorspace"));
			sp3->SetColorSpace1(DXGI_COLOR_SPACE_RGB_FULL_G10_NONE_P709);//DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020);
			sp3->Release();
		}
	}

	bLastFullscreen = bFullscreen;

	// Metallicafan212:	Get the closer value to the DX11 resource limit!!!!
	if (ResolutionScale == 1.0f)
	{
		ScaledSizeY = SizeY;
		ScaledSizeX = SizeX;
	}
	else
	{
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
	CD3D11_UNORDERED_ACCESS_VIEW_DESC bUAV = CD3D11_UNORDERED_ACCESS_VIEW_DESC(m_BackBuffTex, D3D11_UAV_DIMENSION_TEXTURE2D, ScreenFormat);

	hr = m_D3DDevice->CreateUnorderedAccessView(m_BackBuffTex, &bUAV, &m_BackBuffUAV);

	ThrowIfFailed(hr);

	// Metallicafan212:	Now create the MSAA target
	//					ClampUserOptions already checks what levels of MSAA are supported, and clamps to that
	CD3D11_TEXTURE2D_DESC RTMSAA = CD3D11_TEXTURE2D_DESC();
	RTMSAA.Width				= ScaledSizeX;
	RTMSAA.Height				= ScaledSizeY;
	RTMSAA.BindFlags			= D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	RTMSAA.Format				= ScreenFormat;
	RTMSAA.MipLevels			= 1;
	RTMSAA.SampleDesc.Count		= NumAASamples;
	RTMSAA.SampleDesc.Quality	= 0;//D3D11_STANDARD_MULTISAMPLE_PATTERN;//0;
	RTMSAA.ArraySize			= 1;

	hr = m_D3DDevice->CreateTexture2D(&RTMSAA, nullptr, &m_ScreenBuffTex);

	ThrowIfFailed(hr);

	// Metallicafan212:	Now create the views
	hr = m_D3DDevice->CreateRenderTargetView(m_ScreenBuffTex, nullptr, &m_D3DScreenRTV);

	ThrowIfFailed(hr);

	// Metallicafan212:	Create a shader resource view for MSAA resolving
	CD3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = CD3D11_SHADER_RESOURCE_VIEW_DESC();
	srvDesc.Format						= ScreenFormat;
	srvDesc.ViewDimension				= NumAASamples > 1 ? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;//D3D11_SRV_DIMENSION_TEXTURE2DMS;
	srvDesc.Texture2D.MostDetailedMip	= 0;
	srvDesc.Texture2D.MipLevels			= 1;

	hr = m_D3DDevice->CreateShaderResourceView(m_ScreenBuffTex, &srvDesc, &m_ScreenRTSRV);

	ThrowIfFailed(hr);

	// Metallicafan212:	Intermediate texture for MSAA resolving
	RTMSAA.SampleDesc.Count = 1;
	hr = m_D3DDevice->CreateTexture2D(&RTMSAA, nullptr, &m_MSAAResolveTex);
	ThrowIfFailed(hr);

	// Metallicafan212:	Now a resource view for it
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	hr = m_D3DDevice->CreateShaderResourceView(m_MSAAResolveTex, &srvDesc, &m_MSAAResolveSRV);
	ThrowIfFailed(hr);

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

	// Metallicafan212:	Version for selection
	depthStencilDesc.SampleDesc.Count = 1;

	hr = m_D3DDevice->CreateTexture2D(&depthStencilDesc, nullptr, &m_SelectionDSTex);

	ThrowIfFailed(hr);

	// Metallicafan212:	Now we need to declare the view as the right format
	CD3D11_DEPTH_STENCIL_VIEW_DESC dtVDesc = CD3D11_DEPTH_STENCIL_VIEW_DESC();
	dtVDesc.Flags				= 0;
	dtVDesc.Format				= DSTFormat;//DXGI_FORMAT_D24_UNORM_S8_UINT;
	dtVDesc.ViewDimension		= NumAASamples > 1 ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
	dtVDesc.Texture2D.MipSlice	= 0;

	hr = m_D3DDevice->CreateDepthStencilView(m_ScreenDSTex, &dtVDesc, &m_D3DScreenDSV);

	ThrowIfFailed(hr);

	// Metallicafan212:	Selection version of the view
	dtVDesc.ViewDimension		= D3D11_DSV_DIMENSION_TEXTURE2D;

	hr = m_D3DDevice->CreateDepthStencilView(m_SelectionDSTex, &dtVDesc, &m_SelectionDSV);

	ThrowIfFailed(hr);


	// Metallicafan212:	Now make the depth shader resource
	srvDesc.Format				= DSTSRVFormat;
	srvDesc.ViewDimension		= NumAASamples > 1 ? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;
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
#endif

	// Metallicafan212:	Make a totally blank texture
	D3D11_TEXTURE2D_DESC Desc;
	appMemzero(&Desc, sizeof(Desc));

	Desc.Format				= DXGI_FORMAT_R8_UNORM;
	Desc.Width				= 2;
	Desc.Height				= 2;
	Desc.MipLevels			= 1;
	Desc.Usage				= D3D11_USAGE_DEFAULT;
	Desc.BindFlags			= D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	Desc.ArraySize			= 1;
	// Metallicafan212:	Not needed
	Desc.CPUAccessFlags		= 0;//D3D11_CPU_ACCESS_WRITE;
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
	m_RenderContext->OMSetDepthStencilState(m_DefaultZState, 1);

	// Metallicafan212:	And now a version with no z writing
	dsDesc.DepthWriteMask					= D3D11_DEPTH_WRITE_MASK_ZERO;
	dsDesc.DepthFunc						= D3D11_COMPARISON_LESS_EQUAL;
	hr = m_D3DDevice->CreateDepthStencilState(&dsDesc, &m_DefaultNoZState);
	ThrowIfFailed(hr);

	// Metallicafan212:	Set the index and vertex buffers now (since we don't swap them in and out)
	UINT Stride		= sizeof(FD3DVert);
	UINT SecStride	= sizeof(FD3DSecondaryVert);
	UINT Offset		= 0;
	
	if (m_D3DDeferredContext != nullptr)
	{
		m_D3DDeferredContext->IASetIndexBuffer(IndexBuffer, INDEX_FORMAT, 0);
		m_D3DDeferredContext->IASetVertexBuffers(0, 1, &VertexBuffer, &Stride, &Offset);
#if EXTRA_VERT_INFO
		m_D3DDeferredContext->IASetVertexBuffers(1, 1, &SecondaryVertexBuffer, &SecStride, &Offset);
#endif
	}
	else
	{
		m_D3DDeviceContext->IASetIndexBuffer(IndexBuffer, INDEX_FORMAT, 0);
		m_D3DDeviceContext->IASetVertexBuffers(0, 1, &VertexBuffer, &Stride, &Offset);
#if EXTRA_VERT_INFO
		m_D3DDeviceContext->IASetVertexBuffers(1, 1, &SecondaryVertexBuffer, &SecStride, &Offset);
#endif
	}
	

	// Metallicafan212:	Keep the lock version updated
	LastAASamples		= NumAASamples;
	LastResolutionScale = ResolutionScale;

	// Metallicafan212:	Make all the samplers that should be needed
	for (INT i = 0; i < 2; i++)
	{
		GetSamplerState(PF_NoSmooth, i, 0);
		GetSamplerState(PF_ClampUVs, i, 0);

		GetSamplerState(PF_NoSmooth | PF_ClampUVs, i, 0);
	}

	if (HDRWhiteBalanceNits <= 0)
	{
		// Metallicafan212:	Autodetect it
		AutodetectWhiteBalance();
	}

	unguard;
}

UBOOL UICBINDx11RenderDevice::SetRes(INT NewX, INT NewY, INT NewColorBytes, UBOOL Fullscreen)
{
	guard(UICBINDx11RenderDevice::SetRes);

	INT TestX = Max(NewX, 2);
	INT TestY = Max(NewY, 2);

	if (Viewport != nullptr)
	{
		// Metallicafan212:	Force 32bit color at all times????
		if (!Viewport->ResizeViewport(BLIT_Direct3D | (Fullscreen ? BLIT_Fullscreen : BLIT_HardwarePaint), TestX, TestY, 4))
		{
			return 0;
		}
	}

	// Metallicafan212:	Save the values
	//					We also have to clamp here since unreal could pass bad values (the editor opening, for example)
	SizeX		= TestX;
	SizeY		= TestY;

	// Metallicafan212:	Resetup resources that need to be sized
	bFullscreen = Fullscreen;
	SetupResources();

	// Metallicafan212:	Set the viewport now
	SetSceneNode(nullptr);

	return 1;

	unguard;
}

void UICBINDx11RenderDevice::Exit()
{
	guard(UICBINDx11RenderDevice::Exit);

	Flush(0);

	// Metallicafan212:	Free the memory buffer if it's used
	if (ConversionMemory != nullptr)
	{
		appFree(ConversionMemory);
		ConversionMemory = nullptr;
	}

	SAFE_RELEASE(VertexBuffer);
	SAFE_RELEASE(IndexBuffer);
	SAFE_RELEASE(SecondaryVertexBuffer);

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
	//SAFE_DELETE(FMSAAShader);
	SAFE_DELETE(ShaderManager);
#if P8_COMPUTE_SHADER
	SAFE_DELETE(FP8ToRGBAShader);
#endif
#if USE_COMPUTE_SHADER
	SAFE_DELETE(FMshLghtCompShader);
#endif

#if DX11_HP2
#if !USE_UNODERED_MAP_EVERYWHERE
	// Metallicafan212:	Cleanup all the fonts
	for (TMap<FString, IDWriteTextFormat*>::TIterator It(FontMap); It; ++It)
	{
		It.Value()->Release();
	}
	FontMap.Empty();
#else
	for (auto i = FontMap.begin(); i != FontMap.end(); i++)
	{
		i->second->Release();
	}
	FontMap.clear();
#endif
#endif

	// Metallicafan212:	Cleanup the blend states
#if !USE_UNODERED_MAP_EVERYWHERE
	for (TMap<FPLAG, ID3D11BlendState*>::TIterator It(BlendMap); It; ++It)
	{
		It.Value()->Release();
	}
	BlendMap.Empty();
#else
	for (auto i = BlendMap.begin(); i != BlendMap.end(); i++)
	{
		i->second->Release();
	}
	BlendMap.clear();
#endif

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
	SAFE_RELEASE(m_D3DDeferredContext);
	SAFE_RELEASE(m_D3DCommandList);
	SAFE_RELEASE(m_D3DDevice);

	unguard;
}

void UICBINDx11RenderDevice::Flush(UBOOL AllowPrecache)
{
	guard(UICBINDx11RenderDevice::Flush);

	TextureMap.Flush();

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

	// Metallicafan212:	Check gamma as well
#if !DX11_HP2
	Gamma = Viewport->GetOuterUClient()->Brightness * 2.0f;
#endif

	/*
	if (bUseDeferredRendering && m_D3DDeferredContext != nullptr)
	{
		m_RenderContext = m_D3DDeferredContext;
	}
	else
	{
		m_RenderContext = m_D3DDeviceContext;
	}
	*/

	// Metallicafan212:	Hold onto the hit related info
	m_HitData		= HitData;
	m_HitSize		= HitSize;
	m_HitCount		= 0;

	if (m_HitData != nullptr)
	{
		// Metallicafan212:	Reset the pixel hit state
		//PixelHitInfo.AddItem(FPixHitInfo());
		PixelTopIndex = -1;

		m_HitBufSize	= *m_HitSize;

		// Metallicafan212:	Tell unreal there was no hits (so far)
		*m_HitSize		= 0;
	}

	// Metallicafan212:	Require setting buffering on the first draw
	m_CurrentBuff = BT_None;

	// Metallicafan212:	Check if our lock flags changed
	if (LastAASamples != NumAASamples || LastAFSamples != NumAFSamples || LastResolutionScale != ResolutionScale || LastAdditionalBuffers != NumAdditionalBuffers)
	{
		if (LastAASamples != NumAASamples || LastResolutionScale != ResolutionScale || LastAdditionalBuffers != NumAdditionalBuffers)
		{
			// Metallicafan212:	Doing this will also flush the sampler state
			SetupResources();
		}
		else
		{
			FlushTextureSamplers();
		}
	}

	// Metallicafan212:	Only calculate it if needed
	if (LastHDRWhiteBalanceNits != HDRWhiteBalanceNits)
	{
		// Metallicafan212:	If it was set to 0 or below, autodetect it again
		if (HDRWhiteBalanceNits <= 0)
		{
			AutodetectWhiteBalance();
		}

		FrameShaderVars.WhiteLevel	= HDRWhiteBalanceNits / 80.0f;

		// Metallicafan212:	Update the last var
		LastHDRWhiteBalanceNits		= HDRWhiteBalanceNits;
	}

	// Metallicafan212:	Gamma is disabled in HP2 because a speedrunning trick involves messing with the brighness bar
	//					So, to account for that, there's a manual gamma value (for the time being...)
	//					11/28/23, added a manual gamma offset (for the time being, until I add in multiple gamma modes)
	FrameShaderVars.Gamma				= (!Viewport->IsOrtho() ? Gamma + GammaOffset : 1.0f);

	FrameShaderVars.GammaMode			= GammaMode;

	FrameShaderVars.HDRExpansion		= AdditionalHDRExpansion;

	FrameShaderVars.ResolutionScale		= ResolutionScale;

	// Metallicafan212:	DX9 specific gamma vars
	//					This is based on the UD3D9RenderDevice::BuildGammaRamp code, although it might not be very accurate (yet)
	FrameShaderVars.GammaOffsetRed		= (1.0f / (1.25f * (FrameShaderVars.Gamma + GammaOffsetRed)));
	FrameShaderVars.GammaOffsetGreen	= (1.0f / (1.25f * (FrameShaderVars.Gamma + GammaOffsetGreen)));
	FrameShaderVars.GammaOffsetBlue		= (1.0f / (1.25f * (FrameShaderVars.Gamma + GammaOffsetBlue)));

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

#if DX11_HP2
	FrameShaderVars.bDepthDraw				= Viewport->Actor != nullptr ? Viewport->Actor->RendMap == REN_Depth : 0;
	FrameShaderVars.DepthZRange				= DepthDrawZLimit;
#endif

	// Metallicafan212:	Set this ONCE!
	FrameShaderVars.bDoSelection			= HitData != nullptr;

	FrameShaderVars.bEnableCorrectFogging	= bEnableCorrectFogging;
	FrameShaderVars.bOneXLightmaps			= bOneXLightmaps;
	FrameShaderVars.bHDR					= ActiveHDR;

	// Metallicafan212:	Make sure the RT is set?
	//if (RTStack.Num())
	//{
	RestoreRenderTarget();
	//}

	// Metallicafan212:	Only clear if we're in the editor?
	if (//GIsEditor || 
		//(RenderLockFlags & LOCKR_ClearScreen))
		1)
	{
		SetBlend(PF_Occlude);

		// Metallicafan212:	Only clear if we have the screen clear set
		//if (RenderLockFlags & LOCKR_ClearScreen)
		// Metallicafan212:	TODO! Figure out why this fucks up with the loading screen
		if (1)
		{
			// Metallicafan212:	If we're hit testing, always clear to the first hit color
			if (HitData != nullptr)
			{
				constexpr FLOAT FirstIndex[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
				//m_RenderContext->ClearRenderTargetView(m_D3DScreenRTV, FirstIndex);
				m_RenderContext->ClearRenderTargetView(m_BackBuffRT, FirstIndex);

				m_RenderContext->ClearDepthStencilView(m_SelectionDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
			}
			else
			{
				m_RenderContext->ClearRenderTargetView(m_D3DScreenRTV, &ScreenClear.X);
			}
		}

		// Metallicafan212:	However, always clear depth
		m_RenderContext->ClearDepthStencilView(m_D3DScreenDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}

	// Metallicafan212:	Make sure Z buffering is ALWAYS on
	m_RenderContext->OMSetDepthStencilState(m_DefaultZState, 0);

	// Metallicafan212:	Hold onto the flash fog for future render
	FlashScale		= InFlashScale;
	FlashFog		= InFlashFog;

	if (FogShaderVars.bDoDistanceFog || FogShaderVars.bFadeFogValues)
		TickDistanceFog();

	// Metallicafan212:	Update the shader variables
	UpdateGlobalShaderVars();

	unguard;
}

void UICBINDx11RenderDevice::Unlock(UBOOL Blit)
{
	guard(UICBINDx11RenderDevice::Unlock);

	// Metallicafan212:	Reset the buffered state
	EndBuffering();

	// Metallicafan212:	Restore our render target (as it may have been changed due to RT textures)
	while(RTStack.Num())
		RestoreRenderTarget();

	// Metallicafan212:	Fix the render state
#if DX11_HP2
	if (ExtraRasterFlags != 0)
	{
		ExtraRasterFlags = 0;
		SetRasterState(DXRS_Normal);
	}
#endif

	/*
	// Metallicafan212:	Render now?
	if (bUseDeferredRendering)
	{
		// Metallicafan212:	Get a command list to execute
		ID3D11CommandList* CommandList	= nullptr;
		HRESULT hr						= m_D3DDeferredContext->FinishCommandList(TRUE, &CommandList);

		if (SUCCEEDED(hr))
		{
			m_D3DDeviceContext->ExecuteCommandList(CommandList, TRUE);

			CommandList->Release();
		}
	}
	*/

	// Metallicafan212:	Get the selection
	if (m_HitData != nullptr)
	{
		// Metallicafan212:	Check the hits now
		DetectPixelHit();

		// Metallicafan212:	Clear them now
		PixelHitInfo.Empty();

		//ClearPixelHits();
		*m_HitSize = m_HitCount;

		// Metallicafan212:	Selection is drawn directly to the back buffer now (to save on resources)
		//					So running the shader on top isn't needed
		if(bDebugSelection)
			goto JUST_PRESENT;
		// Metallicafan212:	TODO! Add this as a debug option
		//if(bDebugSelection)
		//	Blit = 1;
	}

	if (Blit)
	{
		// Metallicafan212:	Copy to the screen
		if (NumAASamples > 1)
		{
#if DX11_USE_MSAA_SHADER

#if USE_MSAA_COMPUTE
			/*
			if (bUseMSAAComputeShader)
			{
				// Metallicafan212:	Use a compute shader instead!!!
				SetTexture(0, nullptr, 0);
				//SetTexture(1, nullptr, 0);

				m_D3DDeviceContext->OMSetRenderTargets(0, nullptr, nullptr);

				m_D3DDeviceContext->CSSetShaderResources(0, 1, &m_ScreenRTSRV);
				//m_RenderContext->CSSetShaderResources(1, 1, &m_ScreenDTSRV);

				// Metallicafan212:	Now bind the output
				m_D3DDeviceContext->CSSetUnorderedAccessViews(0, 1, &m_BackBuffUAV, nullptr);

				// Metallicafan212:	Bind the shader
				FMSAAShader->Bind(m_D3DDeviceContext);

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
				*/
#else

				// Metallicafan212:	We have to do geometry now...
				EndBuffering();

				// Metallicafan212:	Order of operations, make sure the alpha rejection is set
				SetBlend(0);

				SetTexture(0, nullptr, 0);
				SetTexture(1, nullptr, 0);

				// Metallicafan212:	Manually setup the vars...
				m_RenderContext->OMSetRenderTargets(1, &m_BackBuffRT, nullptr);
				m_RenderContext->PSSetShaderResources(0, 1, &m_ScreenRTSRV);
				m_RenderContext->PSSetShaderResources(1, 1, &m_ScreenDTSRV);

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
				//m_RenderContext->OMGetDepthStencilState(&CurState, &Sten);
				//m_RenderContext->OMSetDepthStencilState(m_DefaultNoZState, 0);

				LockVertexBuffer(6 * sizeof(FD3DVert));

				//m_RenderContext->IASetInputLayout(nullptr);

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

				//m_RenderContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				AdvanceVertPos();//6);

				// Metallicafan212:	Draw
				EndBuffering();

				// Metallicafan212:	Reset Z state
				//m_RenderContext->OMSetDepthStencilState(CurState, Sten);

				SetTexture(0, nullptr, 0);
				SetTexture(1, nullptr, 0);

				RestoreRenderTarget();
#endif
			/*
			}
			else
			*/
#endif
			{
				//m_D3DDeviceContext->ResolveSubresource(m_BackBuffTex, 0, m_ScreenBuffTex, 0, ScreenFormat);
				m_D3DDeviceContext->ResolveSubresource(m_MSAAResolveTex, 0, m_ScreenBuffTex, 0, ScreenFormat);
			}
		}
		// Metallicafan212:	Always use the resolution scaling shader, so we can do final effects on the screen
		//else
		{
#if USE_RES_COMPUTE
			// Metallicafan212:	Use a compute shader instead!!!
			SetTexture(0, nullptr, 0);

			m_D3DDeviceContext->OMSetRenderTargets(0, nullptr, nullptr);

			m_D3DDeviceContext->CSSetShaderResources(0, 1, &m_ScreenRTSRV);
			//m_RenderContext->CSSetShaderResources(1, 1, &m_ScreenDTSRV);

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
				if (ResolutionScale == 1.0f)
				{
					SDesc.Filter			= D3D11_FILTER_MIN_MAG_MIP_POINT;
				}
				else
				{
					SDesc.Filter			= D3D11_FILTER_MIN_MAG_MIP_LINEAR;
				}//D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR;//D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;//D3D11_FILTER_MIN_MAG_MIP_LINEAR;
				SDesc.AddressU			= D3D11_TEXTURE_ADDRESS_CLAMP;
				SDesc.AddressV			= SDesc.AddressU;
				SDesc.AddressW			= SDesc.AddressU;//D3D11_TEXTURE_ADDRESS_WRAP;//SDesc.AddressU;
				SDesc.MinLOD			= 0.0f;
				SDesc.MaxLOD			= FLT_MAX;//0.0f;
				SDesc.MipLODBias		= 0.0f;//-16.0f;
				SDesc.MaxAnisotropy		= 1;//16;//16;
				SDesc.ComparisonFunc	= D3D11_COMPARISON_NEVER;

				HRESULT hr				= m_D3DDevice->CreateSamplerState(&SDesc, &ScreenSamp);

				ThrowIfFailed(hr);
			}

			// Metallicafan212:	Start buffering now
			StartBuffering(BT_ScreenFlash);

			// Metallicafan212:	Render as a quad 🙃🙃🙃🙃🙃🙃🙃
			//EndBuffering();

			// Metallicafan212:	Order of operations, make sure the alpha rejection is set
			SetBlend(PF_Occlude);

			SetTexture(0, nullptr, 0);

			m_RenderContext->PSSetSamplers(0, 1, &ScreenSamp);

			// Metallicafan212:	Manually setup the vars...
			m_RenderContext->OMSetRenderTargets(1, &m_BackBuffRT, nullptr);
			m_RenderContext->PSSetShaderResources(0, 1, NumAASamples > 1 ? &m_MSAAResolveSRV : &m_ScreenRTSRV);

			SetSceneNode(nullptr);

			FResScaleShader->Bind(m_RenderContext);

			/*
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

			//RPX1 *= Z;
			//RPX2 *= Z;
			//RPY1 *= Z;
			//RPY2 *= Z;
			*/

			//LockVertexBuffer(6 * sizeof(FD3DVert));
			LockVertAndIndexBuffer(6);

			appMemcpy(m_VertexBuff, ScreenVerts, sizeof(FD3DVert) * 6);

			/*
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
			*/

			//m_RenderContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			AdvanceVertPos();

			// Metallicafan212:	Draw
			EndBuffering();

			//SetTexture(0, nullptr, 0);
			// Metallicafan212:	Fix the shader holding onto the RT texture
			m_RenderContext->PSSetShaderResources(0, 1, &BlankResourceView);

			RestoreRenderTarget();

			/*
			// Metallicafan212:	Render now?
			//					TODO! This should probably be called on the direct device context
			if (bUseDeferredRendering)
			{
				// Metallicafan212:	Get a command list to execute
				ID3D11CommandList* CommandList	= nullptr;
				HRESULT hr						= m_D3DDeferredContext->FinishCommandList(TRUE, &CommandList);

				if (SUCCEEDED(hr))
				{
					m_D3DDeviceContext->ExecuteCommandList(CommandList, TRUE);

					CommandList->Release();
				}
			}
			*/
#endif
		}

	JUST_PRESENT:
		static constexpr DXGI_PRESENT_PARAMETERS Parm{ 0, nullptr, nullptr, nullptr };
		HRESULT hr = m_D3DSwapChain->Present1(UseVSync ? 1 : 0, (bAllowTearing && !bFullscreen && !UseVSync ? DXGI_PRESENT_ALLOW_TEARING : 0), &Parm);

		// Metallicafan212:	Check the return code
		switch (hr)
		{
			// Metallicafan212:	Check if DXGI needs a resize (alt+tab in fullscreen for example)
			case DXGI_ERROR_INVALID_CALL:
			{
				SetupResources();
				break;
			}

			// Metallicafan212:	Errors here indicate large failures
			case DXGI_ERROR_DEVICE_REMOVED:
			case DXGI_ERROR_DEVICE_RESET:
			{
				// Metallicafan212:	Figure out the issue
				HRESULT reason = m_D3DDevice->GetDeviceRemovedReason();

				switch (reason)
				{
					case DXGI_ERROR_DEVICE_HUNG:
					{
						appErrorf(TEXT("DX11: Device hung, cannot continue"));

						break;
					}

					case DXGI_ERROR_DEVICE_REMOVED:
					{
						GWarn->Logf(TEXT("DX11: Device removed, recreating the driver resources"));

						SetupDevice();
						SetupResources();

						break;
					}

					case DXGI_ERROR_DEVICE_RESET:
					{
						GWarn->Logf(TEXT("DX11: Device reset, recreating the driver resources"));
						SetupDevice();
						SetupResources();

						break;
					}

					case DXGI_ERROR_DRIVER_INTERNAL_ERROR:
					{
						GWarn->Logf(TEXT("DX11: Driver internal error, recreating the device"));

						SetupDevice();
						SetupResources();

						break;
					}

					case DXGI_ERROR_INVALID_CALL:
					{
						appErrorf(TEXT("DX11: Device removed reason is invalid call"));

						break;
					}
				}

				break;
			}

			default:
			{
				// Metallicafan212:	Other errors? Throw if so
				ThrowIfFailed(hr);
			}
		}
	}

	// Metallicafan212:	Allow the user to see the selection rendering (rather than quickly swapping back)
	if (bDebugSelection && m_HitData != nullptr)
	{
		appSleep(1.0f);
	}

	// Metallicafan212:	Get optional debug info
	if (m_D3DQueue != nullptr)
	{
		UINT64 n = m_D3DQueue->GetNumStoredMessages();

		if (n > 0)
		{
			// Metallicafan212:	Get the current timestamp
			const TCHAR* tStamp = appTimestamp();

			GLog->Logf(TEXT("DX11: Warnings and errors during this frame:"));

			for (INT i = 0; i < n; i++)
			{
				// Metallicafan212:	Sigh.... No better way to do this...
				D3D11_MESSAGE* temp = nullptr;

				// Metallicafan212:	Get the size of this message
				SIZE_T mSize = 0;

				m_D3DQueue->GetMessage(i, nullptr, &mSize);

				if (mSize != 0)
				{
					temp = (D3D11_MESSAGE*)appMalloc(mSize, TEXT("DX11 Debug Message"));

					m_D3DQueue->GetMessage(i, temp, &mSize);

					// Metallicafan212:	Now log it
					GWarn->Logf(TEXT("DX11 debug message (%s): %s at %s"), GetD3DDebugSeverity(temp->Severity), appFromAnsi(temp->pDescription), tStamp);

					appFree(temp);
				}
			}
		}
	}

	unguard;
}

void UICBINDx11RenderDevice::ClearZ(FSceneNode* Frame)
{
	guard(UICBINDx11RenderDevice::ClearZ);

	EndBuffering();

	// Metallicafan212:	Turn back on Z buffering
	SetBlend(PF_Occlude);

	// Metallicafan212:	Clear the current DSV instead of the local one
	if (BoundRT != nullptr)
	{
		m_RenderContext->ClearDepthStencilView(BoundRT->DTView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}
	else
	{
		if (m_HitData != nullptr)
		{
			m_RenderContext->ClearDepthStencilView(m_SelectionDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		}
		else
		{
			m_RenderContext->ClearDepthStencilView(m_D3DScreenDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		}
	}

	unguard;
}

void UICBINDx11RenderDevice::EndFlash()
{
	guard(UICBINDx11RenderDevice::EndFlash);

	// Metallicafan212:	Draw it as a tile, but using the generic shader
#if DX11_HP2
	if ((FlashScale != FPlane(0.5f, 0.5f, 0.5f, 0.5f)) || (FlashFog != FPlane(0.0f, 0.0f, 0.0f, 0.0f)))
#else
	if ((FlashScale != FPlane(0.5f, 0.5f, 0.5f, 0.0f)) || (FlashFog != FPlane(0.0f, 0.0f, 0.0f, 0.0f)))
#endif
	{
		EndBuffering();

		// Metallicafan212:	Start buffering now
		StartBuffering(BT_ScreenFlash);

		// Metallicafan212:	Order of operations, make sure the alpha rejection is set
		SetBlend(PF_Highlighted);

		SetTexture(0, nullptr, 0);

		FGenShader->Bind(m_RenderContext);

#if DX11_HP2
		FPlane Color = FPlane(FlashFog.X, FlashFog.Y, FlashFog.Z, 1.0f - Min(FlashScale.W * 2.0f, 1.0f));
#else
		FPlane Color = FPlane(FlashFog.X, FlashFog.Y, FlashFog.Z, 1.0f - Min(FlashScale.X * 2.0f, 1.0f));
#endif

		FLOAT Z		= 0.5f;
		FLOAT X		= 0.0f;
		FLOAT Y		= 0.0f;

		// Metallicafan212:	Test?
		if (m_nearZRangeHackProjectionActive)
		{
			Z = 0.1f;
		}

		FLOAT XL	= ScaledSceneNodeX;
		FLOAT YL	= ScaledSceneNodeY;

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
		m_RenderContext->OMGetDepthStencilState(&CurState, &Sten);
		m_RenderContext->OMSetDepthStencilState(m_DefaultNoZState, 0);

		//LockVertexBuffer(6 * sizeof(FD3DVert));
		LockVertAndIndexBuffer(6);

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

		AdvanceVertPos();

		// Metallicafan212:	Draw
		EndBuffering();

		// Metallicafan212:	Reset Z state
		m_RenderContext->OMSetDepthStencilState(CurState, Sten);
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

		m_RenderContext->RSSetViewports(1, &viewport);

		// Metallicafan212:	All of this is copied from the DX9 driver
		// Precompute stuff.
		FLOAT rcpFrameFX = 1.0f / NewX;
		m_Aspect = NewY * rcpFrameFX;

		// Metallicafan212:	This is HP2 specific! Since I have a viewport FOV that is calcuated to be a hor+ FOV, so 90 @ 16x9 is 109
#if DX11_HP2
		m_RProjZ = appTan(Viewport->FOVAngle * PI / 360.0);
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
		// Metallicafan212:	Scale the resolution!
		FLOAT NewX	= Frame->FX;
		FLOAT NewY	= Frame->FY;
		FLOAT NewXB	= Frame->XB;
		FLOAT NewYB = Frame->YB;

		if (BoundRT == nullptr)
		{
			NewX	*= ResolutionScale;
			NewY	*= ResolutionScale;
			NewXB	*= ResolutionScale;
			NewYB	*= ResolutionScale;

			// Metallicafan212:	Scaled FX2 and FY2
			ScaledFX2 = Frame->FX2 * ResolutionScale;
			ScaledFY2 = Frame->FY2 * ResolutionScale;
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
			NewXB, NewYB,
			NewX, NewY, 0.f, 1.f
		};

		m_RenderContext->RSSetViewports(1, &viewport);

		SizeX = Frame->X;
		SizeY = Frame->Y;

		// Metallicafan212:	All of this is copied from the DX9 driver
		// Precompute stuff.
		FLOAT rcpFrameFX = 1.0f / NewX;
		m_Aspect = NewY * rcpFrameFX;

		// Metallicafan212:	This is HP2 specific! Since I have a viewport FOV that is calcuated to be a hor+ FOV, so 90 @ 16x9 is 109
#if DX11_HP2
		m_RProjZ = appTan(Viewport->FOVAngle * PI / 360.0);
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

	FrameShaderVars.ViewX = ScaledSceneNodeX;
	FrameShaderVars.ViewY = ScaledSceneNodeY;

	SetProjectionStateNoCheck(0, 1);

	unguard;
}

// Metallicafan212:	TODO! Rewrite this to check if it needs an update (changed variables, etc.)
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
#if DX11_HP2 || DX11_UNREAL_227 || DX11_UT_469
	// Metallicafan212:	Increased to the next power of two
	zFar = 65535.0f;
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

#if 0

	FrameShaderVars.Proj.m[0][0] = 2.0f * zNear * invRightMinusLeft;
	FrameShaderVars.Proj.m[0][1] = 0.0f;
	FrameShaderVars.Proj.m[0][2] = 0.0f;
	FrameShaderVars.Proj.m[0][3] = 0.0f;

	FrameShaderVars.Proj.m[1][0] = 0.0f;
	FrameShaderVars.Proj.m[1][1] = 2.0f * zNear * invTopMinusBottom;
	FrameShaderVars.Proj.m[1][2] = 0.0f;
	FrameShaderVars.Proj.m[1][3] = 0.0f;

#if RES_SCALE_IN_PROJ
	FrameShaderVars.Proj.m[2][0] = 1.0f		/ m_sceneNodeX;//(FLOAT)m_sceneNodeX;
	FrameShaderVars.Proj.m[2][1] = -1.0f	/ m_sceneNodeY;//(FLOAT)m_sceneNodeY;
#else
	FrameShaderVars.Proj.m[2][0] = 1.0f		/ ScaledSceneNodeX;//(FLOAT)m_sceneNodeX;
	FrameShaderVars.Proj.m[2][1] = -1.0f	/ ScaledSceneNodeY;//(FLOAT)m_sceneNodeY;
#endif
	FrameShaderVars.Proj.m[2][2] = zScaleVal * (zFar * invNearMinusFar);
	FrameShaderVars.Proj.m[2][3] = -1.0f;

	FrameShaderVars.Proj.m[3][0] = 0.0f;
	FrameShaderVars.Proj.m[3][1] = 0.0f;
	FrameShaderVars.Proj.m[3][2] = zScaleVal * zScaleVal * (zNear * zFar * invNearMinusFar);
	FrameShaderVars.Proj.m[3][3] = 0.0f;

#if RES_SCALE_IN_PROJ
	//FrameShaderVars.Proj *= DirectX::XMMatrixScaling(1.0f / ResolutionScale, 1.0f / ResolutionScale, 1.0f);
#endif

	//FrameShaderVars.Proj = DirectX::XMMatrixPerspectiveFovLH(m_RProjZ, ((FLOAT)m_sceneNodeX) / ((FLOAT)m_sceneNodeY), zScaleVal * zNear, zScaleVal * zFar);

	//FrameShaderVars.Proj = DirectX::XMMatrixPerspectiveOffCenterLH(0.0f, m_sceneNodeX, m_sceneNodeY, 0.0f, zNear * zScaleVal, zFar * zScaleVal);//DirectX::XMMatrixPerspectiveFovLH(m_RProjZ, m_Aspect, zNear * zScaleVal, zFar * zScaleVal);
	// Metallicafan212:	Transpose???
	FLOAT Temp[4][4];
	Temp[0][0]	= FrameShaderVars.Proj.m[0][0];
	Temp[0][1]	= -FrameShaderVars.Proj.m[1][0];
	Temp[0][2]	= -FrameShaderVars.Proj.m[2][0];
	Temp[0][3]	= FrameShaderVars.Proj.m[3][0];

	Temp[1][0]	= FrameShaderVars.Proj.m[0][1];
	Temp[1][1]	= -FrameShaderVars.Proj.m[1][1];
	Temp[1][2]	= -FrameShaderVars.Proj.m[2][1];
	Temp[1][3]	= FrameShaderVars.Proj.m[3][1];

	Temp[2][0]	= FrameShaderVars.Proj.m[0][2];
	Temp[2][1]	= -FrameShaderVars.Proj.m[1][2];
	Temp[2][2]  = -FrameShaderVars.Proj.m[2][2];
	Temp[2][3]  = FrameShaderVars.Proj.m[3][2];

	Temp[3][0]  = FrameShaderVars.Proj.m[0][3];
	Temp[3][1]  = -FrameShaderVars.Proj.m[1][3];
	Temp[3][2]  = -FrameShaderVars.Proj.m[2][3];
	Temp[3][3]  = FrameShaderVars.Proj.m[3][3];

	// Metallicafan212:	Now back????
	appMemcpy(&FrameShaderVars.Proj.m[0][0], &Temp[0][0], sizeof(FLOAT[4][4]));
#else

	appMemzero(FrameShaderVars.Proj.m, sizeof(FLOAT[4][4]));

	// Metallicafan212:	I've fixed this to the correct order it should be
	FrameShaderVars.Proj.m[0][0] = 2.0f * zNear * invRightMinusLeft;
	FrameShaderVars.Proj.m[0][2] = -1.0f / ScaledSceneNodeX;
	FrameShaderVars.Proj.m[1][1] = -2.0f * zNear * invTopMinusBottom;
	FrameShaderVars.Proj.m[1][2] = 1.0f / ScaledSceneNodeY;
	FrameShaderVars.Proj.m[2][2] = -zScaleVal * (zFar * invNearMinusFar);
	FrameShaderVars.Proj.m[2][3] = zScaleVal * zScaleVal * (zNear * zFar * invNearMinusFar);
	FrameShaderVars.Proj.m[3][2] = 1.0f;
#endif

	// Metallicafan212:	Now update the bind
	UpdateGlobalShaderVars();
}

void UICBINDx11RenderDevice::PrecacheTexture(FTextureInfo& Info, PFLAG PolyFlags)
{
	guard(UICBINDx11RenderDevice::PrecacheTexture);

	// Metallicafan212:	Upload it now
	CacheTextureInfo(Info, PolyFlags);

	unguard;
}
