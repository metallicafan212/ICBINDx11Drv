#if HP2_HARDCODE
#include "HP2ShaderDefs.h"
#endif

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
	float4 addColor		: COLOR2;
	
	// Metallicafan212:	This is used for complex surfaces
	// Metallicafan212:	Per channel UV info
	float4  DM			: TEXCOORD1;
	float4	FX			: TEXCOORD2;
};
#endif

#ifndef PI
#define PI 					(3.1415926535897932f)
#endif

// Metallicafan212:	TODO! Get these imported when compiling for other targets with pre-compiled headers!
#ifndef RENMAPS

#define RENMAPS 1

// Metallicafan212:	Don't use any of the code needed for the custom render methods or rmodes (can reevaluate later for other games that might need similar features)
#undef NO_CUSTOM_RMODES
#define NO_CUSTOM_RMODES 1

#define REN_None			0		// Hide completely.
#define REN_Wire			1		// Wireframe of EdPolys.
#define REN_Zones			2		// Show zones and zone portals.
#define REN_Polys			3		// Flat-shaded Bsp.
#define REN_PolyCuts		4		// Flat-shaded Bsp with normals displayed.
#define REN_DynLight		5		// Illuminated texture mapping.
#define REN_PlainTex		6		// Plain texture mapping.
#define REN_OrthXY			13		// Orthogonal overhead (XY) view.
#define REN_OrthXZ			14		// Orthogonal XZ view.
#define REN_OrthYZ			15		// Orthogonal YZ view.
#define REN_TexView			16		// Viewing a texture (no actor).
#define REN_TexBrowser		17		// Viewing a texture browser (no actor).
#define REN_MeshView		18		// Viewing a mesh.
#define REN_MatineeIP		19		// Matinee - interpolation point editing.
#define REN_LightingOnly	20		// Forces the BSP surfaces to use the same solid color to show lighting
#define REN_Prefab			21		// Metallicafan212:	Normal, no BSP prefabs
#define REN_PrefabCompiled  22		// Metallicafan212:	Compiled prefab with BSP enabled
#define REN_AnimView		23		// Metallicafan212:	New mesh browser that is a level
#define REN_AnimViewWire	24		// Metallicafan212:	Wireframe for the mesh browser
#define REN_Leafs			25		// Metallicafan212: Visibility (lighting) leafs
#define REN_SpecialPoly		26		// Metallicafan212:	Display special poly (climbable) surfaces
#define REN_RenderPasses	27		// Metallicafan212:	Color pass 1 objects blue, pass 2 objects green
#define REN_ParticleView	28		// Metallicafan212:	New particleFX browser
#define REN_Depth			29		// Metallicafan212:	Depth RMode (exclusive to DX11)
#define REN_Normals			30		// Metallicafan212:	Render vertex normals
#define REN_MAX				31
	
#endif

// Metallicafan212:	If it didn't get defined up there, assume it's HP2
#ifndef NO_CUSTOM_RMODES
#define NO_CUSTOM_RMODES 0
#endif

#ifndef FIRST_USER_CONSTBUFF
#define FIRST_USER_CONSTBUFF b4
#endif

// Metallicafan212:	OLDVER!
#ifndef START_CONST_NUM
#define START_CONST_NUM FIRST_USER_CONSTBUFF
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
	#if WINE
	#define TEX_ARRAY_SIZE 4
	#else
	#define TEX_ARRAY_SIZE (MAX_TEX_NUM / 4)
	#endif
#endif

#ifndef COMMON_VARS
	#define COMMON_VARS
#endif

