#include "D3D11Drv.h"

// Metallicafan212:	This file defines the generic (no texture) rendering shader
FD3DResScalingShader::FD3DResScalingShader(UD3D11RenderDevice* InParent)
	: FD3DShader(InParent)
{
	guard(FD3DResScalingShader::FD3DResScalingShader);

	// Metallicafan212:	TODO! Read the code from file, or write it out automatically
	VertexFile = SHADER_FOLDER TEXT("ResScaling.hlsl");
	PixelFile = SHADER_FOLDER TEXT("ResScaling.hlsl");
	VertexFunc = TEXT("VertShader");
	PixelFunc = TEXT("PxShader");

	Init();

	unguard;
}

// Metallicafan212:	Shader interface
void FD3DResScalingShader::Init()
{
	guard(FD3DResScalingShader::Init);

	FD3DShader::Init();

	unguard;
}

void FD3DResScalingShader::Bind()
{
	guard(FD3DResScalingShader::Bind);

	FD3DShader::Bind();

	unguard;
}