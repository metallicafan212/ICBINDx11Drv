#define DO_STANDARD_BUFFER 0
#include "ShaderGlobals.h"

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
#include "ShaderGlobals.h"

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

PSInput VertShader(VSInput input)
{
	PSInput output = (PSInput)0;
	
#if !NO_CUSTOM_RMODES
	// Metallicafan212:	Rotate this vert if we're doing rotated drawing
	if(bDoRot)
	{
		input.pos.xyz = float3(dot(input.pos, XAxis), dot(input.pos, YAxis), dot(input.pos, ZAxis));//mul(input.pos, Coords);
	}
	
	// Metallicafan212:	TODO! If this is a UI tile, don't do the HP2 specific black and white effect
	//					Should make this configurable....
	output.bRejectBW	= input.pos.z <= 1.0f;
#endif
	
	// Metallicafan212:	Save the original Z for distance fog calculations
	output.origZ		= input.pos.z;
	
	// Metallicafan212:	Set the W to 1 so matrix math works
	input.pos.w 		= 1.0f;
	
	// Metallicafan212:	Transform it out
	output.pos 			= mul(input.pos, Proj);
	output.color		= input.color;
	output.uv.xy		= input.uv.xy;
	
	// Metallicafan212:	Do the final fog value
	//output.distFog		= DoDistanceFog(output.pos.z);
	
	return output;
}

#if PIXEL_SHADER
float4 PxShader(PSInput input) : SV_TARGET
{
	// Metallicafan212:	See if we should use the normal alpha reject or not...
	if(input.origZ <= 1.0f && (ShaderFlags & SF_AlphaEnabled) && AlphaReject != AltAlphaReject)
	{
		// Metallicafan212:	The only way they differ is if masked is enabled, so use the alternative reject in this case...
		CurrentAlphaReject = AltAlphaReject;
	}
	else
	{
		// Metallicafan212:	Set the alpha reject
		CurrentAlphaReject	= AlphaReject;
	}
	
	// Metallicafan212:	If we're selected, go black and white....
	//					This prevents issues with missing colors in a masked/alpha/translucent texture
	float4 DiffColor 		= Diffuse.SampleBias(DiffState, input.uv, 0.0f);
	
	if(bSelected)
	{
		DiffColor.xyz 	= (DiffColor.x + DiffColor.y + DiffColor.z) / 3.0f;
	}
	
	/*
	// Metallicafan212:	If we're doing lighting only, do just that color
	if(RendMap == REN_LightingOnly)
	{
		DiffColor.xyz = input.color.xyz;
		
		// Metallicafan212:	Do alpha rejecting
		//					TODO! This also sets the global selection color for the editor!
		//					Reevaluate how we do this!!!!
		CLIP_PIXEL(DiffColor);
	}
	else
	{
	*/
	DiffColor *= input.color;
	
	// Metallicafan212:	Do alpha rejecting
	//					TODO! This also sets the global selection color for the editor!
	//					Reevaluate how we do this!!!!
	CLIP_PIXEL(DiffColor);
	
#if !NO_CUSTOM_RMODES
	// Metallicafan212:	Calculate distance fog
	if(bDoDistanceFog)
	{
		float Fog = DoDistanceFog(input.origZ);
		DiffColor = DoPixelFog(Fog, DiffColor);
	}
	//}
	
	bRejectBW = input.bRejectBW;
#endif
	
	DiffColor = DoFinalColor(DiffColor, input.color);

	return DiffColor;
}
#endif