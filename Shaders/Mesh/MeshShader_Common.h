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
Texture2D 		Diffuse 	: register(t0);
SamplerState 	DiffState	: register(s0);

// Metallicafan212: Extended support, mesh detail and macro texture support
Texture2D 		Detail		: register(t1);
SamplerState	DetailState	: register(s1);
Texture2D		Macro		: register(t2);
SamplerState	MacroState	: register(s2);

#define DETAIL_BOUND 	0x2
#define MACRO_BOUND		0x4

struct PSInput 
{
	float4 pos 		: SV_POSITION0; 
	float2 uv		: TEXCOORD0;
	float2 dUV		: TEXCOORD1;
	float2 mUV		: TEXCOORD2;
	float4 color	: COLOR0; 
	float4 fog		: COLOR1;
	float  origZ	: COLOR2;
};