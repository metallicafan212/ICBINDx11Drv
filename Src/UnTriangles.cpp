#include "ICBINDx11Drv.h"

// Metallicafan212:	TODO! Move these into the shader?
static FPlane NoFog				= FPlane(0.f, 0.f, 0.f, 0.f);

FORCEINLINE void DoVert(FTransTexture* P, FD3DVert* m_Vert, PFLAG& PolyFlags, UBOOL& bDoFog, FLOAT& UMult, FLOAT& VMult, UBOOL& bDoSelection, FPlane& SelectionColor)
{
	// Metallicafan212:	Speed this up by just copying it
	appMemcpy(&m_Vert->X, &P->Point.X, sizeof(FLOAT) * 3);

	// Metallicafan212:	TODO! This might be quicker to copy too
	m_Vert->U	= P->U * UMult;
	m_Vert->V	= P->V * VMult;

	if (bDoSelection)
	{
		appMemcpy(&m_Vert->Color, &SelectionColor, sizeof(FLOAT) * 4);
	}
	else if (bDoFog)
	{
		appMemcpy(&m_Vert->Color, &P->Light, sizeof(FLOAT) * 8);
	}
	else
	{
		//m_Vert->Color	= P->Light;
		appMemcpy(&m_Vert->Color, &P->Light, sizeof(FLOAT) * 4);
		m_Vert->Fog		= NoFog;
	}
}

#if DX11_HP2
// Metallicafan212:	Functions relating to mesh/particle drawing
//					TODO! Add in a config option to turn off and on "corrected fog", where it can completely fog actor verts!
void UICBINDx11RenderDevice::DrawTriangles(FSceneNode* Frame, FTextureInfo& Info, FTransTexture** Pts, INT NumPts, _WORD* Indices, INT NumIndices, QWORD PolyFlags, FSpanBuffer* Span)
{
	guard(UICBINDx11RenderDevice::DrawTriangles);

	// Metallicafan212:	Reject invalid number of triangles
	if(NumPts < 0 || NumIndices < 0)
		return;

	// Metallicafan212:	Start buffering now
	StartBuffering(BT_Triangles);

	REMOVE_BAD_PFLAGS(PolyFlags);

	// Metallicafan212:	TODO! Check if we were rendering something else and then flush it?
	if (GIsEditor && (PolyFlags & PF_Selected) && m_HitData == nullptr)
	{
		GlobalPolyflagVars.SelectionColor	= ActorSelectionColor.Plane().Vect();

		// Metallicafan212:	TODO! Maybe mark a var that sets if we should be flushing the polyflag vars?
		if (CurrentShader != FMeshShader)
		{
			UpdatePolyflagsVars();
		}
	}

	SetBlend(PolyFlags);

	// Metallicafan212:	Request normal raster state
	SetRasterState(DXRS_Normal);

	// Metallicafan212:	Set the texture
	SetTexture(0, &Info, PolyFlags);

	// Metallicafan212:	In HP2, I got tired of seeing the random numbers everywhere, so I made a definition for the flags, and then updated them engine-wide
	if ((GUglyHackFlags & HF_PostRender))
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
	FMeshShader->bNoMeshOpacity = (m_HitData != nullptr ? 1 : 0);

	FMeshShader->Bind(m_RenderContext);

	//LockVertexBuffer(NumPts * sizeof(FD3DVert));
	LockVertAndIndexBuffer(NumPts, NumIndices);

#if !INT_INDEX_BUFF
	if (Indices != nullptr)
	{
		// Metallicafan212:	TODO! Do something to be able to draw indexed AND buffer at the same time
		EndBuffering();
	}
#endif

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

	UBOOL bDoSelection = (m_HitData != nullptr);

	for (INT i = 0; i < NumPts; i++)
	{
		DoVert(Pts[i],  &Mshy[i], PolyFlags, drawFog, BoundTextures[0].UMult, BoundTextures[0].VMult, bDoSelection, CurrentHitColor);
	}

#if !INT_INDEX_BUFF
	if (Indices != nullptr)
	{
		appMemcpy(m_IndexBuff, Indices, sizeof(_WORD) * NumIndices);
	}
#endif


	AdvanceVertPos();

	unguard;
}
#endif

