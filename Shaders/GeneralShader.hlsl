#include "ShaderGlobals.h"

struct PSInput 
{
	float4 pos 		: SV_POSITION0; 
	float2 uv		: TEXCOORD0;
	float4 color	: COLOR0; 
	float4 fog		: COLOR1;
	float  distFog	: COLOR2;
};


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
	
	// Metallicafan212:	Do the final fog value
	output.distFog	= DoDistanceFog(output.pos.z);
	
	return output;
}

float4 PxShader(PSInput input) : SV_TARGET
{	
	// Metallicafan212:	TODO! Texturing		
	CLIP_PIXEL(input.color);
	
	input.color = DoPixelFog(input.distFog, input.color);
		
	//return input.color + input.fog;
	return DoFinalColor(input.color);
}
