#include "ShaderGlobals.h"

struct VSInput 
{ 
	float4 pos 		: POSITION0;
	float4 uv		: TEXCOORD0;
	float4 color	: COLOR0;
	float4 fog		: COLOR1;
};

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


PSInput VertShader(VSInput input)
{	
	PSInput output = (PSInput)0;
	
	// Metallicafan212:	Set the W to 1 so matrix math works
	input.pos.w 	= 1.0f;
	
	// Metallicafan212:	Transform it out
	output.pos 		= mul(input.pos, Proj);
	output.color	= input.color;
	output.fog.xyzw = 0.0f;
	output.uv.xy	= input.uv.xy;
	
	return output;
}

float4 PxShader(PSInput input) : SV_TARGET
{	
	return float4(Screen.SampleBias(ScreenState, input.uv, 0.0f).xyz, 1.0f);
}
