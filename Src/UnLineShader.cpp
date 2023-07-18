#include "ICBINDx11Drv.h"

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
