#define DO_STANDARD_BUFFER 0
#include "..\ShaderGlobals.h"

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
#include "..\ShaderGlobals.h"


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