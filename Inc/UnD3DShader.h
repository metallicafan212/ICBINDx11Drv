#pragma once

#define USE_MSAA_COMPUTE 1
#define USE_RES_COMPUTE 0

#define FIRST_USER_CONSTBUFF 4
#define MACRO_STR(mac) #mac
#define MACRO_TO_STR(mac) MACRO_STR(mac)
#define FIRST_USER_CONSTBUFF_STR MACRO_TO_STR(FIRST_USER_CONSTBUFF)

// Metallicafan212:	Shader folder
//					Some people may not want to have shaders next to Textures, Sounds, etc.
//					So, this allows you to redefine the path
#ifndef SHADER_FOLDER
#define SHADER_FOLDER TEXT("..\\Shaders\\")
#endif

// Metallicafan212:	Common draw vars that should only be uploaded once per frame (if we can get away with it, that is)
//					These are GLOBAL to the render device, not per-shader
//					The renderer will deal with updating them and writing them out
struct FFrameShaderVars
{
	DirectX::XMMATRIX	Proj;
	FLOAT				Gamma;
	FLOAT				ViewX;
	FLOAT				ViewY;
	UBOOL				bDoSelection;

	// Metallicafan212:	TODO! More settings!!!
	UBOOL				bOneXLightmaps;
	UBOOL				bEnableCorrectFogging;

	UBOOL				bHDR;

	// Metallicafan212:	User-chosen gamma mode (corresponds to the EGammaMode enum)
	INT					GammaMode;

	// Metallicafan212:	sRGB to HDR expansion value
	FLOAT				HDRExpansion;

	// Metallicafan212:	Resolution scale value
	FLOAT				ResolutionScale;

	// Metallicafan212:	Display white level
	FLOAT				WhiteLevel;

	// Metallicafan212:	If to output depth as the color, instead of the normal color
	//UBOOL				bDepthDraw;
	// Metallicafan212:	Current render map
	INT					RendMap;

	// Metallicafan212:	Configured Z range for the depth draw mode
	FLOAT				DepthZRange;

	// Metallicafan212:	DX9 mode gamma correction variables
	FLOAT				GammaOffsetRed;
	FLOAT				GammaOffsetGreen;
	FLOAT				GammaOffsetBlue;

	// Metallicafan212:	Additional padding
	//FLOAT				Pad[3];

	FFrameShaderVars()
	{
		appMemzero(this, sizeof(FFrameShaderVars));
	}
};

// Metallicafan212:	Just a buffer for fog settings
//					Only used in HP2
struct FDistFogVars
{
	FPlane	DistanceFogColor;
	FPlane	DistanceFogSettings;
	UBOOL	bDistanceFogEnabled;
	FLOAT	Pad[3];
};

struct FPolyflagVars
{
	UBOOL	bSelected;
	FLOAT	AlphaReject;
	FLOAT	BWPercent;

	// Metallicafan212:	If alpha is currently enabled
	UBOOL	bAlphaEnabled;

	// Metallicafan212:	Temp hack for modulation until I recode gamma again....
	UBOOL	bModulated;

	// Metallicafan212:	Selection color (no alpha)
	FVector	SelectionColor;

	FPolyflagVars()
		:
		bSelected(0),
		AlphaReject(0.0f),//1e-6f),
		BWPercent(0.0f),
		bAlphaEnabled(0),
		bModulated(0),
		SelectionColor(0.f, 0.f, 0.f)
	{

	}
};

// Metallicafan212:	Common shader variables
struct FShaderVarCommon
{
	// Metallicafan212:	What textures are currently bound
	//UBOOL	BoundTextures[MAX_TEXTURES];
};

struct FBoundTextures
{
	// Metallicafan212:	What textures are currently bound
	UBOOL	BoundTextures[MAX_TEXTURES];
};

#define D3D_CMP_STD_INC ((ID3DInclude*)(UINT_PTR)1)

