#pragma once
// Metallicafan212:	TODO!
#include "UnBuild.h"
#include "UnObjVer.h"
#include <unordered_map>

// Metallicafan212:	If to use UpdateSubresource instead of Map for shader constants
#define UPDATESUBRESOURCE_CONSTANTS 1

// Metallicafan212:	TODO! Eventually, I should swap back to TMap, but it's really slow when we're doing a lot of lookups
#define USE_UNODERED_MAP_EVERYWHERE 1

// Metallicafan212:	If instead, we should use a sampler array (for speed reasons!)
#define USE_SAMPLER_ARRAY 1

// Metallicafan212:	If for raster descriptions, we use an array
#define USE_RASTER_ARRAY 1

#define MAX_MIP_BIAS 2

#define MAX_TEXTURES 16

// Metallicafan212: 4 flag combos (off, clamp, no smooth, clamp + no smooth) * 3 mip bias (none, 1, 2) * 2 AF flag (off, on)
//					TODO! Try and see if we can make this a bit more automatic....
#define MAX_SAMPLERS (4 * (MAX_MIP_BIAS + 1) * 2)

// Metallicafan212:	If complex surface stuff is added to each vert
#define EXTRA_VERT_INFO 1

// Metallicafan212:	If we pack the UVs of each texture into each vert instead (when EXTRA_VERT_INFO is on)
#define COMPLEX_SURF_MANUAL_UVs 1

// Metallicafan212:	EXPLICIT HP2 new engine check
//					Modify this to add in more game macros
//					This is (currently) ONLY used to turn off specific code blocks, not to redefine the functions
//					TODO!!!! Block more code behind this!!!!!!
#define DX11_HP2 (!defined(DX11_UT_99) && !defined(DX11_UT_469) && !defined(DX11_UNREAL_227) && !defined(DX11_HP1) && !defined(DX11_RUNE) && !defined(DX11_DX))

// Metallicafan212:	Separate define for Direct2D definitions
#define DX11_D2D DX11_HP2

#define DX11_D2D_CLIP_LAYER 1

// Metallicafan212:	TODO!!!! This is EXTREMELY bugged
#define DX11_D2D_LAYOUT_CACHING 0

#define RES_SCALE_IN_PROJ 0

#define P8_COMPUTE_SHADER 0 

#if UNREAL_TOURNAMENT_OLDUNREAL
# define DX11_UT_469 1
# undef DX11_HP2

// Metallicafan212:	HACK! So we render tiles right, we need to clamp UVs so it doesn't cause looping when using AF
//					Reuse the big wavy flag, as it's unused and (unlikely) to be used....
# define PF_ClampUVs PF_BigWavy//PF_Memorized

// Metallicafan212:	32bit check
#define UNREAL32 !BUILD_64
#define UNREAL64 BUILD_64

// Metallicafan212:	DISABLE THE WARNING IF WE'RE IN 32BIT!!!!
#if UNREAL32
#define WINDOWS_IGNORE_PACKING_MISMATCH 1
#endif

// Metallicafan212:	Hard-code shaders for comp play
//					TODO! Should probably only be enabled for build built with the official patch and released on Github
#define DX11_HARDCODE_SHADERS 1

#endif

#if ENGINE_VERSION==227
# define DX11_UNREAL_227 1
# undef DX11_HP2

// Metallicafan212:	HACK! So we render tiles right, we need to clamp UVs so it doesn't cause looping when using AF
//					Reuse the big wavy flag, as it's unused and (unlikely) to be used....
# define PF_ClampUVs PF_BigWavy//PF_Memorized

// Metallicafan212:	32bit check
#define UNREAL32 !BUILD_64
#define UNREAL64 BUILD_64

// Metallicafan212:	DISABLE THE WARNING IF WE'RE IN 32BIT!!!!
#if UNREAL32
#define WINDOWS_IGNORE_PACKING_MISMATCH 1
#endif

// Metallicafan212:	Hard-code shaders for comp play
//					TODO! Should probably only be enabled for build built with the official patch and released on Github

// Metallicafan212:	For now, leave the shaders un-hardcoded so that we can work with distance fog, etc.
//#define DX11_HARDCODE_SHADERS 1

#endif

// Metallicafan212:	Needed for non-227 targets
#if !DX11_UNREAL_227
enum ERenderZTest
{
	ZTEST_Less,
	ZTEST_Equal,
	ZTEST_LessEqual,
	ZTEST_Greater,
	ZTEST_GreaterEqual,
	ZTEST_NotEqual,
	ZTEST_Always,
	ZTEST_MAX
};
#endif

// Metallicafan212:	Flags for the depth stencil 
enum EDepthStencilFlags
{
	DS_None		= 0x0,
	DS_NoZWrite	= 0x1,
	DS_NoDepth	= 0x2,
};

// Metallicafan212:	TODO! Generic game support
#if !DX11_HP2 && !DX11_UT_469 && !DX11_UNREAL_227
// Metallicafan212:	HACK! So we render tiles right, we need to clamp UVs so it doesn't cause looping when using AF
//					Reuse the big wavy flag, as it's unused and (unlikely) to be used....
# define PF_ClampUVs PF_BigWavy//PF_Memorized

// Metallicafan212:	32bit check
#define UNREAL32 1

// Metallicafan212:	DISABLE THE WARNING IF WE'RE IN 32BIT!!!!
#if UNREAL32
#define WINDOWS_IGNORE_PACKING_MISMATCH 1
#endif

// Metallicafan212:	Hard-code shaders for comp play
//					TODO! Should probably only be enabled for build built with the official patch and released on Github
#define DX11_HARDCODE_SHADERS 1

// Metallicafan212:	Other UE1's don't have alpha blend, define it now
#define PF_AlphaBlend PF_CloudWavy

#endif

// Metallicafan212:	Distance fog settings
#define DX11_DISTANCE_FOG DX11_HP2 || DX11_UNREAL_227

#define INT_INDEX_BUFF 1

#if INT_INDEX_BUFF
typedef unsigned int INDEX;
#define INDEX_FORMAT DXGI_FORMAT_R32_UINT
#else
typedef unsigned short INDEX;
#define INDEX_FORMAT DXGI_FORMAT_R16_UINT
#endif

// Metallicafan212:	Define the Polyflags datatype...
//					I made polyflags a QWORD in HP2, all other UE1 games are DWORD
#if DX11_HP2
#define PFLAG QWORD
#else
#define PFLAG DWORD
#endif


#if DX11_HP2
#define ADJUST_PFLAGS(PolyFlags) \
	/* Metallicafan212:	Cut it down to only specific flags */ \
	if (!(PolyFlags & (PF_Translucent | PF_Modulated | PF_Highlighted | PF_LumosAffected))) \
	{ \
		PolyFlags |= PF_Occlude; \
	} \
	else if (PolyFlags & PF_Translucent) \
	{ \
		PolyFlags &= ~(PF_Masked | PF_ColorMask); \
	} \
	/* Metallicafan212:	Pixel based selection requires that we have alpha blending and nothing else */ \
	if (GIsEditor && m_HitData != nullptr) \
	{ \
		PolyFlags = (PolyFlags & ~(PF_Translucent | PF_Modulated | PF_Highlighted | PF_LumosAffected)); \
	}
#else
#define ADJUST_PFLAGS(PolyFlags) \
	/* Metallicafan212:	Cut it down to only specific flags */ \
	if (!(PolyFlags & (PF_Translucent | PF_Modulated | PF_Highlighted ))) \
	{ \
		PolyFlags |= PF_Occlude; \
	} \
	else if (PolyFlags & PF_Translucent) \
	{ \
		PolyFlags &= ~(PF_Masked); \
	} \
	/* Metallicafan212:	Pixel based selection requires that we have alpha blending and nothing else */ \
	if (GIsEditor && m_HitData != nullptr) \
	{ \
		PolyFlags = (PolyFlags & ~(PF_Translucent | PF_Modulated | PF_Highlighted )); \
	}
#endif


// Metallicafan212:	Maybe?
//					TODO! Fix the projection code so that I can use intrinsics again!
#define _XM_NO_INTRINSICS_ 1

#define USE_COMPUTE_SHADER 0

#define DX11_USE_MSAA_SHADER 1

#define D3D_DRIVER_VERSION TEXT("0.94 Alfalfa-Alpha")

// Metallicafan212:	Compile time
#define COMPILED_AT			*FString::Printf(TEXT("%s @ %s"), appFromAnsi(__DATE__), appFromAnsi(__TIME__))

#include <d3d11_1.h>
#include <dxgi1_2.h>
#include <dxgi1_5.h>

// Metallicafan212:	Windows 10+ specific!!!!!
#include <dxgi1_6.h>

#include <d2d1.h>
#include <d2d1_1.h>
#if DX11_D2D
#include <dwrite.h>
#include <dwrite_1.h>
#include <dwrite_2.h>
#endif
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

#if !DX11_HP2
#include "WindowsVersions.h"
#endif

// Metallicafan212:	Compile-time check for engine revision
constexpr int c_strcmp(const TCHAR* lhs, const TCHAR* rhs)
{
	return (('\0' == lhs[0]) && ('\0' == rhs[0])) ? 0
		: (lhs[0] != rhs[0]) ? (lhs[0] - rhs[0])
		: c_strcmp(lhs+1, rhs+1);
}

enum ERasterFlags
{
	DXRS_Normal		= 0x00,
	DXRS_Wireframe	= 0x01,
	DXRS_NoAA		= 0x02,
	DXRS_MAX		= 0xFF,
};

#if USE_RASTER_ARRAY
#define RASTER_ARRAY_SIZE (DXRS_NoAA * DXRS_NoAA)
#endif

// Metallicafan212:	Include the gamma mode settings
//					This is heavy todo!
#include "GammaModes.h"

// Metallicafan212:	Shader set flags
#include "ShaderFlags.h"

class UICBINDx11RenderDevice;

#include "UnShaderManager.h"

// Metallicafan212:	Just cutting down on the needed typing
namespace MS = Microsoft::WRL;

