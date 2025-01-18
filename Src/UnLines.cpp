#include "ICBINDx11Drv.h"

// Metallicafan212:	Line/point related functions
void UICBINDx11RenderDevice::Draw3DLine(FSceneNode* Frame, FPlane Color, DWORD LineFlags, FVector P1, FVector P2)
{
	guard(UICBINDx11RenderDevice::Draw3DLine);

#if !RES_SCALE_IN_PROJ
	FLOAT ExtraScale = (BoundRT == nullptr ? ResolutionScale : 1.0f);
#else
	FLOAT ExtraScale = 1.0f;
#endif

	// Metallicafan212:	If doing hit testing, make the lines a minimum of 2 pixels thick
	FLOAT Thick = m_HitData != nullptr ? Clamp(ThreeDeeLineThickness, 2.0f, FLT_MAX) : ThreeDeeLineThickness;

	// Metallicafan212:	We need to transform some lines into screen-space, as only some aren't already done
	//					So the collision boxes are already in worldspace, so doing this transform causes it to skew
	//					Consequently, all other lines do not render without this

	// So we check for pre transformed lines and don't transform them
#if DX11_HP2
	if (!(LineFlags & LINE_PreTransformed))
#endif
	{
		// Transform the coords by the frame
		P1 = P1.TransformPointBy(Frame->Coords);
		P2 = P2.TransformPointBy(Frame->Coords);
	}

	if (Frame->Viewport->IsOrtho())
	{
		// Zoom.
		FLOAT rcpZoom = 1.0f / Frame->Zoom;
		P1.X = (P1.X * rcpZoom) + Frame->FX2;
		P1.Y = (P1.Y * rcpZoom) + Frame->FY2;
		P2.X = (P2.X * rcpZoom) + Frame->FX2;
		P2.Y = (P2.Y * rcpZoom) + Frame->FY2;

		P1.Z = P2.Z = 1.0f;

		// See if points form a line parallel to our line of sight (i.e. line appears as a dot).
		if (Abs(P2.X - P1.X) + Abs(P2.Y - P1.Y) >= 0.2f)
		{
			Draw2DLine(Frame, Color, LineFlags, P1, P2);
		}
		else if (Frame->Viewport->Actor->OrthoZoom < ORTHO_LOW_DETAIL)
		{
			Draw2DPoint(Frame, Color, 0, P1.X - 1.0f, P1.Y - 1.0f, P1.X + 1.0f, P1.Y + 1.0f, P1.Z);
		}
	}
	else
	{
		// Metallicafan212:	Start buffering now
		StartBuffering(BT_Lines);

		SetBlend(PF_Highlighted | PF_Occlude);

#if DX11_HP2
		// Metallicafan212:	Make the alpha reversed as well
		//					I may want to make alpha'd lines in the future
		Color.W = 1.0f - Color.W;

		// Metallicafan212:	Make sure wireframe doesn't get set on lines
		DWORD OldFlags = ExtraRasterFlags;

		if (ExtraRasterFlags & DXRS_Wireframe)
			ExtraRasterFlags = 0;

		SetRasterState(DXRS_Normal);

		ExtraRasterFlags = OldFlags;

		FPlane LineThick(GExtraLineSize * ThreeDeeLineThickness * ExtraScale, 0.0f, 0.0f, 0.0f);
#else
		Color.W = 1.0f;

		SetRasterState(DXRS_Normal);

		FPlane LineThick(Thick * ExtraScale, 0.0f, 0.0f, 0.0f);
#endif

		// Metallicafan212:	Selection testing
		if (m_HitData != nullptr)
		{
			Color = CurrentHitColor;

			// Metallicafan212:	If this isn't HP2, set the priority on lines/points higher
#if 0//!DX11_HP2
			if (PixelTopIndex != -1)
				PixelHitInfo(PixelTopIndex).Priority = 10;
#endif
		}

		// Metallicafan212:	TODO! Other games don't have the flags I added... So we need to make it possible to project lines above everything (maybe a config option???)
#if DX11_HP2
		if (LineFlags & LINE_DrawOver || Viewport->Actor->ShowFlags & SHOW_Lines)
		{
			if (!m_nearZRangeHackProjectionActive)
				SetProjectionStateNoCheck(true);
		}
		else
#elif DX11_UT_469 || DX11_UNREAL_227
		if (!(LineFlags & LINE_DepthCued))//(!(Viewport->Actor->ShowFlags & SHOW_OccludeLines))
		{
			if (!m_nearZRangeHackProjectionActive)
				SetProjectionStateNoCheck(true);
		}
		else
#endif
		{
			if (m_nearZRangeHackProjectionActive)
				SetProjectionStateNoCheck(false);
		}

		// Metallicafan212:	TODO! Line specific shader for making the lines thiccc
		if(CurrentShader != FLineShader)
			FLineShader->Bind(m_RenderContext);

		LockVertAndIndexBuffer(2);

		m_VertexBuff[0].X		= P1.X;
		m_VertexBuff[0].Y		= P1.Y;
		m_VertexBuff[0].Z		= P1.Z;
		m_VertexBuff[0].Color	= Color;
		// Metallicafan212:	Line thickness
		m_VertexBuff[0].Fog		= LineThick;

		m_VertexBuff[1].X		= P2.X;
		m_VertexBuff[1].Y		= P2.Y;
		m_VertexBuff[1].Z		= P2.Z;
		m_VertexBuff[1].Color	= Color;
		// Metallicafan212:	Line thickness
		m_VertexBuff[1].Fog		= LineThick;

		AdvanceVertPos();
	}


	unguard;
}