#if DX11_HP2
#define GET_MACRO_PTR(M) M.GetData()
#else
#define GET_MACRO_PTR(M) (D3D_SHADER_MACRO*)M.GetData()
#endif

// Metallicafan212:	Very basic shader definition
class FD3DShader
{
public:
	// Metallicafan212:	Global backwards pointer to the device (so we can access all variables if needed)
	class UICBINDx11RenderDevice* ParentDevice;

	// Metallicafan212:	Global stuff for it
	ID3D11PixelShader*			PixelShader;
	ID3D11VertexShader*			VertexShader;
	ID3D11GeometryShader*		GeoShader;

	// Metallicafan212:	Vertex input definition
	ID3D11InputLayout*			InputLayout;

	D3D11_INPUT_ELEMENT_DESC*	InputDesc;

	// Metallicafan212:	Count of elements in this input description
	INT							InputCount;

	// Metallicafan212:	Matrix buffer input
	ID3D11Buffer*				ShaderConstantsBuffer;

	// Metallicafan212:	String representing the path to this shader
	FString						VertexFile;
	FString						PixelFile;
	FString						GeoFile;

	// Metallicafan212:	The vertex shader function to find (on compile)
	FString						VertexFunc;

	// Metallicafan212:	The pixel shader function to find (on compile)
	FString						PixelFunc;

	// Metallicafan212:	The geometry shader function to find (on compile). OPTIONAL!!!
	FString						GeoFunc;

	// Metallicafan212:	Macro array
	TArray<D3D_SHADER_MACRO>	Macros;

	FD3DShader() :
		ParentDevice(nullptr),
		PixelShader(nullptr),
		VertexShader(nullptr),
		InputLayout(nullptr),
		ShaderConstantsBuffer(nullptr),
		GeoShader(nullptr)
	{
		InputDesc	= FBasicInLayout;
		InputCount	= ARRAY_COUNT(FBasicInLayout);

		// Metallicafan212:	There must be a null terminated value here
		Macros.AddZeroed();
	}

	// Metallicafan212:	Constructor that inits the device pointer
	FD3DShader(class UICBINDx11RenderDevice* InParent) :
		ParentDevice(InParent),
		PixelShader(nullptr),
		VertexShader(nullptr),
		InputLayout(nullptr),
		ShaderConstantsBuffer(nullptr),
		GeoShader(nullptr)
	{
		InputDesc	= FBasicInLayout;
		InputCount	= ARRAY_COUNT(FBasicInLayout);

		// Metallicafan212:	Add the wine definition
#if 1//DX11_HP2
		if (GWineAndDine)
		{
			Macros.AddItem({ "WINE", "1" });
		}
		else
#endif
		{
			Macros.AddItem({ "WINE", "0" });
		}

		Macros.AddItem({ "FIRST_USER_CONSTBUFF", "b" FIRST_USER_CONSTBUFF_STR});

#if EXTRA_VERT_INFO
		Macros.AddItem({"EXTRA_VERT_INFO", "1"});
#if !COMPLEX_SURF_MANUAL_UVs
		Macros.AddItem({"COMPLEX_SURF_MANUAL_UVs", "0"});
#else
		Macros.AddItem({"COMPLEX_SURF_MANUAL_UVs", "1"});
#endif
#else
		Macros.AddItem({"EXTRA_VERT_INFO", "0"});
#endif

		// Metallicafan212:	Add the gamma modes
#define DECLARE_ENUM
#define DECLARE_GM(name, val)Macros.AddItem({#name, #val});
#include "GammaModes.h"

		// Metallicafan212:	HP2 render modes
#if DX11_HP2
		Macros.AddItem({ "RENMAPS", "1" });
#undef BEGIN_REND_ENUM
#undef END_REND_ENUM
#undef DECLARE_RENDMODE
#define BEGIN_REND_ENUM
#define END_REND_ENUM
#define DECLARE_RENDMODE(name, val)Macros.AddItem({#name, #val});
#include "UnRenderTypes.h"
#undef DECLARE_RENDMODE
#undef BEGIN_REND_ENUM
#undef END_REND_ENUM
#endif

		// Metallicafan212:	There must be a null terminated value here
		Macros.AddZeroed();
	}

