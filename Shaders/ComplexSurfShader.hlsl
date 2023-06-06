#define DO_STANDARD_BUFFER 0
#include "ShaderGlobals.h"

// Metallicafan212:	Constant buffer, but with the added complex surface info
shared cbuffer CommonBuffer : register (b0)
{
	COMMON_VARS;
	
	// Metallicafan212:	The info we use for this specific shader
	float4 	XAxis 		: packoffset(c12);
	float4 	YAxis 		: packoffset(c13);
	
	float4 	PanScale[5]	: packoffset(c14);
	
	// Metallicafan212:	We need the original values passed in for the lightmap scale...
	float4 	LightScale 	: packoffset(c19);
	float 	SurfAlpha	: packoffset(c20.x);
	float3	Pad3		: packoffset(c20.y);
};

// Metallicafan212:	HACK!!!! This includes this twice to define the final color function, as HLSL cannot do out of order compiling
//					The buffer variables have to be defined before they can be used
#define DO_FINAL_COLOR
#include "ShaderGlobals.h"

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

struct VSInput 
{ 
	float4 pos 		: POSITION0;
	float4 uv		: TEXCOORD0;
	float4 color	: COLOR0;
	float4 fog		: COLOR1;
};

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
	float  distFog	: COLOR2;
	float2 detVars	: COLOR3;
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
	
	// Metallicafan212:	Detail texture
	output.dUV.x	= (U - PanScale[4].x) * PanScale[4].z;
	output.dUV.y	= (V - PanScale[4].y) * PanScale[4].w;
	
	// Metallicafan212:	Pass out the original Z
	output.dUV.z	= input.pos.z;
	
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
	
	// Metallicafan212:	Diffuse texture
	if(bTexturesBound[0].x != 0)
	{
		float4 Diff  	= Diffuse.SampleBias(DiffState, input.uv, 0.0f);
		DiffColor.xyz  *= Diff.xyz;
		DiffColor.w	   *= Diff.w;
	}
	
	// Metallicafan212:	Detail texture
	//					We're applying this now so that when detail textures fade in, they don't reduce the lightmap as much
	//					TODO! Using the vars from DX7. Allow the user to specify this!!!!
	if(bTexturesBound[1].x && input.dUV.z < 380.0f)
	{
		// Metallicafan212:	Sample it
		float3 Det = Detail.SampleBias(DetailState, input.dUV.xy, 0.0f).xyz;
		
		// Metallicafan212:	Now lerp it
		float alpha = input.dUV.z / 380.0f;
		Det = lerp(alpha, float3(1.0f, 1.0f, 1.0f), Det);
		
		// Metallicafan212:	Average the color
		//					TODO! Should we actually be doing this???
		Det.xyz = (Det.x + Det.y + Det.z) / 3.0f;
		
		// Metallicafan212:	Now add it to the image
		//					When there's no detail texture, this operation effectively returns nothing
		DiffColor.xyz = (DiffColor.xyz * Det);
		DiffColor.xyz += DiffColor.xyz;
		DiffColor.xyz /= 2.0f;
	}
	
	// Metallicafan212:	Macro texture
	//					This just modulates the color, like the lightmap
	if(bTexturesBound[0].z)
	{
		DiffColor.xyz *= (Macro.SampleBias(MacroState, input.mUV, 0.0f)).xyz;
	}
	
	// Metallicafan212:	Lightmap
	//					TODO! Allow the user to specify the lightmap multiplication (some people like one-x scaling)
	if(bTexturesBound[0].y != 0)
	{
		DiffColor.xyz *= (Light.SampleBias(LightState, input.lUV, 0.0f) * 2).xyz;
	}
	
	// Metallicafan212:	Fog map
	if(bTexturesBound[0].w)
	{
		DiffColor.xyz += Fogmap.SampleBias(FogState, input.fUV, 0.0f).xyz;
	}
	
	// Metallicafan212:	TODO! This also sets the selection color for the editor! This should be re-evaluated
	CLIP_PIXEL(DiffColor);

	
	DiffColor = DoPixelFog(input.distFog, DiffColor);
		
	return DoFinalColor(DiffColor);
}
