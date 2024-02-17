#include "agk.h"
#include "DebugDraw.h"

using namespace AGK;

cSpriteMgrEx::cSpriteMgrEx()
{
	
}

cSpriteMgrEx::~cSpriteMgrEx()
{
	ClearAll();
}

void cSpriteMgrEx::ClearSprites()
{
	cSpriteContainer *pCont = m_pSprites;
	cSpriteContainer *pLast = 0;
	
	while ( pCont )
	{
		if ( pCont->GetType() == 1 )
		{
			cSpriteContainer *pNext = pCont->m_pNext;
			if ( pNext ) pNext->m_pPrev = pLast;

			if ( m_pLastSprite == pCont ) m_pLastSprite = pLast;
			
			if ( pLast ) pLast->m_pNext = pNext;
			else m_pSprites = pNext;

			delete pCont;
			pCont = pNext;
		}
		else
		{
			pLast = pCont;
			pCont = pCont->m_pNext;
		}
	}
	m_pLastSprite = pLast;

	if ( m_pDrawList ) delete [] m_pDrawList;
	if ( pVertexData ) delete [] pVertexData;
	if ( pIndices ) delete [] pIndices;

	m_pDrawList = 0;
	pVertexData = 0;
	pIndices = 0;
	
	iCurrentCount = 0;
	iCurrentCountAll = 0;
	iCurrentResizeCount = 0;
	iCurrentResizeAllCount = 0;

	if ( m_pInternalVertices ) agk::GetRenderer()->DeleteVertices( m_pInternalVertices );
	m_pInternalVertices = 0;

	if ( m_pInternalPipeline ) agk::GetRenderer()->DeletePipeline( m_pInternalPipeline );
	m_pInternalPipeline = 0;

	if ( m_pInternalConstants ) agk::GetRenderer()->DeleteShaderConstants( m_pInternalConstants );
	m_pInternalConstants = 0;
}

void cSpriteMgrEx::ClearAll()
{
	cSpriteContainer *pCont;
	
	while ( m_pSprites )
	{
		pCont = m_pSprites;
		m_pSprites = m_pSprites->m_pNext;
		delete pCont;
	}
	m_pSprites = 0;
	m_pLastSprite = 0;

	if ( m_pDrawList ) delete [] m_pDrawList;
	if ( pVertexData ) delete [] pVertexData;
	if ( pIndices ) delete [] pIndices;

	m_pDrawList = 0;
	pVertexData = 0;
	pIndices = 0;
	
	iCurrentCount = 0;
	iCurrentCountAll = 0;
	iCurrentResizeCount = 0;
	iCurrentResizeAllCount = 0;

	Renderer *pRenderer = agk::GetRenderer();
	if ( pRenderer )
	{
		if ( m_pInternalVertices ) pRenderer->DeleteVertices( m_pInternalVertices );
		if ( m_pInternalPipeline ) pRenderer->DeletePipeline( m_pInternalPipeline );
		if ( m_pInternalConstants ) pRenderer->DeleteShaderConstants( m_pInternalConstants );
	}
	m_pInternalVertices = 0;
	m_pInternalPipeline = 0;
	m_pInternalConstants = 0;
}

void cSpriteMgrEx::AddSprite( cSprite* sprite )
{
	if ( !sprite ) return;

	cSpriteContainer *pNewMember = new cSpriteContainer();
	pNewMember->SetSprite( sprite );
	sprite->m_bManaged = true;

	if ( !AddContainer( pNewMember ) ) delete pNewMember;
}

void cSpriteMgrEx::AddParticles( cParticleEmitter* particles )
{
	if ( !particles ) return;

	cSpriteContainer *pNewMember = new cSpriteContainer();
	pNewMember->SetParticles( particles );

	if ( !AddContainer( pNewMember ) ) delete pNewMember;
}

void cSpriteMgrEx::AddText( cText* text )
{
	if ( !text ) return;

	cSpriteContainer *pNewMember = new cSpriteContainer();
	pNewMember->SetText( text );

	if ( !AddContainer( pNewMember ) ) delete pNewMember;
}

void cSpriteMgrEx::AddEditBox( cEditBox* editbox )
{
	if ( !editbox ) return;

	cSpriteContainer *pNewMember = new cSpriteContainer();
	pNewMember->SetEditBox( editbox );

	if ( !AddContainer( pNewMember ) ) delete pNewMember;
}