// Metallicafan212:	A UTexture that holds onto our RT and DT
//					TODO! This needs a bit more work, but whatever
//					Using ComPtr since there was a memory leak in the DX9 version...
class UDX11RenderTargetTexture : public UTexture
{
	DECLARE_CLASS(UDX11RenderTargetTexture, UTexture, CLASS_Config | CLASS_Transient | CLASS_NoUserCreate, D3D11Drv);

	// Metallicafan212:	Pointer to our render device
	class UICBINDx11RenderDevice*			D3DDev;

	// Metallicafan212:	Vars to hold the targets
	MS::ComPtr<ID3D11Texture2D>				RTTex;
	MS::ComPtr<ID3D11Texture2D>				NonMSAATex;
	MS::ComPtr<ID3D11RenderTargetView>		RTView;

	// Metallicafan212:	Depth info
	MS::ComPtr<ID3D11Texture2D>				DTTex;
	MS::ComPtr<ID3D11DepthStencilView>		DTView;
	MS::ComPtr<ID3D11ShaderResourceView>	DTSRView;

	// Metallicafan212:	Direct2D RT
	MS::ComPtr<ID2D1RenderTarget>			RTD2D;
	MS::ComPtr<IDXGISurface>				RTDXGI;

	// Metallicafan212:	Copy of the render target, so we can map it to a texture slot without issues
	MS::ComPtr<ID3D11Texture2D>				RTTexCopy;

	MS::ComPtr<ID3D11ShaderResourceView>	RTSRView;

#if DX11_UNREAL_227
	FRenderToTexture*						EngineRTTex;
#endif

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
		RTDXGI(nullptr),
		RTTexCopy(nullptr),
		D3DDev(nullptr)
#if DX11_UNREAL_227
		,EngineRTTex(nullptr)
#endif
	{

	}

	void Destroy();

	void Lock(FTextureInfo& TextureInfo, FTime CurrentTime, INT LOD, URenderDevice* RenDev);
};

// Metallicafan212:	Reset back to 50,000 verts, as that's the performance sweetspot apparently
#define VBUFF_SIZE 50000//1000000//50000//20000//200000
#define IBUFF_SIZE (VBUFF_SIZE * 2)//20000//200000

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

// Metallicafan212:	Extra info that's only used for complex surfaces
//					This is stored in a separate buffer and is only locked/used when needed
struct FD3DSecondaryVert
{
#if !COMPLEX_SURF_MANUAL_UVs
	FPlane	XAxis;
	FPlane	YAxis;
	FPlane	PanScale[5];
	FPlane	LFScale;
#else
	// Metallicafan212:	UV is used for the diffuse, UX and UY is used for the lightmaps
	//					So, detail, macro, and fog need UVs
	FLOAT	DU;
	FLOAT	DV;
	FLOAT	MU;
	FLOAT	MV;
	FLOAT	FU;
	FLOAT	FV;
#endif
};
// Metallicafan212:	Cache stuff
typedef unsigned long long D3DCacheId;

/*
// Metallicafan212:	Base class for shader variables, so we can support doing multiple draws at a time
class BaseShaderConstants
{
public:
	// Metallicafan212:	No constructor, children deal with that

	// Metallicafan212:	TODO! Define an interface to deal with the creation/copying of the variable values
	//					We have to be able to setup most item changes into one call as we can (textures, flags, raster state, etc.)
	//					Possibly, we should use a child struct to wrap it and make memcpy as easy as possible
	virtual BaseShaderConstants* GetNewShaderClass() = 0;
};
*/

struct FFogShaderVars
{
	UBOOL				bDoDistanceFog;
	UBOOL				bForceFogOff;

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
	FFogShaderVars() 
	{
		// Metallicafan212:	Everything should be zero, so just make it quicker
		appMemzero(this, sizeof(FFogShaderVars));
	}
};

// Metallicafan212:	We're not using TMap now, std::unorded_map completely nukes it in terms of performance
//					Until I write my own datastructure, this will do for now

// Metallicafan212:	Thanks to CacoFFF from OldUnreal for pointing this out for me!
//					If you don't change the default TMap hashing, it'll place half the textures in one bin....
//					TODO! This may only be a problem when compiling for UT99/227. I changed out the GetTypeHash to a QWORD in HP2
#if 0//!DX11_HP2 //&& !DX11_UT_469
static inline DWORD GetTypeHash(const D3DCacheId& A)
{
	D3DCacheId Value		= (D3DCacheId)A;
	return (DWORD)Value ^ ((DWORD)(Value >> 16)) ^ ((DWORD)(Value >> 32));
}
#endif

// Metallicafan212:	Texture bind definition
struct FD3DTexture
{
	ID3D11Texture2D*					m_Tex;
	ID3D11ShaderResourceView*			m_View;

	// Metallicafan212:	Unreal texture pointer, only used for detecting RT textures...
	UTexture*							Tex;

	// Metallicafan212:	The DX11 format
	DXGI_FORMAT							TexFormat;

	// Metallicafan212:	UE Format of this texture
	ETextureFormat						Format;

	// Metallicafan212:	Pointer to the texture type, so we can access the info
	struct FD3DTexType*					D3DTexType;

	// Metallicafan212:	Flags this was uploaded as
	//					This only matters if Masked is toggled on or off
	PFLAG								PolyFlags;

	// Metallicafan212:	If this texture should have UVs clamped (UClamp == USize && VClamp == VSize)
	UBOOL								bShouldUVClamp;

	// Metallicafan212:	If this is really a RT texture
	UBOOL								bIsRT;

	// Metallicafan212:	If this texture needs to be decompressed (BC1-7 in non-pow2 sizes)
	UBOOL								bDecompress;

	// Metallicafan212:	Number of mips to skip
	INT									MipSkip;

	// Metallicafan212:	The cache ID Unreal provided us when it was uploaded
	D3DCacheId							CacheID;

	// Metallicafan212:	Bind information
	INT									USize;
	INT									VSize;

	// Metallicafan212:	Scaling info
	INT									UClamp;
	INT									VClamp;

	// Metallicafan212:	UT469 (and now HP2) tracked number of changes to this texture
	INT									RealtimeChangeCount;

	// Metallicafan212:	Number of mips
	INT									NumMips;

	// Metallicafan212:	Color for the "color masking" system I added
	FPlane								MaskedColor;
	FPlane								MaskedGranularity;

	// Metallicafan212:	Array of UAVs for this texture
	TArray<ID3D11UnorderedAccessView*>	UAVMips;

	// Metallicafan212:	TODO! Keep around an array of nullptr to reset the compute shader
	TArray<ID3D11UnorderedAccessView*>	UAVBlank;

	// Metallicafan212:	Conversion texture for the P8 data
	ID3D11Texture2D*					P8ConvTex;
	ID3D11ShaderResourceView*			P8ConvSRV;
};


// Metallicafan212:	Start of a cache manager class
//					This way, it's generically used in the renderer code, and can be changed without changing other game's definitions
#include "UnTexCache.h"

struct FD3DBoundTex
{
	D3DCacheId					TexInfoHash;

	UBOOL						bIsRT;

	// Metallicafan212:	Currently bound shader resource view
	ID3D11ShaderResourceView*	m_SRV;

	// Metallicafan212:	TODO! More bound vars
	FLOAT						UPan;
	FLOAT						VPan;
	FLOAT						UMult;
	FLOAT						VMult;
	FLOAT						UScale;
	FLOAT						VScale;
	PFLAG						Flags;

	// Metallicafan212:	Slot mask for telling the shaders that this shader is bound
	DWORD						Mask;
};

// Metallicafan212:	Draw call info

/*
// Metallicafan212:	Cached render state
//					This is so we can execute as many draw calls at once as we can
//					This is a HEAVY TODO!!!!!
//					The idea is that we only set what we need, when we need to
struct FDrawCall
{
	// Metallicafan212:	Bend state to use, created from Polyflags
	UBOOL						bSetBlend;
	ID3D11BlendState*			BlendState;

	// Metallicafan212:	Raster state to use
	UBOOL						bSetRaster;
	ID3D11RasterizerState*		RasterState;

	// Metallicafan212:	Depth-stencil state
	UBOOL						bSetDState;
	ID3D11DepthStencilState*	DSState;

	// Metallicafan212:	Vertex range (start and size)
	SIZE_T						VStart;

	// Metallicafan212:	Index range (start and size)
	SIZE_T						IStart;
	SIZE_T						ISize;

	// Metallicafan212:	Maps instead, so we can just loop and ONLY set the changed textures
	std::unordered_map<INT, ID3D11ShaderResourceView*>	TBinds;
	std::unordered_map<INT, ID3D11SamplerState*>		SBinds;

	// Metallicafan212:	Shader to use for this draw
	//					Changing the shader results in a new draw call
	UBOOL						bSetShader;
	class FD3DShader*			Shader;

	// Metallicafan212:	Render target and depth to set (if needed)
	UBOOL						bSetRT;
	ID3D11RenderTargetView*		RTV;
	ID3D11DepthStencilView*		DSV;

	// Metallicafan212:	TODO! Use TArrays to store the memory
	UBOOL						bSetFrameConstants;
	TArray<BYTE>				FrameShaderConstants;

	UBOOL						bSetDFogConstants;
	TArray<BYTE>				DFogShaderConstants;

	UBOOL						bSetFlagConstants;
	TArray<BYTE>				FlagShaderConstants;

	UBOOL						bSetTexConstants;
	TArray<BYTE>				TexShaderConstants;

	UBOOL						bSetUserConstants;
	TArray<BYTE>				UserConstants;

	// Metallicafan212:	Buffer to set for user constants
	ID3D11Buffer*				UserBuffer;

	// Metallicafan212:	Topology to set for this draw
	UBOOL						bSetTopology;
	D3D_PRIMITIVE_TOPOLOGY		Topology;

	FDrawCall()
		:
		bSetBlend(0), BlendState(nullptr), bSetRaster(0), RasterState(nullptr), bSetDState(0), DSState(nullptr),
		VStart(0), IStart(0), ISize(0), TBinds(), SBinds(), bSetShader(0), Shader(nullptr), bSetRT(0), RTV(nullptr), DSV(nullptr),
		bSetFrameConstants(0), bSetDFogConstants(0), bSetFlagConstants(0), bSetTexConstants(0), bSetUserConstants(0), UserBuffer(nullptr),
		bSetTopology(0), Topology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
	{

	}
};
*/


