#pragma once
// Metallicafan212:	TODO!
#include "UnBuild.h"
#include "UnObjVer.h"

// Metallicafan212:	EXPLICIT HP2 new engine check
//					Modify this to add in more game macros
//					This is (currently) ONLY used to turn off specific code blocks, not to redefine the functions
//					TODO!!!! Block more code behind this!!!!!!
#define DX11_HP2 (!defined(DX11_UT_99) && !defined(DX11_UT_469) && !defined(DX11_UNREAL_227) && !defined(DX11_HP1) && !defined(DX11_RUNE) && !defined(DX11_DX))


#if UNREAL_TOURNAMENT_OLDUNREAL
# define DX11_UT_469 1
# undef DX11_HP2
// Unsupported PolyFlags

// Metallicafan212:	HACK! So we render tiles right, we need to clamp UVs so it doesn't cause looping when using AF
# define PF_ClampUVs PF_Memorized

# define PF_LumosAffected 0
# define PF_ForceZWrite 0
# define PF_ForceFog 0
# define PF_ColorMask 0
// Unsupported LineFlags
# define LINE_PreTransformed 0
# define LINE_DrawOver 0
// Unsupported ShowFlags
# define SHOW_Lines 0
// Differently named HackFlags
# define HF_Weapon HACKFLAGS_PostRender
// Misc
# define GExtraLineSize 1.0f

// Metallicafan212:	32bit check
#define UNREAL32 !BUILD_64

#endif

// Metallicafan212:	Define the Polyflags datatype...
//					I made polyflags a QWORD in HP2, all other UE1 games are DWORD
#if DX11_HP2
#define FPLAG QWORD
#else
#define FPLAG DWORD
#endif



// Metallicafan212:	Maybe?
//					TODO! Fix the projection code so that I can use intrinsics again!
#define _XM_NO_INTRINSICS_ 1

#define USE_COMPUTE_SHADER 0

#define DX11_USE_MSAA_SHADER 0

#define D3D_DRIVER_VERSION TEXT("0.35 Alpha")

// Metallicafan212:	Compile time
#define COMPILED_AT			*FString::Printf(TEXT("%s @ %s"), appFromAnsi(__DATE__), appFromAnsi(__TIME__))

#include <d3d11_1.h>
#include <dxgi1_2.h>
#include <dxgi1_5.h>
#include <d2d1.h>
#include <d2d1_1.h>
#include <dwrite.h>
#include <dwrite_1.h>
#include <dwrite_2.h>
//#include <dwrite_3.h>
#include <DirectXMath.h>
#include <DirectXColors.h>
#include <d3dcompiler.h>
#include <Windows.h>

#include <wrl/client.h>
#undef clock

#include "Core.h"
#include "Engine.h"
#include "UnRender.h"
#include "UnRenDev.h"


#define MAX_TEXTURES 16

enum ERasterFlags
{
	DXRS_Normal		= 0x00,
	DXRS_Wireframe	= 0x01,
	DXRS_NoAA		= 0x02,
	DXRS_MAX		= 0xFF,
};

class UD3D11RenderDevice;


// Metallicafan212:	Just cutting down on the needed typing
namespace MS = Microsoft::WRL;

// Metallicafan212:	A UTexture that holds onto our RT and DT
//					TODO! This needs a bit more work, but whatever
//					Using ComPtr since there was a memory leak in the DX9 version...
class UDX11RenderTargetTexture : public UTexture
{
	DECLARE_CLASS(UDX11RenderTargetTexture, UTexture, CLASS_Config | CLASS_Transient | CLASS_NoUserCreate, D3D11Drv);

	// Metallicafan212:	Pointer to our render device
	class UD3D11RenderDevice*				D3DDev;

	// Metallicafan212:	Vars to hold the targets
	MS::ComPtr<ID3D11Texture2D>				RTTex;
	MS::ComPtr<ID3D11Texture2D>				NonMSAATex;
	MS::ComPtr<ID3D11RenderTargetView>		RTView;
	MS::ComPtr<ID3D11ShaderResourceView>	RTSRView;

	// Metallicafan212:	Depth info
	MS::ComPtr<ID3D11Texture2D>				DTTex;
	MS::ComPtr<ID3D11DepthStencilView>		DTView;
	MS::ComPtr<ID3D11ShaderResourceView>	DTSRView;

	// Metallicafan212:	Direct2D RT
	MS::ComPtr<ID2D1RenderTarget>			RTD2D;
	MS::ComPtr<IDXGISurface>				RTDXGI;

	// Metallicafan212:	Variable to hold the texture object (for ->SetTexture)
	UDX11RenderTargetTexture()
		: UTexture(),
		RTTex(nullptr),
		RTView(nullptr),
		RTSRView(nullptr),
		DTTex(nullptr),
		DTView(nullptr),
		DTSRView(nullptr),
		NonMSAATex(nullptr),
		RTD2D(nullptr),
		RTDXGI(nullptr)
	{

	}

	void Destroy();

	void Lock(FTextureInfo& TextureInfo, FTime CurrentTime, INT LOD, URenderDevice* RenDev);
};

#define VBUFF_SIZE 200000
#define IBUFF_SIZE 200000

// Metallicafan212:	Base vertex definition
struct FD3DVert
{
	FLOAT	X;
	FLOAT	Y;
	FLOAT	Z;
	FLOAT	W;

	FLOAT	U;
	FLOAT	V;
	// Metallicafan212:	Extra vert info
	FLOAT	UX;
	FLOAT	VX;

