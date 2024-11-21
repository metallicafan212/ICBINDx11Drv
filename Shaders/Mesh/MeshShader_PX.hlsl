#include "MeshShader_Common.h"

float4 PxShader(PSInput input) : SV_TARGET
{
	// Metallicafan212:	Set the alpha reject
	CurrentAlphaReject	= AlphaReject;
	
	// Metallicafan212:	TODO! Add this as a bool property
	float4 LightColor = input.color;
	if(bEnableCorrectFog)
	{
		FLOAT Scale 		= 1.0f - input.fog.w;
		LightColor.xyz    *= Scale; 
	}
	
	// Metallicafan212:	If we're selected, go black and white....
	//					This prevents issues with missing colors in a masked/alpha/translucent texture
	float4 FinalColor 		= Diffuse.SampleBias(DiffState, input.uv, 0.0f);
	
	if(bSelected)
	{
		FinalColor.xyz 	= (FinalColor.x + FinalColor.y + FinalColor.z) / 3.0f;
	}
	
#if !NO_CUSTOM_RMODES
	// Metallicafan212:	If we're doing lighting only, do just that color
	if(RendMap == REN_LightingOnly)
	{
		// Metallicafan212:	Lighting needs to be halfed to look more like it does in-game
		FinalColor.xyz = input.color.xyz / 2.0f;
		
		CLIP_PIXEL(FinalColor);
	}
	else
#endif
	{
		FinalColor *= LightColor;
		
		CLIP_PIXEL(FinalColor);
		
		FinalColor.xyz += input.fog.xyz;
		
#if DO_DISTANCE_FOG
		// Metallicafan212:	Calculate the distance fog here
		if(bDoDistanceFog)
		{
			float Fog 	= DoDistanceFog(input.origZ);
			FinalColor 	= DoPixelFog(Fog, FinalColor);
		}
#endif
	}
	
	return DoFinalColor(FinalColor, input.color);
}
