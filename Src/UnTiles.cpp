#include "ICBINDx11Drv.h"

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
	if (Color.W != 0.0f)
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
			UF < 0.0f || ULF < 0.0f || VF < 0.0f || VLF < 0.0f
		// Metallicafan212:	Does the UVs cross into a second square
		||	(abs(UC - ULC) > 1.0f || abs(VC - VLC) > 1.0f)
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
	}

#if DX11_HP2
	if (PolyFlags & PF_AlphaBlend)
		PolyFlags &= ~(PF_ForceZWrite | PF_Occlude);
#endif

	// Metallicafan212:	Setup blending
	//					Per Buggie, this causes issues with tiles in his mod, so removing it
#if 0//DX11_UT_469
	ADJUST_PFLAGS(PolyFlags);
#endif
	SetBlend(PolyFlags);

	UBOOL bNoAF = 0;

	//Adjust Z coordinate if Z range hack is active
	//if (1)//(m_useZRangeHack)
	if (1)
	{
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
	}

	// Metallicafan212:	Restore the extra rasterization flags
#if DX11_HP2
	ExtraRasterFlags = OldFlags;
#endif

	/*
	// Metallicafan212:	Does the UVs cross into a second square
	if (abs(UC - ULC) > 1.0f || abs(VC - VLC) > 1.0)
	{
		PolyFlags &= ~PF_ClampUVs;
	}
	else
	{
		PolyFlags |= PF_ClampUVs;

		// Metallicafan212:	It's within a full square, move it to the right location
		U	-= appFloor(UF) * UScale;
		V	-= appFloor(VF) * VScale;
	}
	*/

	/*
	// Metallicafan212:	Tile check, see if it's beyond a full tile
	FLOAT UF	= (UL - U) * TexInfoUMult;/// Info.USize;
	FLOAT VF	= (VL - V) * TexInfoVMult;/// Info.VSize;
	FLOAT UFN	= (UL + U) * TexInfoUMult;
	FLOAT VFN	= (VL + V) * TexInfoVMult;

	// Metallicafan212:	Needed for tiles
	//					Basically, non-looping tiles have AF issues, so I auto clamp to reduce these issues
	//					Fixes editor icons and the like
	//					This should be disabled if the UL and VL will make it loop
	//if ((abs(U + UL) <= Info.USize && abs(V + VL) <= Info.VSize))

	// Metallicafan212:	Reversed and revised this check, as it needs to see if the UVs loop or cross a barrior
	//if (abs(UF) > 1.0f || abs(VF) > 1.0f || std::signbit(UL) != std::signbit(U) || std::signbit(VL) != std::signbit(V))
	{
		PolyFlags &= ~PF_ClampUVs;
	}
	//else
	//{
	//	PolyFlags |= PF_ClampUVs;
	//}
	*/

	SetTexture(0, &Info, PolyFlags, bNoAF);


	FLOAT TexInfoUMult = UDiv;
	FLOAT TexInfoVMult = VDiv;

	if(SceneNodeHack)
	{
		if ((Frame->X != m_sceneNodeX) || (Frame->Y != m_sceneNodeY))
		{
			//m_sceneNodeHackCount++;
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

	if ((bFontHack && ( (NumAASamples > 1 && !bSupportsForcedSampleCount) || bIsNV)))
	{
		// Metallicafan212:	Correct this based on the mip size as well
		ExtraU = TileAAUVMove * TexInfoUMult;
		ExtraV = TileAAUVMove * TexInfoVMult;
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

#if 1//!RES_SCALE_IN_PROJ
	if (BoundRT == nullptr && ResolutionScale != 1.0f)
	{
		RPX1 *= ResolutionScale;
		RPX2 *= ResolutionScale;
		RPY1 *= ResolutionScale;
		RPY2 *= ResolutionScale;
	}
#endif


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

	LockVertAndIndexBuffer(6);

	FLOAT SU1			= (U * TexInfoUMult)		+ ExtraU;
	FLOAT SU2			= ((U + UL) * TexInfoUMult) + ExtraU;
	FLOAT SV1			= (V * TexInfoVMult)		+ ExtraV;
	FLOAT SV2			= ((V + VL) * TexInfoVMult) + ExtraV;

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

	AdvanceVertPos();


	unguard;
}

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