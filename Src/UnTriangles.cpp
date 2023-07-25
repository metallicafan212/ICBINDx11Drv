#include "ICBINDx11Drv.h"

FORCEINLINE void DoVert(FTransTexture* P, FD3DVert* m_Vert, FPLAG PolyFlags, UBOOL bDoFog, FLOAT UMult, FLOAT VMult, UBOOL bDoSelection, FPlane SelectionColor)
{
	m_Vert->X	= P->Point.X;
	m_Vert->Y	= P->Point.Y;
	m_Vert->Z	= P->Point.Z;
	m_Vert->U	= P->U * UMult;
	m_Vert->V	= P->V * VMult;

	/*
	// Metallicafan212:	Turn off Light A if bNoLightOpcacity
	if (bNoLightOpcacity)
	{
		P->Light.W = 1.0f;

		// Metallicafan212:	Force no loss of alpha if the flags aren't there
		if (!drawFog)
			P->Fog.W = 1.0f;
	}
	*/

	if (bDoSelection)
	{
		// Metallicafan212:	Selection testing!!!!
		m_Vert->Color	= SelectionColor;
		m_Vert->Fog		= FPlane(0.0f, 0.0f, 0.0f, 0.0f);
	}
	else
	{

		// Set selection stuff
		if (GIsEditor && (PolyFlags & PF_Selected))
		{
			m_Vert->Color	= FPlane(0.0f, 1.0f, 0.0f, 1.0f);
			m_Vert->Fog		= FPlane(0.0f, 0.0f, 0.0f, 0.0f);
		}
		else if (PolyFlags & PF_Modulated)
		{
			m_Vert->Color	= FPlane(1.0f, 1.0f, 1.0f, 1.0f);
			m_Vert->Fog		= FPlane(0.0f, 0.0f, 0.0f, 0.0f);
		}
		else if (bDoFog)
		{
			m_Vert->Color	= P->Light;
			m_Vert->Fog		= P->Fog;
		}
		else
		{
			m_Vert->Color	= P->Light;
			m_Vert->Fog		= FPlane(0.0f, 0.0f, 0.0f, 0.0f);
		}
	}
}

// Metallicafan212:	Functions relating to mesh/particle drawing
//					TODO! Add in a config option to turn off and on "corrected fog", where it can completely fog actor verts!
void UICBINDx11RenderDevice::DrawTriangles(FSceneNode* Frame, FTextureInfo& Info, FTransTexture** Pts, INT NumPts, _WORD* Indices, INT NumIndices, QWORD PolyFlags, FSpanBuffer* Span)
{
	guard(UICBINDx11RenderDevice::DrawTriangles);

	SetBlend(PolyFlags);

	// Metallicafan212:	Request normal raster state
	SetRasterState(DXRS_Normal);

	// Metallicafan212:	Set the texture
	SetTexture(0, &Info, PolyFlags);

	// Metallicafan212:	In HP2, I got tired of seeing the random numbers everywhere, so I made a definition for the flags, and then updated them engine-wide
#if DX11_HP2
	if ((GUglyHackFlags & HF_Weapon))
#elif DX11_UT_469
	if((GUglyHackFlags & HACKFLAGS_PostRender))
#else
	if((GUglyHackFlags & 0x1))
#endif
	{
		if(!m_nearZRangeHackProjectionActive)
			SetProjectionStateNoCheck(true);
	}
	else
	{
		if(m_nearZRangeHackProjectionActive)
			SetProjectionStateNoCheck(false);
	}

	// Metallicafan212:	TODO!
	FMeshShader->Bind(m_RenderContext);

	// Metallicafan212:	Start buffering now
	StartBuffering(BT_Triangles);

	//LockVertexBuffer(NumPts * sizeof(FD3DVert));
	LockVertAndIndexBuffer(NumPts, NumIndices);

	if (Indices != nullptr)
	{
#if !INT_INDEX_BUFF
		// Metallicafan212:	TODO! Do something to be able to draw indexed AND buffer at the same time
		EndBuffering();
#endif
		//LockIndexBuffer(NumIndices);
	}
	/*
	else if (bIndexedBuffered)
	{
		EndBuffering();
	}
	*/

	// Metallicafan212:	Added in distance fog
	//					All calculations have to be done ourselfs, but at least it's doable
	UBOOL drawFog = (((PolyFlags & (PF_RenderFog | PF_Translucent | PF_Modulated)) == PF_RenderFog));

	// Metallicafan212:	Allow for opacity to draw fog
	drawFog = drawFog || ((PolyFlags & PF_ForceFog) && (PolyFlags & PF_RenderFog));

	FD3DVert* Mshy = (FD3DVert*)m_VertexBuff;

#if INT_INDEX_BUFF
	for (INT i = 0; i < NumIndices; i++)
	{
		m_IndexBuff[i] = ((INT)Indices[i]) + m_BufferedVerts;
	}
#endif

	for (INT i = 0; i < NumPts; i++)
	{
		DoVert(Pts[i],  &Mshy[i], PolyFlags, drawFog, BoundTextures[0].UMult, BoundTextures[0].VMult, m_HitData != nullptr, CurrentHitColor);
	}

	//UnlockVertexBuffer();
	
	// Metallicafan212:	Now copy the indices
	m_RenderContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

#if !INT_INDEX_BUFF
	if (Indices != nullptr)
	{
		appMemcpy(m_IndexBuff, Indices, sizeof(_WORD) * NumIndices);

		//UnlockIndexBuffer();
	}
#else
	/*
	if (Indices != nullptr)
	{
		UnlockIndexBuffer();
	}
	*/
#endif

	//UnlockBuffers();

	AdvanceVertPos();//NumPts, sizeof(FD3DVert), (Indices != nullptr ? NumIndices : 0));

	unguard;
}

