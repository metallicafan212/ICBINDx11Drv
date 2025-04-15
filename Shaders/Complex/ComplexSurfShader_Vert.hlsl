#include "ComplexSurfShader_Common.h"

PSInput VertShader(VSInput input)
{
	PSInput output = (PSInput)0;
	
	// Metallicafan212:	Set the W to 1 so matrix math works
	input.pos.w 		= 1.0f;
	
#if !NO_CUSTOM_RMODES
	// Metallicafan212:	If we're rendering normals, calculate them
	if(RendMap == REN_Normals)
	{
		float4 U 		= input.pos.z - input.pos.x;
		float4 V 		= input.pos.y - input.pos.x;
		output.color 	= float4((U.y * V.z) - (U.z * V.y), (U.z * V.x) - (U.x * V.z), (U.x * V.y) - (U.y * V.x), 1.0f);
	}
#endif
	
	// Metallicafan212:	Transform it out
	output.pos 			= mul(input.pos, Proj);
	output.color		= input.color;
	
	// Metallicafan212:	Mix in the vertex color from the fog value
	output.color.xyz   += (input.fog.xyz * input.fog.w);
	
	// Metallicafan212:	Pass UVs through
	output.uv		= input.uv.xy;
	output.lUV		= input.uv.zw;
	output.mUV		= input.DM.zw;
	output.fUV		= input.FX;
	output.dUV.xy	= input.DM.xy;
	
	// Metallicafan212:	Pass out the original Z
	output.dUV.z	= input.pos.z;
	
	output.origZ	= input.pos.z;
	
	output.addColor	= input.addColor;
	
	return output;
}