#if !DX11_HP2
void UICBINDx11RenderDevice::DrawGouraudPolygon(FSceneNode* Frame, FTextureInfo& Info, FTransTexture** Pts, int NumPts, DWORD PolyFlags, FSpanBuffer* Span)
{
	guard(UICBINDx11RenderDevice::DrawGouraudPolygon);

	// Metallicafan212:	Reject invalid number of points
	if (NumPts < 3)
		return;

	// Metallicafan212:	Start buffering now
	StartBuffering(BT_Triangles);

	REMOVE_BAD_PFLAGS(PolyFlags);

	// Metallicafan212:	TODO! Check if we were rendering something else and then flush it?
	if (GIsEditor && (PolyFlags & PF_Selected) && m_HitData == nullptr)
	{
#if DX11_HP2
		GlobalPolyflagVars.SelectionColor	= ActorSelectionColor.Plane().Vect();
#else
		// Metallicafan212:	TODO! Slice warning
		GlobalPolyflagVars.SelectionColor	= ActorSelectionColor.Plane();
#endif

		// Metallicafan212:	TODO! Maybe mark a var that sets if we should be flushing the polyflag vars?
		if (CurrentShader != FMeshShader)
		{
			UpdatePolyflagsVars();
		}
	}

	SetBlend(PolyFlags);

	// Metallicafan212:	Request normal raster state
	SetRasterState(DXRS_Normal);

	// Metallicafan212:	Set the texture
	SetTexture(0, const_cast<FTextureInfo*>(&Info), PolyFlags);

	// Metallicafan212:	HP2 is HF_PostRender, UT 469 is HACKFLAGS_NoNearZ
#if DX11_UNREAL_227 || DX11_UT_469
	if ((GUglyHackFlags & HACKFLAGS_NoNearZ))
#elif DX11_HP2
	if ((GUglyHackFlags & HF_PostRender))
#else
	if (GUglyHackFlags & 0x01)
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
#if !DX11_UNREAL_227
	FMeshShader->bNoMeshOpacity = 1;
#endif

	FMeshShader->Bind(m_RenderContext);

	LockVertAndIndexBuffer(NumPts, (NumPts - 2) * 3);

	// Metallicafan212:	Added in distance fog
	//					All calculations have to be done ourselfs, but at least it's doable
	UBOOL drawFog = (((PolyFlags & (PF_RenderFog | PF_Translucent | PF_Modulated)) == PF_RenderFog));

	// Metallicafan212:	Turn off Light A if we're missing the required flags for opacity
	//UBOOL bNoOpacity = ((PolyFlags & (PF_Highlighted | PF_Translucent)) != (PF_Highlighted | PF_Translucent));

	FD3DVert* Mshy = (FD3DVert*)m_VertexBuff;

	INDEX vIndex = 0;

	INDEX baseVIndex = m_BufferedVerts;

	UBOOL bDoSelection = (m_HitData != nullptr);

	DoVert(Pts[0], &Mshy[0], PolyFlags, drawFog, BoundTextures[0].UMult, BoundTextures[0].VMult, bDoSelection, CurrentHitColor);
	DoVert(Pts[1], &Mshy[1], PolyFlags, drawFog, BoundTextures[0].UMult, BoundTextures[0].VMult, bDoSelection, CurrentHitColor);

	// Metallicafan212:	First two verts, then we fan out
	for (INT i = 2; i < NumPts; i++)
	{
		DoVert(Pts[i], &Mshy[i], PolyFlags, drawFog, BoundTextures[0].UMult, BoundTextures[0].VMult, bDoSelection, CurrentHitColor);

		// Metallicafan212:	Now the indices
		m_IndexBuff[vIndex++] = baseVIndex;
		m_IndexBuff[vIndex++] = (i - 1) + baseVIndex;
		m_IndexBuff[vIndex++] = (i) + baseVIndex;
	}

	AdvanceVertPos();

	unguard;
}
#endif

