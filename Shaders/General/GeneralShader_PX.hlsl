#include "GeneralShader_Common.h"

float4 PxShader(PSInput input) : SV_TARGET
{
	// Metallicafan212:	Set the alpha reject
	CurrentAlphaReject	= AlphaReject;
	
	// Metallicafan212:	See if this needs alpha rejection		
	CLIP_PIXEL(input.color);
	
#if DO_DISTANCE_FOG
	// Metallicafan212:	Calculate distance fog
	if(bDoDistanceFog)
	{
		float Fog 	= DoDistanceFog(input.origZ);
		input.color = DoPixelFog(Fog, input.color);
	}
#endif
	
	return DoFinalColor(ConvertColorspace(input.color), input.color);
}