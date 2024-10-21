#include "ICBINDx11Drv.h"

struct FMSAAVars : FShaderVarCommon
{
	// Metallicafan212:	The info we use for this specific shader	
	FLOAT	FilterSize;
	FLOAT	GaussianSigma;
	FLOAT	CubicB;
	FLOAT 	CubicC;
	INT		FilterType;
	FLOAT	Pad3[3];
	FPlane 	SampleOffsets[8];
};

// Metallicafan212:	This file defines the MSAA resolving shader
FD3DMSAAShader::FD3DMSAAShader(UICBINDx11RenderDevice* InParent)
#if USE_MSAA_COMPUTE
	: FD3DComputeShader(InParent)//FD3DShader(InParent)
#else
	: FD3DShader(InParent)
#endif
{
	guard(FD3DMSAAShader::FD3DMSAAShader);

	// Metallicafan212:	TODO! Read the code from file, or write it out automatically
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

	Init();

	unguard;
}

// Metallicafan212:	Shader interface
void FD3DMSAAShader::SetupConstantBuffer()
{
	guard(FD3DMSAAShader::SetupConstantBuffer);

	// Metallicafan212:	Matrix layout
	//					TODO! Ask the child shader for this?
	D3D11_BUFFER_DESC MatrixDesc = { sizeof(FMSAAVars), D3D11_USAGE_DYNAMIC, D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE, 0, 0 };

	HRESULT hr = ParentDevice->m_D3DDevice->CreateBuffer(&MatrixDesc, nullptr, &ShaderConstantsBuffer);

	// Metallicafan212:	IDK, do something here?
	ParentDevice->ThrowIfFailed(hr);

	unguard;
}

UBOOL FD3DMSAAShader::WriteConstantBuffer(void* InMem)
{
	guardSlow(FD3DMSAAShader::WriteConstantBuffer);

	// Metallicafan212:	Copy over
#if USE_MSAA_COMPUTE
	FD3DComputeShader::WriteConstantBuffer(InMem);
#else
	FD3DShader::WriteConstantBuffer(InMem);
#endif

	// Metallicafan212:	Now the rest of the data
	FMSAAVars* SDef = (FMSAAVars*)InMem;

	return 0;

	/*
	// Metallicafan212:	Set each var
	//					TODO! Cache this info!!!!
	SDef->FilterSize		= ParentDevice->MSAAFilterSize;
	SDef->GaussianSigma		= ParentDevice->MSAAGaussianSigma;
	SDef->CubicB			= ParentDevice->MSAACubicB;
	SDef->CubicC			= ParentDevice->MSAACubicC;
	SDef->FilterType		= ParentDevice->MSAAFilterType;

	// Metallicafan212:	Copy the gamma over
	//SDef->Gamma				= ParentDevice->Gamma;

	// Metallicafan212:	Sample offsets
	switch (ParentDevice->NumAASamples)
	{
		case 8:
		{
			SDef->SampleOffsets[0] = FPlane(0.580f, 0.298f, 0.0f, 0.0f);
			SDef->SampleOffsets[1] = FPlane(0.419f, 0.698f, 0.0f, 0.0f);
			SDef->SampleOffsets[2] = FPlane(0.819f, 0.580f, 0.0f, 0.0f);
			SDef->SampleOffsets[3] = FPlane(0.298f, 0.180f, 0.0f, 0.0f);
			SDef->SampleOffsets[4] = FPlane(0.180f, 0.819f, 0.0f, 0.0f);
			SDef->SampleOffsets[5] = FPlane(0.058f, 0.419f, 0.0f, 0.0f);
			SDef->SampleOffsets[6] = FPlane(0.698f, 0.941f, 0.0f, 0.0f);
			SDef->SampleOffsets[7] = FPlane(0.941f, 0.058f, 0.0f, 0.0f);
			break;
		}

		case 4:
		{
			SDef->SampleOffsets[0] = FPlane(0.380f, 0.141f, 0.0f, 0.0f);
			SDef->SampleOffsets[1] = FPlane(0.859f, 0.380f, 0.0f, 0.0f);
			SDef->SampleOffsets[2] = FPlane(0.141f, 0.620f, 0.0f, 0.0f);
			SDef->SampleOffsets[3] = FPlane(0.619f, 0.859f, 0.0f, 0.0f);
			break;
		}
		case 2:
		{
			SDef->SampleOffsets[0] = FPlane(0.741f, 0.741f, 0.0f, 0.0f);
			SDef->SampleOffsets[1] = FPlane(0.258f, 0.258f, 0.0f, 0.0f);
			break;
		}
	}

	for (INT i = 0; i < ParentDevice->NumAASamples; i++)
	{
		SDef->SampleOffsets[i] -= FPlane(0.5f, 0.5f, 0.0f, 0.0f);
	}
	*/

	unguardSlow;
}