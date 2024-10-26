#if !EXTRA_VERT_INFO
#define DO_STANDARD_BUFFER 0
#include "..\ShaderGlobals.h"

// Metallicafan212:	Constant buffer, but with the added complex surface info
cbuffer CommonBuffer : register (START_CONST_NUM)
{
	COMMON_VARS
	// Metallicafan212:	The info we use for this specific shader
	float4 	XAxis 		: packoffset(c0);
	float4 	YAxis 		: packoffset(c1);
	
	float4 	PanScale[5]	: packoffset(c2);
	
	// Metallicafan212:	We need the original values passed in for the lightmap scale...
	float2 	LightScale 	: packoffset(c7.x);
	
	// Metallicafan212:	And for the fogmap...
	float2 	FogScale	: packoffset(c8.z);
	
};

// Metallicafan212:	HACK!!!! This includes this twice to define the final color function, as HLSL cannot do out of order compiling
//					The buffer variables have to be defined before they can be used
#define DO_FINAL_COLOR
#include "..\ShaderGlobals.h"
#else
#include "..\ShaderGlobals.h"
#endif

// Metallicafan212:	Possible texture inputs for this shader
//					This has hard inputs, but it checks what's actually bound before using them
//					Otherwise, it's bound to a blank texture.
//					It's a massive hack, but it seems to be better than using effects (at least on the FPS)
//					The order here isn't the order executed below, it's just the order uploaded to the GPU
Texture2D Diffuse 			: register(t0);
Texture2D Light				: register(t1);
Texture2D Macro				: register(t2);
Texture2D Fogmap			: register(t3);
Texture2D Detail			: register(t4);

// Metallicafan212:	Samplers for each texture
SamplerState DiffState 		: register(s0);
SamplerState LightState 	: register(s1);
SamplerState MacroState 	: register(s2);
SamplerState FogState 		: register(s3);
SamplerState DetailState	: register(s4);

struct PSInput 
{
	float4 pos 		: SV_POSITION0; 
	float2 uv		: TEXCOORD0;
	float2 lUV		: TEXCOORD1;
	float2 mUV		: TEXCOORD2;
	float2 fUV		: TEXCOORD3;
	float4 dUV		: TEXCOORD4;
	float4 color	: COLOR0; 
	float4 fog		: COLOR1;
	float  origZ	: COLOR2;
	float2 detVars	: COLOR3;
};

struct PSOutput
{
	float4	Color	: SV_TARGET0;
	//float4	Extra	: SV_TARGET1;
};