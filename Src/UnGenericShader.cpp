#include "ICBINDx11Drv.h"

// Metallicafan212:	This file defines the generic (no texture) rendering shader
FD3DGenericShader::FD3DGenericShader(UICBINDx11RenderDevice* InParent)
	: FD3DShader(InParent)
{
	guard(FD3DGenericShader::FD3DGenericShader);

	// Metallicafan212:	TODO! Read the code from file, or write it out automatically
	VertexFile		= SHADER_FOLDER TEXT("General\\GeneralShader_Vert.hlsl");
	PixelFile		= SHADER_FOLDER TEXT("General\\GeneralShader_PX.hlsl");
	VertexFunc		= TEXT("VertShader");
	PixelFunc		= TEXT("PxShader");

	Init();

	unguard;
}