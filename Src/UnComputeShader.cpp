#include "ICBINDx11Drv.h"

#include "Render.h"

struct FLightingData
{
	// Metallicafan212:	Frame related info
	//					TODO! Might just provide the frame vars globally to all shaders
	FLOAT 			FX15;
	FLOAT			FY15;
	FLOAT 			PrjXM;
	FLOAT			PrjXP;
	FLOAT			PrjYM;
	FLOAT			PrjYP;

	FVector			Proj;

	FLOAT 			Specular;
	FLOAT 			SpecMinCos;
	FLOAT 			Diffuse;
	FVector 		AmbientVector;

	UBOOL			bUnlit;
	UBOOL			bDoFog;

	// Metallicafan212:	These are for the owner of the mesh, so we can fixup the verts
	FLOAT			OwnerOpacity;
	FLOAT			OwnerFatness;
	UBOOL			OwnerbMeshCurvy;
	FVector			TransformedOwnerLocation;
	int				Pad;
};


// Metallicafan212:	Generic compute shader interface
struct FActorLightCommon
{
	// For all lights.
	ELightKind		Opt;					// Light type.
	FVector			Location;				// Transformed screenspace location of light.
	FVector			LightDir;				// Unit direction from actor to light (max diffuse reflection).
	FLOAT			Radius;					// Maximum effective radius.
	FLOAT			RRadiusMult;			// 1.0f / (Radius - RadiusInner);

	// Coloring.
	FPlane			FloatColor;				// Incident lighting color.
	FPlane			VolumetricColor;		// Volumetric lighting color.

	// For volumetric lights.
	FLOAT			VolBrightness;			// Volumetric lighting brightness.
	UBOOL       	VolInside;				// Viewpoint is inside the sphere.
	FLOAT			VolRadiusSquared;		// VolRadius*VolRadius.
	FLOAT			LocationSizeSquared;	// Location.SizeSqurated().
	FLOAT			RVolRadius;				// 1/Volumetric radius.

	// Metallicafan212:	Past this point, it's just hand-fed variables
	UBOOL			bVolumetric;

	// Metallicafan212:	Actor related data for this light
	//					This is added by us!
	DWORD			ActorLightSource;
	FRotator		ActorRotation;
	INT				ActorLightBrightness;
	UBOOL			ActorbDarkLight;
	DWORD			ActorLightEffect;
	INT				Pad[2];
};

FTransTexture* UICBINDx11RenderDevice::InitMeshComputeShader(INT VertCount)
{
	guard(UICBINDx11RenderDevice::InitMeshComputeShader);

#if USE_COMPUTE_SHADER
	// Metallicafan212:	See if it's beyond the number of verts we have
	if (VertCount > FMshLghtCompShader->NumVertsMax)
	{
		// Metallicafan212:	For now, panic
		appErrorf(TEXT("Too many verts for compute shader (%d verts)"), VertCount);
	}

	D3D11_MAPPED_SUBRESOURCE	VertDataMap = { nullptr, 0, 0 };

	m_RenderContext->Map(FMshLghtCompShader->VertBuffer, 0, D3D11_MAP_WRITE, 0, &VertDataMap);

	return (FTransTexture*)VertDataMap.pData;
#else
	return nullptr;
#endif

	unguard;
}

void UICBINDx11RenderDevice::FinishMeshComputeShader()
{
	guard(UICBINDx11RenderDevice::FinishMeshComputeShader);
#if USE_COMPUTE_SHADER
	// Metallicafan212:	Unmap for future use
	m_RenderContext->Unmap(FMshLghtCompShader->VertBuffer, 0);
#endif

	unguard;
}

