#include "TileShader_Common.h"

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
	
#if DO_DISTANCE_FOG
	// Metallicafan212:	Calculate distance fog
	if(bDoDistanceFog)
	{
		float Fog = DoDistanceFog(input.origZ);
		DiffColor = DoPixelFog(Fog, DiffColor);
	}
	//}
#endif

#if !NO_CUSTOM_RMODES
	bRejectBW = input.bRejectBW;
#endif
	
	DiffColor = DoFinalColor(DiffColor, input.color);

	return DiffColor;
}