	FPlane	Color;
	FPlane	Fog;
};

// Metallicafan212:	Cache stuff
typedef QWORD D3DCacheId;

struct FGlobalShaderVars
{
	UBOOL				bColorMasked;
	UBOOL				bDoDistanceFog;

	FLOAT				AlphaReject;
	FLOAT				BWPercent;

	UBOOL				bAlphaEnabled;
	UBOOL				bNVTileHack;

	// Metallicafan212:	Distance fog related stuff
	//					These are the values that get sent to the shader immediately
	FPlane				DistanceFogColor;
	FPlane				DistanceFogFinal;
	FPlane				DistanceFogSettings;

	// Metallicafan212:	Modulated fog
	FPlane				ModFogColor;

	// Metallicafan212:	Translucent fog
	FPlane				TransFogColor;

	// Metallicafan212:	Current fog settings
	FLOAT				CurrentFogStart;
	FLOAT				CurrentFogEnd;

	// Metallicafan212:	Target values
	FPlane				TargetFogColor;
	FPlane				LastFogColor;
	FPlane				TargetFogSettings;
	FPlane				LastFogSettings;

	// Metallicafan212:	Fog fade time
	FLOAT				FogFadeRate;

	// Metallicafan212:	If to fade the current fog values
	UBOOL				bFadeFogValues;

	FLOAT				FogSetTime;

	// Metallicafan212:	Constructor
	FGlobalShaderVars() :
		bColorMasked(0),
		bDoDistanceFog(0),
		AlphaReject(1e-6f),
		BWPercent(0.0f),
		DistanceFogColor(0.0f, 0.0f, 0.0f, 0.0f),
		DistanceFogSettings(1.0f / 32767.0f, 1.0f, 0.0f, 0.0f),
		DistanceFogFinal(0.0f, 0.0f, 0.0f, 0.0f),
		ModFogColor(0.5f, 0.5f, 0.5f, 1.0f),
		TransFogColor(0.0f, 0.0f, 0.0f, 1.0f),
		CurrentFogStart(0.0f),
		CurrentFogEnd(0.0f),
		TargetFogColor(0.0f, 0.0f, 0.0f, 0.0f),
		LastFogColor(0.0f, 0.0f, 0.0f, 0.0f),
		TargetFogSettings(0.0f, 0.0f, 0.0f, 0.0f),
		LastFogSettings(0.0f, 0.0f, 0.0f, 0.0f),
		FogFadeRate(0.0f),
		bFadeFogValues(0),
		FogSetTime(0.0f)

	{

	}
};

// Metallicafan212:	Thanks to CacoFFF from OldUnreal for pointing this out for me!
//					If you don't change the default TMap hashing, it'll place half the textures in one bin....
//					TODO! This may only be a problem when compiling for UT99/227. I changed out the GetTypeHash to a QWORD in HP2
#if !DX11_HP2
static inline DWORD GetTypeHash(const D3DCacheId& A)
{
	QWORD Value		= (QWORD)A;
	return (DWORD)Value ^ ((DWORD)(Value >> 16)) ^ ((DWORD)(Value >> 32));
}
#endif

// Metallicafan212:	Texture bind definition
struct FD3DTexture
{
	ID3D11Texture2D*			m_Tex;
	ID3D11ShaderResourceView*	m_View;

	// Metallicafan212:	Unreal texture pointer, only used for detecting RT textures...
	UTexture*		Tex;

	// Metallicafan212:	The DX11 format
	DXGI_FORMAT		TexFormat;

	// Metallicafan212:	UE Format of this texture
	ETextureFormat	Format;

	// Metallicafan212:	Flags this was uploaded as
	//					This only matters if Masked is toggled on or off
	FPLAG			PolyFlags;

	// Metallicafan212:	If this texture should have UVs clamped (UClamp == USize && VClamp == VSize)
	UBOOL			bShouldUVClamp;

	// Metallicafan212:	If this is really a RT texture
	UBOOL			bIsRT;

	// Metallicafan212:	The cache ID Unreal provided us when it was uploaded
	D3DCacheId		CacheID;

	// Metallicafan212:	Bind information
	INT				USize;
	INT				VSize;

	// Metallicafan212:	Scaling info
	FLOAT			UScale;
	FLOAT			VScale;

	INT				UClamp;
	INT				VClamp;

	FLOAT			UMult;
	FLOAT			VMult;


	// Metallicafan212:	Number of mips
	INT				NumMips;

	// Metallicafan212:	Color for the "color masking" system I added
	FPlane			MaskedColor;
	FPlane			MaskedGranularity;
};

struct FD3DBoundTex
{
	FD3DTexture*				TexInfo;

	UBOOL						bIsRT;

	// Metallicafan212:	Currently bound shader resource view
	ID3D11ShaderResourceView*	m_SRV;

	// Metallicafan212:	TODO! More bound vars
	FLOAT						UPan;
	FLOAT						VPan;
};

// Metallicafan212:	Texture support table info
struct FD3DTexType
{
	typedef void (*UploadFunc)(void* Source, SIZE_T SourceLength, SIZE_T SourcePitch, void* ConversionMem, FD3DTexture* tex, ID3D11DeviceContext* m_D3DDeviceContext, INT USize, INT VSize, INT Mip);
	typedef void (*ConversionFunc)(FColor* Palette, void* Source, SIZE_T SourceLength, SIZE_T SourcePitch, void* ConversionMem, FD3DTexture* tex, ID3D11DeviceContext* m_D3DDeviceContext, INT USize, INT VSize, INT Mip);
	typedef SIZE_T (FD3DTexType::* GetPitch)(INT USize);

