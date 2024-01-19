#include "ShaderGlobals.h"

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
	
	return float4(pow(In, float3(InvGamma, InvGamma, InvGamma)), 1.0f);
}

float4 DX9Gamma(float3 In, float r, float g, float b)
{	
	// Metallicafan212:	Colorize each part of the pixel, using the separate channel gamma controls
	In.x = pow(In.x, r);
	In.y = pow(In.y, g);
	In.z = pow(In.z, b);
	
	return float4(In, 1.0f);
}

PSInput VertShader(VSInput input)
{	
	PSInput output = (PSInput)0;
	output.pos 		= input.pos;
	output.uv.xy	= input.uv.xy;
	
	return output;
}

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
	if(GammaMode == GM_PerObject)
	{
		Out = float4(TexColor, 1.0f);
	}
	else if(GammaMode == GM_XOpenGL)
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
	
	// Metallicafan212:	HDR correct!
	//					TODO! Determine a better constant
	if(bHDR)
	{
		//Out.xyz = pow(Out.xyz, float3(2.2f, 2.2f, 2.2f)) * HDRExpansion;
		//TexColor = SRGBToRec202(TexColor);//pow(TexColor, float3(2.2f, 2.2f, 2.2f)) * HDRExpansion;
		//Out.xyz = SRGBToRec2020(Out.xyz) * WhiteLevel; //* HDRExpansion;
		
		// Metallicafan212:	Convert to linear, since we're using a linear screen format
		Out.xyz = pow(Out.xyz, 2.2);
		
		Out.xyz *= WhiteLevel * HDRExpansion;	
	}
	
	return Out;
}

#if 0
// Metallicafan212:	Compute shader version (slow!!!!)
RWTexture2D<float4>	Out		: register(u0);

float GetColor(float4 InSamp, float2 InFrac, float4 Comp)
{
	//InSamp 		*= Comp;
	
	InFrac 		*= Comp;
	
	float Top 	= lerp(InSamp.w, InSamp.z, InFrac.x);
	float Bot	= lerp(InSamp.x, InSamp.y, InFrac.x);
	
	// Metallicafan212:	Final result
	return lerp(Top, Bot, InFrac.y);
}

// Metallicafan212:	Resolve this pixel based on the source
//					Input coords is the destination size
[numthreads(32, 32, 1)]
void CSMain( uint3 pixelID : SV_DispatchThreadID )
{
	float4 Result = float4(0, 0, 0, 1.0);
	
	// Metallicafan212:	Get the source size
	uint W, SW, H, SH;
	
	Screen.GetDimensions(SW, SH);
	Out.GetDimensions(W, H);
	
	// Metallicafan212:	This method looks best when the screen res is a perfect multiple
	if(ResolutionScale == floor(ResolutionScale))
	{
		float2 UV 		= float2(pixelID.x / float(W), pixelID.y / float(H));
		float3 SS		= float3(SW, SH, 0.0);
		float2 Texel	= float2(1.0 / W, 1.0 / H);
		
		// Metallicafan212:	Sample 9 pixels!!!
		//					First is -0.5, -0.5, then 0.0, -0.5, etc
		float2 	Offset 		= (UV * float2(W, H)) - 0.5;
		float2 	OffFrac		= float2(0, 0);//frac(Offset);
		
		float  	Step		= (1.0f / (ResolutionScale - 1));
		int		NumSteps 	= floor(ResolutionScale);
		
		for(int y = 0; y < NumSteps; y++)
		{
			float2 LocalOffset = Offset;
			
			for(int x = 0; x < NumSteps; x++)
			{
				OffFrac  = frac(LocalOffset);
				
				// Metallicafan212:	Gather the result
				float4 C		= Screen.Gather(ScreenState, UV);
				Result.x		+= GetColor(Screen.GatherRed(ScreenState, UV), OffFrac, C);
				Result.y		+= GetColor(Screen.GatherGreen(ScreenState, UV), OffFrac, C);
				Result.z		+= GetColor(Screen.GatherBlue(ScreenState, UV), OffFrac, C);
				
				
				// Metallicafan212:	Increment
				LocalOffset.x 	+= Step;
			}
			
			Offset.y += Step;
		}
		
		Result.xyz /= pow(ResolutionScale, 2.0);
	}
	else
	{
		// Metallicafan212:	Try this
		float2 UV	= float2(pixelID.x / float(W), pixelID.y / float(H));
		UV 			= UV * SW + 0.5;
		float2	iUV	= floor(UV);
		float2 	fUV	= frac(UV);
		UV			= iUV + fUV * fUV * (3.0 - 2.0 * fUV);
		UV			= (UV - 0.5) / SW;
		
		Result.xyz	= Screen.Load(float3(UV * float2(SW, SH), 0));
		
		/*
		float2	OffFrac = float2(1.0, 1.0);
		
		float4 C		= Screen.Gather(ScreenState, UV);
		Result.x		+= GetColor(Screen.GatherRed(ScreenState, UV), OffFrac, C);
		Result.y		+= GetColor(Screen.GatherGreen(ScreenState, UV), OffFrac, C);
		Result.z		+= GetColor(Screen.GatherBlue(ScreenState, UV), OffFrac, C);
		*/
	}
	
	/*
	// Metallicafan212:	Now a half offset
	float2 Offset 	= (UV * float2(SW, SH)) - 0.5; //+ 0.5;
	float2 OffFrac  = frac(Offset);
	
	// Metallicafan212:	Move to the middle
	//UV += (0.5 - OffFrac) * Texel;
	
	// Metallicafan212:	Now do bilinear filtering
	// 					Gather each color
	Result.x		= GetColor(Screen.GatherRed(ScreenState, UV), OffFrac);
	Result.y		= GetColor(Screen.GatherGreen(ScreenState, UV), OffFrac);
	Result.z		= GetColor(Screen.GatherBlue(ScreenState, UV), OffFrac);
	
	// Metallicafan212:	Pixel 1
	Offset.xy		+= 1.0;
	OffFrac  		= frac(Offset);
	
	Result.x		+= GetColor(Screen.GatherRed(ScreenState, UV), OffFrac);
	Result.y		+= GetColor(Screen.GatherGreen(ScreenState, UV), OffFrac);
	Result.z		+= GetColor(Screen.GatherBlue(ScreenState, UV), OffFrac);
	
	Result.xyz		/= 2.0f;
	*/
	
	// Metallicafan212:	Gamma correct it
	//float OverGamma = 1.0f / Gamma;
	//Result	= float4(pow(Result, float3(OverGamma, OverGamma, OverGamma)), 1.0f);
	
	//Out[pixelID.xy]	= Result;
	Out[pixelID.xy] = float4(Result.xyz, 1.0f);
}
#endif