#if !DX11_HP2
void UICBINDx11RenderDevice::DrawGouraudPolygon(FSceneNode* Frame, FTextureInfo& Info, FTransTexture** Pts, int NumPts, DWORD PolyFlags, FSpanBuffer* Span)
{
	guard(UICBINDx11RenderDevice::DrawGouraudPolygon);

	// Metallicafan212:	Reject invalid number of points
	if (NumPts < 3)
		return;

	SetBlend(PolyFlags);

	// Metallicafan212:	Request normal raster state
	SetRasterState(DXRS_Normal);

	// Metallicafan212:	Set the texture
	SetTexture(0, const_cast<FTextureInfo*>(&Info), PolyFlags);

	// Metallicafan212:	HP2 is HF_Weapon, UT 469 is HACKFLAGS_PostRender
	if ((GUglyHackFlags & HACKFLAGS_PostRender))
	{
		if(!m_nearZRangeHackProjectionActive)
			SetProjectionStateNoCheck(true);
	}
	else
	{
		if(m_nearZRangeHackProjectionActive)
			SetProjectionStateNoCheck(false);
	}

	// Metallicafan212:	TODO!
	FMeshShader->Bind(m_RenderContext);

	// Metallicafan212:	Start buffering now
	StartBuffering(BT_Triangles);

	LockVertAndIndexBuffer(NumPts, (NumPts - 2) * 3);

	//LockVertexBuffer(NumPts * sizeof(FD3DVert));

	//EndBuffering();
	//LockIndexBuffer((NumPts - 2) * 3);

	// Metallicafan212:	Added in distance fog
	//					All calculations have to be done ourselfs, but at least it's doable
	UBOOL drawFog = (((PolyFlags & (PF_RenderFog | PF_Translucent | PF_Modulated)) == PF_RenderFog));

	// Metallicafan212:	Turn off Light A if we're missing the required flags for opacity
	UBOOL bNoOpacity = ((PolyFlags & (PF_Highlighted | PF_Translucent)) != (PF_Highlighted | PF_Translucent));

	FD3DVert* Mshy = (FD3DVert*)m_VertexBuff;

	INDEX vIndex = 0;

	INDEX baseVIndex = m_BufferedVerts;

#if DX11_HP2
	if (bNoOpacity)
#endif
	{
		Pts[0]->Light.W = 1.0f;
		Pts[1]->Light.W = 1.0f;
	}

	DoVert(Pts[0], &Mshy[0], PolyFlags, drawFog, BoundTextures[0].UMult, BoundTextures[0].VMult, m_HitData != nullptr, CurrentHitColor);
	DoVert(Pts[1], &Mshy[1], PolyFlags, drawFog, BoundTextures[0].UMult, BoundTextures[0].VMult, m_HitData != nullptr, CurrentHitColor);

	// Metallicafan212:	First two verts, then we fan out

	for (INT i = 2; i < NumPts; i++)
	{
#if DX11_HP2
		// Metallicafan212:	TODO! This should be done in the shader, not here!
		if (bNoOpacity)
#endif
			Pts[i]->Light.W = 1.0f;

		DoVert(Pts[i], &Mshy[i], PolyFlags, drawFog, BoundTextures[0].UMult, BoundTextures[0].VMult, m_HitData != nullptr, CurrentHitColor);

		// Metallicafan212:	Now the indices
		m_IndexBuff[vIndex++] = baseVIndex;
		m_IndexBuff[vIndex++] = (i - 1) + baseVIndex;
		m_IndexBuff[vIndex++] = (i)+baseVIndex;
	}

	//UnlockVertexBuffer();
	//UnlockIndexBuffer();

	//UnlockBuffers();

	// Metallicafan212:	Now copy the indices
	m_RenderContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	AdvanceVertPos();//NumPts, sizeof(FD3DVert), (NumPts - 2) * 3);

	unguard;
}
#endif

