// Metallicafan212:	Due to proton being hella weird, I have to split the shader files
#include "Line_Common.h"

VS_OUT VertShader(VSInput input)
{
	VS_OUT output = (VS_OUT)0;
	
	// Metallicafan212:	Set the W to 1 so matrix math works
	input.pos.w 	= 1.0f;
	
	// Metallicafan212:	Save the original Z for distance fog calculations
	output.origZ	= input.pos.z;
	
	// Metallicafan212:	Transform it out
	output.pos 		= mul(input.pos, Proj);
	
	// Metallicafan212:	Copy the vert info over
	output.uv.xy	= input.uv.xy;
	output.color	= input.color;
	
	// Metallicafan212:	I've embedded the line size into the fog.x
	output.fog		= input.fog.x;
	
	return output;
}