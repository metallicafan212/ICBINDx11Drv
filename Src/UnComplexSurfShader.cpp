#include "ICBINDx11Drv.h"

#if !EXTRA_VERT_INFO
struct FSurfMatrixDef : FShaderVarCommon
{
#if !EXTRA_VERT_INFO
	FPlane	XAxis, YAxis;
	FPlane	PanScale[5];
	FPlane	LightandFogScale;
	//FLOAT	SurfAlpha;
	//UBOOL	bDrawOpacity;

	// Metallicafan212:	Keep it aligned along register bounds
	//FLOAT	Pad2[2];
#endif
};
#endif

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

// Metallicafan212:	Shader interface

#if !EXTRA_VERT_INFO
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
	guardSlow(FD3DSurfShader::WriteConstantBuffer);

	// Metallicafan212:	Copy over
	FD3DShader::WriteConstantBuffer(InMem);

#if !EXTRA_VERT_INFO
	// Metallicafan212:	Now the rest of the data
	FSurfMatrixDef* SDef	= (FSurfMatrixDef*)InMem;

	FLOAT UDot				= ParentDevice->SurfCoords.XAxis | ParentDevice->SurfCoords.Origin;
	FLOAT VDot				= ParentDevice->SurfCoords.YAxis | ParentDevice->SurfCoords.Origin;
	SDef->XAxis				= FPlane(ParentDevice->SurfCoords.XAxis, UDot);
	SDef->YAxis				= FPlane(ParentDevice->SurfCoords.YAxis, VDot);
	//SDef->SurfAlpha			= SurfAlpha;
	//SDef->bDrawOpacity		= bSurfInvisible;


	// Metallicafan212:	Now the pan info
	for (INT i = 0; i < 5; i++)
	{
		// Metallicafan212:	Copy the pan and scale info
		if (ParentDevice->BoundTextures[i].TexInfoHash != 0)
		{
			SDef->PanScale[i] = FPlane(ParentDevice->BoundTextures[i].UPan, ParentDevice->BoundTextures[i].VPan, ParentDevice->BoundTextures[i].UMult, ParentDevice->BoundTextures[i].VMult);
		}
	}

	// Metallicafan212:	And lastly the original lightmap scale
	if (ParentDevice->BoundTextures[1].TexInfoHash != 0)
	{
		SDef->LightandFogScale.X	= ParentDevice->BoundTextures[1].UScale * 0.5f;
		SDef->LightandFogScale.Y	= ParentDevice->BoundTextures[1].VScale * 0.5f;
	}

	// Metallicafan212:	And the fog scale
	if (ParentDevice->BoundTextures[3].TexInfoHash != 0)
	{
		SDef->LightandFogScale.Z = ParentDevice->BoundTextures[3].UScale * 0.5f;
		SDef->LightandFogScale.W = ParentDevice->BoundTextures[3].VScale * 0.5f;
	}
#endif

	unguardSlow;
}
#endif