void UICBINDx11RenderDevice::Draw2DLine(FSceneNode* Frame, FPlane Color, DWORD LineFlags, FVector P1, FVector P2)
{
	guard(UICBINDx11RenderDevice::Draw2DLine);

#if !RES_SCALE_IN_PROJ
	FLOAT ExtraScale = (BoundRT == nullptr ? ResolutionScale : 1.0f);
#else
	FLOAT ExtraScale = 1.0f;
#endif

	P1.X *= ExtraScale;
	P1.Y *= ExtraScale;
	P2.X *= ExtraScale;
	P2.Y *= ExtraScale;

	// Metallicafan212:	If doing hit testing, make the lines a minimum of 2 pixels thick
	FLOAT Thick = m_HitData != nullptr ? Clamp(OrthoLineThickness, 2.0f, FLT_MAX) : OrthoLineThickness;

	// Metallicafan212:	Start buffering now
	StartBuffering(BT_Lines);


	SetBlend(PF_Highlighted | PF_Occlude);

#if DX11_HP2
	// Metallicafan212:	Make the alpha reversed as well
	//					I may want to make alpha'd lines in the future
	Color.W = 1.0f - Color.W;

	// Metallicafan212:	Make sure wireframe doesn't get set on lines
	DWORD OldFlags = ExtraRasterFlags;

	if (ExtraRasterFlags & DXRS_Wireframe)
		ExtraRasterFlags = 0;

	SetRasterState(DXRS_Normal);

	ExtraRasterFlags = OldFlags;

	FPlane LineThick(GExtraLineSize * Thick * ExtraScale, 0.0f, 0.0f, 0.0f);
#else
	Color.W = 1.0f;

	SetRasterState(DXRS_Normal);

	FPlane LineThick(Thick * ExtraScale, 0.0f, 0.0f, 0.0f);

#endif

	// Metallicafan212:	TODO! Same as above, HP2 has a button on the viewport to toggle on line hack projection
#if DX11_HP2
	if (LineFlags & LINE_DrawOver || Viewport->Actor->ShowFlags & SHOW_Lines)
	{
		if(!m_nearZRangeHackProjectionActive)
			SetProjectionStateNoCheck(true);
	}
	else
#elif DX11_UT_469 || DX11_UNREAL_227
	if (!(LineFlags & LINE_DepthCued))//(!(Viewport->Actor->ShowFlags & SHOW_OccludeLines))
	{
		if (!m_nearZRangeHackProjectionActive)
			SetProjectionStateNoCheck(true);
	}
	else
#endif
	{
		if(m_nearZRangeHackProjectionActive)
			SetProjectionStateNoCheck(false);
	}

	if (CurrentShader != FLineShader)
		FLineShader->Bind(m_RenderContext);

	LockVertAndIndexBuffer(2);

	// Metallicafan212:	Convert the line positions into 3D
	FLOAT X1Pos = m_RFX2 * (P1.X - ScaledFX2);
	FLOAT Y1Pos = m_RFY2 * (P1.Y - ScaledFY2);
	FLOAT X2Pos = m_RFX2 * (P2.X - ScaledFX2);
	FLOAT Y2Pos = m_RFY2 * (P2.Y - ScaledFY2);

	// Metallicafan212:	Selection testing
	if (m_HitData != nullptr)
	{
		Color = CurrentHitColor;

		// Metallicafan212:	If this isn't HP2, set the priority on lines/points higher
#if 0//!DX11_HP2
		if (PixelTopIndex != -1)
			PixelHitInfo(PixelTopIndex).Priority = 10;
#endif
	}

	if (!Frame->Viewport->IsOrtho())
	{
		X1Pos *= P1.Z;
		Y1Pos *= P1.Z;
		X2Pos *= P2.Z;
		Y2Pos *= P2.Z;
	}

	m_VertexBuff[0].X		= X1Pos;
	m_VertexBuff[0].Y		= Y1Pos;
	m_VertexBuff[0].Z		= P1.Z;
	m_VertexBuff[0].Color	= Color;
	// Metallicafan212:	Line thickness
	m_VertexBuff[0].Fog		= LineThick;

	m_VertexBuff[1].X		= X2Pos;
	m_VertexBuff[1].Y		= Y2Pos;
	m_VertexBuff[1].Z		= P2.Z;
	m_VertexBuff[1].Color	= Color;
	// Metallicafan212:	Line thickness
	m_VertexBuff[1].Fog		= LineThick;


	AdvanceVertPos();

	unguard;
}

