#define DO_STANDARD_BUFFER 0
#include "ShaderGlobals.h"

// Metallicafan212:	If to turn on this geo shader or not 
//					I might make it a user option in the future and dynamically compile this define statement
#define USE_GEO_SHADER 1

// Metallicafan212:	Constant buffer, but with the added line stuff
shared cbuffer CommonBuffer : register (b0)
{
	COMMON_VARS;
	
	// Metallicafan212:	The info we use for this specific shader
	float2 	ViewSize	: packoffset(c12);
	float2	Pad2		: packoffset(c12.z);
};

// Metallicafan212:	HACK!!!! This includes this twice to define the final color function, as HLSL cannot do out of order compiling
//					The buffer variables have to be defined before they can be used
#define DO_FINAL_COLOR
#include "ShaderGlobals.h"

struct VSInput 
{ 
	float4 pos 		: POSITION0;
	float4 uv		: TEXCOORD0;
	float4 color	: COLOR0;
	float4 fog		: COLOR1;
};

struct GSInput
{
	float4 pos 		: SV_POSITION0; 
	float2 uv		: TEXCOORD0;
	float4 color	: COLOR0; 
	float  fog		: COLOR1;
	float  distFog	: COLOR2;
};

struct PSInput 
{
	float4 pos 		: SV_POSITION0; 
	float2 uv		: TEXCOORD0;
	float4 color	: COLOR0; 
	float4 fog		: COLOR1;
	float  distFog	: COLOR2;
};			

#if USE_GEO_SHADER
#define VS_OUT GSInput
#else
#define VS_OUT PSInput
#endif

VS_OUT VertShader(VSInput input)
{	
	VS_OUT output = (VS_OUT)0;
	
	// Metallicafan212:	Set the W to 1 so matrix math works
	input.pos.w 	= 1.0f;
	
	// Metallicafan212:	Transform it out
	output.pos 		= mul(input.pos, Proj);
	
	// Metallicafan212:	Copy the vert info over
	output.uv.xy	= input.uv.xy;
	output.color	= input.color;
	
	// Metallicafan212:	I've embedded the line size into the fog.x
	output.fog		= input.fog;
	
	// Metallicafan212:	Do the final fog value
	output.distFog	= DoDistanceFog(output.pos);
	
	return output;
}

#if USE_GEO_SHADER
// Metallicafan212:	Using the algorithm from here https://neure.dy.fi/wide_line.gs
[maxvertexcount(6)]
void GeoShader(line GSInput input[2], inout TriangleStream <PSInput> triStream)
{
	// Metallicafan212:	Now expand out this line
	PSInput output = (PSInput)0;
	
	float4 	lineColor 	= input[0].color;
	
	// Metallicafan212:	Initialize the common vars
	output.color		= lineColor;
	output.uv			= input[0].uv;
	
	float4 P1			= input[0].pos;
	float4 P2			= input[1].pos;
	
	// Metallicafan212:	Clip the lines
	float t0			= P1.z + P1.w;
	float t1			= P2.z + P2.w;
	
	float lineWidth		= input[0].fog;
	
	// Metallicafan212:	This single clip block here is what makes it so the grid lines don't get EXTREMELY huge!
	if(t0 < 0.0f)
	{
		if(t1 < 0.0f)
			return;
			
		// Metallicafan212:	Mix the position
		P1	= lerp(P1, P2, (0.0f - t0) / (t1 - t0));
	}
	
	if(t1 < 0.0f)
	{
		P2	= lerp(P1, P2, (0.0f - t0) / (t1 - t0));
	}
	
	// Metallicafan212:	Compute the line axis and vector in ss
	float2 startInNDC 		= P1.xy / P1.w;
	float2 endInNDC 		= P2.xy / P2.w;
	float2 Diff				= endInNDC - startInNDC;
	float2 startInScreen	= (0.5f * startInNDC 	+ float2(0.5f, 0.5f)) * ViewSize;
	float2 endInScreen		= (0.5f * endInNDC		+ float2(0.5f, 0.5f)) * ViewSize;
	float2 lineInScreen		= Diff * ViewSize;
	float2 axisInScreen		= normalize(lineInScreen);
	float2 sideInScreen		= float2(-axisInScreen.y, axisInScreen.x);
	float2 axisInNDC		= axisInScreen / ViewSize;
	float2 sideInNDC		= sideInScreen / ViewSize;
	float4 axis				= float4(axisInNDC, 0.0f, 0.0f) * lineWidth;
	float4 side				= float4(sideInNDC, 0.0f, 0.0f) * lineWidth;
	
	// Metallicafan212:	Now get the verts
	float4 X1				= (P1 + (side - axis) 	* P1.w);
	float4 Y1				= (P2 + (side + axis) 	* P2.w);
	float4 Y2				= (P2 - (side - axis)	* P2.w);
	float4 X2				= (P1 - (side + axis)	* P1.w);
	
	// Metallicafan212:	Render it using a triangle strip (GPU seems to like it better than a triangle list, where you restartstrip after every 3 verts)
	output.pos			= X1;
	output.distFog		= input[0].distFog;
	triStream.Append(output);
	
	output.pos			= X2;
	output.distFog		= input[0].distFog;
	triStream.Append(output);
	
	output.pos			= Y1;
	output.distFog		= input[1].distFog;
	triStream.Append(output);
	
	output.pos			= Y2;
	output.distFog		= input[1].distFog;
	triStream.Append(output);
	
	triStream.RestartStrip();
	
}
#endif

float4 PxShader(PSInput input) : SV_TARGET
{			
	CLIP_PIXEL(input.color);
	
	input.color = DoPixelFog(input.distFog, input.color);
	
	//return input.color + input.fog;
	return DoFinalColor(input.color);
}