#if !NO_CUSTOM_RMODES || DX11_UNREAL_227
#ifndef DO_DISTANCE_FOG
#define DO_DISTANCE_FOG 1
#endif
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
	matrix 	Proj				: packoffset(c0);
	float	Gamma				: packoffset(c4.x);
	float2	ViewSize			: packoffset(c4.y);
	int		bDoSelection		: packoffset(c4.w);
	int		bOneXLightmaps		: packoffset(c5.x);
	int		bCorrectFog			: packoffset(c5.y);
	//int		bHDR				: packoffset(c5.z);
	dword	FrameShaderFlags	: packoffset(c5.z);
	int 	GammaMode			: packoffset(c5.w);
	float	HDRExpansion		: packoffset(c6.x);
	float 	ResolutionScale 	: packoffset(c6.y);
	float	WhiteLevel			: packoffset(c6.z);
	//float 	PadHDR2			: packoffset(c6.w);
	//int		bDepthDraw			: packoffset(c6.w);
	// Metallicafan212:	Replaced this with rmodes
	int		RendMap				: packoffset(c6.w);
	float	DepthDrawLimit		: packoffset(c7.x);
	
	// Metallicafan212:	DX9 gamma offset values
	float	GammaOffsetRed		: packoffset(c7.y);
	float	GammaOffsetBlue		: packoffset(c7.z);
	float	GammaOffsetGreen	: packoffset(c7.w);
	
	// Metallicafan212: FFrame clipping plane
	float4	ClippingPlane		: packoffset(c8.x);
};

cbuffer PolyflagVars : register (b2)
{
	// Metallicafan212:	New vars for different effects			
	int		bSelected			: packoffset(c0.x);
	float	AlphaReject			: packoffset(c0.y);
	
	// Metallicafan212:	Alternative alpha reject for hud tiles
	float	AltAlphaReject		: packoffset(c0.z);
	
	float	BWPercent			: packoffset(c0.w);
	
	// Metallicafan212:	Pack in the selection color for the editor
	float3	SelectedColor		: packoffset(c1.x);
	
	// Metallicafan212:	TODO! This is ONLY when FVector is aligned to 16 bytes!!!!
	float	PadSelect			: packoffset(c1.w);
	
	// Metallicafan212:	Generalized shader flags, so we're more optimially using memory
	dword	ShaderFlags			: packoffset(c2.x);
	
	// Metallicafan212:	Pad out the rest of the structure
	float3 	PadFlags			: packoffset(c2.y);
};

cbuffer TextureVariables : register (b3)
{
	// Metallicafan212:	Single flag for each slot
	dword 	bTexturesBound 	: packoffset(c0.x);
	
	// Metallicafan212:	If to use Catmull-Rom Bicubic sampling on each texture
	dword 	bSampTexBicubic	: packoffset(c0.y);
	
	float2 TexturePad		: packoffset(c0.z);
	//float3 	TexturePad		: packoffset(c1);
};

#if DO_DISTANCE_FOG
cbuffer DFogVariables : register (b1)
{
	float4 	DistanceFogColor 	: packoffset(c0);
	float4 	DistanceFogSettings	: packoffset(c1);
	int 	bDoDistanceFog		: packoffset(c2.x);
	int		FogMode				: packoffset(c2.y);
	float2	Paddy2				: packoffset(c2.z);
};

#if PIXEL_SHADER
// Metallicafan212:	Distance fog shit
//					TODO! A better algorithm????
//					I have just straight ported the assembly code I wrote a while ago (since I'm a lazy fucking bastard)
float DoDistanceFog(float InZ)
{
	if(!bDoDistanceFog)
		return 0.0f;
	
	float fogStart 		= DistanceFogSettings.x;
	float fogEnd		= DistanceFogSettings.y;
	float fogDensity	= DistanceFogSettings.z;
	float fogAlpha		= DistanceFogSettings.w;
	
	// Metallicafan212:	Calcuate the fog value for the pixel shader
	if(InZ >= fogStart)
	{
		float fogScale 		= (fogEnd - fogStart);
		float dFog		= 1.0 - ((-InZ + fogEnd) / fogScale);
		switch(FogMode)
		{
			// Metallicafan212:	Linear fog
			case 1:
			{				
				return saturate(dFog);
				
				//float dFog = ((InZ * -DistanceFogSettings.x) + DistanceFogSettings.y) * DistanceFogSettings.z;

				//return saturate(DistanceFogSettings.w - dFog);
			}
			
			// Metallicafan212: Expoential fog
			case 2:
			{
				return saturate(1.0 - (1.0 / log(fogDensity * dFog)));
				// Metallicafan212:	This shit don't work, just reimplement it as a expoential ramp-up
				//float dFog = 
				/*
				// Metallicafan212: TODO! ZMax var
				InZ 		=  InZ / 65536.0;
				float dFog 	= 1.0 - exp(DistanceFogSettings.z * -InZ);
				return dFog;
				//return saturate(dFog);
				*/
			}
			
			case 3:
			{
				return saturate(pow(dFog, 2.0));
				/*
				// Metallicafan212: TODO! ZMax var
				InZ 		=  InZ / 65536.0;
				float dFog = 1.0 - exp(pow(DistanceFogSettings.z * -InZ, 2.0));
				return dFog;
				//return saturate(dFog);
				*/
			}
		}
	}
	
	return 0.0f;
}
#endif
#endif