#if DX11_UT_469
void UICBINDx11RenderDevice::DrawGouraudTriangles(const FSceneNode* Frame, const FTextureInfo& Info, FTransTexture* const Pts, INT NumPts, DWORD PolyFlags, DWORD DataFlags, FSpanBuffer* Span)
{
	guard(UICBINDx11RenderDevice::DrawGouraudTriangles);

	// Metallicafan212:	Start buffering now
	StartBuffering(BT_Triangles);

	REMOVE_BAD_PFLAGS(PolyFlags);

	// Metallicafan212:	If we're translucent, we need to render twice...

	// Metallicafan212:	TODO! Check if we were rendering something else and then flush it?
	if (GIsEditor && (PolyFlags & PF_Selected) && m_HitData == nullptr)
	{
#if DX11_HP2
		GlobalPolyflagVars.SelectionColor	= ActorSelectionColor.Plane().Vect();
#else
		// Metallicafan212:	TODO! Slice warning
		GlobalPolyflagVars.SelectionColor	= ActorSelectionColor.Plane();
#endif

		// Metallicafan212:	TODO! Maybe mark a var that sets if we should be flushing the polyflag vars?
		if (CurrentShader != FMeshShader)
		{
			UpdatePolyflagsVars();
		}
	}

	// Metallicafan212:	We have to implement specific effects ourselves when using this
	//					Detect them here
	UBOOL bMirror	= Frame->Mirror < 0.0f;

	UBOOL bEnv		= PolyFlags & PF_Environment;

	UBOOL bTwoSided	= PolyFlags & PF_TwoSided;

	FLOAT UScale	= Info.UScale * Info.USize / 256.0f;
	FLOAT VScale	= Info.VScale * Info.VSize / 256.0f;

	FLOAT EnvScU	= UScale * 128.f;
	FLOAT EnvScV	= VScale * 128.f;

	SetBlend(PolyFlags);

	// Metallicafan212:	Request normal raster state
	SetRasterState(DXRS_Normal);

	// Metallicafan212:	Set the texture
	SetTexture(0, const_cast<FTextureInfo*>(&Info), PolyFlags);

	// Metallicafan212:	HP2 is HF_PostRender, UT 469 is HACKFLAGS_PostRender
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
	FMeshShader->bNoMeshOpacity = 1;

	FMeshShader->Bind(m_RenderContext);

	LockVertAndIndexBuffer(NumPts);

	// Metallicafan212:	Added in distance fog
	//					All calculations have to be done ourselfs, but at least it's doable
	UBOOL drawFog		= (((PolyFlags & (PF_RenderFog | PF_Translucent | PF_Modulated)) == PF_RenderFog));

	FD3DVert* Mshy		= (FD3DVert*)m_VertexBuff;

	UBOOL bDoSelection = (m_HitData != nullptr);

	// Metallicafan212:	Process a whole triangle at a time
	INT M = 0;
	for (INT i = 0; i < NumPts; i += 3)
	{
		// Metallicafan212:	Check if it's outcoded
		//if (/*!bHasDrawn &&*/ Pts[i].Flags & Pts[i + 1].Flags & Pts[i + 2].Flags)
		//{
		//	// Metallicafan212:	Skip this triangle, and decrement the amount of verts requested by the lock
		//	m_VLockCount -= 3;
		//	continue;
		//}

		//bHasDrawn = 1;

		if (bMirror)
		{
			// Metallicafan212:	Swap
			Exchange(Pts[i + 2], Pts[i]);
		}

		// Metallicafan212:	Shamelessly copied and modified from the XOpenGL driver
		//					TODO! Implement these as hardware effects in the shader
		if (bEnv)
		{
			FTransTexture* pTemp = &Pts[i];
			for (INT j = 0; j < 3; j++)
			{
				FVector T = Pts[i + j].Point.UnsafeNormal().MirrorByVector(Pts[i + j].Normal).TransformVectorBy(Frame->Uncoords);
				Pts[i + j].U = (T.X + 1.0f) * EnvScU;
				Pts[i + j].V = (T.Y + 1.0f) * EnvScV;
			}
		}

		// Metallicafan212:	Per the XOpenGL driver, check for two sided
		if (bTwoSided && FTriple(Pts[i].Point, Pts[i + 1].Point, Pts[i + 2].Point) <= 0.0f)
		{
			// Metallicafan212:	Swap, it's mirrored
			Exchange(Pts[i + 2], Pts[i]);
		}

		DoVert(&Pts[i],		&Mshy[M++], PolyFlags, drawFog, BoundTextures[0].UMult, BoundTextures[0].VMult, bDoSelection, CurrentHitColor);
		DoVert(&Pts[i + 1], &Mshy[M++], PolyFlags, drawFog, BoundTextures[0].UMult, BoundTextures[0].VMult, bDoSelection, CurrentHitColor);
		DoVert(&Pts[i + 2], &Mshy[M++], PolyFlags, drawFog, BoundTextures[0].UMult, BoundTextures[0].VMult, bDoSelection, CurrentHitColor);
	}

	AdvanceVertPos();

	unguard;
}
#endif

