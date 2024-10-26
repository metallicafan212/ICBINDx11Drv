#include "ICBINDx11Drv.h"

// Metallicafan212:	This file defines the mesh (one texture???) rendering shader
FD3DMeshShader::FD3DMeshShader(UICBINDx11RenderDevice* InParent)
	: FD3DShader(InParent),
	Constants(FMeshMatrixDef())
{
	guard(FD3DMeshShader::FD3DMeshShader);

	bNoMeshOpacity		= 0;
	bPrevMeshOpacity	= 0;

	// Metallicafan212:	TODO! Read the code from file, or write it out automatically
	VertexFile			= SHADER_FOLDER TEXT("Mesh\\MeshShader_Vert.hlsl");
	PixelFile			= SHADER_FOLDER TEXT("Mesh\\MeshShader_PX.hlsl");
	VertexFunc			= TEXT("VertShader");
	PixelFunc			= TEXT("PxShader");

	// Metallicafan212:	Setup the pointer
	ShaderConstantsMem	= &Constants;

	Init();

	unguard;
}

// Metallicafan212:	Shader interface
void FD3DMeshShader::SetupConstantBuffer()
{
	guard(FD3DMeshShader::SetupConstantBuffer);

#if UPDATESUBRESOURCE_CONSTANTS
	// Metallicafan212:	Declare the constant buffer with a few more entries
	D3D11_BUFFER_DESC MatrixDesc = { sizeof(FMeshMatrixDef), D3D11_USAGE_DEFAULT, D3D11_BIND_CONSTANT_BUFFER, 0, 0, 0 };
#else
	// Metallicafan212:	Declare the constant buffer with a few more entries
	D3D11_BUFFER_DESC MatrixDesc = { sizeof(FMeshMatrixDef), D3D11_USAGE_DYNAMIC, D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE, 0, 0 };
#endif

	HRESULT hr = ParentDevice->m_D3DDevice->CreateBuffer(&MatrixDesc, nullptr, &ShaderConstantsBuffer);

	// Metallicafan212:	IDK, do something here?
	ParentDevice->ThrowIfFailed(hr);

	unguard;
}

UBOOL FD3DMeshShader::WriteConstantBuffer(void* InMem)
{
	guardSlow(FD3DMeshShader::WriteConstantBuffer);

	UBOOL bShouldUpdate = 0;

	if (bPrevMeshOpacity != bNoMeshOpacity)
	{
		// Metallicafan212:	Flush the device
		ParentDevice->EndBuffering();

		bShouldUpdate = 1;
	}

	bPrevMeshOpacity = bNoMeshOpacity;

	// Metallicafan212:	Get the parent info first
	FD3DShader::WriteConstantBuffer(InMem);

#if UPDATESUBRESOURCE_CONSTANTS
	Constants.bNoOpacity = bNoMeshOpacity;
#else
	// Metallicafan212:	Now write the rotation stuff
	FMeshMatrixDef* Def = (FMeshMatrixDef*)InMem;

	Def->bNoOpacity = bNoMeshOpacity;
#endif

	return bShouldUpdate;

	unguardSlow;
}