	virtual ~FD3DShader()
	{
		SAFE_RELEASE(PixelShader);
		SAFE_RELEASE(VertexShader);
		SAFE_RELEASE(InputLayout);
		SAFE_RELEASE(ShaderConstantsBuffer);
		SAFE_RELEASE(GeoShader);
	}

	// Metallicafan212:	TEST! For speed reasons, override this to just check the address!!!
	bool operator==(const FD3DShader* Other)
	{
		return ((SIZE_T)Other) == ((SIZE_T)this);
	}

	//bool operator==(const FD3DShader& Other)
	//{
	//	return ((SIZE_T)&Other) == ((SIZE_T)this);
	//}

	bool operator!=(const FD3DShader* Other)
	{
		return ((SIZE_T)Other) != ((SIZE_T)this);
	}

	//bool operator!=(const FD3DShader& Other)
	//{
	//	return ((SIZE_T)&Other) != ((SIZE_T)this);
	//}

	// Metallicafan212:	TODO! Shader interface?
	virtual void Init();

	virtual void Bind(ID3D11DeviceContext* UseContext);

	// Metallicafan212:	Actually set the shader(s) in the context
	virtual void SetShaders(ID3D11DeviceContext* UseContext);

	// Metallicafan212:	Made this generic so we can copy vars into shaders
	virtual void SetupConstantBuffer();

	virtual void WriteConstantBuffer(void* InMem);

	//virtual void UpdateConstantBuffer();
};

// Metallicafan212:	Compute shader interface
//					The only reason we're extending from FD3DShader is to be able to be tracked in the same ptr lists
class FD3DComputeShader : public FD3DShader
{
public:
	// Metallicafan212:	Compute shader information
	ID3D11ComputeShader*		ComputeShader;
	FString						ComputeFile;
	FString						ComputeFunc;

	FD3DComputeShader() :
		FD3DShader()
	{
	}

	virtual ~FD3DComputeShader()
	{
		SAFE_RELEASE(ComputeShader);
	}

	// Metallicafan212:	Constructor that inits the device pointer
	FD3DComputeShader(class UICBINDx11RenderDevice* InParent);

	virtual void Init();

	virtual void Bind(ID3D11DeviceContext* UseContext);

};

#if USE_COMPUTE_SHADER

#define LIGHT_MAX	100
// Metallicafan212:	TODO! Do something about this????
#define VERT_MAX	65535

class FD3DLghtMshCompShader : public FD3DComputeShader
{
public:

	SIZE_T						NumVertsMax;

	ID3D11Buffer*				LightBuffer;
	ID3D11Buffer*				VertBuffer;

	ID3D11ShaderResourceView*	LightShaderView;
	ID3D11UnorderedAccessView*	VertUnorderedView;

	FD3DLghtMshCompShader()
		: FD3DComputeShader()
	{
		ComputeFunc = TEXT("CSMain");
		ComputeFile = SHADER_FOLDER TEXT("MeshTandLCompute.hlsl");
		NumVertsMax = VERT_MAX;
	}

	~FD3DLghtMshCompShader()
	{
		SAFE_RELEASE(LightBuffer);
		SAFE_RELEASE(VertBuffer);
		SAFE_RELEASE(LightShaderView);
		SAFE_RELEASE(VertUnorderedView);
	}

	// Metallicafan212:	Device pointer version
	FD3DLghtMshCompShader(class UICBINDx11RenderDevice* InParent);

	virtual void Init();

	virtual void Bind();
};
#endif

// Metallicafan212:	Tile shader (for rendering normal and rotated tiles)
class FD3DTileShader : public FD3DShader
{
public:

