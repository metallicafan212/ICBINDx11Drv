#include "ICBINDx11Drv.h"

// Metallicafan212:	Temp info
//					TODO! Store this somewhere else!!!!
struct FUVInfo
{
	FPlane PanScale[5];

	UBOOL bEnabledTex[5];

	FPlane LFScale;

	FVector XAxis;
	FVector YAxis;

	FLOAT UDot;
	FLOAT VDot;
};

#if EXTRA_VERT_INFO && !COMPLEX_SURF_MANUAL_UVs
// Metallicafan212:	TODO! Temp vertex to keep here to copy data over
FD3DSecondaryVert TempVert;
#endif

#define DO_UV_CHANNEL_NO_ADD(chan, uLoc, vLoc) \
/*Metallicafan212: Each one is checked if we need to even cal it */ \
if (UVInfo.bEnabledTex[chan]) \
{ \
	uLoc = (U - UVInfo.PanScale[chan].X) * UVInfo.PanScale[chan].Z; \
	vLoc = (V - UVInfo.PanScale[chan].Y) * UVInfo.PanScale[chan].W; \
} \

#define DO_UV_CHANNEL(chan, uLoc, vLoc, extAddx, extAddy) \
/*Metallicafan212: Each one is checked if we need to even cal it */ \
if (UVInfo.bEnabledTex[chan]) \
{ \
	uLoc = (U - UVInfo.PanScale[chan].X + (extAddx)) * UVInfo.PanScale[chan].Z; \
	vLoc = (V - UVInfo.PanScale[chan].Y + (extAddy)) * UVInfo.PanScale[chan].W; \
} \

#define CALC_UV(p) \
	/* Metallicafan212:	Calculate the UVs of this vertex*/ \
	U = (Poly->Pts[p]->Point | UVInfo.XAxis) - UVInfo.UDot; \
	V = (Poly->Pts[p]->Point | UVInfo.YAxis) - UVInfo.VDot; \
	/* Metallicafan212:	Diffuse */ \
	DO_UV_CHANNEL_NO_ADD(0, m_VertexBuff[Vert].U, m_VertexBuff[Vert].V); \
	/* Metallicafan212:	Lightmap */ \
	DO_UV_CHANNEL(1, m_VertexBuff[Vert].UX, m_VertexBuff[Vert].VX, UVInfo.LFScale.X, UVInfo.LFScale.Y); \
	/* Metallicafan212:	Macro */ \
	DO_UV_CHANNEL_NO_ADD(2, m_SecVert[Vert].MU, m_SecVert[Vert].MV); \
	/* Metallicafan212:	Fog */ \
	DO_UV_CHANNEL(3, m_SecVert[Vert].FU, m_SecVert[Vert].FV, UVInfo.LFScale.Z, UVInfo.LFScale.W); \
	/* Metallicafan212:	Detail */ \
	DO_UV_CHANNEL_NO_ADD(4, m_SecVert[Vert].DU, m_SecVert[Vert].DV);

