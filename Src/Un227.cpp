// Metallicafan212:	227 specific extensions
#include "ICBINDx11Drv.h"

#if 1//DX11_UNREAL_227

// Metallicafan212:	Values from XGL
//constexpr GLenum ModeList[] = { GL_LESS, GL_EQUAL, GL_LEQUAL, GL_GREATER, GL_GEQUAL, GL_NOTEQUAL, GL_ALWAYS };

constexpr D3D11_COMPARISON_FUNC CompModes[] = { D3D11_COMPARISON_LESS, D3D11_COMPARISON_EQUAL, D3D11_COMPARISON_LESS_EQUAL, D3D11_COMPARISON_GREATER, D3D11_COMPARISON_GREATER_EQUAL, D3D11_COMPARISON_NOT_EQUAL, D3D11_COMPARISON_ALWAYS };

BYTE UICBINDx11RenderDevice::SetZTestMode(BYTE Mode)
{
	guard(UICBINDx11RenderDevice::SetZTestMode);

	// Metallicafan212:	Create the bitpacked key
	INT Key		= ((INT)Mode) << 2;

	// Metallicafan212:	Add on the depth stencil modifiers, it's probably not needed to and it with the known keys, but better safe than sorry
	Key		   |= (DepthStencilFlags & (DS_NoDepth | DS_NoZWrite));

	//if (Mode == CurrentZTestMode)
	if(Key == CurrentZTestIndex || Mode >= ZTEST_MAX)
	{
		return Mode;
	}

	// Metallicafan212:	Flush any rendering 
	EndBuffering();

	check(Key < ARRAY_COUNT(DepthStencilStates));

	// Metallicafan212:	Keep the previous
	BYTE OldState		= CurrentZTestMode;

	// Metallicafan212:	Now update our current
	CurrentZTestMode	= Mode;
	CurrentZTestIndex	= Key;

	// Metallicafan212:	Check if we have a valid depth stencil state
	ID3D11DepthStencilState* State = DepthStencilStates[Key];

	if (State == nullptr)
	{
		// Metallicafan212:	Setup the depth stencil state
		//					From a MSDN page https://learn.microsoft.com/en-us/windows/win32/direct3d11/d3d10-graphics-programming-guide-depth-stencil
		D3D11_DEPTH_STENCIL_DESC dsDesc;

		// Depth test parameters
		dsDesc.DepthEnable					= TRUE;
		dsDesc.DepthWriteMask				= (Key & 0x1) ? D3D11_DEPTH_WRITE_MASK_ZERO : D3D11_DEPTH_WRITE_MASK_ALL;

		dsDesc.DepthEnable					= (Key & 0x2) ? FALSE : TRUE;
		dsDesc.StencilEnable				= (Key & 0x2) ? FALSE : TRUE;

		dsDesc.DepthFunc					= CompModes[Mode];

		// Stencil test parameters
		dsDesc.StencilEnable				= FALSE;//TRUE;
		dsDesc.StencilReadMask				= 0xFF;
		dsDesc.StencilWriteMask				= 0xFF;

		// Stencil operations if pixel is front-facing
		dsDesc.FrontFace.StencilFailOp		= D3D11_STENCIL_OP_KEEP;
		dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		dsDesc.FrontFace.StencilPassOp		= D3D11_STENCIL_OP_KEEP;
		dsDesc.FrontFace.StencilFunc		= D3D11_COMPARISON_NEVER;

		// Stencil operations if pixel is back-facing
		dsDesc.BackFace.StencilFailOp		= D3D11_STENCIL_OP_KEEP;
		dsDesc.BackFace.StencilDepthFailOp	= D3D11_STENCIL_OP_DECR;
		dsDesc.BackFace.StencilPassOp		= D3D11_STENCIL_OP_KEEP;
		dsDesc.BackFace.StencilFunc			= D3D11_COMPARISON_NEVER;

		// Metallicafan212:	Now create it
		HRESULT hr = m_D3DDevice->CreateDepthStencilState(&dsDesc, &State);

		ThrowIfFailed(hr);

		// Metallicafan212:	File it away
		DepthStencilStates[Key] = State;
	}

	// Metallicafan212:	Set it
	m_RenderContext->OMSetDepthStencilState(State, 0);

	return OldState;

	unguard;
}
#endif

#if DX11_UNREAL_227
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

	// Metallicafan212:	Only disable if fogging is _actually_ set....
	if (FogShaderVars.bDoDistanceFog)
	{
		SetDistanceFog(0, 0.0f, 0.0f, FPlane(0.0f, 0.0f, 0.0f, 0.0f), 0.0f);
	}

	unguard;
}

#endif