	// Metallicafan212:	The UE format
	ETextureFormat	Format;

	// Metallicafan212:	The DirectX format we're going to use to upload this
	DXGI_FORMAT		DXFormat;

	UBOOL			bSupported;

	INT				BytesPerPixel;
	INT				BlockSize;

	// Metallicafan212:	TODO! Memory uploading function
	UploadFunc		TexUploadFunc;

	// Metallicafan212:	Conversion function, needed to convert an unsupported texture format to a supported one
	//					TODO! Implement conversions for BC1-7 if the hardware doesn't support it!!! (DX9 and DX10 level hardware)
	ConversionFunc	TexConvFunc;

	// Metallicafan212:	Pitch function
	GetPitch		GetTexturePitch;

	// Metallicafan212:	THIS IS ASSUMED RIGHT NOW!!!!
	SIZE_T RawPitch(INT USize)
	{
		return BytesPerPixel * USize;
	}

	SIZE_T BlockCompressionPitch(INT USize)
	{
		return max(1, ((USize + 3) / 4)) * BlockSize;
	}
};

// Metallicafan212:	TODO! Work on this more
void MemcpyTexUpload(void* Source, SIZE_T SourceLength, SIZE_T SourcePitch, void* ConversionMem, FD3DTexture* tex, ID3D11DeviceContext* m_D3DDeviceContext, INT USize, INT VSize, INT Mip);
void P8ToRGBA(FColor* Palette, void* Source, SIZE_T SourceLength, SIZE_T SourcePitch, void* ConversionMem, FD3DTexture* tex, ID3D11DeviceContext* m_D3DDeviceContext, INT USize, INT VSize, INT Mip);
void RGBA7To8(FColor* Palette, void* Source, SIZE_T SourceLength, SIZE_T SourcePitch, void* ConversionMem, FD3DTexture* tex, ID3D11DeviceContext* m_D3DDeviceContext, INT USize, INT VSize, INT Mip);

// Metallicafan212:	Base layout declaration
extern D3D11_INPUT_ELEMENT_DESC FBasicInLayout[4];

#define SAFE_RELEASE(ptr) if(ptr != nullptr){ptr->Release(); ptr = nullptr;}
#define SAFE_DELETE(ptr) if(ptr != nullptr){delete ptr; ptr = nullptr;}

// Metallicafan212:	Different shader definitions
#include "UnD3DShader.h"

// Metallicafan212:	Define an exported renderer
#if DX11_UT_469
class UD3D11RenderDevice : public URenderDeviceOldUnreal469
{
	DECLARE_CLASS(UD3D11RenderDevice, URenderDeviceOldUnreal469, CLASS_Config, D3D11Drv);
#else
class UD3D11RenderDevice : public URenderDevice
{
	DECLARE_CLASS(UD3D11RenderDevice, URenderDevice, CLASS_Config, D3D11Drv);
#endif

	// Metallicafan212:	User options
	INT							NumAASamples;
	INT							NumAFSamples;
	FLOAT						ThreeDeeLineThickness;
	FLOAT						OrthoLineThickness;
	UBOOL						bDebugSelection;
	UBOOL						bUseD3D11On12;

	// Metallicafan212:	TODO! MSAA resolving vars
	FLOAT						MSAAFilterSize;
	FLOAT						MSAAGaussianSigma;
	FLOAT						MSAACubicB;
	FLOAT 						MSAACubicC;
	INT							MSAAFilterType;

	// Metallicafan212:	Versions to check on lock if they changed
	INT							LastAASamples;
	INT							LastAFSamples;

	// Metallicafan212:	HACK coord minus for the current MSAA level...
	//					Certain levels need different coord movements
	FLOAT						TileAAUVMove;

	// Metallicafan212:	If the GPU is AMD/ATI, Intel, or NVidia
	UBOOL						bIsNV;
	UBOOL						bIsAMD;
	UBOOL						bIsIntel;


	// Metallicafan212:	TODO! Generalized D3D variables
	ID3D11Device*				m_D3DDevice;

	IDXGISwapChain1*			m_D3DSwapChain;
	ID3D11DeviceContext*		m_D3DDeviceContext;

	ID3D11Query*				m_D3DQuery;

	// Metallicafan212:	The current feature level, so we know if we support specific features
	D3D_FEATURE_LEVEL			m_FeatureLevel;

	// Metallicafan212:	For debugging only (will be removed later)
	ID3D11Debug*				m_D3DDebug;
	ID3D11InfoQueue*			m_D3DQueue;

	// Metallicafan212:	The backbuffer texture, so we can resolve resource to it (support MSAA)
	ID3D11Texture2D*			m_BackBuffTex;

	// Metallicafan212:	The render target view for the backbuffer (so we can use shaders on the input)
	ID3D11RenderTargetView*		m_BackBuffRT;

	// Metallicafan212:	The screen texture, which will use MSAA
	ID3D11Texture2D*			m_ScreenBuffTex;

	// Metallicafan212:	The screen render target
	ID3D11RenderTargetView*		m_D3DScreenRTV;

	// Metallicafan212:	The screen shader resource view (for MSAA resolving)
	ID3D11ShaderResourceView*	m_ScreenRTSRV;

	// Metallicafan212:	Format for creating depth targets
	DXGI_FORMAT					DSTFormat;

	// Metallicafan212:	Format for the texture
	DXGI_FORMAT					DSTTexFormat;

