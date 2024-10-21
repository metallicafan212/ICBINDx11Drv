#include "ICBINDx11Drv.h"

struct FP8Vars : FShaderVarCommon
{
	// Metallicafan212:	The info we use for this specific shader	
	INT		USize;
	INT		VSize;
	FLOAT	Pad1;
	FLOAT	Pad2;
};

// Metallicafan212:	This file defines the MSAA resolving shader
FD3DP8ToRGBAShader::FD3DP8ToRGBAShader(UICBINDx11RenderDevice* InParent): FD3DComputeShader(InParent)
{
	guard(FD3DP8ToRGBAShader::FD3DP8ToRGBAShader);

	ComputeFile = SHADER_FOLDER TEXT("P8ToRGBA.hlsl");
	ComputeFunc = TEXT("CSMain");

	Init();

	unguard;
}

// Metallicafan212:	Shader interface
void FD3DP8ToRGBAShader::SetupConstantBuffer()
{
	guard(FD3DP8ToRGBAShader::SetupConstantBuffer);

	// Metallicafan212:	Matrix layout
	//					TODO! Ask the child shader for this?
	D3D11_BUFFER_DESC MatrixDesc = { sizeof(FP8Vars), D3D11_USAGE_DYNAMIC, D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE, 0, 0 };

	HRESULT hr = ParentDevice->m_D3DDevice->CreateBuffer(&MatrixDesc, nullptr, &ShaderConstantsBuffer);

	// Metallicafan212:	IDK, do something here?
	ParentDevice->ThrowIfFailed(hr);

	unguard;
}

UBOOL FD3DP8ToRGBAShader::WriteConstantBuffer(void* InMem)
{
	guardSlow(FD3DP8ToRGBAShader::WriteConstantBuffer);

	// Metallicafan212:	Copy over
	//FD3DComputeShader::WriteConstantBuffer(InMem);

	// Metallicafan212:	Now the rest of the data
	FP8Vars* SDef	= (FP8Vars*)InMem;

	SDef->USize		= USize;
	SDef->VSize		= VSize;

	return 1;

	unguardSlow;
}