#include "ICBINDx11Drv.h"

// Metallicafan212:	This file defines the generic (no texture) rendering shader
FD3DGenericShader::FD3DGenericShader(UICBINDx11RenderDevice* InParent)
	: FD3DShader(InParent)
{
	guard(FD3DGenericShader::FD3DGenericShader);

	// Metallicafan212:	TODO! Read the code from file, or write it out automatically
	VertexFile		= SHADER_FOLDER TEXT("GeneralShader.hlsl");
	PixelFile		= SHADER_FOLDER TEXT("GeneralShader.hlsl");
	VertexFunc		= TEXT("VertShader");
	PixelFunc		= TEXT("PxShader");

	Init();

	unguard;
}

// Metallicafan212:	Shader interface
void FD3DGenericShader::Init()
{
	guard(FD3DGenericShader::Init);

	FD3DShader::Init();

	unguard;
}

void FD3DGenericShader::Bind()
{
	guard(FD3DGenericShader::Bind);

	FD3DShader::Bind();

	unguard;
}