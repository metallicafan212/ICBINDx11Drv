#include "D3D11Drv.h"

D3D11_INPUT_ELEMENT_DESC FBasicInLayout[] =
{
	"POSITION",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 0,								D3D11_INPUT_PER_VERTEX_DATA, 0,
	"TEXCOORD",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0,
	"COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0,
	"COLOR",		1, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0
};

inline void CheckShader(HRESULT hr, ID3D10Blob* error)
{
	if (FAILED(hr))
	{
		// Metallicafan212:	Get the error
		if (error != nullptr)
		{
			SIZE_T ErrorSize = error->GetBufferSize();

			// Metallicafan212:	Now alloc memory and log
			ANSICHAR* ErrorStr = new ANSICHAR[ErrorSize]();

			if (ErrorStr != nullptr)
			{
				appMemcpy(ErrorStr, error->GetBufferPointer(), ErrorSize);

				//appUnwindf(TEXT("D3D11: Error compiling shader. Error is %s"), appFromAnsi(ErrorStr));
				appErrorf(TEXT("D3D11: Error compiling shader. Error is %s"), appFromAnsi(ErrorStr));
				delete[] ErrorStr;
			}
			error->Release();
		}
		// Metallicafan212:	Don't do anything else?
		return;
	}

	if (error != nullptr)
		error->Release();
}

// Metallicafan212:	Shader interface
void FD3DShader::Init()
{
	guard(FD3DShader::Init);

	// Metallicafan212:	Create the shader blobs (pixel, vertex), and the stream layout
	ID3D10Blob* error	= nullptr;
	ID3D10Blob* vsBuff	= nullptr;
	ID3D10Blob* psBuff	= nullptr;
	ID3D10Blob* gsBuff	= nullptr;

	UINT Flags = D3DCOMPILE_DEBUG;

	// Metallicafan212:	Compile the shaders
	HRESULT hr = D3DCompileFromFile(*VertexFile, nullptr, D3D_CMP_STD_INC, appToAnsi(*VertexFunc), "vs_5_0", Flags, 0, &vsBuff, &error);

	CheckShader(hr, error);

	// Metallicafan212:	Get it as a vertex shader
	hr = ParentDevice->m_D3DDevice->CreateVertexShader(vsBuff->GetBufferPointer(), vsBuff->GetBufferSize(), nullptr, &VertexShader);

	// Metallicafan212:	IDK, do something here?
	ParentDevice->ThrowIfFailed(hr);

	// Metallicafan212:	Now the pixel shader
	hr = D3DCompileFromFile(*PixelFile, nullptr, D3D_CMP_STD_INC, appToAnsi(*PixelFunc), "ps_5_0", Flags, 0, &psBuff, &error);

	CheckShader(hr, error);

	// Metallicafan212:	Get it as a pixel shader
	hr = ParentDevice->m_D3DDevice->CreatePixelShader(psBuff->GetBufferPointer(), psBuff->GetBufferSize(), nullptr, &PixelShader);

	// Metallicafan212:	IDK, do something here?
	ParentDevice->ThrowIfFailed(hr);

	// Metallicafan212:	Now the geometry shader
	if (GeoFunc.Len())
	{
		hr = D3DCompileFromFile(*GeoFile, nullptr, D3D_CMP_STD_INC, appToAnsi(*GeoFunc), "gs_5_0", Flags, 0, &gsBuff, &error);

		CheckShader(hr, error);

		// Metallicafan212:	Get it as a pixel shader
		hr = ParentDevice->m_D3DDevice->CreateGeometryShader(gsBuff->GetBufferPointer(), gsBuff->GetBufferSize(), nullptr, &GeoShader);

		// Metallicafan212:	IDK, do something here?
		ParentDevice->ThrowIfFailed(hr);
	}

	// Metallicafan212:	Make the input layout

	// Metallicafan212:	Now create the layout pointer
	hr = ParentDevice->m_D3DDevice->CreateInputLayout(InputDesc, InputCount, vsBuff->GetBufferPointer(), vsBuff->GetBufferSize(), &InputLayout);

	// Metallicafan212:	IDK, do something here?
	ParentDevice->ThrowIfFailed(hr);

	// Metallicafan212:	Allow child shaders to setup the constant buffer how they want
	//					TODO! The shared variables should be optimized so that they only upload once per frame/only when they change
	SetupConstantBuffer();


	SAFE_RELEASE(vsBuff);
	SAFE_RELEASE(psBuff);
	SAFE_RELEASE(gsBuff);

	unguard;
}

