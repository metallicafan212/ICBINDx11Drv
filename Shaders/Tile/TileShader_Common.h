#define DO_STANDARD_BUFFER 0
#include "..\ShaderGlobals.h"


// Metallicafan212:	Constant buffer, but with the added tile rotation values
cbuffer CommonBuffer : register (START_CONST_NUM)
{
	COMMON_VARS
	
	// Metallicafan212:	The info we use for this specific shader
	float4		XAxis		: packoffset(c0);
	float4		YAxis		: packoffset(c1);
	float4		ZAxis		: packoffset(c2);
	int			bDoRot		: packoffset(c3.x);
	int			bDoUVHack	: packoffset(c3.y);
	float2		Pad3		: packoffset(c3.z);
};

// Metallicafan212:	HACK!!!! This includes this twice to define the final color function, as HLSL cannot do out of order compiling
//					The buffer variables have to be defined before they can be used
#define DO_FINAL_COLOR
#include "..\ShaderGlobals.h"

// Metallicafan212:	Standard texture sampler
Texture2D 	Diffuse 		: register(t0);

SamplerState DiffState;

struct PSInput 
{
	float4 	pos 					: SV_POSITION0; 
	float2 	uv						: TEXCOORD0;
	float4 	color					: COLOR0; 
	float4 	fog						: COLOR1;
	float 	origZ					: COLOR2;
#if !NO_CUSTOM_RMODES
	int   	bRejectBW				: COLOR3;
#endif
};

// Metallicafan212:	Directly ported from XOpenGL
float Median(float3 color)
{
    return max(min(color.x, color.y), min(max(color.x, color.y), color.z));
}

float ScreenPxRange()
{
    // Metallicafan212:	TODO! Scaling for perspective
    return 2.0;
}