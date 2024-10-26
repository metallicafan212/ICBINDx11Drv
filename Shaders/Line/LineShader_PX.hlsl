#include "Line_Common.h"

float4 PxShader(PSInput input) : SV_TARGET
{
	// Metallicafan212:	Set the alpha reject
	CurrentAlphaReject	= AlphaReject;
	
	CLIP_PIXEL(input.color);
	
#if !NO_CUSTOM_RMODES
	// Metallicafan212:	Calculate distance fog
	if(bDoDistanceFog)
	{
		float Fog	= DoDistanceFog(input.origZ);
		input.color = DoPixelFog(Fog, input.color);
	}
#endif
	
	return DoFinalColor(input.color, input.color);
}