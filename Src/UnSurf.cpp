#include "D3D11Drv.h"

inline void BufferAndIndex(FSurfaceFacet& Facet, FPlane Color, FD3DVert* m_VertexBuff, _WORD* m_IndexBuff, SIZE_T m_BufferedVerts, SIZE_T m_BufferedIndicies)
{
	_WORD vIndex	= 0;
	SIZE_T V		= 0;

	for (FSavedPoly* Poly = Facet.Polys; Poly; Poly = Poly->Next)
	{
		INT NumPts = Poly->NumPts;

		if (NumPts < 3)
			continue;

		// Metallicafan212:	Save the current vertex index
		_WORD baseVIndex = V + m_BufferedVerts;

		// Metallicafan212:	Do the first 2 verts (since we only add on one more vert at a time)
		m_VertexBuff[V].X		= Poly->Pts[0]->Point.X;
		m_VertexBuff[V].Y		= Poly->Pts[0]->Point.Y;
		m_VertexBuff[V].Z		= Poly->Pts[0]->Point.Z;
		m_VertexBuff[V++].Color = Color;

		m_VertexBuff[V].X		= Poly->Pts[1]->Point.X;
		m_VertexBuff[V].Y		= Poly->Pts[1]->Point.Y;
		m_VertexBuff[V].Z		= Poly->Pts[1]->Point.Z;
		m_VertexBuff[V++].Color = Color;


		for (INT i = 2; i < NumPts; i++)
		{
			m_VertexBuff[V].X		= Poly->Pts[i]->Point.X;
			m_VertexBuff[V].Y		= Poly->Pts[i]->Point.Y;
			m_VertexBuff[V].Z		= Poly->Pts[i]->Point.Z;
			m_VertexBuff[V++].Color	= Color;

			// Metallicafan212:	Now assemble this triangle
			//					It always uses the base vertex, the previous one, and the current one
			m_IndexBuff[vIndex++] = baseVIndex;
			m_IndexBuff[vIndex++] = (i - 1) + baseVIndex;
			m_IndexBuff[vIndex++] = (i)+baseVIndex;
		}
	}
}