void cSpriteMgrEx::AddSkeleton2D( Skeleton2D* skeleton )
{
	if ( !skeleton ) return;

	cSpriteContainer *pNewMember = new cSpriteContainer();
	pNewMember->SetSkeleton2D( skeleton );

	if ( !AddContainer( pNewMember ) ) delete pNewMember;
}

bool cSpriteMgrEx::AddContainer( cSpriteContainer* pNewMember )
{
	if ( !pNewMember ) return false;
	if ( pNewMember->GetType() == 0 ) return false;

	pNewMember->m_pNext = 0;
	
	if ( pNewMember->GetType() != 1 )
	{
		pNewMember->m_pPrev = 0;
		pNewMember->m_pNext = m_pSprites;
		if ( m_pSprites ) m_pSprites->m_pPrev = pNewMember;
		m_pSprites = pNewMember;
		if ( !m_pLastSprite ) m_pLastSprite = pNewMember;
	}
	else
	{
		// sort by created
		cSpriteContainer *pCont = m_pSprites;
		while ( pCont )
		{
			if ( pCont->GetCreated() < pNewMember->GetCreated() )
			{
				pNewMember->m_pPrev = pCont->m_pPrev;
				pNewMember->m_pNext = pCont;
				if ( !pCont->m_pPrev ) m_pSprites = pNewMember;
				else pCont->m_pPrev->m_pNext = pNewMember;
				pCont->m_pPrev = pNewMember;
				break;
			}

			pCont = pCont->m_pNext;
		}

		if ( !pCont )
		{
			if ( m_pLastSprite )
			{
				pNewMember->m_pPrev = m_pLastSprite;
				pNewMember->m_pNext = 0;
				m_pLastSprite->m_pNext = pNewMember;
				m_pLastSprite = pNewMember;
			}
			else
			{
				pNewMember->m_pPrev = 0;
				pNewMember->m_pNext = 0;
				m_pSprites = pNewMember;
				m_pLastSprite = pNewMember;
			}
		}
	}

	return true;
}

void cSpriteMgrEx::RemoveSprite( cSprite* sprite )
{
	if ( !sprite ) return;
	sprite->m_bManaged = false;
	if ( !m_pLastSprite ) return;

	// if it is the last in the list we can do a quick remove
	if ( m_pLastSprite->GetType() == 1 && m_pLastSprite->GetSprite() == sprite )
	{
		cSpriteContainer *pCont = m_pLastSprite;
		m_pLastSprite = pCont->m_pPrev;

		if ( !pCont->m_pPrev )
		{
			m_pSprites = 0;
		}
		else
		{
			pCont->m_pPrev->m_pNext = 0;
		}

		delete pCont;
		return;
	}

	cSpriteContainer *pMember = m_pSprites;
	cSpriteContainer *pLast = 0;
	while ( pMember )
	{
		if ( pMember->GetType() == 1 && pMember->GetSprite() == sprite )
		{
			//found, remove it
			cSpriteContainer *pNext = pMember->m_pNext;
			if ( pNext ) pNext->m_pPrev = pLast;

			if ( m_pLastSprite == pMember ) m_pLastSprite = pLast;
			
			if ( pLast ) pLast->m_pNext = pNext;
			else m_pSprites = pNext;

			delete pMember;
			pMember = pNext;
			continue;
		}

		pLast = pMember;
		pMember = pMember->m_pNext;
	}
}

void cSpriteMgrEx::RemoveParticles( cParticleEmitter* particles )
{
	if ( !particles ) return;
	if ( !m_pLastSprite ) return;

	// if it is the last in the list we can do a quick remove
	if ( m_pLastSprite->GetType() == 2 && m_pLastSprite->GetParticles() == particles )
	{
		cSpriteContainer *pCont = m_pLastSprite;
		m_pLastSprite = pCont->m_pPrev;

		if ( !pCont->m_pPrev )
		{
			m_pSprites = 0;
		}
		else
		{
			pCont->m_pPrev->m_pNext = 0;
		}

		delete pCont;
		return;
	}

	cSpriteContainer *pMember = m_pSprites;
	cSpriteContainer *pLast = 0;
	while ( pMember )
	{
		if ( pMember->GetType() == 2 && pMember->GetParticles() == particles )
		{
			//found, remove it
			cSpriteContainer *pNext = pMember->m_pNext;
			if ( pNext ) pNext->m_pPrev = pLast;

			if ( m_pLastSprite == pMember ) m_pLastSprite = pLast;
			
			if ( pLast ) pLast->m_pNext = pNext;
			else m_pSprites = pNext;

			delete pMember;
			pMember = pNext;
			continue;
		}

		pLast = pMember;
		pMember = pMember->m_pNext;
	}
}