void FD3DShader::Bind()
{
	guard(FD3DShader::Bind);

	if (ParentDevice->CurrentShader != this)
		ParentDevice->EndBuffering();

	ParentDevice->CurrentShader = this;

	// Metallicafan212:	Setup this shader for rendering
	ParentDevice->m_D3DDeviceContext->VSSetShader(VertexShader, nullptr, 0);

	// Metallicafan212:	Bind the optional geometry shader
	ParentDevice->m_D3DDeviceContext->GSSetShader(GeoShader, nullptr, 0);

	ParentDevice->m_D3DDeviceContext->PSSetShader(PixelShader, nullptr, 0);


	ParentDevice->m_D3DDeviceContext->IASetInputLayout(InputLayout);

	// Metallicafan212:	Map the matrix(s)
	D3D11_MAPPED_SUBRESOURCE Map;
	HRESULT hr = ParentDevice->m_D3DDeviceContext->Map(ShaderConstantsBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &Map);

	// Metallicafan212:	IDK, do something here?
	ParentDevice->ThrowIfFailed(hr);

	// Metallicafan212:	Copy over this shader's constants
	//					The child shaders can choose to copy more vars to the memory buffer
	WriteConstantBuffer(Map.pData);

	// Metallicafan212:	Now unmap it
	ParentDevice->m_D3DDeviceContext->Unmap(ShaderConstantsBuffer, 0);

	// Metallicafan212:	Now finally set it as a resource
	ParentDevice->m_D3DDeviceContext->VSSetConstantBuffers(0, 1, &ShaderConstantsBuffer);

	if(GeoShader != nullptr)
		ParentDevice->m_D3DDeviceContext->GSSetConstantBuffers(0, 1, &ShaderConstantsBuffer);

	ParentDevice->m_D3DDeviceContext->PSSetConstantBuffers(0, 1, &ShaderConstantsBuffer);

	unguard;
}

// Metallicafan212:	Made this generic so we can copy vars into shaders
void FD3DShader::SetupConstantBuffer()
{
	guard(FD3DShader::SetupConstantBuffer);

	// Metallicafan212:	Matrix layout
	//					TODO! Ask the child shader for this?
	D3D11_BUFFER_DESC MatrixDesc = { sizeof(FShaderVarCommon), D3D11_USAGE_DYNAMIC, D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE, 0, 0 };

	HRESULT hr = ParentDevice->m_D3DDevice->CreateBuffer(&MatrixDesc, nullptr, &ShaderConstantsBuffer);

	// Metallicafan212:	IDK, do something here?
	ParentDevice->ThrowIfFailed(hr);

	unguard;
}

void FD3DShader::WriteConstantBuffer(void* InMem)
{
	guard(FD3DShader::WriteConstantBuffer);

	// Metallicafan212:	Copy over
	FShaderVarCommon* MDef			= ((FShaderVarCommon*)InMem);
	MDef->Proj					= ParentDevice->Proj;
	/*
	MDef->ViewX					= ParentDevice->m_sceneNodeX;
	MDef->ViewY					= ParentDevice->m_sceneNodeY;
	MDef->Aspect				= ParentDevice->m_Aspect;
	MDef->ProjZ					= ParentDevice->m_RProjZ;
	MDef->RFX2					= ParentDevice->m_RFX2;
	MDef->RFY2					= ParentDevice->m_RFY2;
	*/

	// Metallicafan212:	Common static information
	MDef->AlphaReject			= ParentDevice->GlobalShaderVars.AlphaReject;
	MDef->bColorMasked			= ParentDevice->GlobalShaderVars.bColorMasked;
	MDef->bDistanceFogEnabled	= ParentDevice->GlobalShaderVars.bDoDistanceFog;
	MDef->BWPercent				= ParentDevice->GlobalShaderVars.BWPercent;

	// Metallicafan212:	Automatically tell the shader that it's doing selection testing
	MDef->bSelection			= (ParentDevice->m_HitData != nullptr);
	MDef->bAlphaEnabled			= ParentDevice->GlobalShaderVars.bAlphaEnabled;

	// Metallicafan212:	Loop and tell the shader how many textures are bound
	for (INT i = 0; i < MAX_TEXTURES; i++)
	{
		MDef->BoundTextures[i] = (ParentDevice->BoundTextures[i].TexInfo != nullptr || ParentDevice->BoundTextures[i].bIsRT);
	}

	// Metallicafan212:	Fog stuff
	MDef->DistanceFogColor		= ParentDevice->GlobalShaderVars.DistanceFogColor;
	MDef->DistanceFogSettings	= ParentDevice->GlobalShaderVars.DistanceFogSettings;

	unguard;
}