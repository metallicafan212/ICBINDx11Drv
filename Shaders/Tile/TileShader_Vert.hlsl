#include "TileShader_Common.h"

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