void UICBINDx11RenderDevice::Draw2DPoint(FSceneNode* Frame, FPlane Color, DWORD LineFlags, FLOAT X1, FLOAT Y1, FLOAT X2, FLOAT Y2, FLOAT Z)
{
	guard(UICBINDx11RenderDevice::Draw2DPoint);

#if !RES_SCALE_IN_PROJ
	FLOAT ExtraScale = (BoundRT == nullptr ? ResolutionScale : 1.0f);
#else
	FLOAT ExtraScale = 1.0f;
#endif

	// Metallicafan212:	Start buffering now
	StartBuffering(BT_Points);

	// Metallicafan212:	Remove the texture, as the generic shader supports texturing now
	SetTexture(0, nullptr, 0);

	// Metallicafan212:	Add on size to the input vars, just so that the pivot cross shows up (why didn't they use Draw2DLine????)
	X1 -= 1.f;
	Y1 -= 1.f;
	X2 += 1.f;
	Y2 += 1.f;

	X1 *= ExtraScale;
	Y1 *= ExtraScale;
	X2 *= ExtraScale;
	Y2 *= ExtraScale;

	SetBlend(PF_Highlighted | PF_Occlude);

#if DX11_HP2
	// Metallicafan212:	Make the alpha reversed as well
	//					I may want to make alpha'd lines in the future
	Color.W = 1.0f - Color.W;

	// Metallicafan212:	Make sure wireframe doesn't get set on lines
	DWORD OldFlags = ExtraRasterFlags;

	if (ExtraRasterFlags & DXRS_Wireframe)
		ExtraRasterFlags = 0;

	SetRasterState(DXRS_Normal);

	ExtraRasterFlags = OldFlags;
#else
	Color.W = 1.0f;

	SetRasterState(DXRS_Normal);
#endif

#if DX11_HP2
	if (LineFlags & LINE_DrawOver || Viewport->Actor->ShowFlags & SHOW_Lines)
	{
		if(!m_nearZRangeHackProjectionActive)
			SetProjectionStateNoCheck(true);
	}
	else
#elif DX11_UT_469 || DX11_UNREAL_227
	if (!(LineFlags & LINE_DepthCued))//(!(Viewport->Actor->ShowFlags & SHOW_OccludeLines))
	{
		if (!m_nearZRangeHackProjectionActive)
			SetProjectionStateNoCheck(true);
	}
	else
#endif
	{
		if(m_nearZRangeHackProjectionActive)
			SetProjectionStateNoCheck(false);
	}

	FGenShader->Bind(m_RenderContext);

	LockVertAndIndexBuffer(6);


	// Metallicafan212:	Selection testing
	if (m_HitData != nullptr)
	{
		Color = CurrentHitColor;

		// Metallicafan212:	If this isn't HP2, set the priority on lines/points higher
#if 0//!DX11_HP2
		if(PixelTopIndex != -1)
			PixelHitInfo(PixelTopIndex).Priority = 10;
#endif
	}

	//Get point coordinates back in 3D
	FLOAT X1Pos = m_RFX2 * (X1 - ScaledFX2);
	FLOAT Y1Pos = m_RFY2 * (Y1 - ScaledFY2);
	FLOAT X2Pos = m_RFX2 * (X2 - ScaledFX2);
	FLOAT Y2Pos = m_RFY2 * (Y2 - ScaledFY2);

	if (Frame->Viewport->IsOrtho())
	{
		//Z = 0.5f;//1.0f;
		// Metallicafan212:	If we don't have any other flags, render it with hacked Z
		//					The editor is bugged and provides world-space Z when drawing the pivot....
		if (LineFlags == LINE_None || LineFlags == LINE_DepthCued)
		{
			Z = 0.5f;
		}
		else
		{
			Z = abs(Z);
		}
		X1Pos *= Z;
		Y1Pos *= Z;
		X2Pos *= Z;
		Y2Pos *= Z;
	}

	// Metallicafan212:	Only do the point hack if we're drawing straight from Editor.dll
	bool bDoPointHack = ( (LineFlags == LINE_None || LineFlags == LINE_DepthCued) && Frame->Viewport->IsOrtho());
	bool bDo3DPointHack = (LineFlags == LINE_None || LineFlags == LINE_DepthCued) && !bDoPointHack;

	if (bDo3DPointHack)
		Z = 1.0f;

	// Metallicafan212:	Buffer the points
	m_VertexBuff[0].X		= X1Pos;
	m_VertexBuff[0].Y		= Y1Pos;
	m_VertexBuff[0].Z		= Z;
	m_VertexBuff[0].Color	= Color;

	m_VertexBuff[1].X		= X2Pos;
	m_VertexBuff[1].Y		= Y1Pos;
	m_VertexBuff[1].Z		= Z;
	m_VertexBuff[1].Color	= Color;

	m_VertexBuff[2].X		= X2Pos;
	m_VertexBuff[2].Y		= Y2Pos;
	m_VertexBuff[2].Z		= Z;
	m_VertexBuff[2].Color	= Color;

	m_VertexBuff[3].X		= X1Pos;
	m_VertexBuff[3].Y		= Y1Pos;
	m_VertexBuff[3].Z		= Z;
	m_VertexBuff[3].Color	= Color;

	m_VertexBuff[4].X		= X2Pos;
	m_VertexBuff[4].Y		= Y2Pos;
	m_VertexBuff[4].Z		= Z;
	m_VertexBuff[4].Color	= Color;

	m_VertexBuff[5].X		= X1Pos;
	m_VertexBuff[5].Y		= Y2Pos;
	m_VertexBuff[5].Z		= Z;
	m_VertexBuff[5].Color	= Color;

	AdvanceVertPos();

	unguard;
}