#if EXTRA_VERT_INFO
FORCEINLINE void BufferAndIndex(FSurfaceFacet& Facet, FPlane Color, FD3DVert* m_VertexBuff, INDEX* m_IndexBuff, SIZE_T m_BufferedVerts, SIZE_T m_BufferedIndicies, FD3DSecondaryVert* m_SecVert, FUVInfo& UVInfo)
#else
FORCEINLINE void BufferAndIndex(FSurfaceFacet& Facet, FPlane Color, FD3DVert* m_VertexBuff, INDEX* m_IndexBuff, SIZE_T m_BufferedVerts, SIZE_T m_BufferedIndicies, FD3DSecondaryVert* m_SecVert)
#endif
{
	SIZE_T vIndex	= 0;
	SIZE_T Vert		= 0;

	FLOAT U, V;

	INDEX rVIndex	= m_BufferedVerts;

	for (FSavedPoly* Poly = Facet.Polys; Poly; Poly = Poly->Next)
	{
		INT NumPts = Poly->NumPts;

		if (NumPts < 3)
			continue;

		// Metallicafan212:	Save the current vertex index
		//_WORD baseVIndex = V + m_BufferedVerts;
		INDEX baseVIndex = rVIndex + Vert;

		// Metallicafan212:	Do the first 2 verts (since we only add on one more vert at a time)
		m_VertexBuff[Vert].X		= Poly->Pts[0]->Point.X;
		m_VertexBuff[Vert].Y		= Poly->Pts[0]->Point.Y;
		m_VertexBuff[Vert].Z		= Poly->Pts[0]->Point.Z;

		// Metallicafan212:	Pan/Scale info
#if EXTRA_VERT_INFO
#if !COMPLEX_SURF_MANUAL_UVs
		m_SecVert[Vert]			= TempVert;
#else
		// Metallicafan212:	Calculate the UVs of this vertex
		CALC_UV(0);

		/*
		if (bEnabledTex[0])
		{
			m_VertexBuff[Vert].U	= (U - PanScale[0].X) * PanScale[0].Z;
			m_VertexBuff[Vert].V	= (V - PanScale[0].Y) * PanScale[0].W;
		}

		// Metallicafan212:	Lightmap
		if (bEnabledTex[1])
		{
			m_VertexBuff[Vert].UX	= (U - PanScale[1].X + 0.5f * LFScale.X) * PanScale[1].Z;
			m_VertexBuff[Vert].VX	= (V - PanScale[1].Y + 0.5f * LFScale.Y) * PanScale[1].W;
		}

		// Metallicafan212:	Macro
		if (bEnabledTex[2])
		{
			m_SecVert[Vert].MU		= (U - PanScale[2].X) * PanScale[2].Z;
			m_SecVert[Vert].MV		= (V - PanScale[2].Y) * PanScale[2].W;
		}

		// Metallicafan212:	Fog
		if (bEnabledTex[3])
		{
			m_SecVert[Vert].FU		= (U - PanScale[3].X + 0.5f * LFScale.Z) * PanScale[3].Z;
			m_SecVert[Vert].FV		= (V - PanScale[3].Y + 0.5f * LFScale.W) * PanScale[3].W;
		}

		// Metallicafan212:	Detail
		if (bEnabledTex[4])
		{
			m_SecVert[Vert].DU		= (U - PanScale[4].X) * PanScale[4].Z;
			m_SecVert[Vert].DV		= (V - PanScale[4].Y) * PanScale[4].W;
		}
		*/
#endif
#endif

		m_VertexBuff[Vert++].Color = Color;

		m_VertexBuff[Vert].X		= Poly->Pts[1]->Point.X;
		m_VertexBuff[Vert].Y		= Poly->Pts[1]->Point.Y;
		m_VertexBuff[Vert].Z		= Poly->Pts[1]->Point.Z;

		// Metallicafan212:	Pan/Scale info
#if EXTRA_VERT_INFO
#if !COMPLEX_SURF_MANUAL_UVs
		m_SecVert[Vert] = TempVert;
#else
		// Metallicafan212:	Calculate the UVs of this vertex
		CALC_UV(1);
#endif
#endif

		m_VertexBuff[Vert++].Color = Color;


		for (INT i = 2; i < NumPts; i++)
		{
			m_VertexBuff[Vert].X		= Poly->Pts[i]->Point.X;
			m_VertexBuff[Vert].Y		= Poly->Pts[i]->Point.Y;
			m_VertexBuff[Vert].Z		= Poly->Pts[i]->Point.Z;

			// Metallicafan212:	Pan/Scale info
#if EXTRA_VERT_INFO
#if !COMPLEX_SURF_MANUAL_UVs
			m_SecVert[Vert]			= TempVert;
#else
			// Metallicafan212:	Calculate the UVs of this vertex
			CALC_UV(i);
#endif
#endif

			m_VertexBuff[Vert++].Color	= Color;

			// Metallicafan212:	Now assemble this triangle
			//					It always uses the base vertex, the previous one, and the current one
			m_IndexBuff[vIndex++]	= baseVIndex;
			m_IndexBuff[vIndex++]	= (i - 1) + baseVIndex;
			m_IndexBuff[vIndex++]	= i + baseVIndex;
		}
	}
}

