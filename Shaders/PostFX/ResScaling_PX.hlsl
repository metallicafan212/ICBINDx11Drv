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
	
	float LocalGamma = Gamma;
	
	/*
	// Metallicafan212:	Reduce gamma by half when using the 32bpc format...
	if(!(FrameShaderFlags & 0x1) && FrameShaderFlags & 0x2)
	{
		LocalGamma *= 0.5;
	}
	*/
	
	// Metallicafan212:	Determine the gamma mode
	//					Proton/wine doesn't like switches, so we're using if statements....
	if(GammaMode == GM_XOpenGL)
	{
		// Metallicafan212:	Use the over gamma method
		Out = XOpenGLGamma(TexColor, LocalGamma);
	}
	else if(GammaMode == GM_DX9)
	{
		Out = DX9Gamma(TexColor, LocalGamma, GammaOffsetRed, GammaOffsetBlue, GammaOffsetGreen);
	}
	else // Metallicafan212: TODO!
	{
		Out = float4(TexColor, 1.0f);
	}
	
	// Metallicafan212:	Needs colorspace conversion
	if(FrameShaderFlags & 0x2)
	{
		// Metallicafan212:	HDR? To linear
		//if(FrameShaderFlags & 0x1)
		{
			// Metallicafan212:	Convert to linear, since we're using a linear screen format
			Out.xyz = pow(abs(Out.xyz), 2.2);
			
			// Metallicafan212:	Even in SDR we have to correct by the white balance!!!! Windows is auto-HDRing the SDR content, even though the colorspace is r709....
			Out.xyz *= WhiteLevel * HDRExpansion;	
		}
		/*
		// Metallicafan212:	Linear back to sRGB
		else
		{
			float3 S1 	= sqrt(Out.xyz);
			float3 S2 	= sqrt(S1);
			float3 S3 	= sqrt(S2);
			Out.xyz		= 0.662002687 * S1 + 0.684122060 * S2 - 0.323583601 * S3 - 0.0225411470 * Out.xyz;
			//Out.xyz		= 0.3310013435 * S1 + 0.34206103 * S2 - 0.1617918005 * S3 - 0.0112705735 * Out.xyz;
		}
		*/
	}
	// Metallicafan212:	Reverse the HDR stuff
	//					This is for screenshots
	else if(FrameShaderFlags & 0x4)
	{
		Out.xyz /= (WhiteLevel * HDRExpansion);
		
		// Metallicafan212:	Since our intermediate format is RGBA8, a sRGB format, we have to convert BACK to sRGB
		Out.xyz = pow(abs(Out.xyz), 1.0f / 2.2f);
	}
	
	/*
	// Metallicafan212:	HDR correct!
	//					TODO! Determine a better constant
	if(FrameShaderFlags & 0x1)
	{
		Out.xyz *= WhiteLevel * HDRExpansion;	
	}
	*/
	
	return Out;
}