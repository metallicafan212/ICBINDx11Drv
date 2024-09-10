#define DO_STANDARD_BUFFER 0
#include "ShaderGlobals.h"

// Metallicafan212:	Constant buffer, but with different mesh options
cbuffer CommonBuffer : register (START_CONST_NUM)
{
	COMMON_VARS
	
	// Metallicafan212:	TODO! More vars
	int bNoOpacity		: packoffset(c0.x);
	int3 Pad3			: packoffset(c0.y);
};

// Metallicafan212:	HACK!!!! This includes this twice to define the final color function, as HLSL cannot do out of order compiling
//					The buffer variables have to be defined before they can be used
#define DO_FINAL_COLOR
#include "ShaderGlobals.h"


#define bEnableCorrectFog bCorrectFog

// Metallicafan212:	Standard texture sampler
Texture2D Diffuse 		: register(t0);

SamplerState DiffState;

struct PSInput 
{
	float4 pos 		: SV_POSITION0; 
	float2 uv		: TEXCOORD0;
	float4 color	: COLOR0; 
	float4 fog		: COLOR1;
	float  origZ	: COLOR2;
};


PSInput VertShader(VSInput input)
{	
	PSInput output = (PSInput)0;
	
	// Metallicafan212:	Set the W to 1 so matrix math works
	input.pos.w 	= 1.0f;
	
	// Metallicafan212:	Save the original z for distance fog
	output.origZ	= input.pos.z;
	
	// Metallicafan212:	Optionally disable opacity, if it's not needed
	if(bNoOpacity)
	{
		input.color.w = 1.0f;
	}
	
	// Metallicafan212:	Transform it out
	output.pos 		= mul(input.pos, Proj);
	
	// Metallicafan212:	If we're selected, set the light color to the selection color
	if(bSelected)
	{
		output.color	= float4(SelectedColor, 1.0f);
		output.fog		= float4(0.0f, 0.0f, 0.0f, 0.0f);
	}
	// Metallicafan212:	If we're modulated, deal with the color now!
	else if(bModulated)
	{
		output.color	= float4(1.0f, 1.0f, 1.0f, 1.0f);
		output.fog		= float4(0.0f, 0.0f, 0.0f, 0.0f);
	}
	else
	{
		output.color	= input.color;
		output.fog		= input.fog;
	}
	
	// Metallicafan212:	Copy the vert info over
	output.uv.xy	= input.uv.xy;
	
	return output;
}

float4 PxShader(PSInput input) : SV_TARGET
{	
	// Metallicafan212:	TODO! Add this as a bool property
	if(bEnableCorrectFog)
	{
		FLOAT Scale 		= 1.0f - input.fog.w;
		input.color.xyz    *= Scale; 
	}
	
	// Metallicafan212:	If we're selected, go black and white....
	//					This prevents issues with missing colors in a masked/alpha/translucent texture
	float4 FinalColor 		= Diffuse.SampleBias(DiffState, input.uv, 0.0f);
	
	if(bSelected)
	{
		FinalColor.xyz 	= (FinalColor.x + FinalColor.y + FinalColor.z) / 3.0f;
	}
	
	FinalColor *= input.color;
	FinalColor.xyz += input.fog.xyz;
	
	CLIP_PIXEL(FinalColor);
	
	// Metallicafan212:	Calculate the distance fog here
	if(bDoDistanceFog)
	{
		float Fog 	= DoDistanceFog(input.origZ);
		FinalColor 	= DoPixelFog(Fog, FinalColor);
	}
	
	return DoFinalColor(FinalColor);
}
