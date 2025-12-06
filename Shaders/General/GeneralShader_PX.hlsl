#include "GeneralShader_Common.h"

float4 PxShader(PSInput input) : SV_TARGET
{
	// Metallicafan212:	Set the alpha reject
	CurrentAlphaReject	= AlphaReject;
	
	// Metallicafan212:	Diffuse texture
	float4 DiffColor = input.color;
	
	if(bTexturesBound & DIFFUSE_BOUND)
	{
		//DiffColor = Diffuse.Sample(DiffState, input.uv);
		DiffColor = SampleTexture(Diffuse, DiffState, input.uv, DIFFUSE_BOUND);
	}
	
	// Metallicafan212:	See if this needs alpha rejection		
	CLIP_PIXEL(DiffColor);
	
#if DO_DISTANCE_FOG
	// Metallicafan212:	Calculate distance fog
	if(bDoDistanceFog)
	{
		float Fog 	= DoDistanceFog(input.origZ);
		DiffColor 	= DoPixelFog(Fog, DiffColor);
	}
#endif
	
	return DoFinalColor(DiffColor, input.color);
}