	// Metallicafan212:	Format for the depth shader resource view
	DXGI_FORMAT					DSTSRVFormat;

	// Metallicafan212:	The screen depth texture
	ID3D11Texture2D*			m_ScreenDSTex;

	// Metallicafan212:	The screen depth and stencil target
	ID3D11DepthStencilView*		m_D3DScreenDSV;

	// Metallicafan212:	The depth shader resource view (for MSAA resolving)
	ID3D11ShaderResourceView*	m_ScreenDTSRV;

	// Metallicafan212:	Default depth stencil state
	ID3D11DepthStencilState*	m_DefaultZState;

	// Metallicafan212:	Default state with no z writing
	ID3D11DepthStencilState*	m_DefaultNoZState;

	// Metallicafan212:	Raster states
	TMap<DWORD, ID3D11RasterizerState*> RasterMap;

	DWORD						CurrentRasterState;

	// Metallicafan212:	So we can easily request wireframe
	DWORD						ExtraRasterFlags;

#if DX11_HP2
	// Metallicafan212:	DXGI surface for D2D
	IDXGISurface*				m_DXGISurf;

	// Metallicafan212:	Direct2D shit
	ID2D1RenderTarget*			m_D2DRT;

	// Metallicafan212:	Current D2D render target (RT textures will change this!!!!)
	ID2D1RenderTarget*			m_CurrentD2DRT;

	// Metallicafan212:	The factory for this device
	ID2D1Factory1*				m_D2DFact;

	IDWriteFactory1*			m_D2DWriteFact;

	// Metallicafan212:	Text rendering parameters, if we're using AA
	IDWriteRenderingParams*		m_TextParams;

	// Metallicafan212:	Holder for the different font types
	TMap<FString, IDWriteTextFormat*>	FontMap;
#endif

	// Metallicafan212:	Array of RT textures
	TArray<UDX11RenderTargetTexture*>	RTTextures;

	// Metallicafan212:	RTTexture currently bound
	UDX11RenderTargetTexture*			BoundRT;

	// Metallicafan212:	Shader list, so we can cleanup
	//					TODO! Might make each base shader referenced directly rather than an array???
	TArray<FD3DShader*>					Shaders;

	FD3DTileShader*						FTileShader;
	FD3DGenericShader*					FGenShader;

#if USE_COMPUTE_SHADER
	FD3DLghtMshCompShader*				FMshLghtCompShader;
#endif

	FD3DMeshShader*						FMeshShader;

	FD3DSurfShader*						FSurfShader;

	FD3DLineShader*						FLineShader;

	FD3DMSAAShader*						FMSAAShader;

	FD3DShader*							CurrentShader;

	FGlobalShaderVars					GlobalShaderVars;

	// Metallicafan212:	Blending map
	//					We have to keep around blend objects (rather than setting render states directly) so they're mapped to the polyflag that it represents
	TMap<FPLAG, ID3D11BlendState*>		BlendMap;

	// Metallicafan212:	Sampler map
	//					This is required since you can't just on the fly update sampler objects....
	//					Sucks, but it is what it is
	TMap<FPLAG, ID3D11SamplerState*>	SampMap;

	FPLAG								CurrentPolyFlags;

	// Metallicafan212:	If we're using a Freesync/GSync mode
	UBOOL								bAllowTearing;

	// Metallicafan212:	Unreal related variables
	UBOOL								bFullscreen;
	UBOOL								bLastFullscreen;
	INT									SizeX;
	INT									SizeY;

	// Metallicafan212:	Valid display modes
	TArray<FPlane>						Modes;

	// Metallicafan212:	Mouse clicking support
	BYTE*								m_HitData;
	INT*								m_HitSize;

	INT									m_HitCount;
	INT									m_HitBufSize;

	// Metallicafan212:	Hit array, for pixel selection
	class FPixHitInfo
	{
	public:
		TArray<BYTE> HitData;

		//FPixHitInfo* Prev;
		INT Prev;

		INT Index;

		// Metallicafan212:	Priority
		INT Priority;

		FPixHitInfo()
		{
			Priority = 1;
			Index = 0;
			Prev = -1;
		}
	};

	TArray<FPixHitInfo> PixelHitInfo;

	// Metallicafan212:	Keep track of the "top" index
	INT					PixelTopIndex;

	FPlane				CurrentHitColor;

	// Metallicafan212:	Dynamic memory, equal to sizeof(PixelFormat) * USize * VSize;
	//					For most, it's just going to be = 4bpp (to keep it simple)
	BYTE*								ConversionMemory;

	// Metallicafan212:	Texturing support
	//					TODO! Query for this limitation and put the number in a ifdef!
	FD3DBoundTex						BoundTextures[MAX_TEXTURES];

	ID3D11Texture2D*					BlankTexture;
	ID3D11ShaderResourceView*			BlankResourceView;
	ID3D11SamplerState*					BlankSampler;

	TMap<D3DCacheId, FD3DTexture>		TextureMap;

	TMap<ETextureFormat, FD3DTexType>	SupportedTextures;

	// Metallicafan212:	Copied variables from the DX9 renderer
	UBOOL								m_nearZRangeHackProjectionActive;

	// Lock variables.
	FPlane FlashScale, FlashFog;
	FLOAT m_RProjZ, m_Aspect;
	FLOAT m_RFX2, m_RFY2;
	INT m_sceneNodeX, m_sceneNodeY;

	// Metallicafan212:	TODO! Global complex surface info
	FCoords								SurfCoords;

