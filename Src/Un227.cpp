// Metallicafan212:	227 specific extensions
#include "ICBINDx11Drv.h"

#if DX11_UNREAL_227

// Metallicafan212:	Values from XGL
//constexpr GLenum ModeList[] = { GL_LESS, GL_EQUAL, GL_LEQUAL, GL_GREATER, GL_GEQUAL, GL_NOTEQUAL, GL_ALWAYS };
BYTE UICBINDx11RenderDevice::SetZTestMode(BYTE Mode)
{
	guard(UICBINDx11RenderDevice::SetZTestMode);

	// Metallicafan212:	TODO!!!! Do nothing for now, we need to create depth states
	/*
	if (LastZMode == Mode || Mode > 6)
		return Mode;

	// Flush any pending render.
	auto CurrentProgram = ActiveProgram;
	SetProgram(No_Prog);
	SetProgram(CurrentProgram);

	glDepthFunc(ModeList[Mode]);
	BYTE Prev = LastZMode;
	LastZMode = Mode;
	return Prev;
	*/

	unguard;
}

// Metallicafan212:	Link up 227 systems to HP2's systems
//					TODO! This might be wasteful to make and destroy engine textures....
//					I might want to reevaluate turning it into a substruct that I can just have a pointer if needed
UBOOL UICBINDx11RenderDevice::PushRenderToTexture(FRenderToTexture* Tex)
{
	guard(UICBINDx11RenderDevice::PushRenderToTexture);

	// Metallicafan212:	Create a new RT texture
	UDX11RenderTargetTexture* RTTex = static_cast<UDX11RenderTargetTexture*>(CreateRenderTargetTexture(Tex->ScreenX, Tex->ScreenY));

	if (RTTex != nullptr)
	{
		RTTex->EngineRTTex = Tex;

		// Metallicafan212:	Set the target now
		SetRenderTargetTexture(RTTex);

		return 1;
	}

	return 0;

	unguard;
}

void UICBINDx11RenderDevice::PopRenderToTexture()
{
	guard(UICBINDx11RenderDevice::PopRenderToTexture);

	// Metallicafan212:	Get the top RT texture
	if (RTStack.Num())
	{
		UDX11RenderTargetTexture* RTTex = RTStack(RTStack.Num() - 1);

		RestoreRenderTarget();

		// Metallicafan212:	Now we need to bitblit it....
		//					TODO!!!!!

		// Metallicafan212:	Now destory it
		RTTex->ConditionalDestroy();
	}

	unguard;
}

// Metallicafan212:	Distance fog
//					TODO!!!! Enable distance fog in the shaders....
void UICBINDx11RenderDevice::PreDrawGouraud(FSceneNode* Frame, FFogSurf& FogSurf)
{
	guard(UICBINDx11RenderDevice::PreDrawGouraud);

	if (FogSurf.IsValid())
	{
		SetDistanceFog(1, FogSurf.FogDistanceStart, FogSurf.FogDistanceEnd, FogSurf.FogColor, 0.0f);

		// Metallicafan212:	TODO!!!! Enable modes here
		
	}
	else if (FogShaderVars.bDoDistanceFog)
		SetDistanceFog(0, 0.0f, 0.0f, FPlane(0.0f, 0.0f, 0.0f, 0.0f), 0.0f);

	unguard;
}

void UICBINDx11RenderDevice::PostDrawGouraud(FSceneNode* Frame, FFogSurf& FogSurf)
{
	guard(UOpenGLRenderDevice::PostDrawGouraud);

	SetDistanceFog(0, 0.0f, 0.0f, FPlane(0.0f, 0.0f, 0.0f, 0.0f), 0.0f);

	unguard;
}

#endif