#if DX11_UNREAL_227
void UICBINDx11RenderDevice::DrawGouraudPolyList(FSceneNode* Frame, FTextureInfo& Info, FTransTexture* Pts, INT NumPts, DWORD PolyFlags, FSpanBuffer* Span)
{
	guard(UICBINDx11RenderDevice::DrawGouraudPolyList);

	// Metallicafan212:	Reject invalid points (sanity check)
	if(NumPts < 3)
		return;

	// Metallicafan212:	Start buffering now
	StartBuffering(BT_Triangles);

	REMOVE_BAD_PFLAGS(PolyFlags);

	// Metallicafan212:	If we're translucent, we need to render twice...

	// Metallicafan212:	TODO! Check if we were rendering something else and then flush it?
	if (GIsEditor && (PolyFlags & PF_Selected) && m_HitData == nullptr)
	{
		// Metallicafan212:	TODO! Slice warning
		GlobalPolyflagVars.SelectionColor = ActorSelectionColor.Plane();

		// Metallicafan212:	TODO! Maybe mark a var that sets if we should be flushing the polyflag vars?
		if (CurrentShader != FMeshShader)
		{
			UpdatePolyflagsVars();
		}
	}

	if (Info.Modifier)
	{
		FLOAT UM = Info.USize, VM = Info.VSize;
		for (INT i = 0; i < NumPts; ++i)
			Info.Modifier->TransformPointUV(Pts[i].U, Pts[i].V, UM, VM);
	}

	// Metallicafan212:	We have to implement specific effects ourselves when using this
	//					Detect them here
	FLOAT UScale = Info.UScale * Info.USize / 256.0f;
	FLOAT VScale = Info.VScale * Info.VSize / 256.0f;

	SetBlend(PolyFlags);

	// Metallicafan212:	Request normal raster state
	SetRasterState(DXRS_Normal);

	// Metallicafan212:	Set the texture
	SetTexture(0, const_cast<FTextureInfo*>(&Info), PolyFlags);

	if ((GUglyHackFlags & HACKFLAGS_NoNearZ))
	{
		if (!m_nearZRangeHackProjectionActive)
			SetProjectionStateNoCheck(true);
	}
	else
	{
		if (m_nearZRangeHackProjectionActive)
			SetProjectionStateNoCheck(false);
	}

	// Metallicafan212:	TODO!
#if !DX11_UNREAL_227
	FMeshShader->bNoMeshOpacity = 1;
#endif

	FMeshShader->Bind(m_RenderContext);

	LockVertAndIndexBuffer(NumPts);

	// Metallicafan212:	Added in distance fog
	//					All calculations have to be done ourselfs, but at least it's doable
	UBOOL drawFog = (((PolyFlags & (PF_RenderFog | PF_Translucent | PF_Modulated)) == PF_RenderFog));

	FD3DVert* Mshy = (FD3DVert*)m_VertexBuff;

	UBOOL bDoSelection = (m_HitData != nullptr);

	// Metallicafan212:	Process a whole triangle at a time
	INT M = 0;
	for (INT i = 0; i < NumPts; i += 3)
	{
		DoVert(&Pts[i], &Mshy[M++], PolyFlags, drawFog, BoundTextures[0].UMult, BoundTextures[0].VMult, bDoSelection, CurrentHitColor);
		DoVert(&Pts[i + 1], &Mshy[M++], PolyFlags, drawFog, BoundTextures[0].UMult, BoundTextures[0].VMult, bDoSelection, CurrentHitColor);
		DoVert(&Pts[i + 2], &Mshy[M++], PolyFlags, drawFog, BoundTextures[0].UMult, BoundTextures[0].VMult, bDoSelection, CurrentHitColor);
	}

	AdvanceVertPos();

	unguard;
}
#endif