#include "ICBINDx11Drv.h"

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
void UICBINDx11RenderDevice::StaticConstructor()
{
	guard(UICBINDx11RenderDevice::StaticConstructor);

	ClsName = UICBINDx11RenderDevice::StaticClass()->GetPathName();

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
	// Metallicafan212:	HP2 Rendertarget textures
	bSupportsRTTextures = 1;

	// Metallicafan212:	HP2 native wireframe
	bSupportsNativeWireframe = 1;
#endif

#if DX11_UT_469
	NeedsMaskedFonts = 0;

	// Metallicafan212:	DX11 supports massive textures
	UseLightmapAtlas = 1;

	MaxTextureSize		= D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION;

	// Metallicafan212:	Default to off for now, since it's hella unfinished and broken
	AddBoolProp(CPP_PROP(SupportsUpdateTextureRect), 0);//1);
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

	// Metallicafan212:	MSAA is now enabled by default
	AddIntProp(CPP_PROP(NumAASamples), 8);
	AddIntProp(CPP_PROP(NumAFSamples), 16);

	// Metallicafan212:	TODO! Make this engine global!!!!
	AddFloatProp(TEXT("3DLineThickness"), ThreeDeeLineThickness, CPP_PROPERTY(ThreeDeeLineThickness), 1.5f);
	AddFloatProp(CPP_PROP(OrthoLineThickness), 1.2f);

	AddBoolProp(CPP_PROP(bDebugSelection), 0);
	//AddBoolProp(CPP_PROP(bUseD3D11On12), 0);

	// Metallicafan212:	TODO! Go in game to determine what these values should be
	//AddFloatProp(CPP_PROP(TileAAUVMove), 0.1f);
	//TileAAUVMove = 0.1f;

	/*
	// Metallicafan212:	TODO! MSAA resolve related vars
	AddBoolProp(CPP_PROP(bUseMSAAComputeShader), 1);
	AddFloatProp(CPP_PROP(MSAAFilterSize), 0.5f);
	AddFloatProp(CPP_PROP(MSAAGaussianSigma), 1.0f);
	AddFloatProp(CPP_PROP(MSAACubicB), 1.0f);
	AddFloatProp(CPP_PROP(MSAACubicC), 2.69f);
	AddIntProp(CPP_PROP(MSAAFilterType), 8);
	*/

	AddBoolProp(CPP_PROP(bDisableDebugInterface), 1);
	AddBoolProp(CPP_PROP(bDisableSDKLayers), 1);

	/*
	// Metallicafan212:	Load the old setting, if it's available
	UseVSync = 0;
	UBOOL bTest = 0;

	if (GConfig->GetBool(ClsName, TEXT("bVSync"), bTest))
	{
		UseVSync = bTest;
	}
	*/

	AddBoolProp(CPP_PROP(UseVSync), 0);

	// Metallicafan212:	TODO! Should this be 1.2f by default? I'll leave it all alone in case there's some user out there that will complain lol
	AddFloatProp(CPP_PROP(ResolutionScale), 1.0f);

	// Metallicafan212:	Gamma/brightness is done here because a HP2 speedrunning trick involves messing with the brightness bar
#if DX11_HP2
	AddFloatProp(CPP_PROP(Gamma), 1.0f);
#endif

	// Metallicafan212:	Surface selection color
	//					Default is blue with a alpha of 0.5f
	AddColorProp(CPP_PROP(SurfaceSelectionColor), FColor(FPlane(0.0f, 0.0f, 1.0f, 0.5f)));

	// Metallicafan212:	Actor selection color
	//					Default is green with a alpha of 1.0f
	AddColorProp(CPP_PROP(ActorSelectionColor), FColor(FPlane(0.0f, 1.0f, 0.0f, 1.0f)));

	// Metallicafan212:	Number of additional buffers for the swapchain to make
	//					Default is 0, for double buffering
	//					1 = triple buffering
	AddIntProp(CPP_PROP(NumAdditionalBuffers), 1);

	AddBoolProp(CPP_PROP(SceneNodeHack), 0);

	AddFloatProp(CPP_PROP(GammaOffset), 0.0f);

	AddBoolProp(CPP_PROP(bOneXLightmaps), 0);
	AddBoolProp(CPP_PROP(bEnableCorrectFogging), 1);

	// Metallicafan212:	Add an option to use a HDR compatible screen type (in-game only)
	AddBoolProp(CPP_PROP(UseHDR), 0);

	// Metallicafan212:	Add on user selectable gamma modes, so they can pick what fits best for them
	UEnum* GEnum = new (GetClass(), TEXT("GammaModes"))UEnum(nullptr);
#define DECLARE_ENUM
#define DECLARE_GM(name, val) GEnum->Names.AddItem(FName(TEXT(#name) + 3));

#include "GammaModes.h"

	AddByteProp(CPP_PROP(GammaMode), GM_XOpenGL, GEnum);

	// Metallicafan212:	Expansion value for sRGB to linear color
	AddFloatProp(CPP_PROP(HDRExpansion), 1.2f);

	AddBoolProp(CPP_PROP(UseDX9FlatColor), 0);

	unguard;
}