	// Metallicafan212:	Projection matrix
	DirectX::XMMATRIX					Proj;

	// Metallicafan212:	Vertex buffer stuff
	//					Per M$:
	/*
	
	A common use of these two flags involves filling dynamic index/vertex buffers with geometry that can be seen from the camera's current position. 
	The first time that data is entered into the buffer on a given frame, Map is called with D3D11_MAP_WRITE_DISCARD; doing so invalidates the previous contents of the buffer. 
	The buffer is then filled with all available data.

	Subsequent writes to the buffer within the same frame should use D3D11_MAP_WRITE_NO_OVERWRITE. 
	This will enable the CPU to access a resource that is potentially being used by the GPU as long as the restrictions described previously are respected.

	*/

	// Metallicafan212:	Buffering type
	enum EBuffType
	{
		BT_None,
		BT_Triangles,
		BT_Tiles,
		BT_Lines,
		BT_Points,
		BT_BSP,
		BT_ScreenFlash
	};

	// Metallicafan212:	We'll only call do drawing when we need to
	EBuffType					m_CurrentBuff;

	UBOOL						bIndexedBuffered;

	ID3D11Buffer*				VertexBuffer;

	// Metallicafan212:	Position (in bytes!!!!)
	SIZE_T						m_VertexBuffPos;

	// Metallicafan212:	Vertex offset in the stream (since each shader could have a different vert definition)
	SIZE_T						m_DrawnVerts;

	// Metallicafan212:	Size of the buffer (in bytes)
	//					TODO! If we get past this, what then? How do we flush?
	SIZE_T						m_VertexBuffSize;

	// Metallicafan212:	Pointer to the vertex data
	FD3DVert*					m_VertexBuff;

	// Metallicafan212:	How many verts were buffered (how many to render when calling a draw command)
	//					Not currently used
	SIZE_T						m_BufferedVerts;

	// Metallicafan212:	Index buffer
	ID3D11Buffer*				IndexBuffer;

	// Metallicafan212:	Position (in bytes!!!!)
	SIZE_T						m_IndexBuffPos;

	// Metallicafan212:	Index offset in the index stream
	SIZE_T						m_DrawnIndices;

	// Metallicafan212:	Size of the buffer (in bytes)
	//					TODO! If we get past this, what then? How do we flush?
	SIZE_T						m_IndexBuffSize;

	// Metallicafan212:	Pointer to the index data
	_WORD*						m_IndexBuff;

	SIZE_T						m_BufferedIndices;

	// Metallicafan212:	Line buffer?
	ID3D11Buffer*				LineBuffer;

	// Metallicafan212:	Helper function for errors from the M$ sample
	inline void ThrowIfFailed(HRESULT hr)
	{
		if (FAILED(hr))
		{
			// Set a breakpoint on this line to catch DirectX API errors
			//throw std::exception();
			appThrowf(TEXT("DX11 encountered an error (%lu)"), hr);
		}
	}

	inline void LockVertexBuffer(INT BytesToReserve, UBOOL bNoOverwrite = 1)
	{
		// Metallicafan212:	Make sure we have enough room
		if ((BytesToReserve + m_VertexBuffPos) >= m_VertexBuffSize)
			bNoOverwrite = 0;

		if (!bNoOverwrite)
		{
			// Metallicafan212:	Ran out of room, check if there's stuff to render
			if (m_BufferedIndices != 0 || m_BufferedVerts != 0)
			{
				EndBuffering();
			}

			m_VertexBuffPos		= 0;
			m_DrawnVerts		= 0;
		}

		D3D11_MAPPED_SUBRESOURCE	VertexBuffMap = { nullptr, 0, 0 };

		HRESULT hr = m_D3DDeviceContext->Map(VertexBuffer, 0, bNoOverwrite ? D3D11_MAP_WRITE_NO_OVERWRITE : D3D11_MAP_WRITE_DISCARD, 0, &VertexBuffMap);

		m_VertexBuff = (FD3DVert*)((BYTE*)VertexBuffMap.pData + m_VertexBuffPos);

		ThrowIfFailed(hr);
	}

	inline void UnlockVertexBuffer()
	{
		if(m_VertexBuff != nullptr)
			m_D3DDeviceContext->Unmap(VertexBuffer, 0);

		m_VertexBuff = nullptr;
	}

	inline void AdvanceVertPos(INT VertCount, SIZE_T VertexSize = sizeof(FD3DVert), INT IndexCount = 0)
	{
		//m_DrawnVerts	+= VertCount;
		m_BufferedVerts += VertCount;
		m_VertexBuffPos += VertCount * VertexSize;

		if (IndexCount != 0)
		{
			m_IndexBuffPos += sizeof(_WORD) * IndexCount;
			//m_DrawnIndices += IndexCount;
			m_BufferedIndices += IndexCount;

			bIndexedBuffered = 1;
		}
	}

