#include "D3D11Drv.h"

#if DX11_HP2
// Metallicafan212:	Needed for the editorres folder
//					TODO! This is a HP2 new engine specific definition!!!!!
#include "..\..\MWindow\Inc\M212Def.h"
#endif

// Metallicafan212:	This file contains the configuration options support
//					If values aren't in the INI, it'll provide defaults

// Metallicafan212:	TODO! HACK!!!
const TCHAR* ClsName = nullptr;

#define CPP_PROP(PROP) TEXT(#PROP), PROP, CPP_PROPERTY(PROP)

// Metallicafan212:	Properties for preferences
void UD3D11RenderDevice::StaticConstructor()
{
	guard(UD3D11RenderDevice::StaticConstructor);

	ClsName = UD3D11RenderDevice::StaticClass()->GetPathName();

#if DX11_HP2
	// Metallicafan212:	Setup editor info
	bUseInEd				= 1;
	EditorInfo.EditorIcon	= IMG_FOLDER TEXT("inres\\viewport\\RD_D11");
	EditorInfo.Name			= TEXT("Directx 11 Render Device");
	EditorInfo.ToolTip		= TEXT("Directx 11 Rendering");
#endif

	// Metallicafan212:	Tell the engine that we support the lighting shader
	//					TODO! Implement this better! It SUCKS right now
#if USE_COMPUTE_SHADER
	bSupportsMeshLightShader = 1;
#endif

	//Driver flags
	SpanBased			= 0;
	SupportsFogMaps		= 1;
	SupportsDistanceFog = 0;
	VolumetricLighting	= 1;
	FullscreenOnly		= 0;
	SupportsLazyTextures= 0;
	PrefersDeferredLoad = 0;
	SupportsTC			= 1;

#if DX11_HP2
	bSupportsTwoSided	= 0;

	// Metallicafan212:	HP2 Rendertarget textures
	bSupportsRTTextures = 1;

	// Metallicafan212:	HP2 native wireframe
	bSupportsNativeWireframe = 1;
#endif

	// Metallicafan212:	This makes fonts look better, but we still need a MSAA resolve to make it not blurry
#if DX11_UT_469
	NeedsMaskedFonts = 1;
#endif

	if (!GConfig->GetBool(ClsName, TEXT("DetailTextures"), (UBOOL&)DetailTextures))
	{
		GConfig->SetBool(ClsName, TEXT("DetailTextures"), 1);
	}

	if (!GConfig->GetBool(ClsName, TEXT("VolumetricLighting"), (UBOOL&)VolumetricLighting))
	{
		GConfig->SetBool(ClsName, TEXT("VolumetricLighting"), 1);
	}

	if (!GConfig->GetBool(ClsName, TEXT("ShinySurfaces"), (UBOOL&)ShinySurfaces))
	{
		GConfig->SetBool(ClsName, TEXT("ShinySurfaces"), 1);
	}

	if (!GConfig->GetBool(ClsName, TEXT("Coronas"), (UBOOL&)Coronas))
	{
		GConfig->SetBool(ClsName, TEXT("Coronas"), 1);
	}

	if (!GConfig->GetBool(ClsName, TEXT("HighDetailActors"), (UBOOL&)HighDetailActors))
	{
		GConfig->SetBool(ClsName, TEXT("HighDetailActors"), 1);
	}

	// Metallicafan212:	Add on our stuff

	// Metallicafan212:	TODO! More config options
	AddIntProp(CPP_PROP(NumAASamples), 4);
	AddIntProp(CPP_PROP(NumAFSamples), 16);

	// Metallicafan212:	TODO! Make this engine global!!!!
	AddFloatProp(TEXT("3DLineThickness"), ThreeDeeLineThickness, CPP_PROPERTY(ThreeDeeLineThickness), 1.5f);
	AddFloatProp(CPP_PROP(OrthoLineThickness), 1.2f);

	unguard;
}

#undef CPP_PROP

