#include "ComplexSurfShader_Common.h"

PSOutput PxShader(PSInput input)
{
	PSOutput Out;
	
	// Metallicafan212:	Set the alpha reject
	CurrentAlphaReject	= AlphaReject;
	
	// Metallicafan212:	TODO! Texturing
	float4 DiffColor = ConvertColorspace(input.color);
	
	// Metallicafan212:	Test for selection, do software style selection
	if(bSelected)
	{
		#if 0
		// Metallicafan212:	TODO! This does it based on the surface size, not the screen size
		//					Need to figure out screen-to-pixel conversions to make this operate more as expected
		// Metallicafan212:	10 pixel grid????
		//float2 TestUV 	= input.uv * TexSize;
		
		float2 	TestUV 		= (input.uv * 100.0);
		int2	IntUV		= TestUV;
		
		int		TestVal		= 10;
		
		//int2 	Tested		= int2(IntUV.x & 10, IntUV.y & 10);
		
		//if(IntUV.x & 10 != 0 || IntUV.y & 10 != 0)
		//if(IntUV.x & IntUV.y != 0)
		float2 Tested		= float2(fmod(TestUV.x, 10.0), fmod(TestUV.y, 10.0));
		
		//Tested				-= int2(Tested);
		float2 TestInt		= int2(Tested);
		
		//float2(TestUV.x & 10.0f, TestUV.y & 10.0f);
		//if(Tested.x != 0 || Tested.y != 0) //!= 0.0 || (TestUV.y & 10.0) != 0.0)
		//if((Tested.x && Tested.y))
		if(TestInt.x == Tested.x || TestInt.y == Tested.y)
		{
			discard;
		}
		#endif
	}
	// Metallicafan212:	Diffuse texture
	else if(bTexturesBound & DIFFUSE_BOUND)//bTexturesBound[0].x != 0)
	{
		float4 Diff  	= ConvertColorspace(Diffuse.Sample(DiffState, input.uv));
		DiffColor.xyz  *= Diff.xyz;
		DiffColor.w	   *= Diff.w;
	}

	
	// Metallicafan212:	TODO! This also sets the selection color for the editor! This should be re-evaluated
	CLIP_PIXEL(DiffColor);
	
	// Metallicafan212:	This was replaced with a common function to allow meshes to use the same exact logic
	// float3 DoDetailTexture(float3 DiffColor, float2 dUV, float z, Texture2D Detail, SamplerState DetailState)
	if(bTexturesBound & DETAIL_BOUND)
	{
		DiffColor.xyz = DoDetailTexture(DiffColor.xyz, input.dUV.xy, input.origZ, Detail, DetailState);
	}
	
	/*
	// Metallicafan212:	Detail texture
	//					We're applying this now so that when detail textures fade in, they don't reduce the lightmap as much
	//					TODO! Using the vars from DX7. Allow the user to specify this!!!!
	if(bTexturesBound & 0x10 && input.dUV.z < 380.0f)//bTexturesBound[1].x != 0 && input.dUV.z < 380.0f)
	{
		// Metallicafan212:	Sample it
		//					Multiply the input color by 2 to make it work like lightmaps
		float3 Det = ConvertColorspace(Detail.Sample(DetailState, input.dUV.xy) * 2.0f).xyz;
		
		// Metallicafan212:	Now lerp it
		float alpha = input.dUV.z / 380.0f;
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
	*/
	
	
	// Metallicafan212:	Macro texture
	//					This just modulates the color, like the lightmap
	//if(bTexturesBound[0].z != 0)
	if(bTexturesBound & MACRO_BOUND)
	{
		DiffColor.xyz *= ConvertColorspace(Macro.Sample(MacroState, input.mUV) * 2.0f).xyz;
	}
	
	//float lAlpha = 1.0f;
	
	// Metallicafan212:	Lightmap
	//					TODO! Allow the user to specify the lightmap multiplication (some people like one-x scaling)
	if(bTexturesBound & LIGHT_BOUND)//bTexturesBound[0].y != 0)
	{
		float Mult		= 4.0f;
		
		// Metallicafan212:	Check for oneX blending
		if(bOneXLightmaps)
		{
			Mult 		= 2.0f;
		}
		
		float4 LColor 	= ConvertColorspace(Light.Sample(LightState, input.lUV) * Mult);

		DiffColor.xyz 	*= LColor.xyz + input.addColor.xyz;
		
#if !NO_CUSTOM_RMODES
		// Metallicafan212:	If we're doing lighting only, set it out now
		if(RendMap == REN_LightingOnly)
		{
			Out.Color = float4(DoFinalColor(LColor / 2.0f, input.color).xyz, DiffColor.w);
			
			return Out;
		}
#endif
	}
#if !NO_CUSTOM_RMODES
	// Metallicafan212:	If we don't have lighting, then return a midpoint color
	else if(RendMap == REN_LightingOnly)
	{
		Out.Color = float4(0.5f, 0.5f, 0.5f, DiffColor.w);
		
		return Out;
	}
#endif
	
	// Metallicafan212:	Fog map
	if(bTexturesBound & FOG_BOUND)//bTexturesBound[0].w != 0)
	{
		// Metallicafan212: 2024-12, we're multiplying by 2 because RGB7 was changed to do the expansion in the shaders, not the texture upload stage
		float4 FogColor = ConvertColorspace(Fogmap.Sample(FogState, input.fUV) * 2.0f);
		DiffColor.xyz 	= (DiffColor.xyz * (1.0f - FogColor.w)) + FogColor.xyz;//mad(DiffColor.xyz, (1.0f - FogColor.w), FogColor.xyz);
	}
	
	// Metallicafan212:	Set our alpha for lumos
	//DiffColor.w *= SurfAlpha;
	
#if DO_DISTANCE_FOG
	// Metallicafan212:	Get the distance fog, using the pixel depth
	if(bDoDistanceFog)
	{
		float NewFog = DoDistanceFog(input.origZ);
		
		DiffColor = DoPixelFog(NewFog, DiffColor);
	}
#endif
	
	//return DoFinalColor(DiffColor);
	Out.Color = DoFinalColor(DiffColor, input.color);
	
	return Out;
}