// Metallicafan212:	Generic function to handle all texture types
typedef void (UICBINDx11RenderDevice::*UploadFunc)(const FTextureInfo& Info, FD3DTexture* Tex, INT Mip, UBOOL bPartial, INT UpdateX, INT UpdateY, INT UpdateW, INT UpdateH);

// Metallicafan212:	Texture support table info
struct FD3DTexType
{
	typedef SIZE_T (FD3DTexType::* GetTypePitch)(INT USize);

	// Metallicafan212:	The UE format
	ETextureFormat	Format;

	// Metallicafan212:	The DirectX format we're going to use to upload this
	DXGI_FORMAT		DXFormat;

	UBOOL			bSupported;

	// Metallicafan212:	If this is a compressed texture type. These need clamping in FL 11.0
	UBOOL			bIsCompressed;

	INT				BytesPerPixel;
	INT				BlockSize;

	// Metallicafan212:	TODO! Memory uploading function
	UploadFunc		TexUploadFunc;

	// Metallicafan212:	Pitch function
	GetTypePitch	GetTexturePitch;

	FORCEINLINE SIZE_T GetPitch(INT USize)
	{
#if DX11_UT_469 || DX11_UNREAL_227
		return FTextureBlockBytes(Format)* FTextureBlockAlignedWidth(Format, USize) / FTextureBlockWidth(Format);
#else
		return (this->*GetTexturePitch)(USize);
#endif
	}

	// Metallicafan212:	THIS IS ASSUMED RIGHT NOW!!!!
	FORCEINLINE SIZE_T RawPitch(INT USize)
	{
		return BytesPerPixel * USize;
	}

	FORCEINLINE SIZE_T BlockCompressionPitch(INT USize)
	{
		return max(1, ((USize + 3) / 4)) * BlockSize;
	}
};

// Metallicafan212:	Base layout declaration
#if EXTRA_VERT_INFO
#if !COMPLEX_SURF_MANUAL_UVs
extern D3D11_INPUT_ELEMENT_DESC FBasicInLayout[12];
#else
extern D3D11_INPUT_ELEMENT_DESC FBasicInLayout[6];
#endif
#else
extern D3D11_INPUT_ELEMENT_DESC FBasicInLayout[4];
#endif

#define SAFE_RELEASE(ptr) if(ptr != nullptr){ptr->Release(); ptr = nullptr;}
#define SAFE_DELETE(ptr) if(ptr != nullptr){delete ptr; ptr = nullptr;}

#if DX11_D2D && USE_UNODERED_MAP_EVERYWHERE
template <>
struct std::hash<FString>
{
	std::size_t operator()(const FString& k) const
	{
		// Metallicafan212:	Use the default string hashing
#if UNICODE
		return hash<std::wstring>()(*k);
#else
		return hash<std::string>()(*k);
#endif
	}
};
#endif

#if DX11_UT_469
#define RD_CLASS URenderDeviceOldUnreal469
#else
#define RD_CLASS URenderDevice
#endif

#if DX11_D2D
// Metallicafan212:	Stores a single line of text to draw, so we can try to do less thrashing on the render device
struct FD2DStringDraw
{
	IDWriteTextLayout*		Layout;
	IDWriteTextFormat*		Font;
	PFLAG					Flags;
	FPlane					TrueColor;
	FString					TrueText;
	ID2D1SolidColorBrush*	Color;
	ID2D1SolidColorBrush*	ShadowColor;
	D2D1_POINT_2F			Point;
	D2D1_POINT_2F			ShadowPoint;
	FLOAT					ClipW;
	FLOAT					ClipH;
	FLOAT					ClipX;
	FLOAT					ClipY;
	UBOOL					bDoNotCombine:1;
	UBOOL					bShadow:1;
	//UBOOL					bWordWrap:1;

	// Metallicafan212:	Underline ranges
	TArray<DWRITE_TEXT_RANGE> Ranges;
};
#endif


// Metallicafan212:	Different shader definitions
#include "UnD3DShader.h"

// Metallicafan212:	Define an exported renderer
class UICBINDx11RenderDevice : public RD_CLASS
{
	DECLARE_CLASS(UICBINDx11RenderDevice, RD_CLASS, CLASS_Config, ICBINDx11Drv);

	// Metallicafan212:	User options
	INT							NumAASamples;
	INT							NumAFSamples;
	FLOAT						ThreeDeeLineThickness;
	FLOAT						OrthoLineThickness;
	UBOOL						bDebugSelection;
	UBOOL						bUseD3D11On12;
	UBOOL						bDisableDebugInterface;
	UBOOL						bDisableSDKLayers;
	UBOOL						bUseMultiThreadedDevice;
	UBOOL						UseVSync;
	UBOOL						bUseDeferredRendering;

	// Metallicafan212:	Alternative alpha reject for masked hud tiles
	UBOOL						bSmoothHudTiles;

	// Metallicafan212:	If to use the DX9 style flat colors instead (a lot brighter and hides the original textures)
	UBOOL						UseDX9FlatColor;

	// Metallicafan212:	If to use HDR (note that it'll look wrong on non-HDR screens)
	UBOOL						UseHDR;

	// Metallicafan212:	If to use HDR in the editor (NOTE! This isn't finished, and windows isn't auto detecting the app for HDR, so it just makes the image extremely bright...)
	UBOOL						UseHDRInEditor;

	// Metallicafan212:	If HDR is active (and the screenformat is set)
	UBOOL						ActiveHDR;

	// Metallicafan212:	If the user wants to automatically override the HDR active detection
	UBOOL						ForceHDR;

	// Metallicafan212:	If the user wants to turn off freesync/gsync. This constructs the swap chain WITHOUT tearing support
	UBOOL						DisableFreeGSync;

	// Metallicafan212:	HACK coord minus for the current MSAA level...
	//					Certain levels need different coord movements
	FLOAT						TileAAUVMove;

	// Metallicafan212:	TODO! Float to provide super/min'd resolution scaling
	FLOAT						ResolutionScale;

	// Metallicafan212:	Gamma correction value for the final output
	FLOAT						Gamma;

	// Metallicafan212:	User selectable gamma mode
	BYTE						GammaMode;

	// Metallicafan212:	Surface selection color
	FColor						SurfaceSelectionColor;

	// Metallicafan212:	Actor selection color
	FColor						ActorSelectionColor;

	// Metallicafan212:	Additional buffer count
	//					0 = 2 buffers (back, front)
	//					1 = 3 buffers, triple buffering
	INT							NumAdditionalBuffers;

	// Metallicafan212:	If to use the DX9 style scene node hack
	UBOOL						SceneNodeHack;

	// Metallicafan212:	Gamma offset value
	//					Default is 0.0f, setting it above 0 will brighten the image for the same brightness setting, negative will reduce it
	FLOAT						GammaOffset;

	// Metallicafan212:	Gamma offsets for each color channel (DX9 mode only)
	FLOAT						GammaOffsetRed;
	FLOAT						GammaOffsetBlue;
	FLOAT						GammaOffsetGreen;

	// Metallicafan212:	HDR color correction value, since we have to expand sRGB to linear color for HDR to work
	FLOAT						AdditionalHDRExpansion;

	// Metallicafan212:	User definable nits for the screen
	//					If this is <= 0, the driver will attempt to autodetect it
	INT							HDRWhiteBalanceNits;

	INT							DetectedWhiteBalance;

	// Metallicafan212:	If we shouldn't multiply lightmaps by 2 in the complex surface shader
	UBOOL						bOneXLightmaps;

	// Metallicafan212:	If actor fogging should reduce lighting coverage, or use the broken DX7 fogging
	UBOOL						bEnableCorrectFogging;

	// Metallicafan212:	Alpha rejection value, for masking
	FLOAT						MaskedAlphaReject;

	// Metallicafan212:	Alternative masking reject for hud tiles, if bSmoothHudTiles
	FLOAT						SmoothMaskedAlphaReject;

	// Metallicafan212:	Configured Z range for the depth RMode
	//					Max depth is 65535.0f
	FLOAT						DepthDrawZLimit;

	// Metallicafan212:	If to always use the precompiled shaders, rather than recompiling the source HLSL files
	//					This is only for the base shaders, user shaders will have to be recompiled
	//					This basically just re-inits the shader cache using the hard-coded shader bytes
	UBOOL						bUsePrecompiledShaders;

	INT							LastAdditionalBuffers;

	// Metallicafan212:	Versions to check on lock if they changed
	INT							LastAASamples;
	INT							LastAFSamples;

	FLOAT						LastResolutionScale;

	// Metallicafan212:	Version to check gamma
	FLOAT						LastGamma;

	// Metallicafan212:	Last HDRWhiteBalanceNits value
	INT							LastHDRWhiteBalanceNits;

	// Metallicafan212:	If the GPU is AMD/ATI, Intel, or NVidia
	UBOOL						bIsNV;
	UBOOL						bIsAMD;
	UBOOL						bIsIntel;

	// Metallicafan212:	If we're using RGBA8 instead of BGRA8
	UBOOL						bForceRGBA;

	DXGI_FORMAT					ScreenFormat;

	// Metallicafan212:	HACK for resolution scale! If this is set, it means that the scene node set isn't the same as the viewport size
	UBOOL						bSmallerSceneNode;


	// Metallicafan212:	TODO! Generalized D3D variables
	ID3D11Device*				m_D3DDevice;

	// Metallicafan212:	This device version is only needed for the raster description
	ID3D11Device1*				m_D3DDevice1;

	IDXGISwapChain1*			m_D3DSwapChain;
	ID3D11DeviceContext*		m_D3DDeviceContext;