#if !NO_CUSTOM_RMODES && PIXEL_SHADER
// Metallicafan212:	HACK!!! To reject black and white on UI tiles, I don't want to have to update all the shaders...
static bool		bRejectBW;

// Metallicafan212:	Hacked global!!!
static float	CurrentAlphaReject;

#define CLIP_PIXEL(ColorIn) \
	if (RendMap == REN_Normals) \
		ColorIn.xyzw = input.color; \
	else if(!(ShaderFlags & SF_AlphaEnabled)) \
		ColorIn.w = 1.0f; \
	else if(ColorIn.w < CurrentAlphaReject) \
		discard; \
	if(RendMap == REN_Depth) \
		ColorIn.xyz = input.origZ / DepthDrawLimit;

#else
#if PIXEL_SHADER

// Metallicafan212:	Hacked global!!!
static float	CurrentAlphaReject;

#define CLIP_PIXEL(ColorIn) \
	if(!(ShaderFlags & SF_AlphaEnabled)) \
		ColorIn.w = 1.0f; \
	else if(ColorIn.w < CurrentAlphaReject) \
		discard;
#endif
#endif

#if PIXEL_SHADER && DO_DISTANCE_FOG
float4 DoPixelFog(float DistFog, float4 Color)
{
	// Metallicafan212:	Early bail
	if(!bDoDistanceFog || DistFog <= 0.0f)
		return Color;
	
	// Metallicafan212:	Now mix the colors
	//					Again this is just a straight port of the asm I wrote a while ago
	return float4(lerp(Color.xyz, DistanceFogColor.xyz, DistFog * DistanceFogColor.w), Color.w);
}
#endif

#define select(check, a, b) (check ? a : b)

// Metallicafan212:	Moved from the ResScaling.hlsl shader
// 					From https://github.com/Microsoft/DirectX-Graphics-Samples/blob/master/MiniEngine/Core/Shaders/ColorSpaceUtility.hlsli
float3 SRGBToRec2020(float3 In)
{
	static const float3x3 ConvMat =
    {
        0.627402, 0.329292, 0.043306,
        0.069095, 0.919544, 0.011360,
        0.016394, 0.088028, 0.895578
    };
    return mul(ConvMat, In);
}

float3 RemoveSRGBCurve( float3 x )
{
    // Approximately pow(x, 2.2)
    return select(x < 0.04045, x / 12.92, pow( (x + 0.055) / 1.055, 2.4 ));
}

