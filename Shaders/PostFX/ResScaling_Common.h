#include "..\ShaderGlobals.h"

struct PSInput 
{
	float4 pos 		: SV_POSITION0; 
	float2 uv		: TEXCOORD0;
	float4 color	: COLOR0; 
	float4 fog		: COLOR1;
	float  distFog	: COLOR2;
};

// Metallicafan212:	We're sampling from the screen lol
Texture2D Screen 			: register(t0);
SamplerState ScreenState	: register(s0);

// Metallicafan212:	Gamma correction functions
float4 XOpenGLGamma(float3 In)
{
	// Metallicafan212:	TODO! Branching might be more expensive than just doing the math, might want to test
	if(Gamma == 1.0f)
	{
		return float4(In, 1.0f);
	}
	
	float InvGamma = 1.0f / (Gamma);
	
	return float4(pow(abs(In), float3(InvGamma, InvGamma, InvGamma)), 1.0f);
}

float4 DX9Gamma(float3 In, float r, float g, float b)
{	
	// Metallicafan212:	Colorize each part of the pixel, using the separate channel gamma controls
	float3 Exp = float3(r, g, b);
	//In.x = pow(abs(In.x), r);
	//In.y = pow(abs(In.y), g);
	//In.z = pow(abs(In.z), b);
	In.xyz = pow(abs(In.xyz), Exp);
	
	return float4(In, 1.0f);
}