#include "..\ShaderGlobals.h"

// Metallicafan212:	Possible texture inputs for this shader
//					This has hard inputs, but it checks what's actually bound before using them
//					Otherwise, it's bound to a blank texture.
//					It's a massive hack, but it seems to be better than using effects (at least on the FPS)
//					The order here isn't the order executed below, it's just the order uploaded to the GPU
Texture2D Diffuse 			: register(t0);
Texture2D Light				: register(t1);
Texture2D Fogmap			: register(t2);//register(t3);
Texture2D Detail			: register(t3);//register(t4);
Texture2D Macro				: register(t4);//register(t2);

// Metallicafan212:	Samplers for each texture
SamplerState DiffState 		: register(s0);
SamplerState LightState 	: register(s1);
SamplerState FogState 		: register(s2);//register(s3);
SamplerState DetailState	: register(s3);//register(s4);
SamplerState MacroState 	: register(s4);//register(s2);

// Metallicafan212:	Define shader texture values
#define DIFFUSE_BOUND 	0x1
#define LIGHT_BOUND		0x2
#define	FOG_BOUND		0x4
#define DETAIL_BOUND	0x8
#define MACRO_BOUND		0x10

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
	float4 addColor	: COLOR4;
};

struct PSOutput
{
	float4	Color	: SV_TARGET0;
	//float4	Extra	: SV_TARGET1;
};