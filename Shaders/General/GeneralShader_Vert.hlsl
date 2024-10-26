#include "GeneralShader_Common.h"

PSInput VertShader(VSInput input)
{	
	PSInput output = (PSInput)0;
	
	// Metallicafan212:	Set the W to 1 so matrix math works
	input.pos.w 	= 1.0f;
	
	// Metallicafan212:	Save the original Z for distance fog
	output.origZ	= input.pos.z;
	
	// Metallicafan212:	Transform it out
	output.pos 		= mul(input.pos, Proj);
	output.color	= input.color;
	output.fog.xyzw = 0.0f;
	output.uv.xy	= input.uv.xy;
	
	return output;
}


