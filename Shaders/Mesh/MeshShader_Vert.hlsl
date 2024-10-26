#include "MeshShader_Common.h"

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
	
	// Metallicafan212:	Save the original z for distance fog
	output.origZ	= input.pos.z;
	
	// Metallicafan212:	Optionally disable opacity, if it's not needed
	if(bNoOpacity)
	{
		input.color.w = 1.0f;
	}
	
	// Metallicafan212:	Transform it out
	output.pos 		= mul(input.pos, Proj);
	
	// Metallicafan212:	If selection is active, just output the input color
	if(bDoSelection)
	{
		output.color 	= input.color;
		output.fog		= float4(0.0f, 0.0f, 0.0f, 0.0f);
	}
	// Metallicafan212:	If we're selected, set the light color to the selection color
	else if(bSelected)
	{
		output.color	= float4(SelectedColor, 1.0f);
		output.fog		= float4(0.0f, 0.0f, 0.0f, 0.0f);
	}
	// Metallicafan212:	If we're modulated, deal with the color now!
	else if(ShaderFlags & SF_Modulated)
	{
		output.color	= float4(1.0f, 1.0f, 1.0f, 1.0f);
		output.fog		= float4(0.0f, 0.0f, 0.0f, 0.0f);
	}
	else
	{
		output.color	= input.color;
		output.fog		= input.fog;
	}
	
	// Metallicafan212:	Copy the vert info over
	output.uv.xy	= input.uv.xy;
	
	return output;
}