	inline void LockIndexBuffer(INT IndicesToReserver, UBOOL bNoOverwrite = 1)
	{
		// Metallicafan212:	TODO! Check if we have enough room
		if ((IndicesToReserver * sizeof(_WORD)) + m_IndexBuffPos >= m_IndexBuffSize)
			bNoOverwrite = 0;

		if (!bNoOverwrite)
		{
			// Metallicafan212:	Ran out of room, check if there's stuff to render
			//					This is REALLY shit, but whatever
			if (m_BufferedIndices != 0 || m_BufferedVerts != 0)
			{
				EndBuffering();
			}

			m_IndexBuffPos = 0;
			m_DrawnIndices = 0;
		}

		D3D11_MAPPED_SUBRESOURCE	IndexBuffMap = { nullptr, 0, 0 };

		HRESULT hr = m_D3DDeviceContext->Map(IndexBuffer, 0, bNoOverwrite ? D3D11_MAP_WRITE_NO_OVERWRITE : D3D11_MAP_WRITE_DISCARD, 0, &IndexBuffMap);

		m_IndexBuff = (_WORD*)((BYTE*)IndexBuffMap.pData + m_IndexBuffPos);

		ThrowIfFailed(hr);

		bIndexedBuffered = 1;
	}

	inline void UnlockIndexBuffer()
	{
		if(m_IndexBuff != nullptr)
			m_D3DDeviceContext->Unmap(IndexBuffer, 0);

		m_IndexBuff = nullptr;
	}

	inline void EndBuffering()
	{
		UBOOL bLockVert = 0;
		UBOOL bLockInd = 0;

		if (m_CurrentBuff != BT_None && (m_BufferedIndices != 0 || m_BufferedVerts != 0))
		{
			// Metallicafan212:	Check if the different buffers are locked, if so, unlock and lock at the end
			if (m_VertexBuff != nullptr)
			{
				bLockVert = 1;
				UnlockVertexBuffer();
			}

			if (m_IndexBuff != nullptr)
			{
				UnlockIndexBuffer();
				bLockInd = 1;
			}

			if (bIndexedBuffered)
			{
				m_D3DDeviceContext->DrawIndexed(m_BufferedIndices, m_DrawnIndices, m_DrawnVerts);
				//bIndexedBuffered = 0;
			}
			else
			{
				m_D3DDeviceContext->Draw(m_BufferedVerts, m_DrawnVerts);
			}

			// Metallicafan212:	Increment the buffer counters
			m_DrawnIndices	+= m_BufferedIndices;
			m_DrawnVerts	+= m_BufferedVerts;
		}

		// Metallicafan212:	Init the values
		//m_CurrentBuff		= BT_None;
		m_BufferedIndices	= 0;
		m_BufferedVerts		= 0;
		bIndexedBuffered	= 0;

		// Metallicafan212:	Relock it if it was locked
		//					We already checked if it had room or not
		if (bLockVert)
			LockVertexBuffer(0);

		if (bLockInd)
			LockIndexBuffer(0);
		//else
		//	bIndexedBuffered = 0;
	}

	// Metallicafan212:	Start and end buffering (for different types)
	inline void StartBuffering(EBuffType inBuff, UBOOL bNeedsIndices = 0)
	{
		// Metallicafan212:	Check if we need to draw
		//					TEST! Hold onto BSP buffers for longer, since we always start at the buffered index location
		if (inBuff != m_CurrentBuff || (inBuff != BT_BSP && (bIndexedBuffered || (bNeedsIndices && !bIndexedBuffered))) )
		{
			EndBuffering();
		}

		m_CurrentBuff = inBuff;
	}

	// Metallicafan212:	From the DX9 driver since I'm a fucking lazy bastard
	static QSORT_RETURN CDECL CompareRes(const FPlane* A, const FPlane* B)
	{
		return (QSORT_RETURN)(((A->X - B->X) != 0.0f) ? (A->X - B->X) : (A->Y - B->Y));
	}

	void RegisterTextureFormat(ETextureFormat Format, DXGI_FORMAT DXFormat, UBOOL bRequiresConversion, INT ByteOrBlockSize = 4, FD3DTexType::GetPitch PitchFunc = &FD3DTexType::RawPitch, FD3DTexType::UploadFunc UFunc = MemcpyTexUpload, FD3DTexType::ConversionFunc UConv = nullptr);

	// Metallicafan212:	Texture setting code
	void SetTexture(INT TexNum, FTextureInfo* Info, FPLAG PolyFlags);

	void CacheTextureInfo(FTextureInfo& Info, FPLAG PolyFlags, UBOOL bJustSampler = 0);

	void MakeTextureSampler(FD3DTexture* Bind, FPLAG PolyFlags);

	inline void FlushTextureSamplers()
	{
		for (TMap<FPLAG, ID3D11SamplerState*>::TIterator It(SampMap); It; ++It)
		{
			It.Value()->Release();
		}
		SampMap.Empty();
	}

	inline void FlushRasterStates()
	{
		for (TMap<DWORD, ID3D11RasterizerState*>::TIterator It(RasterMap); It; ++It)
		{
			It.Value()->Release();
		}

		RasterMap.Empty();
	}

	void SetRasterState(DWORD State);

