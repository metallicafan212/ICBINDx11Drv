#include "ShaderGlobals.h"

struct PSInput 
{
	float4 pos 		: SV_POSITION0; 
	float2 uv		: TEXCOORD0;
	float4 color	: COLOR0; 
	float4 fog		: COLOR1;
	float  origZ	: COLOR2;
};

// Metallicafan212:	Multiple outputs, as we need to output to a screen texture to later apply a decal on-screen
struct PSOutput
{
	
};

PSInput VertShader(VSInput input)
{	
	PSInput output = (PSInput)0;
	
	// Metallicafan212:	Set the W to 1 so matrix math works
	input.pos.w 	= 1.0f;
	
	// Metallicafan212:	Save the original Z for distance fog
	output.origZ	= input.pos.z;
	
	// Metallicafan212:	Transform it out
	output.pos 		= mul(input.pos, Proj);
	output.color	= input.color;
	output.fog.xyzw = 0.0f;
	output.uv.xy	= input.uv.xy;
	
	return output;
}

float4 PxShader(PSInput input) : SV_TARGET
{	
	// Metallicafan212:	See if this needs alpha rejection		
	CLIP_PIXEL(input.color);
	
#if !NO_CUSTOM_RMODES
	// Metallicafan212:	Calculate distance fog
	if(bDoDistanceFog)
	{
		float Fog 	= DoDistanceFog(input.origZ);
		input.color = DoPixelFog(Fog, input.color);
	}
#endif
	
	return DoFinalColor(input.color, input.color);
}