	// Metallicafan212:	Deferred command list support
	ID3D11DeviceContext*		m_D3DDeferredContext;
	ID3D11CommandList*			m_D3DCommandList;

	// Metallicafan212:	Pointer to use for rendering, so that we can swap in and out of the experimental mode
	ID3D11DeviceContext*		m_RenderContext;

	ID3D11Query*				m_D3DQuery;

	// Metallicafan212:	The current feature level, so we know if we support specific features
	D3D_FEATURE_LEVEL			m_FeatureLevel;

	// Metallicafan212:	The levels to use when compiling shaders
	const ANSICHAR*				MaxVSLevel{};
	const ANSICHAR*				MaxPSLevel{};
	const ANSICHAR*				MaxGSLevel{};

	// Metallicafan212:	The GPU name, so we can invalidate the shader cache if it changes
	FString						GPUDesc;
	LARGE_INTEGER				GPUDriverVer;

	// Metallicafan212:	If to use geo shaders at all (Shader version 3.0 has no geo shaders)
	UBOOL						bUseGeoShaders;

	// Metallicafan212:	If we can use ForcedSampleCount
	UBOOL						bSupportsForcedSampleCount;

	UBOOL						bUseForcedSampleCount;

	// Metallicafan212:	For debugging only (will be removed later)
	ID3D11Debug*				m_D3DDebug;
	ID3D11InfoQueue*			m_D3DQueue;

	// Metallicafan212:	The backbuffer texture, so we can resolve resource to it (support MSAA)
	ID3D11Texture2D*			m_BackBuffTex;

	// Metallicafan212:	The render target view for the backbuffer (so we can use shaders on the input)
	ID3D11RenderTargetView*		m_BackBuffRT;

	// Metallicafan212:	The unordered view for the backbuffer, so we can use a compute shader on it
	ID3D11ShaderResourceView*	m_BackBuffSRV;

	// Metallicafan212:	The shader view for the back buffer, so we can draw it on things
	ID3D11UnorderedAccessView*	m_BackBuffUAV;

	// Metallicafan212:	Selection texture, in one format so that the actual screen draw can be in any format
	ID3D11Texture2D*			m_SelectionTex;

	// Metallicafan212:	Selection render target
	ID3D11RenderTargetView*		m_SelectionRTV;

	// Metallicafan212:	Selection shader view, so it can be rendered to the screen when the debug selection bool is on
	ID3D11ShaderResourceView*	m_SelectionSRV;


	// Metallicafan212:	The number of threads to use for the MSAA compute shader
	//					TODO! Work on this more so that the shader can be recompiled with the right base level of threads!!!!
	INT							MSAAThreadX;
	INT							MSAAThreadY;

	// Metallicafan212:	Screen holding texture for MSAA resolving, when NumAASamples > 1
	ID3D11Texture2D*			m_MSAAResolveTex;

	// Metallicafan212:	The shader resource view, so we can use it as a input when doing the secondary shader
	ID3D11ShaderResourceView*	m_MSAAResolveSRV;

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

	// Metallicafan212:	Selection (no MSAA) depth texture
	ID3D11Texture2D*			m_SelectionDSTex;

	// Metallicafan212:	Selection (no MSAA) depth and stencil target
	ID3D11DepthStencilView*		m_SelectionDSV;

	// Metallicafan212:	The depth shader resource view (for MSAA resolving)
	ID3D11ShaderResourceView*	m_ScreenDTSRV;

	/*
	// Metallicafan212:	Default depth stencil state
	ID3D11DepthStencilState*	m_DefaultZState;

	// Metallicafan212:	Default state with no z writing
	ID3D11DepthStencilState*	m_DefaultNoZState;

	// Metallicafan212:	State for the special no AA mode when drawing tiles
	ID3D11DepthStencilState*	m_DefaultNoZWriteState;
	*/

	// Metallicafan212:	Z test states
	//					TODO! Maybe make this a bit better?
	ID3D11DepthStencilState*	DepthStencilStates[ZTEST_MAX << 2];

	// Metallicafan212:	Raster states
#if USE_RASTER_ARRAY
	ID3D11RasterizerState*		RasterArray[RASTER_ARRAY_SIZE];
#else
#if !USE_UNODERED_MAP_EVERYWHERE
	TMap<DWORD, ID3D11RasterizerState*> RasterMap;
#else
	std::unordered_map<DWORD, ID3D11RasterizerState*> RasterMap;
#endif
#endif

	DWORD						CurrentRasterState;

	// Metallicafan212:	So we can easily request wireframe
	DWORD						ExtraRasterFlags;

#if DX11_D2D
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

	// Metallicafan212:	Raster state for rendering scissored text
	ID3D11RasterizerState*		m_D2DRasterState;

	// Metallicafan212:	Font to layout caching
	//					This allows the renderer to cache commonly drawn strings again and again, so that it's not esessively constructing layouts between calls
	//					For sanity reasons (for now...), the layout will be released when drawn
	std::unordered_map<IDWriteTextFormat*, std::unordered_map<FString, IDWriteTextLayout*>> FontToLayoutMap;

	// Metallicafan212:	Holder for the different font types
#if !USE_UNODERED_MAP_EVERYWHERE
	TMap<FString, IDWriteTextFormat*>	FontMap;
#else
	std::unordered_map<FString, IDWriteTextFormat*>	FontMap;
#endif

	// Metallicafan212:	Current clipping setup
	FLOAT						StringClipX;
	FLOAT						StringClipY;
	FLOAT						StringClipW;
	FLOAT						StringClipH;

	// Metallicafan212:	Cached string draw call
	TArray<FD2DStringDraw>		BufferedStrings;
#endif

	// Metallicafan212:	Orthoganal zoom value from the frame
	FLOAT						OrthoZoom;

	// Metallicafan212:	Array of RT textures
	TArray<UDX11RenderTargetTexture*>	RTTextures;

	// Metallicafan212:	RTTexture currently bound
	UDX11RenderTargetTexture*			BoundRT;

	// Metallicafan212:	Array representing the stack of RTTextures currently bound (so they can do dumb shit)
	TArray<UDX11RenderTargetTexture*>	RTStack;

	// Metallicafan212:	Shader list, so we can cleanup
	//					TODO! Might make each base shader referenced directly rather than an array???
	TArray<FD3DShader*>					Shaders;

	FD3DTileShader*						FTileShader;
	FD3DGenericShader*					FGenShader;
	FD3DResScalingShader*				FResScaleShader;

#if USE_COMPUTE_SHADER
	FD3DLghtMshCompShader*				FMshLghtCompShader;
#endif

	FD3DMeshShader*						FMeshShader;
	FD3DSurfShader*						FSurfShader;
	FD3DLineShader*						FLineShader;
	
#if P8_COMPUTE_SHADER
	FD3DP8ToRGBAShader*					FP8ToRGBAShader;
#endif

	FD3DShader*							CurrentShader;

	FShaderManager*						ShaderManager;

	// Metallicafan212:	Holds onto values that the shaders (and renderer) code will be interested in
	//					This does not directly map to variables in the shader itself, as there's copies used to do fading
	FFogShaderVars						FogShaderVars;

	// Metallicafan212:	HACK!!!! This tracks how many times we've pushed onto the distance fog stack
	INT									FogHackPopCount;

	// Metallicafan212:	Holds onto global variables for the shaders, so we're not uploading so much info all the time
	FFrameShaderVars					FrameShaderVars;

	// Metallicafan212:	Constant buffer for global info
	ID3D11Buffer*						FrameConstantsBuffer;

#if DX11_DISTANCE_FOG
	// Metallicafan212:	Distance fog settings
	FDistFogVars						GlobalDistFogSettings;
	ID3D11Buffer*						GlobalDistFogBuffer;

	// Metallicafan212:	2024, stacks of distance fog, so we can do subframe rendering with different fog values
	//					Basically: it'll push the current values here to update the fog, and then we can pop out the fog
	TArray<FFogShaderVars>				DistanceFogStack;
#endif

	// Metallicafan212:	The global vars JUST for flags
	FPolyflagVars						GlobalPolyflagVars;
	ID3D11Buffer*						GlobalPolyflagsBuffer;

	// Metallicafan212:	Buffer just for bound textures
	FBoundTextures						BoundTexturesInfo;
	ID3D11Buffer*						BoundTexturesBuffer;

	// Metallicafan212:	If to write it out
	UBOOL								bWriteTexturesBuffer;

	// Metallicafan212:	Blending map
	//					We have to keep around blend objects (rather than setting render states directly) so they're mapped to the polyflag that it represents
#if !USE_UNODERED_MAP_EVERYWHERE
	TMap<PFLAG, ID3D11BlendState*>		BlendMap;
	TMap<PFLAG, ID3D11BlendState*>		BlendCache;
#else
	std::unordered_map<PFLAG, ID3D11BlendState*> BlendMap;
	std::unordered_map<PFLAG, ID3D11BlendState*> BlendCache;
#endif

	// Metallicafan212:	Sampler map
	//					This is required since you can't just on the fly update sampler objects....
	//					Sucks, but it is what it is

	// Metallicafan212:	Actually, instead use an array of sampliers!
#if USE_SAMPLER_ARRAY
	ID3D11SamplerState*					SampArray[MAX_SAMPLERS];
#else

#if !USE_UNODERED_MAP_EVERYWHERE
	TMap<PFLAG, ID3D11SamplerState*>	SampMap;
#else
	std::unordered_map<PFLAG, ID3D11SamplerState*> SampMap;
#endif

#endif

	// Metallicafan212:	Sampler for drawing the scaled screen (using ResolutionScale)
	ID3D11SamplerState*					ScreenSamp;

	PFLAG								CurrentPolyFlags;

	// Metallicafan212:	Current 227 style Z test mode
	BYTE								CurrentZTestMode;

	// Metallicafan212:	Current INDEX into the depth stencil array
	//					This is so we can (optionally) turn off features and have this still work
	INT									CurrentZTestIndex;