void UICBINDx11RenderDevice::ExecuteMeshLightShader(FSceneNode* Frame, INT VertCount, void* MeshLights, void* LastLight, FLOAT Specular, FLOAT SpecMinCos, FLOAT Diffuse, FVector AmbientVector, AActor* Actor, QWORD PolyFlags)
{
	guard(UICBINDx11RenderDevice::ExecuteMeshLightShader);
#if USE_COMPUTE_SHADER
	// Metallicafan212:	Assemble the common actor data!
	FMshLghtCompShader->Bind();

	// Metallicafan212:	Now get the data for each input buffer
	D3D11_MAPPED_SUBRESOURCE	CommDataMap = { nullptr, 0, 0 };

	m_RenderContext->Map(FMshLghtCompShader->ShaderConstantsBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &CommDataMap);

	FLightingData* CommData = (FLightingData*)CommDataMap.pData;
	// Metallicafan212:	A HUGE amount of data copying...
	CommData->FX15						= Frame->FX15;
	CommData->FY15						= Frame->FY15;
	CommData->PrjXM						= Frame->PrjXM;
	CommData->PrjXP						= Frame->PrjXP;
	CommData->PrjYM						= Frame->PrjYM;
	CommData->PrjYP						= Frame->PrjYP;
	CommData->Proj						= Frame->Proj;
	CommData->Specular					= Specular;
	CommData->SpecMinCos				= SpecMinCos;
	CommData->Diffuse					= Diffuse;
	CommData->AmbientVector				= AmbientVector;
	CommData->bUnlit					= PolyFlags & PF_Unlit;
	CommData->bDoFog					= PolyFlags & PF_RenderFog;
	CommData->OwnerOpacity				= Actor->Opacity;
	CommData->OwnerFatness				= (Actor->Fatness != 128 ? (Actor->Fatness / 16.0f) - 8.0f : 1.0f);
	CommData->OwnerbMeshCurvy			= Actor->bMeshCurvy;
	CommData->TransformedOwnerLocation	= Actor->Location.TransformPointBy(Frame->Coords);

	// Metallicafan212:	Now unmap
	m_RenderContext->Unmap(FMshLghtCompShader->ShaderConstantsBuffer, 0);

	// Metallicafan212:	Map the input lights
	D3D11_MAPPED_SUBRESOURCE	LightDataMap = { nullptr, 0, 0 };

	m_RenderContext->Map(FMshLghtCompShader->LightBuffer, 0, D3D11_MAP_WRITE, 0, &LightDataMap);

	FActorLightCommon* Lght = (FActorLightCommon*)LightDataMap.pData;

	INT LightCount = 0;

	// Metallicafan212:	Get the light data and assemble it
	FLightInfo* InfoList = (FLightInfo*)MeshLights;

	// Metallicafan212:	Now loop and copy!
	for (; InfoList < LastLight; InfoList++)
	{
		// Metallicafan212:	Copy the common data
		appMemcpy(&Lght[LightCount], &InfoList->ActorCommonData, sizeof(FLightInfo::FLightCommon));

		// Metallicafan212:	Now the rest
		Lght[LightCount].ActorLightSource		= Actor->LightSource;
		Lght[LightCount].ActorRotation			= Actor->Rotation;
		Lght[LightCount].ActorLightBrightness	= Actor->LightBrightness;
		Lght[LightCount].ActorbDarkLight		= Actor->bDarkLight;
		Lght[LightCount].ActorLightEffect		= Actor->LightEffect;

		LightCount++;
	}

	// Metallicafan212:	Now unmap
	m_RenderContext->Unmap(FMshLghtCompShader->LightBuffer, 0);

	/*
	// Metallicafan212:	Finally, the verts
	D3D11_MAPPED_SUBRESOURCE	VertDataMap = { nullptr, 0, 0 };

	m_RenderContext->Map(FMshLghtCompShader->VertBuffer, 0, D3D11_MAP_WRITE, 0, &VertDataMap);

	FTransSample* Verts = (FTransSample*)VertDataMap.pData;

	appMemcpy(Verts, MeshVerts, sizeof(FTransSample) * VertCount);

	// Metallicafan212:	Now unmap
	m_RenderContext->Unmap(FMshLghtCompShader->VertBuffer, 0);
	*/

	// Metallicafan212:	We need to unmap it to execute
	m_RenderContext->Unmap(FMshLghtCompShader->VertBuffer, 0);

	// Metallicafan212:	Execute the shader
	//m_RenderContext->Begin(m_D3DQuery);

	m_RenderContext->Dispatch(VertCount, 1, 1);

	m_RenderContext->End(m_D3DQuery);

	// Metallicafan212:	Wait for it
	BOOL bDone = 0;

	while (m_RenderContext->GetData(m_D3DQuery, &bDone, sizeof(BOOL), 0) != S_OK);

	// Metallicafan212:	Unmap?
	m_RenderContext->CSSetShader(nullptr, nullptr, 0);
	m_RenderContext->CSSetUnorderedAccessViews(0, 0, nullptr, nullptr);
	m_RenderContext->CSSetShaderResources(0, 0, nullptr);

	//m_D3DDevice->CreateQuery()

	/*
	// Metallicafan212:	Copy back
	m_RenderContext->Map(FMshLghtCompShader->VertBuffer, 0, D3D11_MAP_READ, 0, &VertDataMap);
	appMemcpy(MeshVerts, VertDataMap.pData, sizeof(FTransSample) * VertCount);
	*/

	//D3D11_MAPPED_SUBRESOURCE	VertDataMap = { nullptr, 0, 0 };
	//m_RenderContext->Map(FMshLghtCompShader->VertBuffer, 0, D3D11_MAP_READ, 0, &VertDataMap);

	// Metallicafan212:	Now unmap
	//m_RenderContext->Unmap(FMshLghtCompShader->VertBuffer, 0);
#endif

	unguard;
}