// Metallicafan212:	Definitions relating to complex surface (BSP) drawing
void UD3D11RenderDevice::DrawComplexSurface(FSceneNode* Frame, FSurfaceInfo& Surface, FSurfaceFacet& Facet, QWORD PolyFlags, BYTE cAlpha)
{
	guard(UD3D11RenderDevice::DrawComplexSurface);

	// Metallicafan212:	This breaks mirrors, we HAVE to render invisible surfaces....
	//if (PolyFlags & PF_Invisible)
	//	return;

	SetProjectionStateNoCheck(false);

	// Metallicafan212:	TODO! Do indexed buffering
	EndBuffering();

#ifndef DX11_HP1
	// Metallicafan212:	Editor shit, so we can see lumos
	if (GIsEditor && (PolyFlags & PF_LumosAffected))
	{
		// Metallicafan212:	Strip it and render translucent
		PolyFlags &= ~PF_LumosAffected;
		PolyFlags |= PF_Occlude | PF_Translucent;
	}
#endif

	// Metallicafan212:	Copy over the surface alpha
	FSurfShader->SurfAlpha = cAlpha / 255.0f;

	// Metallicafan212:	I've hacked lumos as DX11's reverse alpha mode doesn't go to 100% alpha, causing lumos to not occlude things behind it (when it's undescovered)
	if (PolyFlags & PF_LumosAffected)
	{
		FSurfShader->SurfAlpha = 1.0f - FSurfShader->SurfAlpha;

		// Metallicafan212:	If we have lumos, but the alpha is maxed, we need to turn on occlusion!!!

		if (FSurfShader->SurfAlpha >= 1.0f)
		{
			PolyFlags |= PF_Occlude;
		}
	}


	SetBlend(PolyFlags);

	// Metallicafan212:	Raster state
	SetRasterState(DXRS_Normal);

	//if (SceneNodeHack)
	if(1)
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

	// Metallicafan212:	TODO! Maybe embedd this instead of doing it in the shader?
	//					It would allow us to buffer surfaces
	SurfCoords = Facet.MapCoords;

	FSurfShader->Bind();

	check(Surface.Texture);

	// Metallicafan212:	Just use the lightmap for color?
	FPlane TestColor = FPlane(1.f, 1.f, 1.f, 1.f);

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

		//Lock vertexColor and texCoord0 buffers
		//LockVertexColorBuffer();
		//LockTexCoordBuffer(0);

		//FGLTexCoord* pTexCoordArray = m_pTexCoordArray[0];
		//FGLVertexColor* pVertexColorArray = m_pVertexColorArray;

		SIZE_T j = 1;
		
		// Metallicafan212:	Set the base
		/*
		m_VertexBuff[V].X		= Poly->Pts[0]->Point.X;
		m_VertexBuff[V].Y		= Poly->Pts[0]->Point.Y;
		m_VertexBuff[V].Z		= Poly->Pts[0]->Point.Z;
		m_VertexBuff[V].Color	= TestColor;
		*/

		for (SIZE_T i = V; i < (NumPts + V); i)
		{
			/*
			pTexCoordArray[i].u = 0.5f;
			pTexCoordArray[i].v = 0.5f;

			pVertexColorArray[i].x = Poly->Pts[i]->Point.X;
			pVertexColorArray[i].y = Poly->Pts[i]->Point.Y;
			pVertexColorArray[i].z = Poly->Pts[i]->Point.Z;
			pVertexColorArray[i].color = polyColor;
			*/

			/*
			m_VertexBuff[i].X		= Poly->Pts[i]->Point.X;
			m_VertexBuff[i].Y		= Poly->Pts[i]->Point.Y;
			m_VertexBuff[i].Z		= Poly->Pts[i]->Point.Z;
			m_VertexBuff[i].Color	= TestColor;
			*/

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

		//UnlockVertexBuffer();

		//Unlock vertexColor and texCoord0 buffers
		//UnlockVertexColorBuffer();
		//UnlockTexCoordBuffer(0);

		//Draw the triangle fan
		//m_d3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, m_curVertexBufferPos, NumPts - 2);
		//DO_DRAW(m_d3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, m_curVertexBufferPos, NumPts - 2));

		//Advance vertex buffer position
		//m_curVertexBufferPos += NumPts;

		// Metallicafan212:	Now draw
		//m_D3DDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		//m_D3DDeviceContext->Draw(NumPts, m_DrawnVerts);
		//AdvanceVertPos(NumPts);		
	}

	UnlockVertexBuffer();
	m_D3DDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_D3DDeviceContext->Draw(VertRequest, m_DrawnVerts);
	AdvanceVertPos(VertRequest);

#else
	// Metallicafan212:	Experiment, draw indexed
	SIZE_T IndexRequest = 0;
	for (FSavedPoly* Poly = Facet.Polys; Poly; Poly = Poly->Next)
	{
		VertRequest		+= (Poly->NumPts);
		IndexRequest	+= (Poly->NumPts - 2) * 3;
	}

	LockVertexBuffer(sizeof(FD3DVert) * VertRequest);
	LockIndexBuffer(IndexRequest);

	// Metallicafan212:	Start buffering now
	StartBuffering(BT_BSP, 1);

	BufferAndIndex(Facet, TestColor, m_VertexBuff, m_IndexBuff, m_BufferedVerts, m_BufferedIndices);

	UnlockIndexBuffer();
	UnlockVertexBuffer();

	m_D3DDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	AdvanceVertPos(VertRequest, sizeof(FD3DVert), IndexRequest);

#endif

	// Metallicafan212:	TODO! Support selection
	// UnrealEd selection.
	if ((GIsEditor && (PolyFlags & PF_Selected)) || (PolyFlags & PF_FlatShaded))
	{
		// Metallicafan212:	We have to draw the previous indexed surface first!!!!
		EndBuffering();

		LockVertexBuffer(sizeof(FD3DVert)* VertRequest);
		LockIndexBuffer(IndexRequest);

		// Metallicafan212:	Start buffering now
		StartBuffering(BT_BSP, 1);

		//SetBlend(PF_Translucent);
		SetTexture(0, nullptr, 0);
		SetTexture(1, nullptr, 0);
		SetTexture(2, nullptr, 0);
		SetTexture(3, nullptr, 0);

		// Metallicafan212:	Rebind the shader....

#ifndef DX11_HP1
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
					FSurfShader->SurfAlpha = 1.0f;
				}
				else
				{
					TestColor = Surface.FlatColor.Plane();
					TestColor.W = 0.75f;
					FSurfShader->SurfAlpha = 0.75f;
				}
			}
			else
			{
				TestColor = FPlane(0.0f, 0.0f, 1.0f, 1.0f);
				FSurfShader->SurfAlpha = 0.5f;
			}
		}

		FSurfShader->Bind();

		// Metallicafan212:	Rebuffer the verts (again)
		BufferAndIndex(Facet, TestColor, m_VertexBuff, m_IndexBuff, m_BufferedVerts, m_BufferedIndices);

		UnlockVertexBuffer();
		UnlockIndexBuffer();

		AdvanceVertPos(VertRequest, sizeof(FD3DVert), IndexRequest);
	}

	unguard;
}