#undef CPP_PROP

void UICBINDx11RenderDevice::AddByteProp(const TCHAR* Name, BYTE& InParam, ECppProperty CPP, INT Offset, BYTE bDefaultVal, UEnum* InEnum)
{
	guard(UICBINDx11RenderDevice::AddByteProp);

	// Metallicafan212:	Create it
	new(GetClass(), Name, RF_Public)UByteProperty(CPP, Offset, TEXT("Display"), CPF_Config, InEnum);

	InParam = bDefaultVal;

	// Metallicafan212:	See if the default is provided
	FString Test;

	if (!GConfig->GetString(ClsName, Name, Test))
	{
		// Metallicafan212:	If we were provided a enum, use it for the value
		//					This ASSUMES that the array size is correct
		if (InEnum != nullptr)
		{
			GConfig->SetString(ClsName, Name, *InEnum->Names(bDefaultVal));
		}
		else
		{
			// Metallicafan212:	Use the default value
			GConfig->SetInt(ClsName, Name, bDefaultVal);
		}

		InParam = bDefaultVal;
	}
	else if (InEnum != nullptr)
	{
		// Metallicafan212:	Get back the user's choice
		INT NewValue = bDefaultVal;

		for (INT i = 0; i < InEnum->Names.Num(); i++)
		{
			if (InEnum->Names(i) == *Test)
			{
				NewValue = i;
			}
		}

		// Metallicafan212:	Either use the user's option or set the default
		InParam = NewValue;
	}
	else
	{
		// Metallicafan212:	Read it as an int
		INT val;
		GConfig->GetInt(ClsName, Name, val);

		InParam = Clamp(val, 0, 255);
	}

	unguard;
}

void UICBINDx11RenderDevice::AddColorProp(const TCHAR* Name, FColor& InParam, ECppProperty CPP, INT Offset, FColor DefaultVal)
{
	guard(UICBINDx11RenderDevice::AddColorProp);

	// Metallicafan212:	Set the default
	InParam = DefaultVal;

	// Metallicafan212:	Find the Object.uc Color struct (adapted from DX9)
	UStruct* ColorS = FindObject<UStruct>(UObject::StaticClass(), TEXT("Color"));

	if (ColorS == nullptr)
		ColorS = new(UObject::StaticClass(), TEXT("Color")) UStruct(NULL);

	ColorS->PropertiesSize = sizeof(FColor);
	new(GetClass(), Name, RF_Public)UStructProperty(CPP, Offset, TEXT("Display"), CPF_Config, ColorS);

	unguard;
}

void UICBINDx11RenderDevice::AddBoolProp(const TCHAR* Name, UBOOL& InParam, ECppProperty CPP, INT Offset, UBOOL bDefaultVal)
{
	guard(UICBINDx11RenderDevice::AddBoolProp);

	new(GetClass(), Name, RF_Public)UBoolProperty(CPP, Offset, TEXT("Display"), CPF_Config);

	// Metallicafan212:	Now set the default value
	if (!GConfig->GetBool(ClsName, Name, InParam))
	{
		// Metallicafan212:	Set it
		GConfig->SetBool(ClsName, Name, bDefaultVal);
		InParam = bDefaultVal;
	}

	unguard;
}

