#include "ICBINDx11Drv.h"

// Metallicafan212:	Shader interface
void FD3DShader::Init()
{
	guard(FD3DShader::Init);

	UINT Flags = 0;//D3DCOMPILE_DEBUG;

	HRESULT hr = S_OK;

	// Metallicafan212:	Compile the shaders
	if (VertexFunc.Len())
	{
		TArray<BYTE>* ShaderBytes = ParentDevice->ShaderManager->GetShaderBytes(VertexFile, VertexFunc, ParentDevice->MaxVSLevel, GET_MACRO_PTR(Macros), Flags);

		// Metallicafan212:	Get it as a vertex shader
		hr = ParentDevice->m_D3DDevice->CreateVertexShader(ShaderBytes->GetData(), ShaderBytes->Num(), nullptr, &VertexShader);

		// Metallicafan212:	IDK, do something here?
		ParentDevice->ThrowIfFailed(hr);


		// Metallicafan212:	Now create the layout pointer
		if (VertexShader != nullptr)
		{
			hr = ParentDevice->m_D3DDevice->CreateInputLayout(InputDesc, InputCount, ShaderBytes->GetData(), ShaderBytes->Num(), &InputLayout);

			// Metallicafan212:	IDK, do something here?
			ParentDevice->ThrowIfFailed(hr);
		}
	}

	// Metallicafan212:	Now the pixel shader
	if (PixelFunc.Len())
	{
		// Metallicafan212:	HACK! Predefine this here...
		//					This is so the shader knows it's the pixel shader
		INT PixelShaderMac			= Macros.Num() - 1;
		Macros.Insert(Macros.Num() - 1);//Macros.AddItem({"PIXEL_SHADER", "1"});
		Macros(PixelShaderMac)		= { "PIXEL_SHADER", "1" };

		TArray<BYTE>* ShaderBytes	= ParentDevice->ShaderManager->GetShaderBytes(PixelFile, PixelFunc, ParentDevice->MaxPSLevel, GET_MACRO_PTR(Macros), Flags);

		// Metallicafan212:	Get it as a pixel shader
		hr = ParentDevice->m_D3DDevice->CreatePixelShader(ShaderBytes->GetData(), ShaderBytes->Num(), nullptr, &PixelShader);

		// Metallicafan212:	Now remove it
		Macros.Remove(PixelShaderMac);

		// Metallicafan212:	IDK, do something here?
		ParentDevice->ThrowIfFailed(hr);
	}

	// Metallicafan212:	Now the geometry shader
	if (ParentDevice->bUseGeoShaders && GeoFunc.Len())
	{
		TArray<BYTE>* ShaderBytes = ParentDevice->ShaderManager->GetShaderBytes(GeoFile, GeoFunc, ParentDevice->MaxGSLevel, GET_MACRO_PTR(Macros), Flags);

		// Metallicafan212:	Get it as a pixel shader
		hr = ParentDevice->m_D3DDevice->CreateGeometryShader(ShaderBytes->GetData(), ShaderBytes->Num(), nullptr, &GeoShader);

		// Metallicafan212:	IDK, do something here?
		ParentDevice->ThrowIfFailed(hr);
	}

	// Metallicafan212:	Allow child shaders to setup the constant buffer how they want
	//					TODO! The shared variables should be optimized so that they only upload once per frame/only when they change
	SetupConstantBuffer();

	unguard;
}

void FD3DShader::SetShaders(ID3D11DeviceContext* UseContext)
{
	// Metallicafan212:	Setup this shader for rendering
	UseContext->VSSetShader(VertexShader, nullptr, 0);

	// Metallicafan212:	Bind the optional geometry shader
	UseContext->GSSetShader(GeoShader, nullptr, 0);

	UseContext->PSSetShader(PixelShader, nullptr, 0);

	UseContext->IASetInputLayout(InputLayout);
}

// Metallicafan212:	TODO! This should be re-evalled and the bound textures moved to another constant buffer
//					Probably causing a lot of issues having multiple, but I'll combine more of them together later
void FD3DShader::Bind(ID3D11DeviceContext* UseContext)
{
	guardSlow(FD3DShader::Bind);

	UBOOL bShaderIsUs = 1;

	if (ParentDevice->CurrentShader != this)
	{
		// Metallicafan212:	Flush rendering if needed
		ParentDevice->EndBuffering();

		ParentDevice->CurrentShader = this;

		bShaderIsUs = 0;
	}

	// Metallicafan212:	Only do this if the current shader isn't ours!!!!!
	if (!bShaderIsUs)
	{
		SetShaders(UseContext);
	}

	if (ShaderConstantsBuffer != nullptr)
	{
#if UPDATESUBRESOURCE_CONSTANTS
		// Metallicafan212:	Call the function with a nullptr (leftover old def)
		if (WriteConstantBuffer(nullptr))
		{
			// Metallicafan212:	Now update it
			UseContext->UpdateSubresource(ShaderConstantsBuffer, 0, nullptr, ShaderConstantsMem, 0, 0);
		}
#else
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
#endif

		// Metallicafan212:	Now finally set it as a resource
		if (VertexShader != nullptr)
			UseContext->VSSetConstantBuffers(FIRST_USER_CONSTBUFF, 1, &ShaderConstantsBuffer);

		if (GeoShader != nullptr)
			UseContext->GSSetConstantBuffers(FIRST_USER_CONSTBUFF, 1, &ShaderConstantsBuffer);

		if (PixelShader != nullptr)
			UseContext->PSSetConstantBuffers(FIRST_USER_CONSTBUFF, 1, &ShaderConstantsBuffer);
	}

	unguardSlow;
}

// Metallicafan212:	Made this generic so we can copy vars into shaders
void FD3DShader::SetupConstantBuffer()
{
	// Metallicafan212:	Blank now since we don't do anything in the standard shader now
	/*
	guard(FD3DShader::SetupConstantBuffer);

	// Metallicafan212:	Matrix layout
	//					TODO! Ask the child shader for this?
	D3D11_BUFFER_DESC MatrixDesc = { sizeof(FShaderVarCommon), D3D11_USAGE_DYNAMIC, D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE, 0, 0 };

	HRESULT hr = ParentDevice->m_D3DDevice->CreateBuffer(&MatrixDesc, nullptr, &ShaderConstantsBuffer);

	// Metallicafan212:	IDK, do something here?
	ParentDevice->ThrowIfFailed(hr);

	unguard;
	*/

	ShaderConstantsBuffer = nullptr;
}

// Metallicafan212:	TODO!!!!! Move this to a separate buffer, and ONLY set when the texture(s) change
UBOOL FD3DShader::WriteConstantBuffer(void* InMem)
{
	guardSlow(FD3DShader::WriteConstantBuffer);

	/*
	// Metallicafan212:	Copy over
	FShaderVarCommon* MDef			= ((FShaderVarCommon*)InMem);

	// Metallicafan212:	Loop and tell the shader how many textures are bound
	for (INT i = 0; i < MAX_TEXTURES; i++)
	{
		MDef->BoundTextures[i] = (ParentDevice->BoundTextures[i].TexInfoHash != 0 || ParentDevice->BoundTextures[i].bIsRT);
	}
	*/

	return 0;

	unguardSlow;
}