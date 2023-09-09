// Metallicafan212:	Resolution scaling shader, that will correctly filter down the screen texture to the destination
#define DO_STANDARD_BUFFER 0
#include "ShaderGlobals.h"

// Metallicafan212:	Constant buffer, but with the added NumAASamples
shared cbuffer CommonBuffer : register (b0)
{
	COMMON_VARS
	
	// Metallicafan212:	The info we use for this specific shader	
	int2	TextureSize				: packoffset(c12.x);
	float2	Pad3					: packoffset(c12.z);
};

// Metallicafan212:	HACK!!!! This includes this twice to define the final color function, as HLSL cannot do out of order compiling
//					The buffer variables have to be defined before they can be used
#define DO_FINAL_COLOR
#include "ShaderGlobals.h"

Texture2D<uint> 		P8		: register(t0);
Texture2D<float4> 		Palette	: register(t1);

RWTexture2D<float4>		Out[16];	//: register(u0);

// Metallicafan212:	Resolve the P8 texture to a RGBA8 one
[numthreads(32, 32, 1)]
//[numthreads(1, 1, 1)]
void CSMain( uint3 pixelID : SV_DispatchThreadID )
{
	// Metallicafan212:	Exit out early if the pixel ID is bad
	//if(pixelID.x >= TextureSize.x || pixelID.y >= TextureSize.y)
	//	return;
	
	uint X, Y, Levels;
	
	P8.GetDimensions(pixelID.z, X, Y, Levels);
	
	if(pixelID.x > X || pixelID.y > Y || pixelID.z > Levels)
		return;
	
	// Metallicafan212:	Now read and write to the texture
	int PID = clamp(P8.Load(pixelID.xyz), 0, 255);//clamp(255.0f * P8.Load(pixelID.xyz), 0, 255);
	
	// Metallicafan212:	Get the color
	float4 PColor = Palette.Load(int3(PID, 0, 0));
	
	// Metallicafan212:	Enforce that only palette entry 0 gets alpha
	if(PID != 0)
	{
		PColor.a = 1.0f;
	}
	
	// Metallicafan212:	This is TERRIBLE!!!! You have to use a LITERAL index if you have multiple indicies
	switch(pixelID.z)
	{
		case 0:
		{
			Out[0][pixelID.xy] = PColor;
			break;
		}
		
		case 1:
		{
			Out[1][pixelID.xy] = PColor;
			break;
		}
		
		case 2:
		{
			Out[2][pixelID.xy] = PColor;
			break;
		}
		
		case 3:
		{
			Out[3][pixelID.xy] = PColor;
			break;
		}
		
		case 4:
		{
			Out[4][pixelID.xy] = PColor;
			break;
		}
		
		case 5:
		{
			Out[5][pixelID.xy] = PColor;
			break;
		}
		
		case 6:
		{
			Out[6][pixelID.xy] = PColor;
			break;
		}
		
		case 7:
		{
			Out[7][pixelID.xy] = PColor;
			break;
		}
		
		case 8:
		{
			Out[8][pixelID.xy] = PColor;
			break;
		}
		
		case 9:
		{
			Out[9][pixelID.xy] = PColor;
			break;
		}
		
		case 10:
		{
			Out[10][pixelID.xy] = PColor;
			break;
		}
		
		case 11:
		{
			Out[11][pixelID.xy] = PColor;
			break;
		}
		
		case 12:
		{
			Out[12][pixelID.xy] = PColor;
			break;
		}
		
		case 13:
		{
			Out[13][pixelID.xy] = PColor;
			break;
		}
		
		case 14:
		{
			Out[15][pixelID.xy] = PColor;
			break;
		}
		
		case 15:
		{
			Out[15][pixelID.xy] = PColor;
			break;
		}
		
		//default:
		//{
			//Out[0][pixelID.xy] = PColor;
			//break;
		//}
	}
}