// Metallicafan212:	Compute shader implementation
// 
// Metallicafan212:	Constructor that inits the device pointer
FD3DComputeShader::FD3DComputeShader(UICBINDx11RenderDevice* InParent)
	: FD3DShader(InParent),
	ComputeShader(nullptr)
{

}

/*
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
				GWarn->Logf(TEXT("D3D11: Error compiling shader. Error is %s"), appFromAnsi(ErrorStr));
				delete[] ErrorStr;
			}
			//error->Release();
		}
		// Metallicafan212:	Don't do anything else?
		//return;
	}

	if (error != nullptr)
		error->Release();
}
*/

void FD3DComputeShader::Init()
{
	guard(FD3DComputeShader::Init);

	FD3DShader::Init();

	// Metallicafan212:	TODO! Create the input parameters
	//					Child shaders should track their own constant buffers....
	// Metallicafan212:	Create the shader blobs (pixel, vertex), and the stream layout
	//ID3D10Blob* error = nullptr;
	//ID3D10Blob* csBuff = nullptr;

	/*
	// Metallicafan212:	Compile the shaders
	HRESULT hr = D3DCompileFromFile(*ComputeFile, GET_MACRO_PTR(Macros), D3D_CMP_STD_INC, appToAnsi(*ComputeFunc), "cs_5_0", 0, 0, &csBuff, &error);

	CheckShader(hr, error);
	*/

	TArray<BYTE>* ShaderBytes = ParentDevice->ShaderManager->GetShaderBytes(ComputeFile, ComputeFunc, "cs_5_0", GET_MACRO_PTR(Macros), 0);

	// Metallicafan212:	Get it as a compute shader
	HRESULT hr = ParentDevice->m_D3DDevice->CreateComputeShader(ShaderBytes->GetData(), ShaderBytes->Num(), nullptr, &ComputeShader);//CreateVertexShader(vsBuff->GetBufferPointer(), vsBuff->GetBufferSize(), nullptr, &VertexShader);

	// Metallicafan212:	IDK, do something here?
	ParentDevice->ThrowIfFailed(hr);

	unguard;
}

void FD3DComputeShader::Bind(ID3D11DeviceContext* UseContext)
{
	guardSlow(FD3DComputeShader::Bind);

	FD3DShader::Bind(UseContext);

	// Metallicafan212:	Update the pointer so that the device knows that we need to cleanup
	//ParentDevice->CurrentShader = this;

	// Metallicafan212:	Bind the compute shader
	//					We'll do the actual variable layout and input binding in the children
	UseContext->CSSetShader(ComputeShader, nullptr, 0);

	// Metallicafan212:	Bind the constant buffer as well
	UseContext->CSSetConstantBuffers(FIRST_USER_CONSTBUFF, 1, &ShaderConstantsBuffer);

	unguardSlow;
}

#if USE_COMPUTE_SHADER
// Metallicafan212:	Device pointer version
FD3DLghtMshCompShader::FD3DLghtMshCompShader(UICBINDx11RenderDevice* InParent)
	: FD3DComputeShader(InParent)
{
	ComputeFile = SHADER_FOLDER TEXT("MeshTandLCompute.hlsl");
	ComputeFunc = TEXT("CSMain");

	NumVertsMax = VERT_MAX;

	Init();
}

