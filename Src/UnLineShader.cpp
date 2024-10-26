#include "ICBINDx11Drv.h"

// Metallicafan212:	This file defines the line shader, with optional line size
FD3DLineShader::FD3DLineShader(UICBINDx11RenderDevice* InParent)
	: FD3DShader(InParent)
{
	guard(FD3DLineShader::FD3DLineShader);

	// Metallicafan212:	TODO! Read the code from file, or write it out automatically
	VertexFile	= SHADER_FOLDER TEXT("Line\\LineShader_Vert.hlsl");
	PixelFile	= SHADER_FOLDER TEXT("Line\\LineShader_PX.hlsl");
	VertexFunc	= TEXT("VertShader");
	PixelFunc	= TEXT("PxShader");

	// Metallicafan212:	TODO! Add in a local detection routine for wine/proton
	//					I know it's not "correct" to detect when the app is running on it, but there just seems to be removed features from the HLSL compiler....
	//					This would be solved if I precompiled the shaders, but that would defeat the purpose of defining them this way
#if 1//DX11_HP2
	if (ParentDevice->bUseGeoShaders)//!GWineAndDine)
#endif
	{
		GeoFile = SHADER_FOLDER TEXT("Line\\Line_GeoShader.hlsl");
		GeoFunc = TEXT("GeoShader");

		Macros.Insert(0);
		Macros(0) = {"USE_GEO_SHADER", "1"};
	}
#if 1//DX11_HP2
	else
	{
		Macros.Insert(0);
		Macros(0) = { "USE_GEO_SHADER", "0" };
	}
#endif

	Init();

	unguard;
}