// Metallicafan212:	From https://gist.github.com/TheRealMJP/c83b8c0f46b63f3a88a5986f4fa982b1
// The following code is licensed under the MIT license: https://gist.github.com/TheRealMJP/bc503b0b87b643d3505d41eab8b332ae
// Samples a texture with Catmull-Rom filtering, using 9 texture fetches instead of 16.
// See http://vec3.ca/bicubic-filtering-in-fewer-taps/ for more details
float4 SampleTextureCatmullRom(in Texture2D<float4> tex, in SamplerState linearSampler, in float2 uv, in float2 texSize)
{
    // We're going to sample a a 4x4 grid of texels surrounding the target UV coordinate. We'll do this by rounding
    // down the sample location to get the exact center of our "starting" texel. The starting texel will be at
    // location [1, 1] in the grid, where [0, 0] is the top left corner.
    float2 samplePos = uv * texSize;
    float2 texPos1 = floor(samplePos - 0.5f) + 0.5f;

    // Compute the fractional offset from our starting texel to our original sample location, which we'll
    // feed into the Catmull-Rom spline function to get our filter weights.
    float2 f = samplePos - texPos1;

    // Compute the Catmull-Rom weights using the fractional offset that we calculated earlier.
    // These equations are pre-expanded based on our knowledge of where the texels will be located,
    // which lets us avoid having to evaluate a piece-wise function.
    float2 w0 = f * (-0.5f + f * (1.0f - 0.5f * f));
    float2 w1 = 1.0f + f * f * (-2.5f + 1.5f * f);
    float2 w2 = f * (0.5f + f * (2.0f - 1.5f * f));
    float2 w3 = f * f * (-0.5f + 0.5f * f);

    // Work out weighting factors and sampling offsets that will let us use bilinear filtering to
    // simultaneously evaluate the middle 2 samples from the 4x4 grid.
    float2 w12 = w1 + w2;
    float2 offset12 = w2 / (w1 + w2);

    // Compute the final UV coordinates we'll use for sampling the texture
    float2 texPos0 = texPos1 - 1;
    float2 texPos3 = texPos1 + 2;
    float2 texPos12 = texPos1 + offset12;

    texPos0 /= texSize;
    texPos3 /= texSize;
    texPos12 /= texSize;

    float4 result = 0.0f;
	
	result += tex.Sample(linearSampler, float2(texPos0.x, texPos0.y), 0.0f) * w0.x * w0.y;
    result += tex.Sample(linearSampler, float2(texPos12.x, texPos0.y), 0.0f) * w12.x * w0.y;
    result += tex.Sample(linearSampler, float2(texPos3.x, texPos0.y), 0.0f) * w3.x * w0.y;
	
    result += tex.Sample(linearSampler, float2(texPos0.x, texPos12.y), 0.0f) * w0.x * w12.y;
    result += tex.Sample(linearSampler, float2(texPos12.x, texPos12.y), 0.0f) * w12.x * w12.y;
    result += tex.Sample(linearSampler, float2(texPos3.x, texPos12.y), 0.0f) * w3.x * w12.y;
	
    result += tex.Sample(linearSampler, float2(texPos0.x, texPos3.y), 0.0f) * w0.x * w3.y;
    result += tex.Sample(linearSampler, float2(texPos12.x, texPos3.y), 0.0f) * w12.x * w3.y;
    result += tex.Sample(linearSampler, float2(texPos3.x, texPos3.y), 0.0f) * w3.x * w3.y;

    return result;
}

float4 SampleTexture(in Texture2D Texture, in SamplerState Sampler, in float2 UV, in dword TextureFlag)
{
	// Metallicafan212:	Should we use bicubic sampling?
	if(bSampTexBicubic & TextureFlag)
	{
		float2 texSize;
		Texture.GetDimensions(texSize.x, texSize.y);
		return SampleTextureCatmullRom(Texture, Sampler, UV, texSize);
	}
	else
	{
		// Metallicafan212:	Sample normally
		return Texture.Sample(Sampler, UV);
	}
}

// This is the new HDR transfer function, also called "PQ" for perceptual quantizer.  Note that REC2084
// does not also refer to a color space.  REC2084 is typically used with the REC2020 color space.
float3 ApplyREC2084Curve(float3 L)
{
    float m1 = 2610.0 / 4096.0 / 4;
    float m2 = 2523.0 / 4096.0 * 128;
    float c1 = 3424.0 / 4096.0;
    float c2 = 2413.0 / 4096.0 * 32;
    float c3 = 2392.0 / 4096.0 * 32;
    float3 Lp = pow(L, m1);
    return pow((c1 + c2 * Lp) / (1 + c3 * Lp), m2);
}

// Metallicafan212:	This is an approximation from http://chilliant.blogspot.com/2012/08/srgb-approximations-for-hlsl.html
float3 SRGBToLinear(float3 sRGB)
{
	return sRGB * (sRGB * (sRGB * 0.305306011 + 0.682171111) + 0.012522878);
}

