// Metallicafan212:	TODO! More shader globals

#define PI 					(3.1415926535897932f)

// Metallicafan212:	If to define the "final" color function
//					The complex surface shader redefines the input buffer (as I don't want to keep track of multiple independant buffers)
//					So we need a way to JUST define this function (if we include it again

#ifndef DO_STANDARD_BUFFER
#define DO_STANDARD_BUFFER 1
#define DO_FINAL_COLOR 1
#endif

#define MAX_TEXTURE_NUMBER 16

/*
// Metallicafan212:	These weren't needed anymore, so I removed them
float	ViewX 									: packoffset(c4.x); \
float 	ViewY 									: packoffset(c4.y); \
float	Aspect 									: packoffset(c4.z); \
float	ProjZ 									: packoffset(c4.w); \
float 	RFX2 									: packoffset(c5.x); \
float	RFY2 									: packoffset(c5.y); \

*/

#define COMMON_VARS \
matrix 	Proj									: packoffset(c0); 	\
/* Metallicafan212:	New vars for different effects */ 				\
int		bColorMasked							: packoffset(c4.x); \
int		bDoDistanceFog							: packoffset(c4.y); \
int		bDoSelection							: packoffset(c4.z);	\
float	AlphaReject								: packoffset(c4.w); \
float	BWPercent								: packoffset(c5.x);	\
int		bAlphaEnabled							: packoffset(c5.y); \
/*int		bNVTileHack								: packoffset(c5.z); */	\
float2	Pad										: packoffset(c5.z);	\
float4	DistanceFogColor						: packoffset(c6);	\
float4	DistanceFogSettings						: packoffset(c7);	\
int4	bTexturesBound[4]						: packoffset(c8);

#if DO_STANDARD_BUFFER
shared cbuffer CommonBuffer : register (b0)
{
    COMMON_VARS;
};
#endif
#ifdef DO_FINAL_COLOR

// Metallicafan212:	Distance fog shit
//					TODO! A better algorithm????
//					I have just straight ported the assembly code I wrote a while ago (since I'm a lazy fucking bastard)
float DoDistanceFog(float4 InPos)
{
	if(!bDoDistanceFog)
		return 0.0f;
	
	// Metallicafan212:	Calcuate the fog value for the pixel shader
	if(InPos.z > DistanceFogSettings.y)
	{
		float dFog = ((InPos.z * -DistanceFogSettings.x) + DistanceFogSettings.y) * DistanceFogSettings.z;//mad(-InPos.z, DistanceFogSettings.x, DistanceFogSettings.y) * DistanceFogSettings.z;

		return saturate(DistanceFogSettings.w - dFog);
	}
	
	return 0.0f;
}

// Metallicafan212:	Global selection color
static float4 SelectionColor;

// Metallicafan212:	Color masking is currently unimplemented and may not be reimplemented

// Metallicafan212:	Do masked rejection
#define CLIP_PIXEL(ColorIn) if(!bAlphaEnabled) ColorIn.w = 1.0f; SelectionColor = input.color; clip(ColorIn.w - AlphaReject);

float4 DoPixelFog(float DistFog, float4 Color)
{
	// Metallicafan212:	Early bail
	if(!bDoDistanceFog || DistFog <= 0.0f)
		return Color;
	
	// Metallicafan212:	Now mix the colors
	//					Again this is just a straight port of the asm I wrote a while ago
	float3 Temp = DistanceFogColor.xyz - Color.xyz;
	Temp *= DistanceFogColor.w;
	
	Temp = mad(Temp, DistFog, Color);
	
	return float4(Temp, Color.w);
}

float4 DoFinalColor(float4 ColorIn)
{
	// Metallicafan212:	If doing selection, move out the selection color
	if(bDoSelection)
	{
		if(ColorIn.a < AlphaReject)
		{
			return float4(0.0f, 0.0f, 0.0f, 0.0f);
		}
		else
		{
			return float4(SelectionColor.xyz, 1.0f);
		}
	}
	
	// Metallicafan212:	Early return
	if(BWPercent <= 0.0f)
	{
		return ColorIn;
	}
	else
	{
		// Metallicafan212:	Sum and divide by 3
		ColorIn.xyz = lerp(ColorIn.xyz, (ColorIn.x + ColorIn.y + ColorIn.z) / 3.0f, BWPercent);
		return ColorIn;
	}
}
#endif