void cSpriteMgrEx::RemoveText( cText* text )
{
	if ( !text ) return;
	if ( !m_pLastSprite ) return;

	// if it is the last in the list we can do a quick remove
	if ( m_pLastSprite->GetType() == 3 && m_pLastSprite->GetText() == text )
	{
		cSpriteContainer *pCont = m_pLastSprite;
		m_pLastSprite = pCont->m_pPrev;

		if ( !pCont->m_pPrev )
		{
			m_pSprites = 0;
		}
		else
		{
			pCont->m_pPrev->m_pNext = 0;
		}

		delete pCont;
		return;
	}

	cSpriteContainer *pMember = m_pSprites;
	cSpriteContainer *pLast = 0;
	while ( pMember )
	{
		if ( pMember->GetType() == 3 && pMember->GetText() == text )
		{
			//found, remove it
			cSpriteContainer *pNext = pMember->m_pNext;
			if ( pNext ) pNext->m_pPrev = pLast;

			if ( m_pLastSprite == pMember ) m_pLastSprite = pLast;
			
			if ( pLast ) pLast->m_pNext = pNext;
			else m_pSprites = pNext;

			delete pMember;
			pMember = pNext;
			continue;
		}

		pLast = pMember;
		pMember = pMember->m_pNext;
	}
}

void cSpriteMgrEx::RemoveEditBox( cEditBox* editbox )
{
	if ( !editbox ) return;
	if ( !m_pLastSprite ) return;

	// if it is the last in the list we can do a quick remove
	if ( m_pLastSprite->GetType() == 4 && m_pLastSprite->GetEditBox() == editbox )
	{
		cSpriteContainer *pCont = m_pLastSprite;
		m_pLastSprite = pCont->m_pPrev;

		if ( !pCont->m_pPrev )
		{
			m_pSprites = 0;
		}
		else
		{
			pCont->m_pPrev->m_pNext = 0;
		}

		delete pCont;
		return;
	}

	cSpriteContainer *pMember = m_pSprites;
	cSpriteContainer *pLast = 0;
	while ( pMember )
	{
		if ( pMember->GetType() == 4 && pMember->GetEditBox() == editbox )
		{
			//found, remove it
			cSpriteContainer *pNext = pMember->m_pNext;
			if ( pNext ) pNext->m_pPrev = pLast;

			if ( m_pLastSprite == pMember ) m_pLastSprite = pLast;
			
			if ( pLast ) pLast->m_pNext = pNext;
			else m_pSprites = pNext;

			delete pMember;
			pMember = pNext;
			continue;
		}

		pLast = pMember;
		pMember = pMember->m_pNext;
	}
}

void cSpriteMgrEx::RemoveSkeleton2D( Skeleton2D* skeleton )
{
	if ( !skeleton ) return;
	if ( !m_pLastSprite ) return;

	// if it is the last in the list we can do a quick remove
	if ( m_pLastSprite->GetType() == 5 && m_pLastSprite->GetSkeleton2D() == skeleton )
	{
		cSpriteContainer *pCont = m_pLastSprite;
		m_pLastSprite = pCont->m_pPrev;

		if ( !pCont->m_pPrev )
		{
			m_pSprites = 0;
		}
		else
		{
			pCont->m_pPrev->m_pNext = 0;
		}

		delete pCont;
		return;
	}

	cSpriteContainer *pMember = m_pSprites;
	cSpriteContainer *pLast = 0;
	while ( pMember )
	{
		if ( pMember->GetType() == 5 && pMember->GetSkeleton2D() == skeleton )
		{
			//found, remove it
			cSpriteContainer *pNext = pMember->m_pNext;
			if ( pNext ) pNext->m_pPrev = pLast;

			if ( m_pLastSprite == pMember ) m_pLastSprite = pLast;
			
			if ( pLast ) pLast->m_pNext = pNext;
			else m_pSprites = pNext;

			delete pMember;
			pMember = pNext;
			continue;
		}

		pLast = pMember;
		pMember = pMember->m_pNext;
	}
}