// Metallicafan212:	Definitions relating to complex surface (BSP) drawing
#if DX11_HP2
void UICBINDx11RenderDevice::DrawComplexSurface(FSceneNode* Frame, FSurfaceInfo& Surface, FSurfaceFacet& Facet, QWORD PolyFlags, FLOAT Alpha)//BYTE cAlpha)
{
#else
void UICBINDx11RenderDevice::DrawComplexSurface(FSceneNode* Frame, FSurfaceInfo& Surface, FSurfaceFacet& Facet)
{
	PFLAG PolyFlags = Surface.PolyFlags;
#endif
	guard(UICBINDx11RenderDevice::DrawComplexSurface);

	// Metallicafan212:	This breaks mirrors, we HAVE to render invisible surfaces....
	//if (PolyFlags & PF_Invisible)
	//	return;

	if(m_nearZRangeHackProjectionActive)
		SetProjectionStateNoCheck(false);

	// Metallicafan212:	TODO! Do indexed buffering
	//					This means that we HAVE to pack the pan information into the verts somehow...
	//					Otherwise, the textures will start skewing due to the changed surface info.
	//					This does provide a performance boost (when drawing in textured view) but it obviously doesn't work as intended yet
	EndBuffering();

#if DX11_HP2
	// Metallicafan212:	Editor shit, so we can see lumos
	if (GIsEditor && (PolyFlags & PF_LumosAffected))
	{
		// Metallicafan212:	Strip it and render translucent
		PolyFlags &= ~PF_LumosAffected;
		PolyFlags |= PF_Occlude | PF_Translucent;
	}
#endif

#if DX11_HP2
	// Metallicafan212:	Copy over the surface alpha
	//FSurfShader->SurfAlpha = cAlpha / 255.0f;

	FLOAT AlphaMult = Alpha;//cAlpha / 255.0f;

	// Metallicafan212:	I've hacked lumos as DX11's reverse alpha mode doesn't go to 100% alpha, causing lumos to not occlude things behind it (when it's undescovered)
	if (PolyFlags & PF_LumosAffected)
	{
		AlphaMult = 1.0f - AlphaMult;

		// Metallicafan212:	If we have lumos, but the alpha is maxed, we need to turn on occlusion!!!

		if (AlphaMult >= 1.0f)
		{
			PolyFlags |= PF_Occlude;
			AlphaMult = 1.0f;
		}
	}
	else
	{
		AlphaMult = 1.0f;
	}
/*
#else
	// Metallicafan212:	TODO! Other games might use a surface alpha (like Rune iirc?). Reevaluate this later
	FSurfShader->SurfAlpha = 1.0f;
*/
#endif

	SetBlend(PolyFlags);

	// Metallicafan212:	Raster state
	SetRasterState(DXRS_Normal);

	//if (SceneNodeHack)
	//if(1)
	if(SceneNodeHack)
	{
		if ((Frame->X != m_sceneNodeX) || (Frame->Y != m_sceneNodeY))
		{
			//m_sceneNodeHackCount++;
			SetSceneNode(Frame);
		}
	}

	// Metallicafan212:	Set the textures
	SetTexture(0, Surface.Texture, PolyFlags);

	SetTexture(1, Surface.LightMap, 0);

	SetTexture(2, Surface.MacroTexture, 0);

	SetTexture(3, Surface.FogMap, 0);

	// Metallicafan212:	Also bind the detail texture lmao
	SetTexture(4, Surface.DetailTexture, 0);

#if !EXTRA_VERT_INFO
	// Metallicafan212:	TODO! Maybe embedd this instead of doing it in the shader?
	//					It would allow us to buffer surfaces
	SurfCoords = Facet.MapCoords;
#endif

	FSurfShader->Bind(m_RenderContext);

	checkSlow(Surface.Texture);

	// Metallicafan212:	Just use the lightmap for color?
	FPlane TestColor = FPlane(1.f, 1.f, 1.f, 1.f);

#if DX11_HP2
	TestColor.W *= AlphaMult;
#endif

	// Metallicafan212:	Selection testing!!!!
	if (m_HitData != nullptr)
		TestColor = CurrentHitColor;

	// Metallicafan212:	Turn it into a triangle list....
	//					Triangle fans aren't supported
	//					But!!! Since these will all have the SAME textures/flags, we can queue it up

	// Metallicafan212:	Assemble how many verts we need to use
	SIZE_T VertRequest = 0;

#if UNFAN
	for (FSavedPoly* Poly = Facet.Polys; Poly; Poly = Poly->Next)
	{
		VertRequest += (Poly->NumPts - 2) * 3;
	}

	LockVertexBuffer(sizeof(FD3DVert) * VertRequest);

	for (FSavedPoly* Poly = Facet.Polys; Poly; Poly = Poly->Next)
	{
		INT NumPts = (Poly->NumPts - 2) * 3;

		if (NumPts < 3)
			continue;

		SIZE_T j = 1;
		
		// Metallicafan212:	Set the base

		for (SIZE_T i = V; i < (NumPts + V); i)
		{
			// Metallicafan212:	Assemble each triangle
			//					Each triangle needs to be the base first, then the next two?
			m_VertexBuff[i].X		= Poly->Pts[0]->Point.X;
			m_VertexBuff[i].Y		= Poly->Pts[0]->Point.Y;
			m_VertexBuff[i].Z		= Poly->Pts[0]->Point.Z;
			m_VertexBuff[i++].Color	= TestColor;

			m_VertexBuff[i].X		= Poly->Pts[j]->Point.X;
			m_VertexBuff[i].Y		= Poly->Pts[j]->Point.Y;
			m_VertexBuff[i].Z		= Poly->Pts[j++]->Point.Z;
			m_VertexBuff[i++].Color	= TestColor;

			m_VertexBuff[i].X		= Poly->Pts[j]->Point.X;
			m_VertexBuff[i].Y		= Poly->Pts[j]->Point.Y;
			m_VertexBuff[i].Z		= Poly->Pts[j]->Point.Z;
			m_VertexBuff[i++].Color	= TestColor;
		}

		V += NumPts;	
	}

	UnlockVertexBuffer();
	m_RenderContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_RenderContext->Draw(VertRequest, m_DrawnVerts);
	AdvanceVertPos();//VertRequest);

#else
	// Metallicafan212:	Experiment, draw indexed
	SIZE_T IndexRequest = 0;
	for (FSavedPoly* Poly = Facet.Polys; Poly; Poly = Poly->Next)
	{
		// Metallicafan212:	Reject invalid number of verts lmao
		if (Poly->NumPts < 3)
			continue;

		VertRequest		+= Poly->NumPts;
		IndexRequest	+= (Poly->NumPts - 2) * 3;
	}

	// Metallicafan212:	Start buffering now
	StartBuffering(BT_BSP);

	LockVertAndIndexBuffer(VertRequest, IndexRequest);

	// Metallicafan212:	Secondary color buffer
#if EXTRA_VERT_INFO
	LockSecondaryVertBuffer();

	// Metallicafan212:	Calculate all the extra info
	FUVInfo UVInfo;
	/*FLOAT*/ UVInfo.UDot = Facet.MapCoords.XAxis | Facet.MapCoords.Origin;
	/*FLOAT*/ UVInfo.VDot = Facet.MapCoords.YAxis | Facet.MapCoords.Origin;
#if !COMPLEX_SURF_MANUAL_UVs
	TempVert.XAxis		= FPlane(Facet.MapCoords.XAxis, UVInfo.UDot);
	TempVert.YAxis		= FPlane(Facet.MapCoords.YAxis, UVInfo.VDot);
	//SDef->SurfAlpha			= SurfAlpha;
	//SDef->bDrawOpacity		= bSurfInvisible;


	// Metallicafan212:	Now the pan info
	for (INT i = 0; i < 5; i++)
	{
		// Metallicafan212:	Copy the pan and scale info
		if (BoundTextures[i].TexInfoHash != 0)
		{
			TempVert.PanScale[i] = FPlane(BoundTextures[i].UPan, BoundTextures[i].VPan, BoundTextures[i].UMult, BoundTextures[i].VMult);
		}
	}

	// Metallicafan212:	And lastly the original lightmap scale
	if (BoundTextures[1].TexInfoHash != 0)
	{
		TempVert.LFScale.X = BoundTextures[1].UScale * 0.5f;
		TempVert.LFScale.Y = BoundTextures[1].VScale * 0.5f;
	}

	// Metallicafan212:	And the fog scale
	if (BoundTextures[3].TexInfoHash != 0)
	{
		TempVert.LFScale.Z = BoundTextures[3].UScale * 0.5f;
		TempVert.LFScale.W = BoundTextures[3].VScale * 0.5f;
	}
#else
	UVInfo.XAxis = Facet.MapCoords.XAxis;
	UVInfo.YAxis = Facet.MapCoords.YAxis;

	// Metallicafan212:	Now the pan info
	for (INT i = 0; i < 5; i++)
	{
		// Metallicafan212:	Copy the pan and scale info
		if (BoundTextures[i].TexInfoHash != 0)
		{
			UVInfo.bEnabledTex[i] = 1;
			UVInfo.PanScale[i] = FPlane(BoundTextures[i].UPan, BoundTextures[i].VPan, BoundTextures[i].UMult, BoundTextures[i].VMult);
		}
		else
		{
			UVInfo.bEnabledTex[i] = 0;
		}
	}

	// Metallicafan212:	And lastly the original lightmap scale
	if (BoundTextures[1].TexInfoHash != 0)
	{
		UVInfo.LFScale.X = BoundTextures[1].UScale * 0.5f;
		UVInfo.LFScale.Y = BoundTextures[1].VScale * 0.5f;
	}

	// Metallicafan212:	And the fog scale
	if (BoundTextures[3].TexInfoHash != 0)
	{
		UVInfo.LFScale.Z = BoundTextures[3].UScale * 0.5f;
		UVInfo.LFScale.W = BoundTextures[3].VScale + 0.5f;
	}

#endif

#endif

#if EXTRA_VERT_INFO
	BufferAndIndex(Facet, TestColor, m_VertexBuff, m_IndexBuff, m_BufferedVerts, m_BufferedIndices, m_SecVertexBuff, UVInfo);
#else
	BufferAndIndex(Facet, TestColor, m_VertexBuff, m_IndexBuff, m_BufferedVerts, m_BufferedIndices, m_SecVertexBuff);
#endif


	m_RenderContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	AdvanceVertPos();

#endif

	// Metallicafan212:	TODO! Make selection a part of the shader???
	//					I tried it before, but it just looked wrong since it had to compete with the underlying texture color
	//					For now, just layer on top to preserve the same look as the other renderers
	if ((GIsEditor && (PolyFlags & PF_Selected)) || (PolyFlags & PF_FlatShaded))
	{
		// Metallicafan212:	We have to draw the previous indexed surface first!!!!
		EndBuffering();

		// Metallicafan212:	Start buffering now
		StartBuffering(BT_BSP);

		LockVertAndIndexBuffer(VertRequest, IndexRequest);

#if EXTRA_VERT_INFO
		LockSecondaryVertBuffer();
#endif

		//LockVertexBuffer(sizeof(FD3DVert) * VertRequest);
		//LockIndexBuffer(IndexRequest);

		//SetBlend(PF_Translucent);
		SetTexture(0, nullptr, 0);
		SetTexture(1, nullptr, 0);
		SetTexture(2, nullptr, 0);
		SetTexture(3, nullptr, 0);
		SetTexture(4, nullptr, 0);

		// Metallicafan212:	Rebind the shader....

#if	DX11_HP2
		SetBlend(PF_AlphaBlend);
#else
		SetBlend(PF_Highlighted | PF_Translucent);
#endif
		// Metallicafan212:	Get the color to layer on top
		if (m_HitData == nullptr)
		{
			if (PolyFlags & PF_FlatShaded)
			{
				if (PolyFlags & PF_Selected)
				{
					TestColor = (Surface.FlatColor.Plane() * 1.5f);
					TestColor.W = 1.0f;
				}
				else
				{
					TestColor = Surface.FlatColor.Plane();
					TestColor.W = 0.75f * 0.75f;//0.75f;
					//FSurfShader->SurfAlpha = 0.75f;
				}
			}
			else
			{
#if !DX11_UT_469
				TestColor = SurfaceSelectionColor.Plane();//FPlane(0.0f, 0.0f, 1.0f, 0.5f);
#else
				TestColor = FPlane(SurfaceSelectionColor.R / 255.0f, SurfaceSelectionColor.G / 255.0f, SurfaceSelectionColor.B / 255.0f, SurfaceSelectionColor.A / 255.0f);//FPlane(SurfaceSelectionColor.Plane(), 0.5f);
#endif
				//FSurfShader->SurfAlpha = 0.5f;
			}
		}

		FSurfShader->Bind(m_RenderContext);

		// Metallicafan212:	Rebuffer the verts (again)
#if EXTRA_VERT_INFO
		BufferAndIndex(Facet, TestColor, m_VertexBuff, m_IndexBuff, m_BufferedVerts, m_BufferedIndices, m_SecVertexBuff, UVInfo);
#else
		BufferAndIndex(Facet, TestColor, m_VertexBuff, m_IndexBuff, m_BufferedVerts, m_BufferedIndices, m_SecVertexBuff);
#endif

		//UnlockVertexBuffer();
		//UnlockIndexBuffer();

		//UnlockBuffers();

		AdvanceVertPos();//VertRequest, sizeof(FD3DVert), IndexRequest);

		// Metallicafan212:	Draw immediately
#if !EXTRA_VERT_INFO
		EndBuffering();
#endif
	}

	unguard;
}