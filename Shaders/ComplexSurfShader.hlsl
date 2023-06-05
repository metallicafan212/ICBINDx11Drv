#define DO_STANDARD_BUFFER 0
#include "ShaderGlobals.h"

// Metallicafan212:	Constant buffer, but with the added complex surface info
shared cbuffer CommonBuffer : register (b0)
{
	COMMON_VARS;
	
	// Metallicafan212:	The info we use for this specific shader
	float4 	XAxis 		: packoffset(c12);
	float4 	YAxis 		: packoffset(c13);
	
	float4 	PanScale[4]	: packoffset(c14);
	
	// Metallicafan212:	We need the original values passed in for the lightmap scale...
	float4 	LightScale 	: packoffset(c18);
	float 	SurfAlpha	: packoffset(c19.x);
	float3	Pad3		: packoffset(c19.y);
};

// Metallicafan212:	HACK!!!! This includes this twice to define the final color function, as HLSL cannot do out of order compiling
//					The buffer variables have to be defined before they can be used
#define DO_FINAL_COLOR
#include "ShaderGlobals.h"

// Metallicafan212:	Standard texture sampler
Texture2D Diffuse 		: register(t0);
Texture2D Light			: register(t1);
Texture2D Macro			: register(t2);
Texture2D Fogmap		: register(t3);

SamplerState DiffState : register(s0);

SamplerState LightState : register(s1);

SamplerState MacroState : register(s2);

SamplerState FogState : register(s3);

struct VSInput 
{ 
	float4 pos 		: POSITION0;
	float4 uv		: TEXCOORD0;
	float4 color	: COLOR0;
	float4 fog		: COLOR1;
	
	// Metallicafan212:	Reinterpret the last part of the vertex definition as the origin of the coords
	//float4 origin	: POSIITON1;
};

struct PSInput 
{
	float4 pos 		: SV_POSITION0; 
	float2 uv		: TEXCOORD0;
	float2 lUV		: TEXCOORD1;
	float2 mUV		: TEXCOORD2;
	float2 fUV		: TEXCOORD3;
	float4 color	: COLOR0; 
	float4 fog		: COLOR1;
	float  distFog	: COLOR2;
};


PSInput VertShader(VSInput input)
{	
	PSInput output = (PSInput)0;
	
	// Metallicafan212:	Set the W to 1 so matrix math works
	input.pos.w 	= 1.0f;
	
	// Metallicafan212:	Transform it out
	output.pos 		= mul(input.pos, Proj);
	output.color	= input.color;
	
	// Metallicafan212:	We pre-calcuated the dots
	float UDot		= XAxis.w;
	float VDot		= YAxis.w;
	float3 X3		= XAxis;
	float3 Y3		= YAxis;
	
	float3 P3		= input.pos;
	float U			= dot(input.pos, X3) - UDot;
	float V			= dot(input.pos, Y3) - VDot;
	
	output.uv.x		= (U - PanScale[0].x) * PanScale[0].z;
	output.uv.y		= (V - PanScale[0].y) * PanScale[0].w;
	
	// Metallicafan212:	Now the lightmap
	output.lUV.x	= (U - PanScale[1].x + 0.5 * LightScale.x) * PanScale[1].z;
	output.lUV.y	= (V - PanScale[1].y + 0.5 * LightScale.y) * PanScale[1].w;
	
	// Metallicafan212:	Macro texture
	output.mUV.x	= (U - PanScale[2].x) * PanScale[2].z;
	output.mUV.y	= (V - PanScale[2].y) * PanScale[2].w;
	
	// Metallicafan212:	Fogmap
	output.fUV.x	= (U - PanScale[3].x) * PanScale[3].z;
	output.fUV.y	= (V - PanScale[3].y) * PanScale[3].w;
	
	// Metallicafan212:	Do the final fog value
	output.distFog	= DoDistanceFog(output.pos);
	
	return output;
}

float4 PxShader(PSInput input) : SV_TARGET
{	
	// Metallicafan212:	TODO! Texturing
	float4 DiffColor = input.color;
	
	// Metallicafan212:	Set our alpha for lumos
	DiffColor.w *= SurfAlpha;
	
	if(bTexturesBound[0].x != 0)
	{
		float4 Diff  	= Diffuse.SampleBias(DiffState, input.uv, 0.0f);
		DiffColor.xyz  *= Diff.xyz;
		DiffColor.w	   *= Diff.w;
	}
		
	if(bTexturesBound[0].y != 0)
	{
		// Metallicafan212:	Light texture
		DiffColor.xyz *= (Light.SampleBias(LightState, input.lUV, 0.0f) * 2).xyz;
	}
		
	if(bTexturesBound[0].z)
	{
		// Metallicafan212:	Macro texture
		
	}
		
	if(bTexturesBound[0].w)
	{
		// Metallicafan212:	Fog map
		DiffColor.xyz += Fogmap.SampleBias(FogState, input.fUV, 0.0f).xyz;
	}
		
	CLIP_PIXEL(DiffColor);

	
	DiffColor = DoPixelFog(input.distFog, DiffColor);
		
	return DoFinalColor(DiffColor);
}