	// Metallicafan212:	Special modifiers to the depth stencil key
	//					This corresponds to the bottom 2 bits
	//					Bit 0 is no z write (~PF_Occlude)
	//					Bit 1 is no depth/stencil (for the no-AA tiles)
	DWORD								DepthStencilFlags;

	// Metallicafan212:	If we're using a Freesync/GSync mode
	UBOOL								bAllowTearing;

	UINT								PresentFlags;

	// Metallicafan212:	If we're requesting the flip discard flag
	UBOOL								bFlipDiscard;

	// Metallicafan212:	Unreal related variables
	UBOOL								bFullscreen;
	UBOOL								bLastFullscreen;
	INT									SizeX;
	INT									SizeY;
	FLOAT								ScaledSizeX;
	FLOAT								ScaledSizeY;

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

	TArray<FPixHitInfo>					PixelHitInfo;

	// Metallicafan212:	Keep track of the "top" index
	INT									PixelTopIndex;

	FPlane								CurrentHitColor;

	// Metallicafan212:	Dynamic memory, equal to sizeof(PixelFormat) * USize * VSize;
	//					For most, it's just going to be = 4bpp (to keep it simple)
	BYTE*								ConversionMemory;
	SIZE_T								ConversionMemSize;

	// Metallicafan212:	Texturing support
	//					TODO! Query for this limitation and put the number in a ifdef!
	FD3DBoundTex						BoundTextures[MAX_TEXTURES];

	ID3D11Texture2D*					BlankTexture;
	ID3D11ShaderResourceView*			BlankResourceView;
	ID3D11SamplerState*					BlankSampler;

	// Metallicafan212:	Generic class so we can implement different caching styles without having to change a bunch of files
	FTextureCache						TextureMap;

#if !USE_UNODERED_MAP_EVERYWHERE
	TMap<ETextureFormat, FD3DTexType>	SupportedTextures;
#else
	std::unordered_map<ETextureFormat, FD3DTexType> SupportedTextures;
#endif

	// Metallicafan212:	Copied variables from the DX9 renderer
	UBOOL								m_nearZRangeHackProjectionActive;

	// Lock variables.
	FPlane FlashScale, FlashFog;
	FLOAT m_RProjZ, m_Aspect;
	FLOAT m_RFX2, m_RFY2;
	INT m_sceneNodeX, m_sceneNodeY;
	UBOOL m_OrthoProjection;
	FLOAT ScaledSceneNodeX, ScaledSceneNodeY;
	FLOAT ScaledFX2, ScaledFY2;

	FLOAT MaxZ;

	// Metallicafan212:	TODO! Global complex surface info
	FCoords								SurfCoords;

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
		BT_ScreenFlash,
		BT_Strings
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

	D3D11_MAPPED_SUBRESOURCE	VertexBuffMap;

	// Metallicafan212:	How many verts were buffered (how many to render when calling a draw command)
	SIZE_T						m_BufferedVerts;

	// Metallicafan212:	Hard-coded array for the screen bounds
	FD3DVert					ScreenVerts[6] =
	{
		{-1.f, -1.f, 0.f, 1.f, 0.f, 1.f, 0.f, 0.f},
		{1.0f, -1.f, 0.f, 1.f, 1.f, 1.f, 0.f, 0.f},
		{-1.f, 1.0f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f},
		{-1.f, 1.0f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f},
		{1.0f, -1.f, 0.f, 1.f, 1.f, 1.f, 0.f, 0.f},
		{1.0f, 1.0f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f},

	};

	// Metallicafan212:	Index buffer
	ID3D11Buffer*				IndexBuffer;

	D3D11_MAPPED_SUBRESOURCE	IndexBuffMap;

	// Metallicafan212:	Position (in bytes!!!!)
	SIZE_T						m_IndexBuffPos;

	// Metallicafan212:	Index offset in the index stream
	SIZE_T						m_DrawnIndices;

	// Metallicafan212:	Size of the buffer (in bytes)
	//					TODO! If we get past this, what then? How do we flush?
	SIZE_T						m_IndexBuffSize;

	// Metallicafan212:	Pointer to the index data
	INDEX*						m_IndexBuff;

	SIZE_T						m_BufferedIndices;

	UBOOL						bNeedsAutoIBuff;

	INT							m_VLockCount;
	INT							m_ILockCount;

	// Metallicafan212:	A pre-defined array containing indicies to copy from
	//					TODO! Redefine the max size, right now (for consistency and preventing memory issues) it matches the iBuffer length
	static UBOOL				bSetupIValArray;
	static INT					IndexValueArray[IBUFF_SIZE];

	// Metallicafan212:	Secondary vertex buffer, which is only locked when needed
	ID3D11Buffer*				SecondaryVertexBuffer;

	FD3DSecondaryVert*			m_SecVertexBuff;

	D3D11_MAPPED_SUBRESOURCE	SecVertBuffMap;

	// Metallicafan212:	Size (in bytes)
	SIZE_T						m_SecVertexBuffSize;

	// Metallicafan212:	Position (in bytes!!!!)
	SIZE_T						m_SecVertexBuffPos;

	// Metallicafan212:	If it should be cleared on next draw
	UBOOL						bClearSec;

	// Metallicafan212:	Draws to execute at the end of the frame
	//TArray<FDrawCall*>			BufferedDraws;

	// Metallicafan212:	Pointer to the current draw
	//FDrawCall*					CurrentDraw;

	inline const TCHAR* GetD3DDebugSeverity(D3D11_MESSAGE_SEVERITY s)
	{
		switch (s)
		{
			case D3D11_MESSAGE_SEVERITY_CORRUPTION:
			{
				return TEXT("Corruption");
			}

			case D3D11_MESSAGE_SEVERITY_ERROR:
			{
				return TEXT("Error");
			}

			case D3D11_MESSAGE_SEVERITY_WARNING:
			{
				return TEXT("Warning");
			}

			case D3D11_MESSAGE_SEVERITY_INFO:
			{
				return TEXT("Info");
			}

			case D3D11_MESSAGE_SEVERITY_MESSAGE:
			{
				return TEXT("Message");
			}

			default:
			{
				return TEXT("");
			}
		}
	}

	// Metallicafan212:	Helper function for errors from the M$ sample
	FORCEINLINE void ThrowIfFailed(HRESULT hr)
	{
		if (FAILED(hr))
		{
			// Set a breakpoint on this line to catch DirectX API errors
			//throw std::exception();
			//appThrowf(TEXT("DX11 encountered an error (%lu)"), hr);

			// Metallicafan212:	Grab all the debug info!!!!
			if (m_D3DQueue != nullptr)
			{
				UINT64 n = m_D3DQueue->GetNumStoredMessages();

				for (INT i = 0; i < n; i++)
				{
					// Metallicafan212:	Sigh.... No better way to do this...
					D3D11_MESSAGE* temp = nullptr;

					// Metallicafan212:	Get the size of this message
					SIZE_T mSize = 0;

					m_D3DQueue->GetMessage(i, nullptr, &mSize);

					if (mSize != 0)
					{
						temp = (D3D11_MESSAGE*)appMalloc(mSize, TEXT("DX11 Error Unrapping"));

						m_D3DQueue->GetMessage(i, temp, &mSize);

						// Metallicafan212:	Now log it
						GWarn->Logf(TEXT("DX11 debug message (%s): %s"), GetD3DDebugSeverity(temp->Severity), appFromAnsi(temp->pDescription));

						appFree(temp);
					}
				}
			}

			appErrorf(TEXT("DX11 encountered an error (%lu). If the debug layer is supported, the log file will likely contain useful debug info!"), hr);
		}
	}

	// Metallicafan212:	TODO! Replace ALL non-indexed drawing with this....
	FORCEINLINE void DoStandardIBuff(INT VertNum)
	{
		appMemcpy(m_IndexBuff, &IndexValueArray[m_BufferedVerts], sizeof(INDEX) * VertNum);
	}

	FORCEINLINE void LockVertAndIndexBuffer(SIZE_T VertCount, SIZE_T IndexCount = 0, UBOOL bNoOverwrite = 1)
	{
		m_VLockCount = VertCount;
		m_ILockCount = IndexCount;

		// Metallicafan212:	Check for room
		QWORD VTest = ((QWORD)m_BufferedVerts) + m_DrawnVerts + VertCount;
		if (VTest >= VBUFF_SIZE)
		{
			bNoOverwrite = 0;
		}

		// Metallicafan212:	Default allotment
		if (IndexCount == 0)
		{
			IndexCount = VertCount;

			// Metallicafan212:	Automatically fill the index buffer
			bNeedsAutoIBuff = 1;

			m_ILockCount = IndexCount;
		}
		else
		{
			bNeedsAutoIBuff = 0;
		}

		QWORD ITest = ((QWORD)m_BufferedIndices) + m_DrawnIndices + IndexCount;

		if (ITest >= IBUFF_SIZE)
		{
			bNoOverwrite = 0;
		}

		if (!bNoOverwrite)
		{
			// Metallicafan212:	Ran out of room, check if there's stuff to render
			if (m_BufferedVerts != 0)
			{
				EndBuffering();
			}

			m_VertexBuffPos = 0;
			m_IndexBuffPos	= 0;
			m_DrawnVerts	= 0;
			m_DrawnIndices	= 0;

#if EXTRA_VERT_INFO
			bClearSec		= 1;
#endif

			if (m_VertexBuff != nullptr)
				UnlockBuffers();
		}

		// Metallicafan212:	Lock ONCE if we can get away with it!
		if (m_VertexBuff == nullptr)
		{
			D3D11_MAP MType = bNoOverwrite ? D3D11_MAP_WRITE_NO_OVERWRITE : D3D11_MAP_WRITE_DISCARD;

			HRESULT hr		= m_D3DDeviceContext->Map(VertexBuffer, 0, MType, 0, &VertexBuffMap);

			ThrowIfFailed(hr);

			// Metallicafan212:	Now lock the IBuff
			hr				= m_D3DDeviceContext->Map(IndexBuffer, 0, MType, 0, &IndexBuffMap);

			ThrowIfFailed(hr);
		}

		// Metallicafan212:	Update the pointers
		m_VertexBuff	= (FD3DVert*)((BYTE*)VertexBuffMap.pData + m_VertexBuffPos);
		m_IndexBuff		= (INDEX*)((BYTE*)IndexBuffMap.pData + m_IndexBuffPos);
	}

