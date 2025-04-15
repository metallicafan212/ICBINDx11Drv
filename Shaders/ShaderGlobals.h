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
	
	//float3	Paddddddd		: packoffset(c7.y);
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
	//int4	bTexturesBound[TEX_ARRAY_SIZE]			: packoffset(c0);
	// Metallicafan212:	Single flag for each slot
	dword 	bTexturesBound 	: packoffset(c0);
	float3 	TexturePad		: packoffset(c1);
};

#if DO_DISTANCE_FOG
cbuffer DFogVariables : register (b1)
{
	float4 	DistanceFogColor 	: packoffset(c0);
	float4 	DistanceFogSettings	: packoffset(c1);
	int 	bDoDistanceFog		: packoffset(c2.x);
	float3	Paddy3				: packoffset(c2.y);
};

#if PIXEL_SHADER
// Metallicafan212:	Distance fog shit
//					TODO! A better algorithm????
//					I have just straight ported the assembly code I wrote a while ago (since I'm a lazy fucking bastard)
float DoDistanceFog(float InZ)
{
	if(!bDoDistanceFog)
		return 0.0f;
	
	// Metallicafan212:	Calcuate the fog value for the pixel shader
	if(InZ > DistanceFogSettings.y)
	{
		float dFog = ((InZ * -DistanceFogSettings.x) + DistanceFogSettings.y) * DistanceFogSettings.z;

		return saturate(DistanceFogSettings.w - dFog);
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
	float3 Temp = DistanceFogColor.xyz - Color.xyz;
	Temp *= DistanceFogColor.w;
	
	Temp = (Temp * DistFog) + Color.xyz;
	
	return float4(Temp, Color.w);
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

// Metallicafan212:	From https://gist.github.com/AlpyneDreams/17e5e8b8b686eca659917a61d4a0b0d3
float4 cubic(float v)
{
	float4 n = float4(1.0, 2.0, 3.0, 4.0) - v;
	float4 s = n * n * n;
	float x = s.x;
	float y = s.y - 4.0 * s.x;
	float z = s.z - 4.0 * s.y + 6.0 * s.x;
	float w = 6.0 - x - y - z;
	return float4(x, y, z, w) * (1.0/6.0);
}

#define BICUBIC_OFFSET 0.5

float4 tex2D_bicubic(sampler samp, Texture2D tex, float2 texCoords)
{	
	// Metallicafan212:	Automatically get the texture size
	float2 texSize;
	tex.GetDimensions(texSize.x, texSize.y);
	float2 invTexSize 	= 1 / texSize;
	
	texCoords 		= texCoords * texSize - BICUBIC_OFFSET;
	
	float2 fxy 		= frac(texCoords);
	texCoords -= fxy;

	float4 xcubic 	= cubic(fxy.x);
	float4 ycubic 	= cubic(fxy.y);

	float4 c 		= texCoords.xxyy + float2(-BICUBIC_OFFSET, BICUBIC_OFFSET * 3.0).xyxy;
	
	float4 s 		= float4(xcubic.xz + xcubic.yw, ycubic.xz + ycubic.yw);
	float4 offset 	= c + float4(xcubic.yw, ycubic.yw) / s;
	
	offset *= invTexSize.xxyy;


	float4 sample0 = tex.Sample(samp, offset.xz);//SampleGrad(samp, offset.xz, dx, dy);
	float4 sample1 = tex.Sample(samp, offset.yz);//SampleGrad(samp, offset.yz, dx, dy);
	float4 sample2 = tex.Sample(samp, offset.xw);//SampleGrad(samp, offset.xw, dx, dy);
	float4 sample3 = tex.Sample(samp, offset.yw);//SampleGrad(samp, offset.yw, dx, dy);

	float sx = s.x / (s.x + s.y);
	float sy = s.z / (s.z + s.w);
	
	return lerp( lerp(sample3, sample2, sx), lerp(sample1, sample0, sx), sy );
}


void LightmapSampleBicubic(in sampler Samp, in Texture2D Tex, in float2 UV, out float4 Output )
{
	Output = tex2D_bicubic( Samp, Tex, UV);
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

float4 ConvertColorspace(float4 In)
{
	// Metallicafan212:	While for many things this looks better, things like particles now don't blend right, so the final image needs to be corrected, not the objects
	/*
	// Metallicafan212:	Are we in linear color?
	if(FrameShaderFlags & 0x2)
	{
		return float4(SRGBToLinear(In.xyz), In.w);
	}
	*/
	
	return In;
}

#if PIXEL_SHADER
float3 DoDetailTexture(float3 DiffColor, float2 dUV, float z, Texture2D Detail, SamplerState DetailState)
{
	// Metallicafan212:	Detail texture
	//					We're applying this now so that when detail textures fade in, they don't reduce the lightmap as much
	//					TODO! Using the vars from DX7. Allow the user to specify this!!!!
	//if(bTexturesBound & 0x10 && input.dUV.z < 380.0f)//bTexturesBound[1].x != 0 && input.dUV.z < 380.0f)
	if( z < 1024.0f)
	{
		// Metallicafan212:	Sample it
		//					Multiply the input color by 2 to make it work like lightmaps
		float3 Det = ConvertColorspace(Detail.Sample(DetailState, dUV.xy) * 2.0f).xyz;
		
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