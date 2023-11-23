#include "ICBINDx11Drv.h"

// Metallicafan212:	Definitions for tile-related functions
void UICBINDx11RenderDevice::DrawTile(FSceneNode* Frame, FTextureInfo& Info, FLOAT X, FLOAT Y, FLOAT XL, FLOAT YL, FLOAT U, FLOAT V, FLOAT UL, FLOAT VL, FSpanBuffer* Span, FLOAT Z, FPlane Color, FPlane Fog, PFLAG PolyFlags)
{
	guard(UICBINDx11RenderDevice::DrawTile);

	// Metallicafan212:	THIS DOESN'T FUCKING WORK!
	//					DX11 doesn't allow you to toggle the AA state!
	//					I'm going to have to investigate some pixel shader approach, or not use MSAA!!!
	/*
	// Metallicafan212:	Turn off AA for tiles
	//					TODO! Maybe make this an ini option?
	SetRasterState(DXRS_Normal | DXRS_NoAA);
	*/

	// Metallicafan212:	The check for memorized really only need to be done in HP2, since I use it to mark a tile as rotated
#if DX11_HP2
	// Metallicafan212:	Make sure wireframe doesn't get set on tiles!
	DWORD OldFlags = ExtraRasterFlags;

	if(ExtraRasterFlags & DXRS_Wireframe)
		ExtraRasterFlags = 0;

	SetRasterState(DXRS_Normal);

	ExtraRasterFlags = OldFlags;

	if(m_nearZRangeHackProjectionActive)
		SetProjectionStateNoCheck(false);

	// Metallicafan212:	Reset the tile state (if needed)
	if (!(PolyFlags & PF_RenderFog))
		FTileShader->bDoTileRotation = 0;

	PolyFlags &= ~PF_RenderFog;
#else
	SetRasterState(DXRS_Normal);

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
#endif

	//if (PolyFlags & PF_Highlighted)
	//{
	//	PolyFlags = PF_Highlighted;
	//}

	// Metallicafan212:	Needed for tiles
	//					Basically, non-looping tiles have AF issues, so I auto clamp to reduce these issues
	//					Fixes editor icons and the like
	if ((abs(UL) <= Info.USize && abs(VL) <= Info.VSize))
		PolyFlags |= PF_ClampUVs;
	else
		PolyFlags &= ~PF_ClampUVs;

#if DX11_HP2
	if (PolyFlags & PF_AlphaBlend)
		PolyFlags &= ~(PF_ForceZWrite | PF_Occlude);
#endif


	// Metallicafan212:	Setup blending
	SetBlend(PolyFlags);

	//if (SceneNodeHack) //&& !bUsingRT) 
	//if(1)
	if(SceneNodeHack)
	{
		if ((Frame->X != m_sceneNodeX) || (Frame->Y != m_sceneNodeY))
		{
			//m_sceneNodeHackCount++;
			SetSceneNode(Frame);
		}
	}

	SetTexture(0, &Info, PolyFlags);

#if 0//DX11_UT_469
	UBOOL bFontHack = (PolyFlags & (PF_NoSmooth | PF_Highlighted)) == (PF_NoSmooth | PF_Highlighted);
#else
	UBOOL bFontHack = (PolyFlags & PF_NoSmooth | PF_Masked) == (PolyFlags & PF_NoSmooth | PF_Masked);//(PolyFlags & (PF_NoSmooth | PF_Masked)) == (PF_NoSmooth | PF_Masked);
#endif

	// Metallicafan212:	Per CacoFFF's suggestion, add/remove 0.1f * U/VSize when rendering fonts
	FLOAT ExtraU = 0.0f;
	FLOAT ExtraV = 0.0f;

	if (bFontHack && (bIsNV || NumAASamples > 1))
	{
		//ExtraU = 0.1f / Info.USize;
		ExtraU = TileAAUVMove / Info.USize;
		//ExtraV = 0.1f / Info.VSize;
		ExtraV = TileAAUVMove / Info.VSize;
	}

	// Metallicafan212:	Use a separate centroid UV input if we have a font tile (no smooth) and have MSAA on!
	FTileShader->bDoMSAAFontHack = (bFontHack && (NumAASamples > 1));


	//Adjust Z coordinate if Z range hack is active
	//if (1)//(m_useZRangeHack)
	if(1)
	{
		// Metallicafan212:	Likely the hud, hack it!
		if ((Z >= 0.5f) && (Z < 8.0f))
		{
			// Metallicafan212:	TODO! There's been some glitchyness due to actor triangles drawing through hud elements, so forcing 0.5 might be needed, or maybe requesting near z range instead
			Z = 0.5f;
			//SetProjectionStateNoCheck(false);
			//Z = (((Z - 0.5f) / 7.5f) * 2.0f) + 2.0f; 
		}
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

	// Metallicafan212:	Tile Alpha is reversed to account for the engine always fucking sending 0 for things that are 100% visible
#if DX11_HP2
	Color.W = 1.0f - Color.W;
#else
	Color.W = 1.0f;
#endif

	// Metallicafan212:	Swap the color out if we're modulated. Somehow I missed this the first time around
	if (PolyFlags & PF_Modulated)
	{
		Color = FPlane(1.f, 1.f, 1.f, 1.f);
	}


	// Metallicafan212:	Selection testing!!!!
	if (m_HitData != nullptr)
		Color = CurrentHitColor;

	// Metallicafan212:	Start buffering now
	StartBuffering(BT_Tiles);

	//LockVertexBuffer(6 * sizeof(FD3DVert));
	LockVertAndIndexBuffer(6);

	FLOAT TexInfoUMult = BoundTextures[0].UMult;
	FLOAT TexInfoVMult = BoundTextures[0].VMult;

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

	//UnlockVertexBuffer();
	//UnlockBuffers();

	// Metallicafan212:	Now draw
	m_RenderContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	AdvanceVertPos();//6);


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