	FORCEINLINE void LockSecondaryVertBuffer()
	{
		// Metallicafan212:	Lock and map it, it defaults to being unmapped
		if (m_SecVertexBuff == nullptr)
		{
			D3D11_MAP MType		= !bClearSec ? D3D11_MAP_WRITE_NO_OVERWRITE : D3D11_MAP_WRITE_DISCARD;

			HRESULT hr			= m_D3DDeviceContext->Map(SecondaryVertexBuffer, 0, MType, 0, &SecVertBuffMap);

			ThrowIfFailed(hr);

			bClearSec			= 0;
		}

		// Metallicafan212:	Set the new position
		m_SecVertexBuffPos		= (m_DrawnVerts + m_BufferedVerts) * sizeof(FD3DSecondaryVert);

		// Metallicafan212:	Update the buffer pointer
		m_SecVertexBuff			= (FD3DSecondaryVert*)((BYTE*)SecVertBuffMap.pData + m_SecVertexBuffPos);
	}

	FORCEINLINE void UnlockBuffers()
	{
		if (m_VertexBuff != nullptr)
			m_D3DDeviceContext->Unmap(VertexBuffer, 0);

		m_VertexBuff = nullptr;

		if (m_IndexBuff != nullptr)
			m_D3DDeviceContext->Unmap(IndexBuffer, 0);

		m_IndexBuff = nullptr;

#if EXTRA_VERT_INFO
		if (m_SecVertexBuff != nullptr)
		{
			m_D3DDeviceContext->Unmap(SecondaryVertexBuffer, 0);

			m_SecVertexBuff = nullptr;
		}
#endif
	}

	FORCEINLINE void AdvanceVertPos()
	{
		if (bNeedsAutoIBuff)
		{
			DoStandardIBuff(m_VLockCount);

			bNeedsAutoIBuff = 0;
		}

		m_BufferedVerts		+= m_VLockCount;
		m_VertexBuffPos		+= m_VLockCount * sizeof(FD3DVert);
		m_IndexBuffPos		+= sizeof(INDEX) * m_ILockCount;
		m_BufferedIndices	+= m_ILockCount;

		// Metallicafan212:	Reset the locked count
		m_VLockCount = 0;
		m_ILockCount = 0;
	}

	FORCEINLINE void EndBuffering()
	{
#if DX11_D2D
		if (m_CurrentBuff == BT_Strings && BufferedStrings.Num())
		{
			// Metallicafan212:	Draw all the strings
			m_CurrentD2DRT->BeginDraw();

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
			// Metallicafan212:	Push global clipping
#if 0//DX11_D2D_CLIP_LAYER
			m_CurrentD2DRT->PushAxisAlignedClip(D2D1::Rect(StringClipX, StringClipY, StringClipX + StringClipW, StringClipY + StringClipH), D2D1_ANTIALIAS_MODE_ALIASED);//D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
#endif
			for (INT i = 0; i < BufferedStrings.Num(); i++)
			{
				FD2DStringDraw& D = BufferedStrings(i);

#if DX11_D2D_CLIP_LAYER
				D2D1_RECT_F Rect;
				Rect.left	= D.ClipX;
				Rect.right	= D.ClipX + D.ClipW;
				Rect.top	= D.ClipY;
				Rect.bottom	= D.ClipY + D.ClipH;
				m_CurrentD2DRT->PushAxisAlignedClip(Rect, D2D1_ANTIALIAS_MODE_ALIASED);//D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

				// Metallicafan212:	Set the width and height of this layout
				D.Layout->SetMaxWidth(Rect.right - Rect.left);
				D.Layout->SetMaxHeight(Rect.bottom - Rect.top);

				// Metallicafan212:	Set the text ranges
				DWRITE_TEXT_RANGE NoRange;
				NoRange.startPosition	= 0;
				NoRange.length			= D.TrueText.Len();

				D.Layout->SetUnderline(FALSE, NoRange);

#endif
				if (D.bShadow)
				{
					m_CurrentD2DRT->DrawTextLayout(D.ShadowPoint, D.Layout, D.ShadowColor, D2D1_DRAW_TEXT_OPTIONS_NO_SNAP);//|  D2D1_DRAW_TEXT_OPTIONS_CLIP);
					D.ShadowColor->Release();
				}

				// Metallicafan212:	Now set the underlines
				for (INT i = 0; i < D.Ranges.Num(); i++)
				{
					D.Layout->SetUnderline(TRUE, D.Ranges(i));
				}

				m_CurrentD2DRT->DrawTextLayout(D.Point, D.Layout, D.Color, D2D1_DRAW_TEXT_OPTIONS_NO_SNAP);//|  D2D1_DRAW_TEXT_OPTIONS_CLIP);

				D.Layout->Release();
				D.Color->Release();

#if DX11_D2D_CLIP_LAYER
				m_CurrentD2DRT->PopAxisAlignedClip();
#endif
			}
#if 0//DX11_D2D_CLIP_LAYER
			m_CurrentD2DRT->PopAxisAlignedClip();
#endif

#if DX11_D2D_LAYOUT_CACHING
			// Metallicafan212:	Empty all buffered layouts
			//					We're using reference counting, so all memory will be freed now
			//FontToLayoutMap.clear();
			
			// Metallicafan212:	Loop and clear all _submaps_.....
			for (auto i = FontToLayoutMap.begin(); i != FontToLayoutMap.end(); i++)
			{
				// Metallicafan212:	Fix if the user used random characters to size.....
				//					TODO! Maybe remove layout caching???? This is too janky and prone to issues....
				for (auto j = i->second.begin(); j != i->second.end(); j++)
				{
					while (j->second->Release() > 0)
					{
						j->second->Release();
					}
				}
				i->second.clear();
			}

			FontToLayoutMap.clear();
#endif

			BufferedStrings.Empty();

			m_CurrentD2DRT->EndDraw();
		}
		else
#endif
		if (m_BufferedVerts != 0 && m_CurrentBuff != BT_None)
		{
			// Metallicafan212:	Update textures now
			if (bWriteTexturesBuffer)
			{
				UpdateBoundTextures();
			}

			// Metallicafan212:	We only lock once to save on performance 
			UnlockBuffers();
			m_RenderContext->DrawIndexed(m_BufferedIndices, m_DrawnIndices, m_DrawnVerts);

			// Metallicafan212:	Increment the buffer counters
			m_DrawnIndices	+= m_BufferedIndices;
			m_DrawnVerts	+= m_BufferedVerts;
			// Metallicafan212:	Init the values
			m_BufferedIndices	= 0;
			m_BufferedVerts		= 0;
		}
	}

	// Metallicafan212:	Start and end buffering (for different types)
	FORCEINLINE void StartBuffering(EBuffType inBuff)
	{
		// Metallicafan212:	Check if we need to draw
		if (inBuff != m_CurrentBuff)
		{
			EndBuffering();

			switch (inBuff)
			{
				case BT_Lines:
				{
					m_RenderContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
					break;
				}

				default:
				{
					// Metallicafan212:	Only set if actually needed to
					if (m_CurrentBuff == BT_Lines || m_CurrentBuff == BT_None)
					{
						m_RenderContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
					}
				}
			}

			m_CurrentBuff = inBuff;
		}
	}

	// Metallicafan212:	Update the global constant buffer in the shaders
	FORCEINLINE void UpdateGlobalShaderVars()
	{
#if UPDATESUBRESOURCE_CONSTANTS
		// Metallicafan212:	Update this using update subresource
		m_RenderContext->UpdateSubresource(FrameConstantsBuffer, 0, nullptr, &FrameShaderVars, 0, 0);
#else
		D3D11_MAPPED_SUBRESOURCE Map;

		m_RenderContext->Map(FrameConstantsBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &Map);

		appMemcpy(Map.pData, &FrameShaderVars, sizeof(FFrameShaderVars));

		m_RenderContext->Unmap(FrameConstantsBuffer, 0);
#endif
	}

	FORCEINLINE void UpdatePolyflagsVars()
	{
#if UPDATESUBRESOURCE_CONSTANTS
		// Metallicafan212:	Update this using update subresource
		m_RenderContext->UpdateSubresource(GlobalPolyflagsBuffer, 0, nullptr, &GlobalPolyflagVars, 0, 0);
#else
		D3D11_MAPPED_SUBRESOURCE Map;

		m_RenderContext->Map(GlobalPolyflagsBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &Map);

		appMemcpy(Map.pData, &GlobalPolyflagVars, sizeof(FPolyflagVars));

		m_RenderContext->Unmap(GlobalPolyflagsBuffer, 0);
#endif
	}

	// Metallicafan212:	TODO!!!!! Maybe find a better way to not have to loop?
	//					We could just have the texture set code set this instead, and then 
	FORCEINLINE void UpdateBoundTextures()
	{
		/*
		BoundTexturesInfo.CurrentBoundTextures = 0;
		for (INT i = 0; i < MAX_TEXTURES; i++)
		{
			BoundTexturesInfo.CurrentBoundTextures |= BoundTextures[i].Mask;
		}
		*/

#if UPDATESUBRESOURCE_CONSTANTS
		// Metallicafan212:	Update this using update subresource
		m_RenderContext->UpdateSubresource(BoundTexturesBuffer, 0, nullptr, &BoundTexturesInfo, 0, 0);
#else
		D3D11_MAPPED_SUBRESOURCE Map;

		m_RenderContext->Map(BoundTexturesBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &Map);

		appMemcpy(Map.pData, &BoundTexturesInfo, sizeof(FBoundTextures));

		m_RenderContext->Unmap(BoundTexturesBuffer, 0);

		bWriteTexturesBuffer = 0;
#endif
	}

