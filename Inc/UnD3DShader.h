#pragma once

#define USE_MSAA_COMPUTE 1
#define USE_RES_COMPUTE 0

// Metallicafan212:	Shader folder
//					Some people may not want to have shaders next to Textures, Sounds, etc.
//					So, this allows you to redefine the path
#ifndef SHADER_FOLDER
#define SHADER_FOLDER TEXT("..\\Shaders\\")
#endif

// Metallicafan212:	Common shader variables
struct FShaderVarCommon
{
	DirectX::XMMATRIX Proj;

	// Metallicafan212:	Other needed shader vars
	UBOOL	bColorMasked;
	UBOOL	bDistanceFogEnabled;
	UBOOL	bSelection;
	FLOAT	AlphaReject;
	FLOAT	BWPercent;

	// Metallicafan212:	If alpha is currently enabled
	UBOOL	bAlphaEnabled;

	// Metallicafan212:	If a texture is PF_NoSmooth AND have 1 mip level!!!
	//UBOOL	bNVTileHack;

	// Metallicafan212:	Pad it!!!!
	FLOAT	Pad[2];

	// Metallicafan212:	Now fog settings
	FPlane	DistanceFogColor;
	FPlane	DistanceFogSettings;

	// Metallicafan212:	TODO! Make this a #define
	UBOOL	BoundTextures[MAX_TEXTURES];
};

#define D3D_CMP_STD_INC ((ID3DInclude*)(UINT_PTR)1)

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


	// Metallicafan212:	TODO! Defined struct for vertex input per shader?
	//					IDK how I'm going to dynamically handle it.... Each thing is going to need to know what each input is I guess.....
	//					Or I go with a very generic input?

	/*
	// Metallicafan212:	TODO! Vertex definition for this shader????
	virtual D3D11_INPUT_ELEMENT_DESC* GetVertexDefinition() = 0;

	virtual D3D11_INPUT_ELEMENT_DESC* GetPixelDefinition() = 0;
	*/

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
	}

	virtual ~FD3DShader()
	{
		SAFE_RELEASE(PixelShader);
		SAFE_RELEASE(VertexShader);
		SAFE_RELEASE(InputLayout);
		SAFE_RELEASE(ShaderConstantsBuffer);
		SAFE_RELEASE(GeoShader);
	}

	// Metallicafan212:	TODO! Shader interface?
	virtual void Init();

	virtual void Bind();

	// Metallicafan212:	Made this generic so we can copy vars into shaders
	virtual void SetupConstantBuffer();

	virtual void WriteConstantBuffer(void* InMem);

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

	virtual void Bind();

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
		TileCoords()
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
	virtual void Init();
	virtual void Bind();

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
	virtual void Init();
	virtual void Bind();
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

	// Metallicafan212:	Shader interface
	virtual void Init();
	virtual void Bind();

	virtual void SetupConstantBuffer();

	virtual void WriteConstantBuffer(void* InMem);
};

// Metallicafan212:	Mesh shader (with texturing!)
class FD3DMeshShader : public FD3DShader
{
public:
	FD3DMeshShader() :
		FD3DShader()
	{
		VertexFile		= SHADER_FOLDER TEXT("MeshShader.hlsl");
		PixelFile		= SHADER_FOLDER TEXT("MeshShader.hlsl");
		VertexFunc		= TEXT("VertShader");
		PixelFunc		= TEXT("PxShader");
	}

	// Metallicafan212:	Constructor that inits the device pointer
	FD3DMeshShader(class UICBINDx11RenderDevice* InParent);

	// Metallicafan212:	Shader interface
	virtual void Init();
	virtual void Bind();
};

class FD3DSurfShader : public FD3DShader
{
public:

	// Metallicafan212:	The alpha of the surface
	FLOAT				SurfAlpha;

	// Metallicafan212:	If the surface we're rendering is invisible
	UBOOL				bSurfInvisible;

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

	// Metallicafan212:	Shader interface
	virtual void Init();
	virtual void Bind();

	virtual void SetupConstantBuffer();

	virtual void WriteConstantBuffer(void* InMem);
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

	// Metallicafan212:	Shader interface
	virtual void Init();
	virtual void Bind();

	virtual void SetupConstantBuffer();

	virtual void WriteConstantBuffer(void* InMem);
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

	// Metallicafan212:	Shader interface
	virtual void Init();
	virtual void Bind();

	virtual void SetupConstantBuffer();

	virtual void WriteConstantBuffer(void* InMem);
};