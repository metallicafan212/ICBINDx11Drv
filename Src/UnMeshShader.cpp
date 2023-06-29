#include "ICBINDx11Drv.h"

// Metallicafan212:	This file defines the mesh (one texture???) rendering shader
FD3DMeshShader::FD3DMeshShader(UICBINDx11RenderDevice* InParent)
	: FD3DShader(InParent)
{
	guard(FD3DMeshShader::FD3DMeshShader);

	// Metallicafan212:	TODO! Read the code from file, or write it out automatically
	VertexFile	= SHADER_FOLDER TEXT("MeshShader.hlsl");
	PixelFile	= SHADER_FOLDER TEXT("MeshShader.hlsl");
	VertexFunc	= TEXT("VertShader");
	PixelFunc	= TEXT("PxShader");

	Init();

	unguard;
}

// Metallicafan212:	Shader interface
void FD3DMeshShader::Init()
{
	guard(FD3DMeshShader::Init);

	FD3DShader::Init();

	unguard;
}

void FD3DMeshShader::Bind()
{
	guard(FD3DMeshShader::Bind);

	FD3DShader::Bind();

	unguard;
}