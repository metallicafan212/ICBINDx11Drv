#include "ICBINDx11Drv.h"

#define QUAD_TILES 1

// Metallicafan212:	Definitions for tile-related functions
void UICBINDx11RenderDevice::DrawTile(FSceneNode* Frame, FTextureInfo& Info, FLOAT X, FLOAT Y, FLOAT XL, FLOAT YL, FLOAT U, FLOAT V, FLOAT UL, FLOAT VL, FSpanBuffer* Span, FLOAT Z, FPlane Color, FPlane Fog, PFLAG PolyFlags)
{
	guard(UICBINDx11RenderDevice::DrawTile);

	// Metallicafan212:	Start buffering now
	StartBuffering(BT_Tiles);

	// Metallicafan212:	The check for memorized really only need to be done in HP2, since I use it to mark a tile as rotated
#if DX11_HP2
	// Metallicafan212:	Make sure wireframe doesn't get set on tiles!
	DWORD OldFlags = ExtraRasterFlags;

	if(ExtraRasterFlags & DXRS_Wireframe)
		ExtraRasterFlags = 0;

	if(m_nearZRangeHackProjectionActive)
		SetProjectionStateNoCheck(false);

	// Metallicafan212:	Reset the tile state (if needed)
	if (!(PolyFlags & PF_RenderFog))
		FTileShader->bDoTileRotation = 0;

	PolyFlags &= ~PF_RenderFog;

#elif DX11_UT_469
	if (m_nearZRangeHackProjectionActive)
		SetProjectionStateNoCheck(false);
#else
	//SetRasterState(DXRS_Normal | DXRS_NoAA);

	if(m_nearZRangeHackProjectionActive)
		SetProjectionStateNoCheck(false);
#endif

#if DX11_HP2
	if (Info.Palette && !(PolyFlags & PF_Translucent | PF_AlphaBlend))
	{
		PolyFlags |= PF_Highlighted | PF_Occlude;
	}
#endif

#if DX11_HP2
	// Metallicafan212:	Adjust the polyflags if we're using alpha
	if (Color.W != 0.0f && !(PolyFlags & (PF_Translucent | PF_Modulated)))
	{
		PolyFlags |= PF_AlphaBlend;
	}
#elif DX11_UNREAL_227
	// Metallicafan212:	Disable alpha if we're not alpha blending
	if (!(PolyFlags & PF_AlphaBlend))
	{
		Color.W = 1.0f;
	}
#elif DX11_UT_469
	if (Info.Texture != nullptr && Info.Texture->Alpha != 0.0f)
	{
		Color.W = Info.Texture->Alpha;
		PolyFlags |= PF_AlphaBlend;
	}
#endif

	// Metallicafan212:	Tile Alpha is reversed to account for the engine always fucking sending 0 for things that are 100% visible
#if DX11_UNREAL_227
#elif DX11_HP2
	Color.W = 1.0f - Color.W;
#elif DX11_UT_469 //|| DX11_UNREAL_227
	if(Color.W == 0.0f)
		Color.W = 1.0f;
#else
	Color.W = 1.0f;
#endif

	// Metallicafan212:	Calculate the UV division
	FLOAT UScale = (Info.UScale * Info.USize);
	FLOAT VScale = (Info.VScale * Info.VSize);
	FLOAT UDiv = 1.0f / UScale;
	FLOAT VDiv = 1.0f / VScale;

	// Metallicafan212:	Convert to floating point UVs
	FLOAT UF	= U * UDiv;
	FLOAT ULF	= (U + UL) * UDiv;
	FLOAT VF	= V * VDiv;
	FLOAT VLF	= (V + VL) * VDiv;

	// Metallicafan212:	Ceil the values to make sure they don't cross into multiple squares
	FLOAT UC	= appCeil(UF);
	FLOAT ULC	= appCeil(ULF);
	FLOAT VC	= appCeil(VF);
	FLOAT VLC	= appCeil(VLF);
	
	// Metallicafan212:	FUCK IT! No more UV shifting, just going to test if the UVs are in 0-1
	if (
			//UF < 0.0f || ULF < 0.0f || VF < 0.0f || VLF < 0.0f
		// Metallicafan212:	Does the UVs cross into a second square
		/*||*/ 	((abs(UC - ULC) > 1.0f || abs(VC - VLC) > 1.0f))//&& (UC <= 1.0f && VC <= 1.0f))
		)
	{
		// Metallicafan212:	The user might've actually wanted it to be clamped
#if !DX11_HP2
		PolyFlags &= ~PF_ClampUVs;
#endif
	}
	else
	{
		PolyFlags |= PF_ClampUVs;

		// Metallicafan212:	Shift UVs into the correct quadrant
		//					TODO! Handle mirrored tiles
		FLOAT Temp;

		// Metallicafan212:	Wrap in the "X" direction if it's above one square
		if ( abs(ULF) > 1.0f )//&& (UL - U >= 0.0f))
		{
			U	= std::modff(U, &Temp);
		}

		// Metallicafan212:	Wrap in the "Y" direction if it's above one square
		if (abs(VLF) > 1.0f )//&& (VL - V >= 0.0f))//&& V < VL)
		{
			V	= std::modff(V, &Temp);
		}
	}

#if DX11_HP2
	if (PolyFlags & PF_AlphaBlend)
		PolyFlags &= ~(PF_ForceZWrite | PF_Occlude);
#endif

	// Metallicafan212:	Setup blending
	SetBlend(PolyFlags);

	UBOOL bNoAF = 0;

	// Metallicafan212:	See if this is rendering the hud, if so ask for no AA for tiles
#if DX11_HP2
	UBOOL bHudTile = GUglyHackFlags & HF_PostRender;
#elif DX11_UT_469
	UBOOL bHudTile = GUglyHackFlags & HACKFLAGS_PostRender;
#else
	// Metallicafan212:	Other games don't render the hud with the hack flag on....
	//					Just define it as a constant here so that the compiler (should) optimize it away
#define bHudTile 1
#endif
#if 1
	// Metallicafan212:	Likely the hud, hack it!
	// Metallicafan212:	NEVERMIND!!!! Somehow this breaks the UT fade!
	if ((Z >= 0.0f) && (Z <= 1.0f)  )//bHudTile)
	{
		// Metallicafan212:	TODO! There's been some glitchyness due to actor triangles drawing through hud elements, so forcing 0.5 might be needed, or maybe requesting near z range instead
		Z = 0.5f;

		// Metallicafan212:	Request no AA if we're a hud tile
		SetRasterState(DXRS_Normal | DXRS_NoAA);

		bNoAF = 1;

		// Metallicafan212:	TODO! Look into doing this a bit less hacky... Maybe as a shader set mode?
		/*
		// Metallicafan212:	Force update masking alpha if we're currently in masked mode
		if (bSmoothHudTiles && (PolyFlags & PF_Masked) && GlobalPolyflagVars.AlphaReject == MaskedAlphaReject)
		{
			// Metallicafan212:	Draw what was there already
			EndBuffering();

			// Metallicafan212:	Now update the value
			GlobalPolyflagVars.AlphaReject = SmoothMaskedAlphaReject;
			UpdatePolyflagsVars();
		}
		*/
	}
	else
	{
		// Metallicafan212:	For normal tiles in the worldspace, request AA with depth (otherwise we get yelled at by DX11)
		SetRasterState(DXRS_Normal);
	}
#endif

	// Metallicafan212:	Restore the extra rasterization flags
#if DX11_HP2
	ExtraRasterFlags = OldFlags;
#endif

	SetTexture(0, &Info, PolyFlags, bNoAF);

	FLOAT TexInfoUMult = UDiv;
	FLOAT TexInfoVMult = VDiv;

	if(SceneNodeHack)
	{
		if ((Frame->X != m_sceneNodeX) || (Frame->Y != m_sceneNodeY))
		{
			SetSceneNode(Frame);
		}
	}

#if 0//DX11_UT_469
	UBOOL bFontHack = (PolyFlags & (PF_NoSmooth | PF_Highlighted)) == (PF_NoSmooth | PF_Highlighted);
#else
	UBOOL bFontHack = ((PolyFlags & (PF_NoSmooth))); //| PF_Masked)) == (PF_NoSmooth | PF_Masked));//(PolyFlags & (PF_NoSmooth | PF_Masked)) == (PF_NoSmooth | PF_Masked);
#endif

	// Metallicafan212:	Per CacoFFF's suggestion, add/remove 0.1f * U/VSize when rendering fonts
	FLOAT ExtraU = 0.0f;
	FLOAT ExtraV = 0.0f;

	if ((bFontHack && ( (NumAASamples > 1 /*&& !bUseForcedSampleCount*/) || bIsNV)))
	{
		// Metallicafan212:	Correct this based on the mip size as well
		ExtraU = TileAAUVMove;
		ExtraV = TileAAUVMove;		
	}

	// Metallicafan212:	Bind the tile shader
	FTileShader->Bind(m_RenderContext);

	FLOAT PX1 = X - Frame->FX2;
	FLOAT PX2 = PX1 + XL;
	FLOAT PY1 = Y - Frame->FY2;
	FLOAT PY2 = PY1 + YL;

	// Metallicafan212:	Scale UP!!!!
	FLOAT RPX1 = m_RFX2 * PX1;
	FLOAT RPX2 = m_RFX2 * PX2;
	FLOAT RPY1 = m_RFY2 * PY1;
	FLOAT RPY2 = m_RFY2 * PY2;

	if (BoundRT == nullptr && ResolutionScale != 1.0f)
	{
		RPX1 *= ResolutionScale;
		RPX2 *= ResolutionScale;
		RPY1 *= ResolutionScale;
		RPY2 *= ResolutionScale;
	}

	if (Frame->Viewport->Actor != nullptr && Frame->Viewport->IsOrtho())
	{
		Z = 1.0f;
	}

	//if (Frame->Viewport->Actor != nullptr && !Frame->Viewport->IsOrtho()) 
	{
		Z = abs(Z);
		RPX1 *= Z;
		RPX2 *= Z;
		RPY1 *= Z;
		RPY2 *= Z;
	}

	// Metallicafan212:	Swap the color out if we're modulated. Somehow I missed this the first time around
	if (PolyFlags & PF_Modulated)
	{
		Color = FPlane(1.f, 1.f, 1.f, 1.f);
	}

	// Metallicafan212:	Selection testing!!!!
	if (m_HitData != nullptr)
		Color = CurrentHitColor;

	FLOAT SU1			= (U * TexInfoUMult)		+ ExtraU;
	FLOAT SU2			= ((U + UL) * TexInfoUMult) + ExtraU;
	FLOAT SV1			= (V * TexInfoVMult)		+ ExtraV;
	FLOAT SV2			= ((V + VL) * TexInfoVMult) + ExtraV;

#if QUAD_TILES
	LockVertAndIndexBuffer(4, 6);

	// Metallicafan212:	Render as a unquaded quad
	m_VertexBuff[0].Color	= Color;
	m_VertexBuff[0].X		= RPX1;
	m_VertexBuff[0].Y		= RPY1;
	m_VertexBuff[0].Z		= Z;
	m_VertexBuff[0].U		= SU1;
	m_VertexBuff[0].V		= SV1;

	m_VertexBuff[1].Color	= Color;
	m_VertexBuff[1].X		= RPX2;
	m_VertexBuff[1].Y		= RPY1;
	m_VertexBuff[1].Z		= Z;
	m_VertexBuff[1].U		= SU2;
	m_VertexBuff[1].V		= SV1;

	m_VertexBuff[2].Color	= Color;
	m_VertexBuff[2].X		= RPX1;
	m_VertexBuff[2].Y		= RPY2;
	m_VertexBuff[2].Z		= Z;
	m_VertexBuff[2].U		= SU1;
	m_VertexBuff[2].V		= SV2;

	m_VertexBuff[3].Color	= Color;
	m_VertexBuff[3].X		= RPX2;
	m_VertexBuff[3].Y		= RPY2;
	m_VertexBuff[3].Z		= Z;
	m_VertexBuff[3].U		= SU2;
	m_VertexBuff[3].V		= SV2;

	// Metallicafan212:	Now indicies
	INDEX baseVIndex		= m_BufferedVerts;
	m_IndexBuff[0]			= baseVIndex;
	m_IndexBuff[1]			= baseVIndex + 1;
	m_IndexBuff[2]			= baseVIndex + 3;
	m_IndexBuff[3]			= baseVIndex + 2;
	m_IndexBuff[4]			= baseVIndex + 3;
	m_IndexBuff[5]			= baseVIndex;
#else
	LockVertAndIndexBuffer(6);
	// Buffer the tiles
	m_VertexBuff[0].Color	= Color;
	m_VertexBuff[0].X		= RPX1;
	m_VertexBuff[0].Y		= RPY1;
	m_VertexBuff[0].Z		= Z;
	m_VertexBuff[0].U		= SU1;
	m_VertexBuff[0].V		= SV1;

	m_VertexBuff[1].Color	= Color;
	m_VertexBuff[1].X		= RPX2;
	m_VertexBuff[1].Y		= RPY1;
	m_VertexBuff[1].Z		= Z;
	m_VertexBuff[1].U		= SU2;
	m_VertexBuff[1].V		= SV1;

	m_VertexBuff[2].Color	= Color;
	m_VertexBuff[2].X		= RPX2;
	m_VertexBuff[2].Y		= RPY2;
	m_VertexBuff[2].Z		= Z;
	m_VertexBuff[2].U		= SU2;
	m_VertexBuff[2].V		= SV2;

	m_VertexBuff[3].Color	= Color;
	m_VertexBuff[3].X		= RPX1;
	m_VertexBuff[3].Y		= RPY1;
	m_VertexBuff[3].Z		= Z;
	m_VertexBuff[3].U		= SU1;
	m_VertexBuff[3].V		= SV1;

	m_VertexBuff[4].Color	= Color;
	m_VertexBuff[4].X		= RPX2;
	m_VertexBuff[4].Y		= RPY2;
	m_VertexBuff[4].Z		= Z;
	m_VertexBuff[4].U		= SU2;
	m_VertexBuff[4].V		= SV2;

	m_VertexBuff[5].Color	= Color;
	m_VertexBuff[5].X		= RPX1;
	m_VertexBuff[5].Y		= RPY2;
	m_VertexBuff[5].Z		= Z;
	m_VertexBuff[5].U		= SU1;
	m_VertexBuff[5].V		= SV2;
#endif

	AdvanceVertPos();


	unguard;
}