	inline ID3D11SamplerState* GetSamplerState(FPLAG PolyFlags)
	{
		guard(UD3D11RenderDevice::GetSamplerState);

		// Metallicafan212:	We have to use global sampler, since per-texture won't ever work... It'll force all of the same texture to no smooth (for example)
		ID3D11SamplerState* S = SampMap.FindRef(PolyFlags & (PF_NoSmooth | PF_ClampUVs));

		if (S == nullptr)
		{
			// Metallicafan212:	Create it
			CD3D11_SAMPLER_DESC SDesc = CD3D11_SAMPLER_DESC(D3D11_DEFAULT);
			
			// Metallicafan212:	TODO! Make more of these user definable options!
			//					D3D11_FILTER_MIN_MAG_MIP_POINT
			//					D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR
			//
			SDesc.Filter			= (PolyFlags & PF_NoSmooth ? D3D11_FILTER_MIN_MAG_MIP_POINT : D3D11_FILTER_ANISOTROPIC);
			SDesc.AddressU			= (PolyFlags & PF_ClampUVs ? D3D11_TEXTURE_ADDRESS_CLAMP : D3D11_TEXTURE_ADDRESS_WRAP);
			SDesc.AddressV			= SDesc.AddressU;
			SDesc.AddressW			= D3D11_TEXTURE_ADDRESS_WRAP;//SDesc.AddressU;
			SDesc.MinLOD			= -D3D11_FLOAT32_MAX;
			SDesc.MaxLOD			= D3D11_FLOAT32_MAX;
			SDesc.MipLODBias		= 0.0f;
			SDesc.MaxAnisotropy		= PolyFlags & PF_NoSmooth ? 1 : NumAFSamples;//16;//16;
			SDesc.ComparisonFunc	= D3D11_COMPARISON_NEVER;

			HRESULT hr = m_D3DDevice->CreateSamplerState(&SDesc, &S);

			ThrowIfFailed(hr);

			// Metallicafan212:	Now set
			SampMap.Set((PolyFlags & (PF_NoSmooth | PF_ClampUVs)), S);
		}

		return S;

		unguard;
	}

	inline void FindAndSetBlend(FPLAG PolyFlag, D3D11_BLEND SrcBlend, D3D11_BLEND DstBlend,
		UINT8 RTWrite = D3D11_COLOR_WRITE_ENABLE_ALL, BOOL bEnableBlending = 1, BOOL bAlphaToCov = 0, D3D11_BLEND_OP BldOp = D3D11_BLEND_OP_ADD, D3D11_BLEND_OP BldOpAlh = D3D11_BLEND_OP_ADD,
		D3D11_BLEND SrcBlendAlpha = D3D11_BLEND_ONE, D3D11_BLEND DstBlendAlpha = D3D11_BLEND_ZERO)
	{
		ID3D11BlendState* bState = BlendMap.FindRef(PolyFlag);

		if (bState == nullptr)
		{
			// Metallicafan212:	Create it
			D3D11_BLEND_DESC Desc;

			appMemzero(&Desc, sizeof(Desc));

			Desc.RenderTarget[0].BlendOp				= BldOp;
			Desc.RenderTarget[0].BlendOpAlpha			= BldOpAlh;
			Desc.RenderTarget[0].SrcBlendAlpha			= SrcBlendAlpha;
			Desc.RenderTarget[0].DestBlendAlpha			= DstBlendAlpha;
			Desc.RenderTarget[0].BlendEnable			= bEnableBlending;
			Desc.RenderTarget[0].SrcBlend				= SrcBlend;
			Desc.RenderTarget[0].DestBlend				= DstBlend;
			Desc.RenderTarget[0].RenderTargetWriteMask	= D3D11_COLOR_WRITE_ENABLE_ALL;

			// Metallicafan212:	Also add in alpha to coverage support
			Desc.AlphaToCoverageEnable					= bAlphaToCov;

			// Metallicafan212:	Now make it
			HRESULT hr = m_D3DDevice->CreateBlendState(&Desc, &bState);

			ThrowIfFailed(hr);

			// Metallicafan212:	Now set it in the map
			BlendMap.Set(PolyFlag, bState);
		}

		// Metallicafan212:	Set the blend state
		m_D3DDeviceContext->OMSetBlendState(bState, nullptr, 0xffffffff);
	}

	// Metallicafan212:	Blend state
	void SetBlend(FPLAG PolyFlags);

	// Metallicafan212:	For detecting the hit after the scene is rendered
	void DetectPixelHit();

	// Metallicafan212:	For setting up the shit for pixel hitting
	void SetupPixelHitTest();

	// Metallicafan212:	Generate the semi-permanent resources needed for the device
	void SetupResources();

	// Metallicafan212:	Setup the device
	void SetupDevice();

	// Metallicafan212:	Set the projection matrix
	void SetProjectionStateNoCheck(UBOOL bRequestingNearRangeHack, UBOOL bForceUpdate = 0);

	// Metallicafan212:	Add on config properties
	void AddBoolProp(const TCHAR* Name, UBOOL& InParam, ECppProperty CPP, INT Offset, UBOOL bDefaultVal);
	void AddFloatProp(const TCHAR* Name, FLOAT& InParam, ECppProperty CPP, INT Offset, FLOAT fDefaultVal);
	void AddIntProp(const TCHAR* Name, INT& InParam, ECppProperty CPP, INT Offset, INT iDefaultVal);

	void ClampUserOptions();

	// Constructors.
	void StaticConstructor();

	// URenderDevice low-level functions that drivers must implement.
	virtual UBOOL Init(UViewport* InViewport, INT NewX, INT NewY, INT NewColorBytes, UBOOL Fullscreen);

	virtual UBOOL SetRes(INT NewX, INT NewY, INT NewColorBytes, UBOOL Fullscreen);

	virtual void Exit();

	virtual void Flush(UBOOL AllowPrecache);

	virtual UBOOL Exec(const TCHAR* Cmd, FOutputDevice& Ar);

	virtual void Lock(FPlane FlashScale, FPlane FlashFog, FPlane ScreenClear, DWORD RenderLockFlags, BYTE* HitData, INT* HitSize);

	virtual void Unlock(UBOOL Blit);

	// Metallicafan212:	TODO! More particle related code
	virtual INT MaxVertices() { return 256; };