void UD3D11RenderDevice::AddBoolProp(const TCHAR* Name, UBOOL& InParam, ECppProperty CPP, INT Offset, UBOOL bDefaultVal)
{
	guard(UD3D11RenderDevice::AddBoolProp);

	new(GetClass(), Name, RF_Public)UBoolProperty(CPP, Offset, TEXT("Options"), CPF_Config);

	// Metallicafan212:	Now set the default value
	if (!GConfig->GetBool(ClsName, Name, InParam))
	{
		// Metallicafan212:	Set it
		GConfig->SetBool(ClsName, Name, bDefaultVal);
		InParam = bDefaultVal;
	}

	unguard;
}

void UD3D11RenderDevice::AddFloatProp(const TCHAR* Name, FLOAT& InParam, ECppProperty CPP, INT Offset, FLOAT fDefaultVal)
{
	guard(UD3D11RenderDevice::AddFloatProp);

	new(GetClass(), Name, RF_Public)UFloatProperty(CPP, Offset, TEXT("Options"), CPF_Config);

	// Metallicafan212:	Now set the default value
	if (!GConfig->GetFloat(ClsName, Name, InParam))
	{
		// Metallicafan212:	Set it
		GConfig->SetFloat(ClsName, Name, fDefaultVal);
		InParam = fDefaultVal;
	}

	unguard;
}

void UD3D11RenderDevice::AddIntProp(const TCHAR* Name, INT& InParam, ECppProperty CPP, INT Offset, INT iDefaultVal)
{
	guard(UD3D11RenderDevice::AddIntProp);

	new(GetClass(), Name, RF_Public)UIntProperty(CPP, Offset, TEXT("Options"), CPF_Config);

	// Metallicafan212:	Now set the default value
	if (!GConfig->GetInt(ClsName, Name, InParam))
	{
		// Metallicafan212:	Set it
		GConfig->SetInt(ClsName, Name, iDefaultVal);
		InParam = iDefaultVal;
	}

	unguard;
}

void UD3D11RenderDevice::ClampUserOptions()
{
	guard(UD3D11RenderDevice::ClampUserOptions);

	// Metallicafan212:	TODO! Add some way to automatically clamp these options!!!
	NumAFSamples = Clamp(NumAFSamples, 0, D3D11_REQ_MAXANISOTROPY);

	ThreeDeeLineThickness	= Clamp(ThreeDeeLineThickness, 1.0f, FLT_MAX);
	OrthoLineThickness		= Clamp(OrthoLineThickness, 1.0f, FLT_MAX);

	// Metallicafan212:	Find the real MSAA levels supported
	UINT SampleCount = 1;
	HRESULT hr = S_OK;

	UINT NumQualityLevels = 0;

	while (SUCCEEDED(hr = m_D3DDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_B8G8R8A8_UNORM, SampleCount, &NumQualityLevels)))
	{
		// Metallicafan212:	See if it's actually supported (the device can choose to send a sucess code but set quality levels to 0, like AMD)
		if (NumQualityLevels <= 0)
		{
			SampleCount /= 2;
			break;
		}

		// Metallicafan212:	Account for the first MSAA sample being 1, not 0
		SampleCount *= 2;
	}

	// Metallicafan212:	We have the final clamp
	NumAASamples = Clamp(NumAASamples, 1, static_cast<INT>(SampleCount));

	// Metallicafan212:	Now make sure it's the lower of the request (if it's odd)
	//					TODO! Rewrite this, if the user specifies 6, we return 8 due to the mod 2 not working since 6 isn't odd
	//					I'm too tired for this shit right now lol....
	if (NumAASamples > 1 && NumAASamples < SampleCount)
	{
		NumAASamples = appPow(2.0, appCeilLogTwo(NumAASamples) - (NumAASamples % 2));
	}

	GLog->Logf(TEXT("DX11: Requesting %d AA samples"), NumAASamples);

	unguard;
}