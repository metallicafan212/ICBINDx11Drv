#include "ICBINDx11Drv.h"

D3D11_INPUT_ELEMENT_DESC FBasicInLayout[] =
{
	"POSITION",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 0,								D3D11_INPUT_PER_VERTEX_DATA, 0,
	"TEXCOORD",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0,
	"COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0,
	"COLOR",		1, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0,
#if EXTRA_VERT_INFO
	"TEXCOORD",		1, DXGI_FORMAT_R32G32B32A32_FLOAT,	1, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0,
	"TEXCOORD",		2, DXGI_FORMAT_R32G32B32A32_FLOAT,	1, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0,
	"TEXCOORD",		3, DXGI_FORMAT_R32G32B32A32_FLOAT,	1, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0,
	"TEXCOORD",		4, DXGI_FORMAT_R32G32B32A32_FLOAT,	1, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0,
	"TEXCOORD",		5, DXGI_FORMAT_R32G32B32A32_FLOAT,	1, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0,
	"TEXCOORD",		6, DXGI_FORMAT_R32G32B32A32_FLOAT,	1, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0,
	"TEXCOORD",		7, DXGI_FORMAT_R32G32B32A32_FLOAT,	1, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0,
	"TEXCOORD",		8, DXGI_FORMAT_R32G32B32A32_FLOAT,	1, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0,
#endif
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

	UINT Flags = 0;//D3DCOMPILE_DEBUG;

	HRESULT hr = S_OK;

	// Metallicafan212:	Compile the shaders
	if (VertexFunc.Len())
	{
		hr = D3DCompileFromFile(*VertexFile, GET_MACRO_PTR(Macros), D3D_CMP_STD_INC, appToAnsi(*VertexFunc), "vs_5_0", Flags, 0, &vsBuff, &error);

		CheckShader(hr, error);

		// Metallicafan212:	Get it as a vertex shader
		hr = ParentDevice->m_D3DDevice->CreateVertexShader(vsBuff->GetBufferPointer(), vsBuff->GetBufferSize(), nullptr, &VertexShader);

		// Metallicafan212:	IDK, do something here?
		ParentDevice->ThrowIfFailed(hr);
	}

	// Metallicafan212:	Now the pixel shader
	if (PixelFunc.Len())
	{
		hr = D3DCompileFromFile(*PixelFile, GET_MACRO_PTR(Macros), D3D_CMP_STD_INC, appToAnsi(*PixelFunc), "ps_5_0", Flags, 0, &psBuff, &error);

		CheckShader(hr, error);

		// Metallicafan212:	Get it as a pixel shader
		hr = ParentDevice->m_D3DDevice->CreatePixelShader(psBuff->GetBufferPointer(), psBuff->GetBufferSize(), nullptr, &PixelShader);

		// Metallicafan212:	IDK, do something here?
		ParentDevice->ThrowIfFailed(hr);
	}

	// Metallicafan212:	Now the geometry shader
	if (GeoFunc.Len())
	{
		hr = D3DCompileFromFile(*GeoFile, GET_MACRO_PTR(Macros), D3D_CMP_STD_INC, appToAnsi(*GeoFunc), "gs_5_0", Flags, 0, &gsBuff, &error);

		CheckShader(hr, error);

		// Metallicafan212:	Get it as a pixel shader
		hr = ParentDevice->m_D3DDevice->CreateGeometryShader(gsBuff->GetBufferPointer(), gsBuff->GetBufferSize(), nullptr, &GeoShader);

		// Metallicafan212:	IDK, do something here?
		ParentDevice->ThrowIfFailed(hr);
	}

	// Metallicafan212:	Make the input layout

	// Metallicafan212:	Now create the layout pointer
	if (VertexShader != nullptr)
	{
		hr = ParentDevice->m_D3DDevice->CreateInputLayout(InputDesc, InputCount, vsBuff->GetBufferPointer(), vsBuff->GetBufferSize(), &InputLayout);

		// Metallicafan212:	IDK, do something here?
		ParentDevice->ThrowIfFailed(hr);
	}

	// Metallicafan212:	Allow child shaders to setup the constant buffer how they want
	//					TODO! The shared variables should be optimized so that they only upload once per frame/only when they change
	SetupConstantBuffer();


	SAFE_RELEASE(vsBuff);
	SAFE_RELEASE(psBuff);
	SAFE_RELEASE(gsBuff);

	unguard;
}

