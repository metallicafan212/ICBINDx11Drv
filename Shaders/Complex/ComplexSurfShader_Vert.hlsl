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
	
	#if EXTRA_VERT_INFO && !COMPLEX_SURF_MANUAL_UVs
	// Metallicafan212:	TODO! I'm lazy, just define the variables here
	float4 XAxis 		= input.XAxis;
	float4 YAxis		= input.YAxis;
	
	float4 PanScale[5];
	PanScale[0]			= input.PanScale1;
	PanScale[1]			= input.PanScale2;
	PanScale[2]			= input.PanScale3;
	PanScale[3]			= input.PanScale4;
	PanScale[4]			= input.PanScale5;
	
	float2 LightScale	= input.LFScale.xy;
	float2 FogScale		= input.LFScale.zw;
	
	#endif
	
	#if !COMPLEX_SURF_MANUAL_UVs
	// Metallicafan212:	We pre-calcuated the dots
	float UDot		= XAxis.w;
	float VDot		= YAxis.w;
	float3 X3		= XAxis;
	float3 Y3		= YAxis;
	
	float3 P3		= input.pos;
	float U			= dot(input.pos, X3) - UDot;
	float V			= dot(input.pos, Y3) - VDot;
	
	output.uv.x		= (U - PanScale[0].x) * PanScale[0].z;
	output.uv.y		= (V - PanScale[0].y) * PanScale[0].w;
	
	// Metallicafan212:	Now the lightmap
	output.lUV.x	= (U - PanScale[1].x + LightScale.x) * PanScale[1].z;
	output.lUV.y	= (V - PanScale[1].y + LightScale.y) * PanScale[1].w;
	
	// Metallicafan212:	Macro texture
	output.mUV.x	= (U - PanScale[2].x) * PanScale[2].z;
	output.mUV.y	= (V - PanScale[2].y) * PanScale[2].w;
	
	// Metallicafan212:	Fogmap
	output.fUV.x	= (U - PanScale[3].x + FogScale.x) * PanScale[3].z;
	output.fUV.y	= (V - PanScale[3].y + FogScale.y) * PanScale[3].w;
	
	// Metallicafan212:	Detail texture
	output.dUV.x	= (U - PanScale[4].x) * PanScale[4].z;
	output.dUV.y	= (V - PanScale[4].y) * PanScale[4].w;
	#else
	
	// Metallicafan212:	Pass UVs through
	output.uv		= input.uv.xy;
	output.lUV		= input.uv.zw;
	output.mUV		= input.DM.zw;
	output.fUV		= input.FX.xy;
	output.dUV.xy	= input.DM.xy;
	#endif
	
	// Metallicafan212:	Pass out the original Z
	output.dUV.z	= input.pos.z;
	
	output.origZ	= input.pos.z;
	
	return output;
}
