#define DO_STANDARD_BUFFER 0
#include "ShaderGlobals.h"

// Metallicafan212:	Constant buffer, but with different mesh options
cbuffer CommonBuffer : register (START_CONST_NUM)
{
	COMMON_VARS
	
	// Metallicafan212:	TODO! More vars
	int bNoOpacity		: packoffset(c4.x);
	int3 Pad3			: packoffset(c4.y);
};

// Metallicafan212:	HACK!!!! This includes this twice to define the final color function, as HLSL cannot do out of order compiling
//					The buffer variables have to be defined before they can be used
#define DO_FINAL_COLOR
#include "ShaderGlobals.h"


#define bEnableCorrectFog 1

// Metallicafan212:	Standard texture sampler
Texture2D Diffuse 		: register(t0);

SamplerState DiffState;

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
	
	// Metallicafan212:	Optionally disable opacity, if it's not needed
	if(bNoOpacity)
	{
		input.color.w = 1.0f;
	}
	
	// Metallicafan212:	Transform it out
	output.pos 		= mul(input.pos, Proj);
	
	// Metallicafan212:	Copy the vert info over
	output.uv.xy	= input.uv.xy;
	output.color	= input.color;
	output.fog		= input.fog;
	
	// Metallicafan212:	Do the final fog value
	output.distFog	= DoDistanceFog(output.pos);
	
	return output;
}

float4 PxShader(PSInput input) : SV_TARGET
{	
	// Metallicafan212:	TODO! Texturing
	//input.color.a = 1.0f;
	
	// Metallicafan212:	TODO! Use it so the register stay the same...
	//input.pos = input.pos * 2.0f;
	
	// Metallicafan212:	TODO! Add this as a bool property
	if(bEnableCorrectFog)
	{
		FLOAT Scale 		= 1.0f - input.fog.w;
		input.color.xyz    *= Scale; 
	}
	
	//return input.color + input.fog;
	float4 FinalColor = (DoGammaCorrection(Diffuse.SampleBias(DiffState, input.uv, 0.0f)) * input.color);
	FinalColor.xyz += input.fog.xyz;
	
	CLIP_PIXEL(FinalColor);
	
	FinalColor = DoPixelFog(input.distFog, FinalColor);
	
	return DoFinalColor(FinalColor);
}