	// Metallicafan212:	From the DX9 driver since I'm a fucking lazy bastard
	static QSORT_RETURN CDECL CompareRes(const FPlane* A, const FPlane* B)
	{
		return (QSORT_RETURN)(((A->X - B->X) != 0.0f) ? (A->X - B->X) : (A->Y - B->Y));
	}

	// Metallicafan212:	Texture conversion/uploading function
	//typedef void (UICBINDx11RenderDevice::* UploadFunc)(void* Source, SIZE_T SourceLength, FD3DTexture* Tex, INT Mip, INT UpdateX, INT UpdateY, INT UpdateW, INT UpdateH, FColor* Palette);
	void DirectCP(const FTextureInfo& Info, FD3DTexture* Tex, INT Mip, UBOOL bPartial, INT UpdateX, INT UpdateY, INT UpdateW, INT UpdateH);
	void P8ToRGBA(const FTextureInfo& Info, FD3DTexture* tex, INT Mip, UBOOL bPartial, INT UpdateX, INT UpdateY, INT UpdateW, INT UpdateH);
	void RGBA7To8(const FTextureInfo& Info, FD3DTexture* tex, INT Mip, UBOOL bPartial, INT UpdateX, INT UpdateY, INT UpdateW, INT UpdateH);

	void RegisterTextureFormat(ETextureFormat Format, DXGI_FORMAT DXFormat, UBOOL bRequiresConversion, UBOOL bIsCompressed = 0, INT ByteOrBlockSize = 4, FD3DTexType::GetTypePitch PitchFunc = &FD3DTexType::RawPitch, UploadFunc UFunc = &DirectCP);//, FD3DTexType::ConversionFunc UConv = nullptr);

	// Metallicafan212:	Texture setting code
	void SetTexture(INT TexNum, const FTextureInfo* Info, PFLAG PolyFlags, UBOOL bNoAF = 0);

	FD3DTexture* CacheTextureInfo(const FTextureInfo& Info, PFLAG PolyFlags, UBOOL bJustSampler = 0);

	//void MakeTextureSampler(FD3DTexture* Bind, PFLAG PolyFlags);

	FORCEINLINE void FlushTextureSamplers()
	{
#if USE_SAMPLER_ARRAY
		for (INT i = 0; i < MAX_SAMPLERS; i++)
		{
			SAFE_RELEASE(SampArray[i]);
		}
#else

#if !USE_UNODERED_MAP_EVERYWHERE
		for (TMap<FPLAG, ID3D11SamplerState*>::TIterator It(SampMap); It; ++It)
		{
				It.Value()->Release();
		}

		SampMap.Empty();
#else
		for (auto i = SampMap.begin(); i != SampMap.end(); i++)
		{
			i->second->Release();
	}

		SampMap.clear();
#endif
#endif

		SAFE_RELEASE(ScreenSamp);
	}

	FORCEINLINE void FlushRasterStates()
	{
#if USE_RASTER_ARRAY
		for (INT i = 0; i < RASTER_ARRAY_SIZE; i++)
		{
			SAFE_RELEASE(RasterArray[i]);
		}
#else
#if !USE_UNODERED_MAP_EVERYWHERE
		for (TMap<DWORD, ID3D11RasterizerState*>::TIterator It(RasterMap); It; ++It)
		{
			It.Value()->Release();
		}

		RasterMap.Empty();
#else
		for (auto i = RasterMap.begin(); i != RasterMap.end(); i++)
		{
			i->second->Release();
		}

		RasterMap.clear();
#endif
#endif

		
	}

	void SetRasterState(DWORD State);

	FORCEINLINE ID3D11SamplerState* GetSamplerState(PFLAG PolyFlags, INT MinMip, UBOOL bForceNoAf = 0)
	{
		guardSlow(UICBINDx11RenderDevice::GetSamplerState);

		checkSlow(MinMip <= MAX_MIP_BIAS);

#if USE_SAMPLER_ARRAY
		// Metallicafan212:	Figure out where the area of the array should be
		INT BaseIndex = 0;
		switch (PolyFlags & (PF_NoSmooth | PF_ClampUVs))
		{
			case PF_NoSmooth:
			{
				BaseIndex = MAX_SAMPLERS / 4;
				break;
			}

			case PF_ClampUVs:
			{
				BaseIndex = 2 * (MAX_SAMPLERS / 4);
				break;
			}

			case (PF_NoSmooth | PF_ClampUVs):
			{
				BaseIndex = 3 * (MAX_SAMPLERS / 4);
				break;
			}
		}

		/*
		// Metallicafan212:	TODO! See if this is quicker
		//					This isn't quicker... I think the lookup table is probably quicker
		INT BaseIndex = (((PolyFlags & PF_NoSmooth) == PF_NoSmooth) * 0x1) + (((PolyFlags & PF_ClampUVs) == PF_ClampUVs) * 0x2);
		BaseIndex *= (MAX_SAMPLERS / 4);
		*/

		// Metallicafan212:	Now offset that by the AF and the MinMip
		//					The MinMip is a "set" of noAF on and off samplers
		BaseIndex += (MinMip * 2) + (bForceNoAf & 0x1);

		ID3D11SamplerState* S = SampArray[BaseIndex];
#else
		// Metallicafan212:	TODO! This has a performance bottleneck....
		
		// Metallicafan212:	We have to use global sampler, since per-texture won't ever work... It'll force all of the same texture to no smooth (for example)

		// Metallicafan212:	Shift the mipbias by 4 to bitpack this. It'll max be like 2 anyways, so a bitshift of 4 is 32
		//					MinMip is (usually) always 0, so I'm not worried. This leaves the rest for polyflags
		// Metallicafan212:	Trust that the polyflags coming in are valid....
		QWORD Key = ((PolyFlags & (PF_NoSmooth | PF_ClampUVs)) << 4) + MinMip + (bForceNoAf << 8);

#if !USE_UNODERED_MAP_EVERYWHERE
		ID3D11SamplerState* S = SampMap.FindRef(Key);
#else
		auto f = SampMap.find(Key);
		ID3D11SamplerState* S = f != SampMap.end() ? f->second : nullptr;
#endif
#endif

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
			SDesc.AddressW			= SDesc.AddressU;
			SDesc.MinLOD			= MinMip;
			SDesc.MaxLOD			= D3D11_FLOAT32_MAX;
			SDesc.MipLODBias		= 0.0f;
			SDesc.MaxAnisotropy		= (PolyFlags & PF_NoSmooth) || bForceNoAf ? 1 : NumAFSamples;
			SDesc.ComparisonFunc	= D3D11_COMPARISON_NEVER;

			HRESULT hr = m_D3DDevice->CreateSamplerState(&SDesc, &S);

			ThrowIfFailed(hr);

#if USE_SAMPLER_ARRAY
			SampArray[BaseIndex] = S;
#else
			// Metallicafan212:	Now set
#if !USE_UNODERED_MAP_EVERYWHERE
			SampMap.Set(Key, S);
#else
			SampMap[Key] = S;
#endif
#endif
		}

		return S;

		unguardSlow;
	}

	FORCEINLINE ID3D11BlendState* GetBlendState(PFLAG PolyFlag)
	{
#if !USE_UNODERED_MAP_EVERYWHERE
		ID3D11BlendState* bState = BlendMap.FindRef(PolyFlag);
#else
		auto f = BlendMap.find(PolyFlag);

		ID3D11BlendState* bState = f != BlendMap.end() ? f->second : nullptr;
#endif

		return bState;
	}

	FORCEINLINE ID3D11BlendState* CreateBlend(PFLAG PolyFlag, D3D11_BLEND SrcBlend, D3D11_BLEND DstBlend,
		UINT8 RTWrite = D3D11_COLOR_WRITE_ENABLE_ALL, BOOL bEnableBlending = 1, BOOL bAlphaToCov = 0, D3D11_BLEND_OP BldOp = D3D11_BLEND_OP_ADD, D3D11_BLEND_OP BldOpAlh = D3D11_BLEND_OP_ADD,
		D3D11_BLEND SrcBlendAlpha = D3D11_BLEND_ONE, D3D11_BLEND DstBlendAlpha = D3D11_BLEND_ZERO)
	{
#if !USE_UNODERED_MAP_EVERYWHERE
		ID3D11BlendState* bState = BlendMap.FindRef(PolyFlag);
#else
		auto f = BlendMap.find(PolyFlag);

		ID3D11BlendState* bState = f != BlendMap.end() ? f->second : nullptr;
#endif

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
#if !USE_UNODERED_MAP_EVERYWHERE
			BlendMap.Set(PolyFlag, bState);
#else
			BlendMap[PolyFlag] = bState;
#endif
		}

		return bState;

		// Metallicafan212:	Set the blend state
		//m_RenderContext->OMSetBlendState(bState, nullptr, 0xffffffff);
	}

	// Metallicafan212:	Blend state
	void SetBlend(PFLAG PolyFlags);

	// Metallicafan212:	For detecting the hit after the scene is rendered
	void DetectPixelHit();

	// Metallicafan212:	For setting up the shit for pixel hitting
	void SetupPixelHitTest();

	// Metallicafan212:	Generate the semi-permanent resources needed for the device
	void SetupResources();

	// Metallicafan212:	Autodetect the nits white balance level for the screen
	UBOOL AutodetectWhiteBalance();

	// Metallicafan212:	Setup the device
	void SetupDevice();

	// Metallicafan212:	Initialize the shaders (if needed)
	//					This was moved to a separate function so we could hold off until after we have the GPU adapter information
	void InitShaders();

	// Metallicafan212:	Set the projection matrix
	void SetProjectionStateNoCheck(UBOOL bRequestingNearRangeHack, UBOOL bForceUpdate = 0);

	// Metallicafan212:	Add on config properties
	void AddBoolProp(const TCHAR* Name, UBOOL& InParam, ECppProperty CPP, INT Offset, UBOOL bDefaultVal);
	void AddFloatProp(const TCHAR* Name, FLOAT& InParam, ECppProperty CPP, INT Offset, FLOAT fDefaultVal);
	void AddIntProp(const TCHAR* Name, INT& InParam, ECppProperty CPP, INT Offset, INT iDefaultVal);
	void AddColorProp(const TCHAR* Name, FColor& InParam, ECppProperty CPP, INT Offset, FColor iDefaultVal);
	void AddByteProp(const TCHAR* Name, BYTE& InParam, ECppProperty CPP, INT Offset, BYTE bDefaultVal, UEnum* InEnum = nullptr);

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

	virtual void DrawTile(FSceneNode* Frame, FTextureInfo& Info, FLOAT X, FLOAT Y, FLOAT XL, FLOAT YL, FLOAT U, FLOAT V, FLOAT UL, FLOAT VL, FSpanBuffer* Span, FLOAT Z, FPlane Color, FPlane Fog, PFLAG PolyFlags);

	// Metallicafan212:	Was a 227 specific extension, now just used in the driver code to control the Z buffer in general
	virtual BYTE SetZTestMode(BYTE Mode);

	// Metallicafan212:	Game specific rendering functions
	//					Most may follow the UT specifications, and I might just ifdef SupportsTextureFormat