// Each sprite has an internal check to update or not, set using SetActive()
void cSpriteMgrEx::UpdateAll( float time )
{
	m_iLastTotal = 0;

	cSpriteContainer *pMember = m_pSprites;
	while ( pMember )
	{
		if ( pMember->GetType() == 1 ) 
		{
			m_iLastTotal++;
			pMember->GetSprite()->Update( time );
		}

		pMember = pMember->m_pNext;
	}
}

// Draw only sprites that match side = 0: all, side = 1: less or equal than depth, side = 2: greater than depth
void cSpriteMgrEx::DrawSplit( int depth, int side )
{
	if ( side != 1 )
	{
		m_iLastDrawn = 0;
		m_iLastDrawCalls = 0;
		m_iLastSorted = 0;
	}

	if ( !m_pSprites ) return;

	int iCount = 0;
	int iCountAll = 0;
	cSpriteContainer *pMember = m_pSprites;
	while ( pMember )
	{
		// 0 = all, 1 = front, 2 = back
		if ( side == 1 )
		{
			if ( pMember->GetDepthInt() > depth )
			{
				pMember = pMember->m_pNext;
				continue;
			}
		}
		else if ( side == 2 )
		{
			if ( pMember->GetDepthInt() <= depth )
			{
				pMember = pMember->m_pNext;
				continue;
			}
		}

		if ( pMember->GetType() == 1 )
		{
			if ( pMember->GetSprite()->GetVisible() && pMember->GetSprite()->GetInScreen() ) 
			{
				iCount++;
				iCountAll++;
			}
		}
		else iCountAll++;
		pMember = pMember->m_pNext;
	}

	if ( iCountAll == 0 ) return;

	if ( iCountAll*2 < iCurrentCountAll ) iCurrentResizeAllCount++;
	if ( iCount*2 < iCurrentCount ) iCurrentResizeCount++;

	// resize arrays if we have more sprites than before, or if we've had too many for a while
	if ( iCountAll > iCurrentCountAll || iCurrentResizeAllCount > 100 )
	{
		iCurrentResizeAllCount = 0;
		iCurrentCountAll = iCountAll + iCountAll/2;

		if ( m_pDrawList ) delete [] m_pDrawList;
		m_pDrawList = new AGKSortValue[ iCurrentCountAll ];
	}

	uint32_t iVertexFloats = cSprite::GetSpriteVertexFloats(); // position + color + uv

	if ( (iCount > iCurrentCount && iCurrentCount < 15000) || iCurrentResizeCount > 101 )
	{
		iCurrentResizeCount = 0;
		iCurrentCount = iCount + iCount/2;
		// always use unsigned short indices in case renderer does not support higher
		if ( iCurrentCount > 15000 ) iCurrentCount = 15000;

		if ( pVertexData ) delete [] pVertexData;
		if ( pIndices ) delete [] pIndices;

		if ( iCurrentCount == 0 ) iCurrentCount = 1;

		// assign arrays
		pVertexData = new float[ iCurrentCount*iVertexFloats*4 ]; // 4 vertices per sprite
		pIndices = new unsigned short[ iCurrentCount*6 ];
		
		for ( int i = 0; i < iCurrentCount; i++ )
		{
			pIndices[ i*6 + 0 ] = i*4 + 0;
			pIndices[ i*6 + 1 ] = i*4 + 1;
			pIndices[ i*6 + 2 ] = i*4 + 2;
			
			pIndices[ i*6 + 3 ] = i*4 + 2;
			pIndices[ i*6 + 4 ] = i*4 + 1;
			pIndices[ i*6 + 5 ] = i*4 + 3;
		}

		if ( !m_pInternalVertices ) agk::GetRenderer()->CreateVertices( iVertexFloats*sizeof(float), &m_pInternalVertices );
		agk::GetRenderer()->SetIndices( m_pInternalVertices, pIndices, iCurrentCount*6 );
	}

	iCount = 0;
	pMember = m_pSprites;
	while ( pMember )
	{
		// 0 = all, 1 = front, 2 = back
		if ( side == 1 )
		{
			if ( pMember->GetDepthInt() > depth )
			{
				pMember = pMember->m_pNext;
				continue;
			}
		}
		else if ( side == 2 )
		{
			if ( pMember->GetDepthInt() <= depth )
			{
				pMember = pMember->m_pNext;
				continue;
			}
		}

		if ( pMember->GetType() == 1 )
		{
			if ( pMember->GetSprite()->GetVisible() && pMember->GetSprite()->GetInScreen() ) 
			{
				m_pDrawList[ iCount ].ptr = (void*)pMember;
				m_pDrawList[ iCount ].iValue = agk::SortIntToUINT( pMember->GetDepthInt() );
				iCount++;
			}
		}
		else
		{
			m_pDrawList[ iCount ].ptr = (void*)pMember;
			m_pDrawList[ iCount ].iValue = agk::SortIntToUINT( pMember->GetDepthInt() );
			iCount++;
		}

		pMember = pMember->m_pNext;
	}
	
	if ( iCount == 0 ) return;

	int iTotalSprites = iCount;
	m_iLastSorted += iCount;

	agk::SortArray( m_pDrawList, iCount );
		
	iCount = 0;
		
	cImage* pCurrentTexture = 0;
	AGKShader *pCurrentShader = 0;

	AGKVertexLayout vertexLayout;
	vertexLayout.m_iVertexSize = (uint16_t) iVertexFloats * sizeof(float);
	vertexLayout.m_iPrimitiveType = AGK_PRIMITIVE_TRIANGLES;

	const AGKRenderState *pCurrRenderState = 0;
	uint32_t iLastHash = 0xFFFFFFFF;

	AGKDynamicArray<cImage*> shaderImages(1);
	
	Renderer *pRenderer = agk::GetRenderer();
	
	for ( int i = iTotalSprites-1; i >= 0; i-- )
	{
		pMember = (cSpriteContainer*) m_pDrawList[ i ].ptr;

		int iMemberType = pMember->GetType();
		if ( iMemberType != 1 )
		{
			if ( iCount > 0 ) 
			{
				m_iLastDrawn += iCount;
				m_iLastDrawCalls++;

				pRenderer->BindPipeline( 0, pCurrentShader, pCurrRenderState, &vertexLayout, &m_pInternalPipeline );
				pRenderer->BindShaderConstants( 0, pCurrentShader, 0, 0, &shaderImages, &m_pInternalConstants );
				pRenderer->SetVertices( m_pInternalVertices, pVertexData, iCount * 4 );
				pRenderer->DrawVertices( 0, m_pInternalVertices, iCount*6 );
				
				iCount = 0;
			}
		}

		switch( iMemberType )
		{
			case 5: // skeleton 2D
			{
				pMember->GetSkeleton2D()->Draw();
				//pMember->GetSkeleton2D()->DrawBones();
				break;
			}

			case 4: // editbox
			{
				pMember->GetEditBox()->Draw();
				break;
			}

			case 3: // text
			{
				pMember->GetText()->Draw();
				break;
			}
			
			case 2: // particles
			{
				pMember->GetParticles()->DrawAll();
				break;
			}

			case 1: // sprite
			{
				// multi-image sprites, sprites using scissors, and shaders with uniforms must be drawn separately
				if ( pMember->GetSprite()->HasAdditionalImages() || pMember->GetSprite()->GetScissorOn() || pMember->GetShader()->HasPerSpriteUniforms() )
				{
					if ( iCount > 0 ) 
					{
						if ( iCount > 0 ) 
						{
							m_iLastDrawn += iCount;
							m_iLastDrawCalls++;
							
							pRenderer->BindPipeline( 0, pCurrentShader, pCurrRenderState, &vertexLayout, &m_pInternalPipeline );
							pRenderer->BindShaderConstants( 0, pCurrentShader, 0, 0, &shaderImages, &m_pInternalConstants );
							pRenderer->SetVertices( m_pInternalVertices, pVertexData, iCount * 4 );
							pRenderer->DrawVertices( 0, m_pInternalVertices, iCount*6 );
						}
						iCount = 0;
					}
					pMember->GetSprite()->Draw();
					continue;
				}

				cImage* pNextTexture = pMember->GetTexture();
				AGKShader *pNextShader = pMember->GetShader();
				cSprite *pSprite = pMember->GetSprite();

				// don't try and draw more than about 15000 sprites in one draw call as indices can't exceed 65535 (4 vertices per sprite)
				if ( (pNextShader != pCurrentShader) 
				  || (pNextTexture != pCurrentTexture)
				  || iCount > 12000
				  || iLastHash != pSprite->GetRenderStateHash() )
				{
					if ( iCount > 0 ) 
					{
						m_iLastDrawn += iCount;
						m_iLastDrawCalls++;
							
						pRenderer->BindPipeline( 0, pCurrentShader, pCurrRenderState, &vertexLayout, &m_pInternalPipeline );
						pRenderer->BindShaderConstants( 0, pCurrentShader, 0, 0, &shaderImages, &m_pInternalConstants );
						pRenderer->SetVertices( m_pInternalVertices, pVertexData, iCount * 4 );
						pRenderer->DrawVertices( 0, m_pInternalVertices, iCount*6 );
					}
					iCount = 0;
					
					shaderImages.SetItem( 0, pNextTexture );
					pCurrentTexture = pNextTexture;
					pCurrentShader = pNextShader;
					cSprite::UpdateVertexLayout( &vertexLayout, pNextShader );
					vertexLayout.m_iPrimitiveType = AGK_PRIMITIVE_TRIANGLES;

					pCurrRenderState = pSprite->GetRenderState();
					iLastHash = pSprite->GetRenderStateHash();
				}
				
				pSprite->BatchDrawQuad( pVertexData + iCount*iVertexFloats*4 ); // 4 vertices per sprite
				iCount++;
				break;
			}

			default: break;
		}
	}

	if ( iCount > 0 ) 
	{
		m_iLastDrawn += iCount;
		m_iLastDrawCalls++;
		
		pRenderer->BindPipeline( 0, pCurrentShader, pCurrRenderState, &vertexLayout, &m_pInternalPipeline );
		pRenderer->BindShaderConstants( 0, pCurrentShader, 0, 0, &shaderImages, &m_pInternalConstants );
		pRenderer->SetVertices( m_pInternalVertices, pVertexData, iCount * 4 );
		pRenderer->DrawVertices( 0, m_pInternalVertices, iCount*6 );
	}
}

