#include "D3D11Drv.h"

struct FSurfMatrixDef : FShaderVarCommon
{
	FPlane	XAxis, YAxis;
	//FPlane LPanScale, MPanScale, FPanScale;
	FPlane	PanScale[5];
	FPlane	LightandFogScale;
	FLOAT	SurfAlpha;
	UBOOL	bDrawOpacity;

	// Metallicafan212:	Keep it aligned along register bounds
	FLOAT	Pad2[2];
	//FLOAT	Pad3[3];
};

// Metallicafan212:	This file defines the mesh (one texture???) rendering shader
FD3DSurfShader::FD3DSurfShader(UD3D11RenderDevice* InParent)
	: FD3DShader(InParent)
{
	guard(FD3DSurfShader::FD3DSurfShader);

	// Metallicafan212:	TODO! Read the code from file, or write it out automatically
	VertexFile		= SHADER_FOLDER TEXT("ComplexSurfShader.hlsl");
	PixelFile		= SHADER_FOLDER TEXT("ComplexSurfShader.hlsl");
	VertexFunc		= TEXT("VertShader");
	PixelFunc		= TEXT("PxShader");

	Init();

	unguard;
}

// Metallicafan212:	Shader interface
void FD3DSurfShader::Init()
{
	guard(FD3DSurfShader::Init);

	FD3DShader::Init();

	unguard;
}

void FD3DSurfShader::Bind()
{
	guard(FD3DSurfShader::Bind);

	FD3DShader::Bind();

	unguard;
}

// Metallicafan212:	Made this generic so we can copy vars into shaders
void FD3DSurfShader::SetupConstantBuffer()
{
	guard(FD3DSurfShader::SetupConstantBuffer);

	// Metallicafan212:	Matrix layout
	//					TODO! Ask the child shader for this?
	D3D11_BUFFER_DESC MatrixDesc = { sizeof(FSurfMatrixDef), D3D11_USAGE_DYNAMIC, D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE, 0, 0 };

	HRESULT hr = ParentDevice->m_D3DDevice->CreateBuffer(&MatrixDesc, nullptr, &ShaderConstantsBuffer);

	// Metallicafan212:	IDK, do something here?
	ParentDevice->ThrowIfFailed(hr);

	unguard;
}

void FD3DSurfShader::WriteConstantBuffer(void* InMem)
{
	guard(FD3DSurfShader::WriteConstantBuffer);

	// Metallicafan212:	Copy over
	FD3DShader::WriteConstantBuffer(InMem);

	// Metallicafan212:	Now the rest of the data
	FSurfMatrixDef* SDef	= (FSurfMatrixDef*)InMem;

	FLOAT UDot				= ParentDevice->SurfCoords.XAxis | ParentDevice->SurfCoords.Origin;
	FLOAT VDot				= ParentDevice->SurfCoords.YAxis | ParentDevice->SurfCoords.Origin;
	SDef->XAxis				= FPlane(ParentDevice->SurfCoords.XAxis, UDot);
	SDef->YAxis				= FPlane(ParentDevice->SurfCoords.YAxis, VDot);
	SDef->SurfAlpha			= SurfAlpha;
	SDef->bDrawOpacity		= bSurfInvisible;


	// Metallicafan212:	Now the pan info
	for (INT i = 0; i < 5; i++)
	{
		// Metallicafan212:	Copy the pan and scale info
		if (ParentDevice->BoundTextures[i].TexInfo != nullptr)
		{
			SDef->PanScale[i] = FPlane(ParentDevice->BoundTextures[i].UPan, ParentDevice->BoundTextures[i].VPan, ParentDevice->BoundTextures[i].TexInfo->UMult, ParentDevice->BoundTextures[i].TexInfo->VMult);
		}
	}

	// Metallicafan212:	And lastly the original lightmap scale
	if (ParentDevice->BoundTextures[1].TexInfo != nullptr)
	{
		SDef->LightandFogScale.X	= ParentDevice->BoundTextures[1].TexInfo->UScale;
		SDef->LightandFogScale.Y	= ParentDevice->BoundTextures[1].TexInfo->VScale;
	}

	// Metallicafan212:	And the fog scale
	if (ParentDevice->BoundTextures[3].TexInfo != nullptr)
	{
		SDef->LightandFogScale.Z = ParentDevice->BoundTextures[3].TexInfo->UScale;
		SDef->LightandFogScale.W = ParentDevice->BoundTextures[3].TexInfo->VScale;
	}

	unguard;
}