#if DX11_HP2
	// Metallicafan212:	TODO! More particle related code
	virtual INT MaxVertices() { return 1024; };

	virtual void DrawTriangles(FSceneNode* Frame, FTextureInfo& Info, FTransTexture** Pts, INT NumPts, _WORD* Indices, INT NumIndices, PFLAG PolyFlags, FSpanBuffer* Span);

	virtual void DrawComplexSurface(FSceneNode* Frame, FSurfaceInfo & Surface, FSurfaceFacet & Facet, PFLAG PolyFlags, FLOAT Alpha);

	virtual void DrawRotatedTile(FSceneNode* Frame, FTextureInfo& Info, FLOAT X, FLOAT Y, FLOAT XL, FLOAT YL, FLOAT U, FLOAT V, FLOAT UL, FLOAT VL, FSpanBuffer* Span, FLOAT Z, FPlane Color, FPlane Fog, PFLAG PolyFlags, FCoords InCoords = GMath.UnitCoords);

	virtual INT DrawString(PFLAG Flags, UFont* Font, INT& DrawX, INT& DrawY, FLOAT ClipX, FLOAT ClipY, FLOAT ClipW, FLOAT ClipH, const TCHAR* Text, const FPlane& Color, UBOOL bHandleApersand = 0, FLOAT Scale = 1.0f);
#elif DX11_UT_469	
	
	virtual void DrawComplexSurface(FSceneNode* Frame, FSurfaceInfo& Surface, FSurfaceFacet& Facet);
	
	virtual void DrawGouraudPolygon(FSceneNode* Frame, FTextureInfo& Info, FTransTexture** Pts, int NumPts, PFLAG PolyFlags, FSpanBuffer* Span);
	
	virtual UBOOL SupportsTextureFormat(ETextureFormat Format);
	
	virtual void DrawGouraudTriangles(const FSceneNode* Frame, const FTextureInfo& Info, FTransTexture* const Pts, INT NumPts, PFLAG PolyFlags, DWORD DataFlags, FSpanBuffer* Span);

	// Metallicafan212:	Support partial uploads
	virtual void UpdateTextureRect(FTextureInfo& Info, INT U, INT V, INT UL, INT VL);

	virtual void DrawTileList(const FSceneNode* Frame, const FTextureInfo& Info, const FTileRect* Tiles, INT NumTiles, FSpanBuffer* Span, FLOAT Z, FPlane Color, FPlane Fog, DWORD PolyFlags);
#elif DX11_UNREAL_227
	virtual void DrawGouraudPolyList(FSceneNode* Frame, FTextureInfo& Info, FTransTexture* Pts, INT NumPts, PFLAG PolyFlags, FSpanBuffer* Span);

	virtual void DrawGouraudPolygon(FSceneNode* Frame, FTextureInfo& Info, FTransTexture** Pts, int NumPts, PFLAG PolyFlags, FSpanBuffer* Span);

	virtual void DrawComplexSurface(FSceneNode* Frame, FSurfaceInfo& Surface, FSurfaceFacet& Facet);

	UBOOL PushRenderToTexture(FRenderToTexture* Tex);

	void PopRenderToTexture();

	void PreDrawGouraud(FSceneNode* Frame, FFogSurf& FogSurf);
	void PostDrawGouraud(FSceneNode* Frame, FFogSurf& FogSurf);
#else
	// Metallicafan212:	Base defs for most UE1 games, may have to be overrided
	virtual void DrawComplexSurface(FSceneNode* Frame, FSurfaceInfo& Surface, FSurfaceFacet& Facet);

	virtual void DrawGouraudPolygon(FSceneNode* Frame, FTextureInfo& Info, FTransTexture** Pts, int NumPts, PFLAG PolyFlags, FSpanBuffer* Span);
#endif

	virtual void Draw3DLine(FSceneNode* Frame, FPlane Color, DWORD LineFlags, FVector OrigP, FVector OrigQ);

	virtual void Draw2DLine(FSceneNode* Frame, FPlane Color, DWORD LineFlags, FVector P1, FVector P2);

	virtual void Draw2DPoint(FSceneNode* Frame, FPlane Color, DWORD LineFlags, FLOAT X1, FLOAT Y1, FLOAT X2, FLOAT Y2, FLOAT Z);

	virtual void ClearZ(FSceneNode* Frame);

	virtual void PushHit(const BYTE* Data, INT Count);

	virtual void PopHit(INT Count, UBOOL bForce);

	virtual void GetStats(TCHAR* Result);

#if DX11_UNREAL_227
	virtual void ReadPixels(FColor* Pixels, UBOOL bGammaCorrectOutput);
#else
	virtual void ReadPixels(FColor* Pixels);
#endif

	virtual void EndFlash();

	virtual void DrawStats(FSceneNode* Frame);

	virtual void SetSceneNode(FSceneNode* Frame);

	virtual void PrecacheTexture(FTextureInfo& Info, PFLAG PolyFlags);

#if DX11_DISTANCE_FOG

	// Metallicafan212:	Viewer-based zone fog
	virtual void SetDistanceFog(UBOOL Enable, FLOAT FogStart, FLOAT FogEnd, FPlane Color, FLOAT FadeRate);

	// Metallicafan212:	Tick the current fog values
	//					We use this to fade the values from one setting to another
	void TickDistanceFog();

	FORCEINLINE void UpdateFogSettings()
	{
		// Metallicafan212:	Now set the vars
		GlobalDistFogSettings.DistanceFogColor		= FogShaderVars.DistanceFogColor;
		GlobalDistFogSettings.DistanceFogSettings	= FogShaderVars.DistanceFogSettings;
		GlobalDistFogSettings.bDistanceFogEnabled	= !FogShaderVars.bForceFogOff && (FogShaderVars.bDoDistanceFog || FogShaderVars.bFadeFogValues);

#if UPDATESUBRESOURCE_CONSTANTS
		// Metallicafan212:	Update it using UpdateSubresource
		m_RenderContext->UpdateSubresource(GlobalDistFogBuffer, 0, nullptr, &GlobalDistFogSettings, 0, 0);
#else
		D3D11_MAPPED_SUBRESOURCE Map;

		m_RenderContext->Map(GlobalDistFogBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &Map);

		//*((FDistFogVars*)Map.pData) = GlobalDistFogSettings;
		appMemcpy(Map.pData, &GlobalDistFogSettings, sizeof(FDistFogVars));

		m_RenderContext->Unmap(GlobalDistFogBuffer, 0);
#endif
	}

	// Metallicafan212:	Push on a distance fog struct
	virtual void PushDistanceFogState()
	{
		guard(UICBINDx11RenderDevice::PushDistanceFogState);

		DistanceFogStack.AddItem(FogShaderVars);

		//FogShaderVars = FFogShaderVars();
		appMemzero(&FogShaderVars, sizeof(FFogShaderVars));

		// Metallicafan212:	Now update
		UpdateFogSettings();

		unguard;
	}

	virtual void PopDistanceFogState()
	{
		guard(UICBINDx11RenderDevice::PopDistanceFogState);

		if (DistanceFogStack.Num())
		{
			// Metallicafan212:	Grab the end and set it as the current value
			FFogShaderVars* Temp = &DistanceFogStack(DistanceFogStack.Num() - 1);
			appMemcpy(&FogShaderVars, Temp, sizeof(FFogShaderVars));

			// Metallicafan212:	Now remove
			DistanceFogStack.Remove(DistanceFogStack.Num() - 1);
		}

		// Metallicafan212:	Update
		UpdateFogSettings();

		unguard;
	}

	virtual INT GetDistanceFogStackCount()
	{
		return DistanceFogStack.Num();
	}

	// Metallicafan212:	Force the current fog color to a specific value
	void ForceSetFogColor(FPlane FogColor);

	// Metallicafan212:	Simple BW shader for doing stupid "old-time" looking shit
	virtual void SetBWPercent(FLOAT Percent);

#endif


	// Metallicafan212:	New RT stuff
	//					Create an RT texture, this is so the texture can be used in normal stuff
	virtual UTexture* CreateRenderTargetTexture(INT W, INT H, UBOOL bCreateDepth = 1);

	virtual void SetRenderTargetTexture(UTexture* Tex);

	virtual void ClearRenderTargetTexture(UTexture* Tex, FPlane ClearColor);

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

	// Metallicafan212:	New buffered drawing state
	void ExecuteBufferedDraws();

	/*
	FORCEINLINE FDrawCall* AddDrawCall()
	{
#if DO_BUFFERED_DRAWS
		FDrawCall* Draw = new FDrawCall();

		BufferedDraws.AddItem(Draw);

		return Draw;
#else
		return nullptr;
#endif
	}

	FORCEINLINE void CheckDrawCall()
	{
#if DO_BUFFERED_DRAWS
		if (CurrentDraw == nullptr)
		{
			CurrentDraw = AddDrawCall();
		}
#endif
	}
	*/
};