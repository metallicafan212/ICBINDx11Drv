#include "ICBINDx11Drv.h"

struct FTileMatrixDef : FShaderVarCommon
{
	FPlane	XAxis;
	FPlane	YAxis;
	FPlane	ZAxis;
	// Metallicafan212:	Sigh.... This is a pain in the ass, so I have to pad it here
	//					HLSL wants to pad a float3x4 to 4 CONSTANTS for some reason
	UBOOL	bDoRot;

	// Metallicafan212:	If it's a MSAA font character tile
	UBOOL	bIsMSAAFontTile;

	// Metallicafan212:	Keep it aligned along register bounds
	FLOAT	Pad3[2];//[3];
};

// Metallicafan212:	This file defines the tile rendering shader
FD3DTileShader::FD3DTileShader(UICBINDx11RenderDevice* InParent)
	: FD3DShader(InParent),
	bDoTileRotation(0),
	TileCoords(GMath.UnitCoords),
	bDoMSAAFontHack(0)
{
	guard(FD3DTileShader::FD3DTileShader);
	
	// Metallicafan212:	TODO! Read the code from file, or write it out automatically
	VertexFile	= SHADER_FOLDER TEXT("TileShader.hlsl");
	PixelFile	= SHADER_FOLDER TEXT("TileShader.hlsl");
	VertexFunc	= TEXT("VertShader");
	PixelFunc	= TEXT("PxShader");

	Init();

	unguard;
}

// Metallicafan212:	Shader interface
void FD3DTileShader::SetupConstantBuffer()
{
	guard(FD3DTileShader::SetupConstantBuffer);

	// Metallicafan212:	Declare the constant buffer with a few more entries
	D3D11_BUFFER_DESC MatrixDesc	= { sizeof(FTileMatrixDef), D3D11_USAGE_DYNAMIC, D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE, 0, 0 };

	HRESULT hr						= ParentDevice->m_D3DDevice->CreateBuffer(&MatrixDesc, nullptr, &ShaderConstantsBuffer);

	// Metallicafan212:	IDK, do something here?
	ParentDevice->ThrowIfFailed(hr);

	unguard;
}

void FD3DTileShader::WriteConstantBuffer(void* InMem)
{
	guardSlow(FD3DTileShader::WriteConstantBuffer);

	// Metallicafan212:	Get the parent info first
	FD3DShader::WriteConstantBuffer(InMem);

	// Metallicafan212:	Now write the rotation stuff
	FTileMatrixDef* Def = (FTileMatrixDef*)InMem;

	Def->bDoRot				= bDoTileRotation;
	Def->bIsMSAAFontTile	= bDoMSAAFontHack;

	// Metallicafan212:	Only copy if needed
	if (bDoTileRotation)
	{
		//Def->TileCoords = TileCoords;
		//Def->Origin			= TileCoords.Origin;
		Def->XAxis			= TileCoords.XAxis;
		Def->YAxis			= TileCoords.YAxis;
		Def->ZAxis			= TileCoords.ZAxis;
	}

	unguardSlow;
}