#if DX11_UT_469
void UICBINDx11RenderDevice::DrawGouraudTriangles(const FSceneNode* Frame, const FTextureInfo& Info, FTransTexture* const Pts, INT NumPts, DWORD PolyFlags, DWORD DataFlags, FSpanBuffer* Span)
{
	guard(UICBINDx11RenderDevice::DrawTriangles);

	SetBlend(PolyFlags);

	// Metallicafan212:	Request normal raster state
	SetRasterState(DXRS_Normal);

	// Metallicafan212:	Set the texture
	SetTexture(0, const_cast<FTextureInfo*>(&Info), PolyFlags);

	// Metallicafan212:	HP2 is HF_Weapon, UT 469 is HACKFLAGS_PostRender
	if ((GUglyHackFlags & HACKFLAGS_PostRender))
	{
		if(!m_nearZRangeHackProjectionActive)
			SetProjectionStateNoCheck(true);
	}
	else
	{
		if(m_nearZRangeHackProjectionActive)
			SetProjectionStateNoCheck(false);
	}

	// Metallicafan212:	TODO!
	FMeshShader->Bind(m_RenderContext);

	//LockVertexBuffer(NumPts * sizeof(FD3DVert));

	// Metallicafan212:	Start buffering now
	StartBuffering(BT_Triangles);

	LockVertAndIndexBuffer(NumPts);

	//if (bIndexedBuffered)
	//{
	//	EndBuffering();
	//}

	// Metallicafan212:	Added in distance fog
	//					All calculations have to be done ourselfs, but at least it's doable
	UBOOL drawFog = (((PolyFlags & (PF_RenderFog | PF_Translucent | PF_Modulated)) == PF_RenderFog));

	// Metallicafan212:	Turn off Light A if we're missing the required flags for opacity
	UBOOL bNoOpacity = ((PolyFlags & (PF_Highlighted | PF_Translucent)) != (PF_Highlighted | PF_Translucent));

	FD3DVert* Mshy = (FD3DVert*)m_VertexBuff;

	for (INT i = 0; i < NumPts; i++)
	{
		// Metallicafan212:	TODO! This should be done in the shader, not here!
		//if (bNoOpacity)
		Pts[i].Light.W = 1.0f;

		DoVert(&Pts[i], &Mshy[i], PolyFlags, drawFog, BoundTextures[0].UMult, BoundTextures[0].VMult, m_HitData != nullptr, CurrentHitColor);
	}

	//UnlockVertexBuffer();

	//UnlockBuffers();

	// Metallicafan212:	Now copy the indices
	m_RenderContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	AdvanceVertPos();//NumPts, sizeof(FD3DVert));

	unguard;
}
#endif