#include "ICBINDx11Drv.h"

struct FResScaleVars : FShaderVarCommon
{
	// Metallicafan212:	The info we use for this specific shader	
	FLOAT	ResolutionScale;
	//FLOAT	Gamma;
	FLOAT	Pad3[3];
};

// Metallicafan212:	This file defines the generic (no texture) rendering shader
FD3DResScalingShader::FD3DResScalingShader(UICBINDx11RenderDevice* InParent)
#if USE_RES_COMPUTE
	: FD3DComputeShader(InParent)
#else
	: FD3DShader(InParent)
#endif
{
	guard(FD3DResScalingShader::FD3DResScalingShader);

	// Metallicafan212:	TODO! Read the code from file, or write it out automatically
#if USE_RES_COMPUTE
	ComputeFile = SHADER_FOLDER TEXT("ResScaling.hlsl");
	ComputeFunc = TEXT("CSMain");
#else
	VertexFile	= SHADER_FOLDER TEXT("PostFX\\ResScaling_Vert.hlsl");
	PixelFile	= SHADER_FOLDER TEXT("PostFX\\ResScaling_PX.hlsl");
	VertexFunc	= TEXT("VertShader");
	PixelFunc	= TEXT("PxShader");
#endif

	Init();

	unguard;
}

/*
// Metallicafan212:	Shader interface
void FD3DResScalingShader::SetupConstantBuffer()
{
	guard(FD3DResScalingShader::SetupConstantBuffer);

	// Metallicafan212:	Matrix layout
	//					TODO! Ask the child shader for this?
	D3D11_BUFFER_DESC MatrixDesc = { sizeof(FResScaleVars), D3D11_USAGE_DYNAMIC, D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE, 0, 0 };

	HRESULT hr = ParentDevice->m_D3DDevice->CreateBuffer(&MatrixDesc, nullptr, &ShaderConstantsBuffer);

	// Metallicafan212:	IDK, do something here?
	ParentDevice->ThrowIfFailed(hr);

	unguard;
}

void FD3DResScalingShader::WriteConstantBuffer(void* InMem)
{
	guardSlow(FD3DResScalingShader::WriteConstantBuffer);

	// Metallicafan212:	Copy over
#if USE_RES_COMPUTE
	FD3DComputeShader::WriteConstantBuffer(InMem);
#else
	FD3DShader::WriteConstantBuffer(InMem);
#endif

	// Metallicafan212:	Now the rest of the data
	FResScaleVars* SDef		= (FResScaleVars*)InMem;

	SDef->ResolutionScale	= ParentDevice->ResolutionScale;
	//SDef->Gamma				= ParentDevice->Gamma;

	unguardSlow;
}
*/