	UBOOL	bDoTileRotation;
	UBOOL	bDoMSAAFontHack;
	FCoords TileCoords;

	FD3DTileShader() :
		FD3DShader(),
		bDoTileRotation(0),
		TileCoords(),
		bDoMSAAFontHack(0)
	{
		//ShaderFile	= TEXT("..\\Shaders\\TileShader.hlsl");
		VertexFile	= SHADER_FOLDER TEXT("TileShader.hlsl");
		PixelFile	= SHADER_FOLDER TEXT("TileShader.hlsl");
		//GeoFile		= TEXT("..\\Shaders\\TileShader.hlsl");
		VertexFunc	= TEXT("VertShader");
		PixelFunc	= TEXT("PxShader");
	}

	// Metallicafan212:	Constructor that inits the device pointer
	FD3DTileShader(class UICBINDx11RenderDevice* InParent);

	// Metallicafan212:	Shader interface
	virtual void SetupConstantBuffer();

	virtual void WriteConstantBuffer(void* InMem);
};

// Metallicafan212:	Generic shader (for now)
class FD3DGenericShader : public FD3DShader
{
public:
	FD3DGenericShader() :
		FD3DShader()
	{
		//ShaderFile	= TEXT("..\\Shaders\\TileShader.hlsl");
		VertexFile	= SHADER_FOLDER TEXT("GeneralShader.hlsl");
		PixelFile	= SHADER_FOLDER TEXT("GeneralShader.hlsl");
		VertexFunc	= TEXT("VertShader");
		PixelFunc	= TEXT("PxShader");
	}

	// Metallicafan212:	Constructor that inits the device pointer
	FD3DGenericShader(class UICBINDx11RenderDevice* InParent);

	// Metallicafan212:	Shader interface
};

#define INSTANCED_LINES 0

// Metallicafan212:	Line shader with a geo shader for line thiccness
class FD3DLineShader : public FD3DShader
{
public:
	FD3DLineShader() :
		FD3DShader()
	{
		VertexFile	= SHADER_FOLDER TEXT("LineShader.hlsl");
		PixelFile	= SHADER_FOLDER TEXT("LineShader.hlsl");
		VertexFunc	= TEXT("VertShader");
		PixelFunc	= TEXT("PxShader");
		GeoFile		= SHADER_FOLDER TEXT("LineShader.hlsl");
		GeoFunc		= TEXT("GeoShader");
	}

	// Metallicafan212:	Constructor that inits the device pointer
	FD3DLineShader(class UICBINDx11RenderDevice* InParent);

	//virtual void SetupConstantBuffer();

	//virtual void WriteConstantBuffer(void* InMem);
};

// Metallicafan212:	Mesh shader (with texturing!)
class FD3DMeshShader : public FD3DShader
{
public:

	UBOOL bNoMeshOpacity;
	UBOOL bPrevMeshOpacity;

	FD3DMeshShader() :
		FD3DShader()
	{
		VertexFile			= SHADER_FOLDER TEXT("MeshShader.hlsl");
		PixelFile			= SHADER_FOLDER TEXT("MeshShader.hlsl");
		VertexFunc			= TEXT("VertShader");
		PixelFunc			= TEXT("PxShader");
		bNoMeshOpacity		= 0;
		bPrevMeshOpacity	= 0;
	}

	// Metallicafan212:	Constructor that inits the device pointer
	FD3DMeshShader(class UICBINDx11RenderDevice* InParent);

	// Metallicafan212:	Shader interface
	virtual void SetupConstantBuffer();

	virtual void WriteConstantBuffer(void* InMem);
};

class FD3DSurfShader : public FD3DShader
{
public:

	// Metallicafan212:	The alpha of the surface
	//FLOAT				SurfAlpha;

	// Metallicafan212:	If the surface we're rendering is invisible
	//UBOOL				bSurfInvisible;