// Metallicafan212:	OU UT469e+ specific tile list support
#if DX11_UT_469
void UICBINDx11RenderDevice::DrawTileList(const FSceneNode* Frame, const FTextureInfo& Info, const FTileRect* Tiles, INT NumTiles, FSpanBuffer* Span, FLOAT Z, FPlane Color, FPlane Fog, DWORD PolyFlags)
{
	guard(UICBINDx11RenderDevice::DrawTileList);

	// Metallicafan212:	Start buffering now
	StartBuffering(BT_Tiles);

	// Metallicafan212:	The check for memorized really only need to be done in HP2, since I use it to mark a tile as rotated
#if DX11_HP2
	// Metallicafan212:	Make sure wireframe doesn't get set on tiles!
	DWORD OldFlags = ExtraRasterFlags;

	if (ExtraRasterFlags & DXRS_Wireframe)
		ExtraRasterFlags = 0;

	if (m_nearZRangeHackProjectionActive)
		SetProjectionStateNoCheck(false);

	// Metallicafan212:	Reset the tile state (if needed)
	if (!(PolyFlags & PF_RenderFog))
		FTileShader->bDoTileRotation = 0;

	PolyFlags &= ~PF_RenderFog;

#elif DX11_UT_469
	if (m_nearZRangeHackProjectionActive)
		SetProjectionStateNoCheck(false);
#else
	//SetRasterState(DXRS_Normal | DXRS_NoAA);

	if (m_nearZRangeHackProjectionActive)
		SetProjectionStateNoCheck(false);
#endif

#if DX11_HP2
	if (Info.Palette && !(PolyFlags & PF_Translucent | PF_AlphaBlend))
	{
		PolyFlags |= PF_Highlighted | PF_Occlude;
	}
#endif

#if DX11_HP2
	// Metallicafan212:	Adjust the polyflags if we're using alpha
	if (Color.W != 0.0f && !(PolyFlags & (PF_Translucent | PF_Modulated)))
	{
		PolyFlags |= PF_AlphaBlend;
	}
#elif DX11_UT_469
	if (Info.Texture != nullptr && Info.Texture->Alpha != 0.0f)
	{
		Color.W = Info.Texture->Alpha;
		PolyFlags |= PF_AlphaBlend;
	}
#endif

	// Metallicafan212:	Tile Alpha is reversed to account for the engine always fucking sending 0 for things that are 100% visible
#if DX11_HP2
	Color.W = 1.0f - Color.W;
#elif DX11_UT_469
	if (Color.W == 0.0f)
		Color.W = 1.0f;
#else
	Color.W = 1.0f;
#endif

	// Metallicafan212:	Calculate the UV division
	FLOAT UScale	= (Info.UScale * Info.USize);
	FLOAT VScale	= (Info.VScale * Info.VSize);
	FLOAT UDiv		= 1.0f / UScale;
	FLOAT VDiv		= 1.0f / VScale;

	// Metallicafan212:	TODO! UV clamping on each tile?
	/*
	// Metallicafan212:	Convert to floating point UVs
	FLOAT UF		= U * UDiv;
	FLOAT ULF		= (U + UL) * UDiv;
	FLOAT VF		= V * VDiv;
	FLOAT VLF		= (V + VL) * VDiv;

	// Metallicafan212:	Ceil the values to make sure they don't cross into multiple squares
	FLOAT UC		= appCeil(UF);
	FLOAT ULC		= appCeil(ULF);
	FLOAT VC		= appCeil(VF);
	FLOAT VLC		= appCeil(VLF);

	// Metallicafan212:	FUCK IT! No more UV shifting, just going to test if the UVs are in 0-1
	if (
		//UF < 0.0f || ULF < 0.0f || VF < 0.0f || VLF < 0.0f
	// Metallicafan212:	Does the UVs cross into a second square
		 	((abs(UC - ULC) > 1.0f || abs(VC - VLC) > 1.0f))//&& (UC <= 1.0f && VC <= 1.0f))
		)
	{
		// Metallicafan212:	The user might've actually wanted it to be clamped
#if !DX11_HP2
		PolyFlags &= ~PF_ClampUVs;
#endif
	}
	else
	{
		PolyFlags |= PF_ClampUVs;

		// Metallicafan212:	Shift UVs into the correct quadrant
		//					TODO! Handle mirrored tiles
		FLOAT Temp;

		// Metallicafan212:	Wrap in the "X" direction if it's above one square
		if (abs(ULF) > 1.0f)//&& (UL - U >= 0.0f))
		{
			U	= std::modff(U, &Temp);
		}

		// Metallicafan212:	Wrap in the "Y" direction if it's above one square
		if (abs(VLF) > 1.0f)//&& (VL - V >= 0.0f))//&& V < VL)
		{
			V	= std::modff(V, &Temp);
		}
	}
	*/

#if DX11_HP2
	if (PolyFlags & PF_AlphaBlend)
		PolyFlags &= ~(PF_ForceZWrite | PF_Occlude);
#endif

	// Metallicafan212:	Setup blending
	SetBlend(PolyFlags);

	UBOOL bNoAF = 0;

	//Adjust Z coordinate if Z range hack is active
	//if (1)//(m_useZRangeHack)
#if 1
	// Metallicafan212:	Likely the hud, hack it!
	if ((Z >= 0.0f) && (Z < 8.0f))
	{
		// Metallicafan212:	TODO! There's been some glitchyness due to actor triangles drawing through hud elements, so forcing 0.5 might be needed, or maybe requesting near z range instead
		Z = 0.5f;

		// Metallicafan212:	Request no AA if we're a hud tile
		SetRasterState(DXRS_Normal | DXRS_NoAA);

		bNoAF = 1;
	}
	else
	{
		// Metallicafan212:	For normal tiles in the worldspace, request AA with depth (otherwise we get yelled at by DX11)
		SetRasterState(DXRS_Normal);
	}
#endif

	// Metallicafan212:	Restore the extra rasterization flags
#if DX11_HP2
	ExtraRasterFlags = OldFlags;
#endif

	SetTexture(0, &Info, PolyFlags, bNoAF);

	FLOAT TexInfoUMult = UDiv;
	FLOAT TexInfoVMult = VDiv;

	if (SceneNodeHack)
	{
		if ((Frame->X != m_sceneNodeX) || (Frame->Y != m_sceneNodeY))
		{
			SetSceneNode(const_cast<FSceneNode*>(Frame));
		}
	}

#if 0//DX11_UT_469
	UBOOL bFontHack = (PolyFlags & (PF_NoSmooth | PF_Highlighted)) == (PF_NoSmooth | PF_Highlighted);
#else
	UBOOL bFontHack = ((PolyFlags & (PF_NoSmooth))); //| PF_Masked)) == (PF_NoSmooth | PF_Masked));//(PolyFlags & (PF_NoSmooth | PF_Masked)) == (PF_NoSmooth | PF_Masked);
#endif

	// Metallicafan212:	Per CacoFFF's suggestion, add/remove 0.1f * U/VSize when rendering fonts
	FLOAT ExtraU = 0.0f;
	FLOAT ExtraV = 0.0f;

	if ((bFontHack && ((NumAASamples > 1 /*&& !bUseForcedSampleCount*/) || bIsNV)))
	{
		// Metallicafan212:	Correct this based on the mip size as well
		ExtraU = TileAAUVMove;
		ExtraV = TileAAUVMove;
	}

	if (Frame->Viewport->Actor != nullptr && Frame->Viewport->IsOrtho())
	{
		Z = 1.0f;
	}

	Z = abs(Z);

	// Metallicafan212:	Bind the tile shader
	FTileShader->Bind(m_RenderContext);

	// Metallicafan212:	Lock early
	LockVertAndIndexBuffer(4 * NumTiles, 6 * NumTiles);

	// Metallicafan212:	Swap the color out if we're modulated. Somehow I missed this the first time around
	if (PolyFlags & PF_Modulated)
	{
		Color = FPlane(1.f, 1.f, 1.f, 1.f);
	}

	// Metallicafan212:	Selection testing!!!!
	if (m_HitData != nullptr)
		Color = CurrentHitColor;

	INDEX baseVIndex		= m_BufferedVerts;

	// Metallicafan212:	Now loop and render
	for (INT i = 0; i < NumTiles; i++)
	{
		const FTileRect& Tile = Tiles[i];

		FLOAT PX1 = Tile.X - Frame->FX2;
		FLOAT PX2 = PX1 + Tile.XL;
		FLOAT PY1 = Tile.Y - Frame->FY2;
		FLOAT PY2 = PY1 + Tile.YL;

		// Metallicafan212:	Scale UP!!!!
		FLOAT RPX1 = m_RFX2 * PX1;
		FLOAT RPX2 = m_RFX2 * PX2;
		FLOAT RPY1 = m_RFY2 * PY1;
		FLOAT RPY2 = m_RFY2 * PY2;

		if (BoundRT == nullptr && ResolutionScale != 1.0f)
		{
			RPX1 *= ResolutionScale;
			RPX2 *= ResolutionScale;
			RPY1 *= ResolutionScale;
			RPY2 *= ResolutionScale;
		}

		//if (Frame->Viewport->Actor != nullptr && !Frame->Viewport->IsOrtho()) 
		{
			RPX1 *= Z;
			RPX2 *= Z;
			RPY1 *= Z;
			RPY2 *= Z;
		}

		FLOAT SU1			= (Tile.U * TexInfoUMult)				+ ExtraU;
		FLOAT SU2			= ((Tile.U + Tile.UL) * TexInfoUMult)	+ ExtraU;
		FLOAT SV1			= (Tile.V * TexInfoVMult)				+ ExtraV;
		FLOAT SV2			= ((Tile.V + Tile.VL) * TexInfoVMult)	+ ExtraV;

#if QUAD_TILES
		// Metallicafan212:	Render as a unquaded quad
		m_VertexBuff[0].Color	= Color;
		m_VertexBuff[0].X		= RPX1;
		m_VertexBuff[0].Y		= RPY1;
		m_VertexBuff[0].Z		= Z;
		m_VertexBuff[0].U		= SU1;
		m_VertexBuff[0].V		= SV1;

		m_VertexBuff[1].Color	= Color;
		m_VertexBuff[1].X		= RPX2;
		m_VertexBuff[1].Y		= RPY1;
		m_VertexBuff[1].Z		= Z;
		m_VertexBuff[1].U		= SU2;
		m_VertexBuff[1].V		= SV1;

		m_VertexBuff[2].Color	= Color;
		m_VertexBuff[2].X		= RPX1;
		m_VertexBuff[2].Y		= RPY2;
		m_VertexBuff[2].Z		= Z;
		m_VertexBuff[2].U		= SU1;
		m_VertexBuff[2].V		= SV2;

		m_VertexBuff[3].Color	= Color;
		m_VertexBuff[3].X		= RPX2;
		m_VertexBuff[3].Y		= RPY2;
		m_VertexBuff[3].Z		= Z;
		m_VertexBuff[3].U		= SU2;
		m_VertexBuff[3].V		= SV2;

		// Metallicafan212:	Go to the next set
		m_VertexBuff += 4;

		// Metallicafan212:	Now indicies
		m_IndexBuff[0]			= baseVIndex;
		m_IndexBuff[1]			= baseVIndex + 1;
		m_IndexBuff[2]			= baseVIndex + 3;
		m_IndexBuff[3]			= baseVIndex + 2;
		m_IndexBuff[4]			= baseVIndex + 3;
		m_IndexBuff[5]			= baseVIndex;

		// Metallicafan212: Next set of vertices
		baseVIndex += 4;

		// Metallicafan212:	Next part of the index array
		m_IndexBuff += 6;
#else
		LockVertAndIndexBuffer(6);
		// Buffer the tiles
		m_VertexBuff[0].Color	= Color;
		m_VertexBuff[0].X		= RPX1;
		m_VertexBuff[0].Y		= RPY1;
		m_VertexBuff[0].Z		= Z;
		m_VertexBuff[0].U		= SU1;
		m_VertexBuff[0].V		= SV1;

		m_VertexBuff[1].Color	= Color;
		m_VertexBuff[1].X		= RPX2;
		m_VertexBuff[1].Y		= RPY1;
		m_VertexBuff[1].Z		= Z;
		m_VertexBuff[1].U		= SU2;
		m_VertexBuff[1].V		= SV1;

		m_VertexBuff[2].Color	= Color;
		m_VertexBuff[2].X		= RPX2;
		m_VertexBuff[2].Y		= RPY2;
		m_VertexBuff[2].Z		= Z;
		m_VertexBuff[2].U		= SU2;
		m_VertexBuff[2].V		= SV2;

		m_VertexBuff[3].Color	= Color;
		m_VertexBuff[3].X		= RPX1;
		m_VertexBuff[3].Y		= RPY1;
		m_VertexBuff[3].Z		= Z;
		m_VertexBuff[3].U		= SU1;
		m_VertexBuff[3].V		= SV1;

		m_VertexBuff[4].Color	= Color;
		m_VertexBuff[4].X		= RPX2;
		m_VertexBuff[4].Y		= RPY2;
		m_VertexBuff[4].Z		= Z;
		m_VertexBuff[4].U		= SU2;
		m_VertexBuff[4].V		= SV2;

		m_VertexBuff[5].Color	= Color;
		m_VertexBuff[5].X		= RPX1;
		m_VertexBuff[5].Y		= RPY2;
		m_VertexBuff[5].Z		= Z;
		m_VertexBuff[5].U		= SU1;
		m_VertexBuff[5].V		= SV2;
#endif
	}

	// Metallicafan212:	All the tiles are done, so now advance the buffer positions
	AdvanceVertPos();

	unguard;
}
#endif

#if DX11_HP2
void UICBINDx11RenderDevice::DrawRotatedTile(FSceneNode* Frame, FTextureInfo& Info, FLOAT X, FLOAT Y, FLOAT XL, FLOAT YL, FLOAT U, FLOAT V, FLOAT UL, FLOAT VL, FSpanBuffer* Span, FLOAT Z, FPlane Color, FPlane Fog, PFLAG PolyFlags, FCoords InCoords)
{
	// Metallicafan212: We call the original version, but tell it to not reset the coord or bool state
	if (!FTileShader->bDoTileRotation && m_CurrentBuff == BT_Tiles)
		EndBuffering();

	FTileShader->bDoTileRotation	= 1;
	FTileShader->TileCoords			= InCoords;
	DrawTile(Frame, Info, X, Y, XL, YL, U, V, UL, VL, Span, Z, Color, Fog, PolyFlags | PF_RenderFog);
}
#endif