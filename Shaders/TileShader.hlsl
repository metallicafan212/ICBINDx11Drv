#define DO_STANDARD_BUFFER 0
#include "ShaderGlobals.h"

// Metallicafan212:	Constant buffer, but with the added tile rotation values
shared cbuffer CommonBuffer : register (b0)
{
	COMMON_VARS;
	
	// Metallicafan212:	The info we use for this specific shader
	//float4 	XAxis 		: packoffset(c11);
	//float4 	YAxis 		: packoffset(c12);
	//float4	ZAxis		: packoffset(c13);
	//float4x4	Coords	: packoffset(c11);
	float4		XAxis	: packoffset(c12);
	float4		YAxis	: packoffset(c13);
	float4		ZAxis	: packoffset(c14);
	//float4		Origin	: packoffset(c14);
	int			bDoRot	: packoffset(c15.x);
	float3		Pad3	: packoffset(c15.y);
};

// Metallicafan212:	HACK!!!! This includes this twice to define the final color function, as HLSL cannot do out of order compiling
//					The buffer variables have to be defined before they can be used
#define DO_FINAL_COLOR
#include "ShaderGlobals.h"

// Metallicafan212:	Standard texture sampler
Texture2D 	Diffuse 		: register(t0);

SamplerState DiffState;

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

PSInput VertShader(VSInput input)
{	
	PSInput output = (PSInput)0;
	
	// Metallicafan212:	Rotate this vert if we're doing rotated drawing
	if(bDoRot)
	{
		input.pos.xyz = float3(dot(input.pos, XAxis), dot(input.pos, YAxis), dot(input.pos, ZAxis));//mul(input.pos, Coords);
	}
	
	// Metallicafan212:	Set the W to 1 so matrix math works
	input.pos.w 	= 1.0f;
	
	// Metallicafan212:	According to the DX10 driver we have to swap y?????
	//output.pos.y =  -output.pos.y;
	
	// Metallicafan212:	Transform it out
	output.pos 		= mul(input.pos, Proj);
	//output.pos		= input.pos;
	//output.pos.z	= mul(float4(1, 1, input.pos.z, 1), Proj).z;
	output.color	= input.color;
	//output.fog.xyzw = 0.0f;
	output.uv.xy	= input.uv.xy;
	
	// Metallicafan212:	Do the final fog value
	output.distFog	= DoDistanceFog(output.pos);
	
	return output;
}

float4 PxShader(PSInput input) : SV_TARGET
{
	// Metallicafan212:	TODO! Texturing
	//input.color.a = 1.0f;
	//input.color.xyz = 1.0f;
	
	// Metallicafan212:	TODO! Use it so the register stay the same...
	//input.pos = input.pos * 2.0f;
	
	//return input.color + input.fog;
	//return input.color;
	float4 DiffColor = Diffuse.SampleBias(DiffState, input.uv, 0.0f) * input.color;
	
	CLIP_PIXEL(DiffColor);
	
	DiffColor = DoPixelFog(input.distFog, DiffColor);
	
	return DoFinalColor(DiffColor);
}