void FD3DLghtMshCompShader::Init()
{
	guard(FD3DLghtMshCompShader::Init);

	// Metallicafan212:	Super it
	FD3DComputeShader::Init();

	// Metallicafan212:	Now make the input and output streams
	D3D11_BUFFER_DESC CommonData = { sizeof(FLightingData), D3D11_USAGE_DYNAMIC, D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE, 0, 0 };

	// Metallicafan212:	Use the shader constants buffer
	HRESULT hr = ParentDevice->m_D3DDevice->CreateBuffer(&CommonData, nullptr, &ShaderConstantsBuffer);

	// Metallicafan212:	IDK, do something here?
	ParentDevice->ThrowIfFailed(hr);

	// Metallicafan212:	Now the light data
	//					Max of 100 lights!!!!!
	D3D11_BUFFER_DESC LightDesc =
	{
		sizeof(FLightingData) * LIGHT_MAX,
		/*D3D11_USAGE_DYNAMIC*/D3D11_USAGE_DEFAULT,
		D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE,
		D3D11_CPU_ACCESS_WRITE, D3D11_RESOURCE_MISC_BUFFER_STRUCTURED,
		sizeof(FLightingData)
	};

	// Metallicafan212:	Now create it
	hr = ParentDevice->m_D3DDevice->CreateBuffer(&LightDesc, nullptr, &LightBuffer);

	ParentDevice->ThrowIfFailed(hr);

	// Metallicafan212:	Create a shader view for the input lights
	D3D11_BUFFER_DESC descBuf;
	ZeroMemory(&descBuf, sizeof(descBuf));
	LightBuffer->GetDesc(&descBuf);

	D3D11_SHADER_RESOURCE_VIEW_DESC descView;
	ZeroMemory(&descView, sizeof(descView));
	descView.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	descView.BufferEx.FirstElement = 0;

	descView.Format = DXGI_FORMAT_UNKNOWN;
	descView.BufferEx.NumElements = descBuf.ByteWidth / descBuf.StructureByteStride;

	/*
	if (FAILED(m_pd3dDevice->CreateShaderResourceView(m_srcDataGPUBuffer,
		&descView, &m_srcDataGPUBufferView)))
	*/
	hr = ParentDevice->m_D3DDevice->CreateShaderResourceView(LightBuffer, &descView, &LightShaderView);

	ParentDevice->ThrowIfFailed(hr);

	// Metallicafan212:	Now the mesh vertices
	D3D11_BUFFER_DESC VertDesc =
	{
		sizeof(FTransTexture) * NumVertsMax,
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE,
		D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ, 
		D3D11_RESOURCE_MISC_BUFFER_STRUCTURED,
		sizeof(FTransTexture)
	};

	// Metallicafan212:	Now create it
	hr = ParentDevice->m_D3DDevice->CreateBuffer(&VertDesc, nullptr, &VertBuffer);

	ParentDevice->ThrowIfFailed(hr);

	// Metallicafan212:	Create a shader view for the input/output mesh verts
	ZeroMemory(&descBuf, sizeof(descBuf));
	VertBuffer->GetDesc(&descBuf);

	D3D11_UNORDERED_ACCESS_VIEW_DESC descUnorderedView;
	ZeroMemory(&descUnorderedView, sizeof(descUnorderedView));
	descUnorderedView.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	descUnorderedView.Buffer.FirstElement = 0;

	descUnorderedView.Format = DXGI_FORMAT_UNKNOWN;
	descUnorderedView.Buffer.NumElements = descBuf.ByteWidth / descBuf.StructureByteStride;

	/*
	if (FAILED(m_pd3dDevice->CreateShaderResourceView(m_srcDataGPUBuffer,
		&descView, &m_srcDataGPUBufferView)))
	*/
	//hr = ParentDevice->m_D3DDevice->CreateShaderResourceView(LightBuffer, &descView, &LightShaderView);
	hr = ParentDevice->m_D3DDevice->CreateUnorderedAccessView(VertBuffer, &descUnorderedView, &VertUnorderedView);

	ParentDevice->ThrowIfFailed(hr);

	unguard;
}

void FD3DLghtMshCompShader::Bind()
{
	guard(FD3DLghtMshCompShader::Bind);

	// Metallicafan212:	Super it
	FD3DComputeShader::Bind();

	// Metallicafan212:	Setup the input streams
	ParentDevice->m_RenderContext->CSSetShaderResources(0, 1, &LightShaderView);
	ParentDevice->m_RenderContext->CSSetUnorderedAccessViews(0, 1, &VertUnorderedView, nullptr);
	ParentDevice->m_RenderContext->CSSetConstantBuffers(0, 1, &ShaderConstantsBuffer);

	unguard;
}
#endif