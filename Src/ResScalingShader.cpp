#include "ICBINDx11Drv.h"

// Metallicafan212:	This file defines the generic (no texture) rendering shader
FD3DResScalingShader::FD3DResScalingShader(UICBINDx11RenderDevice* InParent)
	: FD3DShader(InParent)
{
	guard(FD3DResScalingShader::FD3DResScalingShader);

	VertexFile	= SHADER_FOLDER TEXT("PostFX\\ResScaling_Vert.hlsl");
	PixelFile	= SHADER_FOLDER TEXT("PostFX\\ResScaling_PX.hlsl");
	VertexFunc	= TEXT("VertShader");
	PixelFunc	= TEXT("PxShader");

	Init();

	unguard;
}