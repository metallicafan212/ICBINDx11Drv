#include "ResScaling_Common.h"

float4 PxShader(PSInput input) : SV_TARGET
{
	float3 TexColor;
	
	if(ResolutionScale == 1.0)
	{
		// Metallicafan212:	Do normal sampling
		TexColor = Screen.SampleBias(ScreenState, input.uv, 0.0f).xyz;
	}
	else
	{
		// Metallicafan212:	Try this
		//					This is the algorithm from here: https://www.shadertoy.com/view/XsfGDn
		uint SW, SH;
		Screen.GetDimensions(SW, SH);
		
		float2 UV	= input.uv;
		UV 			= UV * SW + 0.5;
		float2	iUV	= floor(UV);
		float2 	fUV	= frac(UV);
		UV			= iUV + fUV * fUV * (3.0 - 2.0 * fUV);
		UV			= (UV - 0.5) / SW;
		
		TexColor = Screen.SampleBias(ScreenState, UV, 0.0f).xyz;
	}
	
	float4 Out;
	
	// Metallicafan212:	Determine the gamma mode
	//					Proton/wine doesn't like switches, so we're using if statements....
	/*
	if(GammaMode == GM_PerObject)
	{
		Out = float4(TexColor, 1.0f);
	}
	else
	*/
	if(GammaMode == GM_XOpenGL)
	{
		// Metallicafan212:	Use the over gamma method
		Out = XOpenGLGamma(TexColor);
	}
	else if(GammaMode == GM_DX9)
	{
		Out = DX9Gamma(TexColor, GammaOffsetRed, GammaOffsetBlue, GammaOffsetGreen);
	}
	else // Metallicafan212: TODO!
	{
		Out = float4(TexColor, 1.0f);
	}
	
	if(FrameShaderFlags & 0x2)
	{
		// Metallicafan212:	Convert to linear, since we're using a linear screen format
		Out.xyz = pow(abs(Out.xyz), 2.2);
		
		// Metallicafan212:	TODO! Configurable linear expansion value....
		if(!(FrameShaderFlags & 0x1))
		{
			Out.xyz *= 4.0f;
		}
	}
	
	// Metallicafan212:	HDR correct!
	//					TODO! Determine a better constant
	if(FrameShaderFlags & 0x1)
	{
		Out.xyz *= WhiteLevel * HDRExpansion;	
	}
	
	return Out;
}