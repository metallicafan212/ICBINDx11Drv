#include "ResScaling_Common.h"

PSInput VertShader(VSInput input)
{	
	PSInput output = (PSInput)0;
	output.pos 		= input.pos;
	output.uv.xy	= input.uv.xy;
	
	return output;
}
