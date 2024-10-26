#include "Line_Common.h"

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
	output.origZ		= input[0].origZ;
	triStream.Append(output);
	
	output.pos			= X2;
	output.origZ		= input[0].origZ;;
	triStream.Append(output);
	
	output.pos			= Y1;
	output.origZ		= input[1].origZ;;
	triStream.Append(output);
	
	output.pos			= Y2;
	output.origZ		= input[1].origZ;;
	triStream.Append(output);
	
	triStream.RestartStrip();
}
#endif