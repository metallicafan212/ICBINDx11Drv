#include "ComplexSurfShader_Common.h"

PSOutput PxShader(PSInput input)
{
	PSOutput Out;
	
	// Metallicafan212:	Set the alpha reject
	CurrentAlphaReject	= AlphaReject;
	
	float4 DiffColor = input.color;
	
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
	else if(bTexturesBound & DIFFUSE_BOUND)
	{
		float4 Diff  	= SampleTexture(Diffuse, DiffState, input.uv, DIFFUSE_BOUND);
		
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
	
	// Metallicafan212:	Macro texture
	//					This just modulates the color, like the lightmap
	if(bTexturesBound & MACRO_BOUND)
	{
		DiffColor.xyz *= SampleTexture(Macro, MacroState, input.mUV, MACRO_BOUND).xyz * 2.0f;
		
		/*
		// Metallicafan212:	Bicubic sampling
		float4 MacroColor;
		
		if(ShaderFlags & SF_BicubicSampling)
		{
			// Metallicafan212:	Automatically get the texture size
			float2 texSize;
			Macro.GetDimensions(texSize.x, texSize.y);
			MacroColor = SampleTextureCatmullRom(Macro, MacroState, input.mUV, texSize);
		}
		else
		{
			MacroColor = Macro.Sample(MacroState, input.mUV);
		}
		
		//DiffColor.xyz *= ConvertColorspace(Macro.Sample(MacroState, input.mUV) * 2.0f).xyz;
		DiffColor.xyz *= (MacroColor * 2.0f);
		*/
	}
	
	
	// Metallicafan212:	Lightmap
	if(bTexturesBound & LIGHT_BOUND)
	{
		float Mult		= 4.0f;
		
		// Metallicafan212:	Check for oneX blending
		if(bOneXLightmaps)
		{
			Mult 		= 2.0f;
		}
		
		/*
		// Metallicafan212:	Bicubic sampling
		float4 LColor;
		
		if(ShaderFlags & SF_BicubicSampling)
		{
			//LightmapSampleBicubic(LightState, Light, input.lUV, LColor);
			// Metallicafan212:	Automatically get the texture size
			float2 texSize;
			Light.GetDimensions(texSize.x, texSize.y);
			LColor = SampleTextureCatmullRom(Light, LightState, input.lUV, texSize);
		}
		else
		{
			LColor 		= Light.Sample(LightState, input.lUV);
		}
		*/
		
		float4 LColor = SampleTexture(Light, LightState, input.lUV, LIGHT_BOUND) * Mult;
		
		//LColor *= Mult;

		// Metallicafan212:	In HP2, adding in ZoneAmbientLight fucks up dark lights, so we can't do that there
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
	if(bTexturesBound & FOG_BOUND)
	{
		/*
		// Metallicafan212:	Bicubic sampling
		float4 FogColor;
		
		if(ShaderFlags & SF_BicubicSampling)
		{
			//LightmapSampleBicubic(FogState, Fogmap,input.fUV, FogColor);
			// Metallicafan212:	Automatically get the texture size
			float2 texSize;
			Fogmap.GetDimensions(texSize.x, texSize.y);
			FogColor = SampleTextureCatmullRom(Fogmap, FogState, input.fUV, texSize);
		}
		else
		{
			FogColor 		= Fogmap.Sample(FogState, input.fUV);
		}
		
		FogColor 		*= 2.0f;
		*/
		
		float4 FogColor = SampleTexture(Fogmap, FogState, input.fUV, FOG_BOUND) * 2.0f;
		
		DiffColor.xyz 	= (DiffColor.xyz * (1.0f - FogColor.w)) + FogColor.xyz;
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