void UICBINDx11RenderDevice::AddFloatProp(const TCHAR* Name, FLOAT& InParam, ECppProperty CPP, INT Offset, FLOAT fDefaultVal)
{
	guard(UICBINDx11RenderDevice::AddFloatProp);

	new(GetClass(), Name, RF_Public)UFloatProperty(CPP, Offset, TEXT("Display"), CPF_Config);

	// Metallicafan212:	Now set the default value
	if (!GConfig->GetFloat(ClsName, Name, InParam))
	{
		// Metallicafan212:	Set it
		GConfig->SetFloat(ClsName, Name, fDefaultVal);
		InParam = fDefaultVal;
	}

	unguard;
}

void UICBINDx11RenderDevice::AddIntProp(const TCHAR* Name, INT& InParam, ECppProperty CPP, INT Offset, INT iDefaultVal)
{
	guard(UICBINDx11RenderDevice::AddIntProp);

	new(GetClass(), Name, RF_Public)UIntProperty(CPP, Offset, TEXT("Display"), CPF_Config);

	// Metallicafan212:	Now set the default value
	if (!GConfig->GetInt(ClsName, Name, InParam))
	{
		// Metallicafan212:	Set it
		GConfig->SetInt(ClsName, Name, iDefaultVal);
		InParam = iDefaultVal;
	}

	unguard;
}

void UICBINDx11RenderDevice::ClampUserOptions()
{
	guard(UICBINDx11RenderDevice::ClampUserOptions);

	// Metallicafan212:	TODO! Add some way to automatically clamp these options!!!
	NumAFSamples			= Clamp(NumAFSamples, 0, D3D11_REQ_MAXANISOTROPY);

	ThreeDeeLineThickness	= Clamp(ThreeDeeLineThickness, 1.0f, FLT_MAX);
	OrthoLineThickness		= Clamp(OrthoLineThickness, 1.0f, FLT_MAX);

	// Metallicafan212:	TODO! Find the max value allowed!
	NumAdditionalBuffers	= Max(0, NumAdditionalBuffers);

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

	ResolutionScale = Clamp(ResolutionScale, 0.1f, 10.0f);

	// Metallicafan212:	TODO!!!! Don't allow MSAA when the resolution scale isn't 1.0

	// Metallicafan212:	We have the final clamp
	if (ResolutionScale != 1.0f)
		NumAASamples = 1;
	else
		NumAASamples = Clamp(NumAASamples, 1, static_cast<INT>(SampleCount));

	// Metallicafan212:	Now make sure it's the lower of the request (if it's odd)
	//					TODO! Rewrite this, if the user specifies 6, we return 8 due to the mod 2 not working since 6 isn't odd
	//					I'm too tired for this shit right now lol....
	if (NumAASamples > 1 && NumAASamples < static_cast<INT>(SampleCount))
	{
		NumAASamples = appPow(2.0, appCeilLogTwo(NumAASamples) - (NumAASamples % 2));
	}

	if(LastAASamples != NumAASamples)
		GLog->Logf(TEXT("DX11: Requesting %d AA samples"), NumAASamples);

	// Metallicafan212:	TODO! Hard-coded offsets to make tiles not look like ass....
	switch (NumAASamples)
	{
		case 0:
		case 1:
		{
			TileAAUVMove = -0.001f;
			break;
		}

		case 2:
		{
			TileAAUVMove = -0.001f;
			break;
		}

		case 4:
		{
			TileAAUVMove = -0.001f;//0.5f;
			break;
		}

		case 8:
		{
			TileAAUVMove = -0.001f;//0.2f;
			break;
		}
	}

	// Metallicafan212:	Set this here now
	LastGamma = Gamma;

	unguard;
}