// Metallicafan212:	TODO! This should be re-evalled and the bound textures moved to another constant buffer
//					Probably causing a lot of issues having multiple, but I'll combine more of them together later
void FD3DShader::Bind(ID3D11DeviceContext* UseContext)
{
	guardSlow(FD3DShader::Bind);

	UBOOL bShaderIsUs = 1;

	if (ParentDevice->CurrentShader != this)
	{
		ParentDevice->EndBuffering();

		ParentDevice->CurrentShader = this;

		bShaderIsUs = 0;
	}

	// Metallicafan212:	Only do this if the current shader isn't ours!!!!!
	if (!bShaderIsUs)
	{
		// Metallicafan212:	Setup this shader for rendering
		UseContext->VSSetShader(VertexShader, nullptr, 0);

		// Metallicafan212:	Bind the optional geometry shader
		UseContext->GSSetShader(GeoShader, nullptr, 0);

		UseContext->PSSetShader(PixelShader, nullptr, 0);

		UseContext->IASetInputLayout(InputLayout);
	}

	// Metallicafan212:	Map the matrix(s)
	D3D11_MAPPED_SUBRESOURCE Map;
	HRESULT hr = UseContext->Map(ShaderConstantsBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &Map);

	// Metallicafan212:	IDK, do something here?
	ParentDevice->ThrowIfFailed(hr);

	// Metallicafan212:	Copy over this shader's constants
	//					The child shaders can choose to copy more vars to the memory buffer
	WriteConstantBuffer(Map.pData);

	// Metallicafan212:	Now unmap it
	UseContext->Unmap(ShaderConstantsBuffer, 0);

	// Metallicafan212:	Now finally set it as a resource
	if(VertexShader != nullptr)
		UseContext->VSSetConstantBuffers(FIRST_USER_CONSTBUFF, 1, &ShaderConstantsBuffer);

	if(GeoShader != nullptr)
		UseContext->GSSetConstantBuffers(FIRST_USER_CONSTBUFF, 1, &ShaderConstantsBuffer);

	if(PixelShader != nullptr)
		UseContext->PSSetConstantBuffers(FIRST_USER_CONSTBUFF, 1, &ShaderConstantsBuffer);

	unguardSlow;
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
	guardSlow(FD3DShader::WriteConstantBuffer);

	// Metallicafan212:	Copy over
	FShaderVarCommon* MDef			= ((FShaderVarCommon*)InMem);

	/*
	// Metallicafan212:	Common static information
	MDef->AlphaReject			= ParentDevice->GlobalShaderVars.AlphaReject;
	MDef->bColorMasked			= ParentDevice->GlobalShaderVars.bColorMasked;
	MDef->BWPercent				= ParentDevice->GlobalShaderVars.BWPercent;

	// Metallicafan212:	Automatically tell the shader that it's doing selection testing
	MDef->bSelection			= (ParentDevice->m_HitData != nullptr);
	MDef->bAlphaEnabled			= ParentDevice->GlobalShaderVars.bAlphaEnabled;

	// Metallicafan212:	Temp hack to just get modulated rendering right. It'll disable gamma correction
	MDef->bModulated			= (ParentDevice->CurrentPolyFlags & PF_Modulated);
	*/

	// Metallicafan212:	Loop and tell the shader how many textures are bound
	for (INT i = 0; i < MAX_TEXTURES; i++)
	{
		MDef->BoundTextures[i] = (ParentDevice->BoundTextures[i].TexInfoHash != 0 || ParentDevice->BoundTextures[i].bIsRT);
	}

	unguardSlow;
}