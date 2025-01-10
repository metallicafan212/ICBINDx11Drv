#include "..\ShaderGlobals.h"

struct PSInput 
{
	float4 pos 		: SV_POSITION0; 
	float2 uv		: TEXCOORD0;
	float4 color	: COLOR0; 
	float4 fog		: COLOR1;
	float  origZ	: COLOR2;
};

Texture2D Diffuse 			: register(t0);
SamplerState DiffState 		: register(s0);