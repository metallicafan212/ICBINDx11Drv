#include "..\ShaderGlobals.h"

// Metallicafan212:	If to turn on this geo shader or not 
//					I might make it a user option in the future and dynamically compile this define statement
//					This is now provided by the shader defines in the driver
//#define USE_GEO_SHADER 1

struct GSInput
{
	float4 pos 		: SV_POSITION0; 
	float2 uv		: TEXCOORD0;
	float4 color	: COLOR0; 
	float  fog		: COLOR1;
	float  origZ	: COLOR2;
};

struct PSInput 
{
	float4 pos 		: SV_POSITION0; 
	float2 uv		: TEXCOORD0;
	float4 color	: COLOR0; 
	float4 fog		: COLOR1;
	float  origZ	: COLOR2;
};			

#if USE_GEO_SHADER
#define VS_OUT GSInput
#else
#define VS_OUT PSInput
#endif