	FD3DSurfShader() :
		FD3DShader()
	{
		VertexFile		= SHADER_FOLDER TEXT("ComplexSurfShader.hlsl");
		PixelFile		= SHADER_FOLDER TEXT("ComplexSurfShader.hlsl");
		VertexFunc		= TEXT("VertShader");
		PixelFunc		= TEXT("PxShader");
	}

	// Metallicafan212:	Constructor that inits the device pointer
	FD3DSurfShader(class UICBINDx11RenderDevice* InParent);

#if !EXTRA_VERT_INFO
	virtual void SetupConstantBuffer();

	virtual void WriteConstantBuffer(void* InMem);
#endif
};




// Metallicafan212:	Shader to scale up/down the final output (so we can have super resolution)
class FD3DResScalingShader 
#if USE_RES_COMPUTE
	: public FD3DComputeShader
#else
	: public FD3DShader
#endif
{
public:
	FD3DResScalingShader() :
#if USE_RES_COMPUTE
		FD3DComputeShader()
#else
		FD3DShader()
#endif
	{
#if USE_RES_COMPUTE
		ComputeFile = SHADER_FOLDER TEXT("ResScaling.hlsl");
		ComputeFunc = TEXT("CSMain");
#else
		VertexFile	= SHADER_FOLDER TEXT("ResScaling.hlsl");
		PixelFile	= SHADER_FOLDER TEXT("ResScaling.hlsl");
		VertexFunc	= TEXT("VertShader");
		PixelFunc	= TEXT("PxShader");
#endif
	}

	// Metallicafan212:	Constructor that inits the device pointer
	FD3DResScalingShader(class UICBINDx11RenderDevice* InParent);

	//virtual void SetupConstantBuffer();

	//virtual void WriteConstantBuffer(void* InMem);
};

// Metallicafan212:	MSAA resolving shader
class FD3DMSAAShader 
#if USE_MSAA_COMPUTE
	: public FD3DComputeShader
#else
	: public FD3DShader
#endif
{
public:
	FD3DMSAAShader() :
#if USE_MSAA_COMPUTE
	FD3DComputeShader()//FD3DShader
#else
	FD3DShader()
#endif
	{
		//ShaderFile	= TEXT("..\\Shaders\\TileShader.hlsl");
		//VertexFile	= SHADER_FOLDER TEXT("MSAAResolve.hlsl");
		//PixelFile	= SHADER_FOLDER TEXT("MSAAResolve.hlsl");
		//VertexFunc	= TEXT("VertShader");
		//PixelFunc	= TEXT("PxShader");
#if USE_MSAA_COMPUTE
		ComputeFile = SHADER_FOLDER TEXT("MSAAResolve.hlsl");
		ComputeFunc = TEXT("CSMain");
#else
		VertexFile	= SHADER_FOLDER TEXT("MSAAResolve.hlsl");
		PixelFile	= SHADER_FOLDER TEXT("MSAAResolve.hlsl");
		VertexFunc	= TEXT("VertShader");
		PixelFunc	= TEXT("PxShader");
#endif
	}

	// Metallicafan212:	Constructor that inits the device pointer
	FD3DMSAAShader(class UICBINDx11RenderDevice* InParent);

	virtual void SetupConstantBuffer();

	virtual void WriteConstantBuffer(void* InMem);
};

// Metallicafan212:	P8 to RGBA8 compute shader
class FD3DP8ToRGBAShader : public FD3DComputeShader
{
public:

	INT USize, VSize;

	FD3DP8ToRGBAShader() : FD3DComputeShader()
	{
		ComputeFile = SHADER_FOLDER TEXT("P8ToRGBA.hlsl");
		ComputeFunc = TEXT("CSMain");
	}

	// Metallicafan212:	Constructor that inits the device pointer
	FD3DP8ToRGBAShader(class UICBINDx11RenderDevice* InParent);

	virtual void SetupConstantBuffer();

	virtual void WriteConstantBuffer(void* InMem);
};