	virtual void DrawTriangles(FSceneNode* Frame, FTextureInfo& Info, FTransTexture** Pts, INT NumPts, _WORD* Indices, INT NumIndices, QWORD PolyFlags, FSpanBuffer* Span);

#if DX11_HP2
	virtual void DrawComplexSurface(FSceneNode * Frame, FSurfaceInfo & Surface, FSurfaceFacet & Facet, QWORD PolyFlags, BYTE cAlpha);
	
	virtual void DrawTile(FSceneNode * Frame, FTextureInfo & Info, FLOAT X, FLOAT Y, FLOAT XL, FLOAT YL, FLOAT U, FLOAT V, FLOAT UL, FLOAT VL, class FSpanBuffer* Span, FLOAT Z, FPlane Color, FPlane Fog, QWORD PolyFlags);

	virtual void DrawRotatedTile(FSceneNode* Frame, FTextureInfo& Info, FLOAT X, FLOAT Y, FLOAT XL, FLOAT YL, FLOAT U, FLOAT V, FLOAT UL, FLOAT VL, class FSpanBuffer* Span, FLOAT Z, FPlane Color, FPlane Fog, QWORD PolyFlags, FCoords InCoords = GMath.UnitCoords);

	virtual int DrawString(QWORD Flags, UFont * Font, INT & DrawX, INT & DrawY, const TCHAR * Text, const FPlane & Color, FLOAT Scale = 1.0f, FLOAT SpriteScaleX = 1.0f, FLOAT SpriteScaleY = 1.0f);
#elif DX11_UT_469

	virtual void DrawTile(FSceneNode * Frame, FTextureInfo & Info, FLOAT X, FLOAT Y, FLOAT XL, FLOAT YL, FLOAT U, FLOAT V, FLOAT UL, FLOAT VL, FSpanBuffer * Span, FLOAT Z, FPlane Color, FPlane Fog, DWORD PolyFlags);
	
	virtual void DrawComplexSurface(FSceneNode * Frame, FSurfaceInfo & Surface, FSurfaceFacet & Facet);
	
	virtual void DrawGouraudPolygon(FSceneNode * Frame, FTextureInfo & Info, FTransTexture * *Pts, int NumPts, DWORD PolyFlags, FSpanBuffer * Span);
	
	virtual UBOOL SupportsTextureFormat(ETextureFormat Format);
	
	virtual void DrawGouraudTriangles(const FSceneNode * Frame, const FTextureInfo & Info, FTransTexture* const Pts, INT NumPts, DWORD PolyFlags, DWORD DataFlags, FSpanBuffer * Span);
#endif

	virtual void Draw3DLine(FSceneNode* Frame, FPlane Color, DWORD LineFlags, FVector OrigP, FVector OrigQ);

	virtual void Draw2DLine(FSceneNode* Frame, FPlane Color, DWORD LineFlags, FVector P1, FVector P2);

	virtual void Draw2DPoint(FSceneNode* Frame, FPlane Color, DWORD LineFlags, FLOAT X1, FLOAT Y1, FLOAT X2, FLOAT Y2, FLOAT Z);

	virtual void ClearZ(FSceneNode* Frame);

	virtual void PushHit(const BYTE* Data, INT Count);

	virtual void PopHit(INT Count, UBOOL bForce);

	virtual void GetStats(TCHAR* Result);

	virtual void ReadPixels(FColor* Pixels);

	virtual void EndFlash();

	virtual void DrawStats(FSceneNode* Frame);

	virtual void SetSceneNode(FSceneNode* Frame);

	virtual void PrecacheTexture(FTextureInfo& Info, FPLAG PolyFlags);

	// Metallicafan212:	Viewer-based zone fog
	virtual void SetDistanceFog(UBOOL Enable, FLOAT FogStart, FLOAT FogEnd, FPlane Color, FLOAT FadeRate);

	// Metallicafan212:	Tick the current fog values
	//					We use this to fade the values from one setting to another
	void TickDistanceFog();

	// Metallicafan212:	Force the current fog color to a specific value
	void ForceSetFogColor(FPlane FogColor);

	// Metallicafan212:	Simple BW shader for doing stupid "old-time" looking shit
	virtual void SetBWPercent(FLOAT Percent);

	// Metallicafan212:	New RT stuff
	//					Create an RT texture, this is so the texture can be used in normal stuff
	virtual UTexture* CreateRenderTargetTexture(INT W, INT H, UBOOL bCreateDepth = 1);

	virtual void SetRenderTargetTexture(UTexture* Tex);

	virtual void RestoreRenderTarget();

	virtual struct FTransTexture* InitMeshComputeShader(INT VertCount);

	virtual void ExecuteMeshLightShader(FSceneNode* Frame, INT VertCount, void* MeshLights, void* LastLight, FLOAT Specular, FLOAT SpecMinCos, FLOAT Diffuse, FVector AmbientVector, AActor* Actor, QWORD PolyFlags);

	// Metallicafan212:	This function just unmapps the verts
	virtual void FinishMeshComputeShader();

	// Metallicafan212:	Clear out any render targets left
	void ClearRTTextures()
	{
		// Metallicafan212:	Clear all RT textures
		//					Copy them to an array for temp, so we don't skip
		TArray<UDX11RenderTargetTexture*> Temp = RTTextures;

		for (INT i = 0; i < Temp.Num(); i++)
		{
			UDX11RenderTargetTexture* RT = Temp(i);

			if (RT != nullptr)
			{
				RT->Destroy();
			}
		}

		Temp.Empty();
		RTTextures.Empty();
	}
};