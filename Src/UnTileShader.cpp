#include "ICBINDx11Drv.h"

// Metallicafan212:	This file defines the tile rendering shader
FD3DTileShader::FD3DTileShader(UICBINDx11RenderDevice* InParent)
	: FD3DShader(InParent),
	bDoTileRotation(0),
	TileCoords(GMath.UnitCoords),
	//bDoMSAAFontHack(0),
	Constants(FTileMatrixDef())
{
	guard(FD3DTileShader::FD3DTileShader);
	
	// Metallicafan212:	TODO! Read the code from file, or write it out automatically
	VertexFile				= SHADER_FOLDER TEXT("Tile\\TileShader_Vert.hlsl");
	PixelFile				= SHADER_FOLDER TEXT("Tile\\TileShader_PX.hlsl");
	VertexFunc				= TEXT("VertShader");
	PixelFunc				= TEXT("PxShader");
	ShaderConstantsMem		= &Constants;

	Init();

	unguard;
}

// Metallicafan212:	Shader interface
void FD3DTileShader::SetupConstantBuffer()
{
	guard(FD3DTileShader::SetupConstantBuffer);

#if UPDATESUBRESOURCE_CONSTANTS
	// Metallicafan212:	Declare the constant buffer with a few more entries
	D3D11_BUFFER_DESC MatrixDesc	= { sizeof(FTileMatrixDef), D3D11_USAGE_DEFAULT, D3D11_BIND_CONSTANT_BUFFER, 0, 0, 0 };
#else
	// Metallicafan212:	Declare the constant buffer with a few more entries
	D3D11_BUFFER_DESC MatrixDesc	= { sizeof(FTileMatrixDef), D3D11_USAGE_DYNAMIC, D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE, 0, 0 };
#endif

	HRESULT hr						= ParentDevice->m_D3DDevice->CreateBuffer(&MatrixDesc, nullptr, &ShaderConstantsBuffer);

	// Metallicafan212:	IDK, do something here?
	ParentDevice->ThrowIfFailed(hr);

	unguard;
}

UBOOL FD3DTileShader::WriteConstantBuffer(void* InMem)
{
	guardSlow(FD3DTileShader::WriteConstantBuffer);

	// Metallicafan212:	Get the parent info first
	FD3DShader::WriteConstantBuffer(InMem);

#if UPDATESUBRESOURCE_CONSTANTS
	// Metallicafan212:	I'm lazy, don't even bother to rewrite the code lol
	FTileMatrixDef* Def = &Constants;
#else
	// Metallicafan212:	Now write the rotation stuff
	FTileMatrixDef* Def = (FTileMatrixDef*)InMem;
#endif

	// Metallicafan212:	Only care about tile rotation?
	UBOOL bDoUpdate = 0;

	if (bDoTileRotation != Def->bDoRot)
	{
		bDoUpdate = 1;
	}

	Def->bDoRot				= bDoTileRotation;
	Def->bIsMSAAFontTile	= 0;//bDoMSAAFontHack;

	// Metallicafan212:	Only copy if needed
	if (bDoTileRotation)
	{
		Def->XAxis			= TileCoords.XAxis;
		Def->YAxis			= TileCoords.YAxis;
		Def->ZAxis			= TileCoords.ZAxis;

		// Metallicafan212:	Always update when rotating
		bDoUpdate			= 1;
	}

	return bDoUpdate;

	unguardSlow;
}