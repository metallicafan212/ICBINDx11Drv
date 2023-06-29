#include "ICBINDx11Drv.h"

struct FLineMatrixDef : FShaderVarCommon
{
	// Metallicafan212:	Viewport X and Y
	FLOAT	View[2];

	// Metallicafan212:	Keep it aligned along register bounds
	FLOAT	Pad3[2];
};

// Metallicafan212:	This file defines the line shader, with optional line size
FD3DLineShader::FD3DLineShader(UICBINDx11RenderDevice* InParent)
	: FD3DShader(InParent)
{
	guard(FD3DLineShader::FD3DLineShader);

	// Metallicafan212:	TODO! Read the code from file, or write it out automatically
	VertexFile	= SHADER_FOLDER TEXT("LineShader.hlsl");
	PixelFile	= SHADER_FOLDER TEXT("LineShader.hlsl");
	VertexFunc	= TEXT("VertShader");
	PixelFunc	= TEXT("PxShader");
	GeoFile		= SHADER_FOLDER TEXT("LineShader.hlsl");
	GeoFunc		= TEXT("GeoShader");

	Init();

	unguard;
}

// Metallicafan212:	Shader interface
void FD3DLineShader::Init()
{
	guard(FD3DLineShader::Init);

	FD3DShader::Init();

	unguard;
}

void FD3DLineShader::Bind()
{
	guard(FD3DLineShader::Bind);

	FD3DShader::Bind();

	unguard;
}

void FD3DLineShader::SetupConstantBuffer()
{
	guard(FD3DLineShader::SetupConstantBuffer);

	// Metallicafan212:	Declare the constant buffer with a few more entries
	D3D11_BUFFER_DESC MatrixDesc = { sizeof(FLineMatrixDef), D3D11_USAGE_DYNAMIC, D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE, 0, 0 };

	HRESULT hr = ParentDevice->m_D3DDevice->CreateBuffer(&MatrixDesc, nullptr, &ShaderConstantsBuffer);

	// Metallicafan212:	IDK, do something here?
	ParentDevice->ThrowIfFailed(hr);

	unguard;
}

void FD3DLineShader::WriteConstantBuffer(void* InMem)
{
	guard(FD3DLineShader::WriteConstantBuffer);

	// Metallicafan212:	Get the parent info first
	FD3DShader::WriteConstantBuffer(InMem);

	// Metallicafan212:	Now write the line info
	FLineMatrixDef* Def = (FLineMatrixDef*)InMem;

	Def->View[0]		= ParentDevice->ScaledSceneNodeX;//ParentDevice->m_sceneNodeX;
	Def->View[1]		= ParentDevice->ScaledSceneNodeY;//ParentDevice->m_sceneNodeY;

	unguard;
}