float3 LinearToSRGB(float3 Linear)
{
	float3 S1 	= sqrt(Linear);
	float3 S2 	= sqrt(S1);
	float3 S3 	= sqrt(S2);
	return 0.662002687 * S1 + 0.684122060 * S2 - 0.323583601 * S3 - 0.0225411470 * Linear;
}

#if PIXEL_SHADER

// Metallicafan212:	Define default slots for the diffuse and detail.
//					TODO!!! This file needs a HUGE cleanup
#ifndef DIFFUSE_BOUND
#define DIFFUSE_BOUND 0x1
#endif

#ifndef DETAIL_BOUND
#define DETAIL_BOUND 0x2
#endif
float3 DoDetailTexture(float3 DiffColor, float2 dUV, float z, Texture2D Detail, SamplerState DetailState)
{
	// Metallicafan212:	Detail texture
	//					We're applying this now so that when detail textures fade in, they don't reduce the lightmap as much
	//					TODO! Using the vars from DX7. Allow the user to specify this!!!!
	//if(bTexturesBound & 0x10 && input.dUV.z < 380.0f)//bTexturesBound[1].x != 0 && input.dUV.z < 380.0f)
	if( z < 1024.0f)
	{
		/*
		// Metallicafan212:	Sample it using bicubic, as a test.
		// 					TODO! We need to define a lookup for if textures are no smooth and then NOT sample using this algorithm....
		//					Or maybe define the mode per texture?
		float3 Det;
		if(ShaderFlags & SF_BicubicSampling)
		{
			float2 texSize;
			Detail.GetDimensions(texSize.x, texSize.y);
			Det = SampleTextureCatmullRom(Detail, DetailState, dUV.xy, texSize);
		}
		else
		{
			Det = Detail.Sample(DetailState, dUV.xy);
		}
		
		Det *= 2.0f;
		*/
		
		float3 Det = SampleTexture(Detail, DetailState, dUV, DETAIL_BOUND) * 2.0f;
		
		// Metallicafan212:	Sample it
		//					Multiply the input color by 2 to make it work like lightmaps
		//float3 Det = ConvertColorspace(Detail.Sample(DetailState, dUV.xy) * 2.0f).xyz;
		
		// Metallicafan212:	Now lerp it
		float alpha = z / 1024.0f;
		Det = lerp(alpha, float3(1.0f, 1.0f, 1.0f), Det);
		
		// Metallicafan212:	Average the color
		//					TODO! Should we actually be doing this???
		Det.xyz = (Det.x + Det.y + Det.z) / 3.0f;
		
		// Metallicafan212:	Now add it to the image
		//					When there's no detail texture, this operation effectively returns nothing
		DiffColor.xyz = (DiffColor.xyz * Det);
		DiffColor.xyz += DiffColor.xyz;
		DiffColor.xyz /= 2.0f;
	}
	
	return DiffColor;
}

float4 DoFinalColor(float4 ColorIn, float4 SelectionColor)
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
			return float4(SelectionColor.xyz, 1.0f);//SelectedColor.xyz, 1.0f);//SelectionColor.xyz, 1.0f);
		}
	}
	
#if !NO_CUSTOM_RMODES
	// Metallicafan212:	In the depth mode, just return back the color (it's handled in the macro above)
	//					Same for normals mode
	if(RendMap == REN_Depth || RendMap == REN_Normals)
	{
		return ColorIn;
	}
#endif
	
	// Metallicafan212:	Clamp the color
	//if(!bHDR)
	if(!(FrameShaderFlags & 0x1))
	{
		ColorIn.xyz = clamp(ColorIn.xyz, 0.0f, 1.0f);
	}
	
#if !NO_CUSTOM_RMODES
	// Metallicafan212:	Early return
	if(BWPercent <= 0.0f || bRejectBW)
	{
		return ColorIn;
	}
	else
	{
		// Metallicafan212:	Sum and divide by 3
		ColorIn.xyz = lerp(ColorIn.xyz, (ColorIn.x + ColorIn.y + ColorIn.z) / 3.0f, BWPercent);
		return ColorIn;
	}
#else
	return ColorIn;
#endif
}
#endif
#endif