void cSpriteMgrEx::DrawDebug( )
{
	cSpriteContainer *pMember = m_pSprites;
	while ( pMember )
	{
		if ( pMember->GetType() == 1 ) 
		{
			// if shape set but not physics enabled
			if ( pMember->GetSprite()->GetVisible() && pMember->GetSprite()->m_phyShape && !pMember->GetSprite()->m_phyBody )
			{
				switch( pMember->GetSprite()->m_phyShape->m_type )
				{
					case b2Shape::e_circle: 
					{
						b2CircleShape *pShape = (b2CircleShape*)(pMember->GetSprite()->m_phyShape);
						b2Vec2 pos( agk::WorldToPhyX(pMember->GetSprite()->m_fX), agk::WorldToPhyY(pMember->GetSprite()->m_fY) );

						b2Transform transform;
						transform.Set( pos, pMember->GetSprite()->GetAngleRad() );

						b2Vec2 center = b2Mul( transform, pShape->m_p );

						g_DebugDraw.DrawCircle( center, pShape->m_radius, b2Color(0.4f,0.77f,1) );
						break;
					} 

					case b2Shape::e_polygon: 
					{
						b2PolygonShape *pShape = (b2PolygonShape*)(pMember->GetSprite()->m_phyShape);
						b2Vec2 pos( agk::WorldToPhyX(pMember->GetSprite()->m_fX), agk::WorldToPhyY(pMember->GetSprite()->m_fY) );

						b2Transform transform;
						transform.Set( pos, pMember->GetSprite()->GetAngleRad() );

						b2Vec2 newVertices[ b2_maxPolygonVertices ];
						for ( int i = 0; i < pShape->m_vertexCount; i++ ) 
						{
							newVertices[ i ] = b2Mul( transform, pShape->m_vertices[ i ] );
						}
						
						g_DebugDraw.DrawPolygon( newVertices, pShape->m_vertexCount, b2Color(0.4f,0.77f,1) );
						break;
					}
                    default: agk::Error( "Unsupported Box2D shape" );
				}
			}
		}
		pMember = pMember->m_pNext;
	}
}

