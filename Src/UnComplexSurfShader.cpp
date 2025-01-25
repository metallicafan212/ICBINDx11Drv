#include "ICBINDx11Drv.h"

// Metallicafan212:	This file defines the mesh (one texture???) rendering shader
FD3DSurfShader::FD3DSurfShader(UICBINDx11RenderDevice* InParent)
	: FD3DShader(InParent)
{
	guard(FD3DSurfShader::FD3DSurfShader);

	// Metallicafan212:	TODO! Read the code from file, or write it out automatically
	VertexFile		= SHADER_FOLDER TEXT("Complex\\ComplexSurfShader_Vert.hlsl");
	PixelFile		= SHADER_FOLDER TEXT("Complex\\ComplexSurfShader_PX.hlsl");
	VertexFunc		= TEXT("VertShader");
	PixelFunc		= TEXT("PxShader");

	Init();

	unguard;
}