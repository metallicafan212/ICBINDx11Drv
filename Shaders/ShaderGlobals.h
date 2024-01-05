// Metallicafan212:	TODO! More shader globals

// Metallicafan212:	Global vertex definition
#ifndef DEFINED_VERTEX
#define DEFINED_VERTEX 1
struct VSInput 
{ 
	float4 pos 			: POSITION0;
	float4 uv			: TEXCOORD0;
	float4 color		: COLOR0;
	float4 fog			: COLOR1;
	
	// Metallicafan212:	This is used for complex surfaces
	#if EXTRA_VERT_INFO
	#if !COMPLEX_SURF_MANUAL_UVs
	float4 XAxis		: TEXCOORD1;
	float4 YAxis		: TEXCOORD2;
	float4 PanScale1	: TEXCOORD3;
	float4 PanScale2	: TEXCOORD4;
	float4 PanScale3	: TEXCOORD5;
	float4 PanScale4	: TEXCOORD6;
	float4 PanScale5	: TEXCOORD7;
	float4 LFScale		: TEXCOORD8;
	#else
	// Metallicafan212:	Per channel UV info
	float4  DM			: TEXCOORD1;
	float4	FX			: TEXCOORD2;
	//float2 D			: TEXCOORD1;
	//float2 M			: TEXCOORD2;
	//float2 F			: TEXCOORD3;
	#endif
	#endif
};
#endif

#ifndef PI
#define PI 					(3.1415926535897932f)
#endif

#ifndef START_CONST_NUM
#define START_CONST_NUM		b3
#endif

// Metallicafan212:	If to define the "final" color function
//					The complex surface shader redefines the input buffer (as I don't want to keep track of multiple independant buffers)
//					So we need a way to JUST define this function (if we include it again

#ifndef DO_STANDARD_BUFFER
#define DO_STANDARD_BUFFER 1
#define DO_FINAL_COLOR 1
#endif

#ifndef MAX_TEX_NUM
#define MAX_TEX_NUM 16
// Metallicafan212:	Proton doesn't like math in definitions.....
#define TEX_ARRAY_SIZE 4//(MAX_TEX_NUM / 4)
#endif

#ifndef COMMON_VARS	
	#define COMMON_VARS \
		int4	bTexturesBound[TEX_ARRAY_SIZE]			: packoffset(c0);
#endif

#if DO_STANDARD_BUFFER
cbuffer CommonBuffer : register (START_CONST_NUM)
{
    COMMON_VARS
};
#endif
#ifdef DO_FINAL_COLOR

// Metallicafan212:	Define the base constant buffer!!!!
cbuffer FrameVariables : register (b0)
{
	matrix 	Proj			: packoffset(c0);
	float	Gamma			: packoffset(c4.x);
	float2	ViewSize		: packoffset(c4.y);
	int		bDoSelection	: packoffset(c4.w);
	int		bOneXLightmaps	: packoffset(c5.x);
	int		bCorrectFog		: packoffset(c5.y);
	int		bHDR			: packoffset(c5.z);
	int 	GammaMode		: packoffset(c5.w);
	float	HDRExpansion	: packoffset(c6.x);
	float 	ResolutionScale : packoffset(c6.y);
	float	WhiteLevel		: packoffset(c6.z);
	float 	PadHDR2			: packoffset(c6.w);
};

// Metallicafan212:	I did the same thing I did for polyflags, and made the whole enum a set of defines
/*
// Metallicafan212:	TODO! Find a way to mirror this as defines?
#define GM_XOpenGL 		0
#define GM_PerObject	1
#define GM_DX9			2
*/

cbuffer DFogVariables : register (b1)
{
	float4 	DistanceFogColor 	: packoffset(c0);
	float4 	DistanceFogSettings	: packoffset(c1);
	int 	bDoDistanceFog		: packoffset(c2.x);
	float3	Paddy3				: packoffset(c2.y);
};

cbuffer PolyflagVars : register (b2)
{
	// Metallicafan212:	New vars for different effects			
	int		bColorMasked		: packoffset(c0.x);
	float	AlphaReject			: packoffset(c0.y);
	float	BWPercent			: packoffset(c0.z);
	int		bAlphaEnabled		: packoffset(c0.w);
	// Metallicafan212: Temp hack until I recode gamma to be screen-based again, using a different algo
	int		bModulated			: packoffset(c1.x);
	float3	Pad					: packoffset(c1.y);
};

// Metallicafan212:	Distance fog shit
//					TODO! A better algorithm????
//					I have just straight ported the assembly code I wrote a while ago (since I'm a lazy fucking bastard)
float DoDistanceFog(float InZ)//float4 InPos)
{
	if(!bDoDistanceFog)
		return 0.0f;
	
	// Metallicafan212:	Calcuate the fog value for the pixel shader
	if(InZ > DistanceFogSettings.y)
	{
		float dFog = ((InZ * -DistanceFogSettings.x) + DistanceFogSettings.y) * DistanceFogSettings.z;//mad(-InPos.z, DistanceFogSettings.x, DistanceFogSettings.y) * DistanceFogSettings.z;

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
	
	Temp = (Temp * DistFog) + Color;//mad(Temp, DistFog, Color);
	
	return float4(Temp, Color.w);
}

float4 DoGammaCorrection(float4 ColorIn)
{
	if(GammaMode != GM_PerObject || Gamma == 1.0f || bModulated)
		return ColorIn;
	
	float OverGamma = 1.0f / Gamma;
	ColorIn.xyz = pow(ColorIn.xyz, float3(OverGamma, OverGamma, OverGamma));
	
	return ColorIn;
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
	
	// Metallicafan212:	Gamma correct the color!!!!
	//float OverGamma = 1.0f / Gamma;
	//ColorIn.xyz = pow(ColorIn.xyz, float3(OverGamma, OverGamma, OverGamma));
	
	// Metallicafan212:	If doing HDR, change to linear color
	//if(GammaMode == GM_PerObject && bHDR && !bModulated)
	//{
	//	ColorIn.xyz = pow(ColorIn.xyz, float3(2.2f, 2.2f, 2.2f)) * HDRExpansion;
	//}
	
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