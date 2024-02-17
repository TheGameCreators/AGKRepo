// Common Include
//#include "cSprite.h"
//#include "Wrapper.h"
#include "agk.h"
#include "Box2D/Box2D.h"

// Namespace
using namespace AGK;

int cSprite::g_iPixelsDrawn = 0;
cSprite* cSprite::g_pAllSprites = 0;
cSprite* cSprite::g_pLastSprite = 0;
void* cSprite::m_pInternalVertices = 0;

uint32_t cSprite::g_iCreated = 0;

void cSprite::RemoveImage( cImage *pDelImage )
{
	cSprite *pSprite = g_pAllSprites;
	while ( pSprite )
	{
		if ( pSprite->m_pImage == pDelImage )
		{
			pSprite->SwitchImage( 0 );
		}
		
		pSprite = pSprite->m_pNextSprite;
	}
}

//
// Internal functions
//

void cSprite::Initialize ( void )
{
	m_iCreated = g_iCreated;
	g_iCreated++;

	m_pNextSprite = 0;

	if ( g_pLastSprite ) 
	{
		m_pPrevSprite = g_pLastSprite;
		g_pLastSprite->m_pNextSprite = this;
	}
	else
	{
		m_pPrevSprite = 0;
		g_pAllSprites = this;
	}

	g_pLastSprite = this;
	
	m_renderState.SetCullMode( AGK_CULL_NONE );
	m_renderState.SetDepthEnable( 0 );
	m_renderState.SetBlendEnable( 0 );
	
	m_pShader = AGKShader::g_pShaderColor;

	m_colResult = new b2DistanceOutput();

	m_fGravityScale = 1.0f;
	m_bAwake = true;
	m_bSleep = true;
}

cSprite::cSprite() : m_shaderImages(1)
{
	Initialize();
	SetSize( -1, -1 );
	SetPosition( 0,0 );
}

cSprite::cSprite( const uString &szImage ) : m_shaderImages(1)
{
	Initialize();
	SetImage(szImage);
	SetSize( -1, -1 );
	SetPosition( 0,0 );
}

cSprite::cSprite( cImage *pImage ) : m_shaderImages(1)
{
	Initialize();
	SetImage(pImage);
	SetSize( -1, -1 );
	SetPosition( 0,0 );
}

cSprite::cSprite( const cSprite *pOtherSprite ) : m_shaderImages(0) // 0 because it will inherit the other sprites pointers, so don't create any of its own
{
	m_renderState = pOtherSprite->m_renderState;
	m_vertexLayout.Clone( &pOtherSprite->m_vertexLayout );
	m_shaderImages.Clone( &pOtherSprite->m_shaderImages );

	if ( pOtherSprite->m_pShaderConstantPosition ) m_pShaderConstantPosition = pOtherSprite->m_pShaderConstantPosition->Clone();
	if ( pOtherSprite->m_pShaderConstantSize ) m_pShaderConstantSize = pOtherSprite->m_pShaderConstantSize->Clone();

	m_sName.SetStr( pOtherSprite->m_sName );
	
	m_iCreated = g_iCreated;
	g_iCreated++;

	m_iID = pOtherSprite->m_iID;
	m_fX = pOtherSprite->m_fX;
	m_fY = pOtherSprite->m_fY;
	m_fZ = pOtherSprite->m_fZ;
	m_iDepth = pOtherSprite->m_iDepth;
	m_fWidth = pOtherSprite->m_fWidth;
	m_fHeight = pOtherSprite->m_fHeight;
	m_fOffsetX = pOtherSprite->m_fOffsetX;
	m_fOffsetY = pOtherSprite->m_fOffsetY;
	m_fAngle = pOtherSprite->m_fAngle;
	m_fOrigWidth = pOtherSprite->m_fOrigWidth;
	m_fOrigHeight = pOtherSprite->m_fOrigHeight;
	m_fOrigRadius = pOtherSprite->m_fOrigRadius;
	m_iFlags = pOtherSprite->m_iFlags | AGK_SPRITE_VERTEX_COLOR_NEEDS_UPDATE;
	m_fVisualRadius = pOtherSprite->m_fVisualRadius;
	m_fColRadius = pOtherSprite->m_fColRadius;

	m_pBone = pOtherSprite->m_pBone;
	m_pSkeleton = pOtherSprite->m_pSkeleton;


	// check for non shared image and create another copy
	if ( pOtherSprite->m_pImage )
	{
		if ( (pOtherSprite->m_iFlags & AGK_SPRITE_SHAREDIMAGE) == 0 )
		{
			if ( pOtherSprite->m_pImage->GetPath() && strlen( pOtherSprite->m_pImage->GetPath() ) > 0 )
			{
				m_pImage = new cImage( pOtherSprite->m_pImage->GetPath() );
				static int warned = 0;
				if ( warned == 0 )
				{
					warned = 1;
					agk::Warning( "Warning, cloning a sprite created with LoadSprite will load the image multiple times, use LoadImage and CreateSprite instead for better performance" );
				}
			}
			else
			{
				// todo find another way to clone the image
				m_pImage = 0;
			}

			m_shaderImages.SetItem( 0, m_pImage );
		}
		else
		{
			m_pImage = pOtherSprite->m_pImage;
		}
	}
		

	// register with image
	if ( m_iFlags & AGK_SPRITE_MANAGE_IMAGES )
	{
		if ( m_pImage ) m_pImage->AddSprite( this );
		for ( int i = 1; i <= m_shaderImages.GetHighestIndex(); i++ ) 
		{
			cImage *pImage = m_shaderImages.GetItem( i );
			if ( pImage ) pImage->AddSprite( this );
		}
	}

	m_pFontImage = pOtherSprite->m_pFontImage;
	if ( m_pFontImage ) m_pFontImage->m_iRefCount++;

	m_iImageID = pOtherSprite->m_iImageID;
	m_fColorRed = pOtherSprite->m_fColorRed;
	m_fColorGreen = pOtherSprite->m_fColorGreen;
	m_fColorBlue = pOtherSprite->m_fColorBlue;
	m_fColorAlpha = pOtherSprite->m_fColorAlpha;

	m_pShader = pOtherSprite->m_pShader;

	m_fClipX = pOtherSprite->m_fClipX;
	m_fClipY = pOtherSprite->m_fClipY;
	m_fClipX2 = pOtherSprite->m_fClipX2;
	m_fClipY2 = pOtherSprite->m_fClipY2;

	m_fUVBorder = pOtherSprite->m_fUVBorder;
	m_fUOffset = pOtherSprite->m_fUOffset;
	m_fVOffset = pOtherSprite->m_fVOffset;
	m_fUScale = pOtherSprite->m_fUScale;
	m_fVScale = pOtherSprite->m_fVScale;
	
	m_bManaged = pOtherSprite->m_bManaged;

	m_bUVOverride = pOtherSprite->m_bUVOverride;
	m_fU1 = pOtherSprite->m_fU1;
	m_fV1 = pOtherSprite->m_fV1;
	m_fU2 = pOtherSprite->m_fU2;
	m_fV2 = pOtherSprite->m_fV2;
	m_fU3 = pOtherSprite->m_fU3;
	m_fV3 = pOtherSprite->m_fV3;
	m_fU4 = pOtherSprite->m_fU4;
	m_fV4 = pOtherSprite->m_fV4;
	
	m_iGroup = pOtherSprite->m_iGroup;
	m_iCategories = pOtherSprite->m_iCategories;
	m_iCategoryMask = pOtherSprite->m_iCategoryMask;
	m_fPolygonPointsTemp = 0;
	m_iPolygonPointsNum = 0;

	m_iFrameCount = pOtherSprite->m_iFrameCount;
	m_iFrameArraySize = pOtherSprite->m_iFrameArraySize;
	m_iFrameWidth = pOtherSprite->m_iFrameWidth;
	m_iFrameHeight = pOtherSprite->m_iFrameHeight;
	m_iCurrentFrame = pOtherSprite->m_iCurrentFrame;
	m_iFrameStart = pOtherSprite->m_iFrameStart;
	m_iFrameEnd = pOtherSprite->m_iFrameEnd;
	m_fFrameTimer = pOtherSprite->m_fFrameTimer;
	m_fFrameChangeTime = pOtherSprite->m_fFrameChangeTime;
	
	// copy animation frames
	if ( pOtherSprite->m_pFrames )
	{
		m_pFrames = new cSpriteFrame[ pOtherSprite->m_iFrameArraySize ];
		for ( int i = 0; i < pOtherSprite->m_iFrameCount; i++ )
		{
			m_pFrames[ i ] = pOtherSprite->m_pFrames[ i ];
			if ( pOtherSprite->m_pFrames[ i ].m_pFrameImage == pOtherSprite->m_pImage )
			{
				m_pFrames[ i ].m_pFrameImage = m_pImage;
			}
			else
			{
				if ( m_pFrames[ i ].m_pFrameImage && (m_iFlags & AGK_SPRITE_MANAGE_IMAGES) ) m_pFrames[ i ].m_pFrameImage->AddSprite( this );
			}
		}
	}

	if ( g_pLastSprite ) 
	{
		m_pPrevSprite = g_pLastSprite;
		g_pLastSprite->m_pNextSprite = this;
	}
	else
	{
		m_pPrevSprite = 0;
		g_pAllSprites = this;
	}
	g_pLastSprite = this;

	// set physics values
	// can't clone body!
	if ( pOtherSprite->m_phyShape ) m_phyShape = pOtherSprite->m_phyShape->CloneHeap();
	m_eShape = pOtherSprite->m_eShape;

	m_colResult = new b2DistanceOutput();

	if ( pOtherSprite->m_iNumAdditionalShapes == 0 || pOtherSprite->m_phyAdditionalShapes == 0 )
	{
		m_iNumAdditionalShapes = 0;
		m_phyAdditionalShapes = 0;
	}
	else
	{
		m_iNumAdditionalShapes = pOtherSprite->m_iNumAdditionalShapes;
		m_phyAdditionalShapes = new b2Shape*[ m_iNumAdditionalShapes ];
		for( int i = 0; i < (int)pOtherSprite->m_iNumAdditionalShapes; i++ )
		{
			m_phyAdditionalShapes[i] = pOtherSprite->m_phyAdditionalShapes[i]->CloneHeap();
		}
	}

	m_iUserBlendFunc = pOtherSprite->m_iUserBlendFunc;
	m_iTransparencyMode = pOtherSprite->m_iTransparencyMode;
}

cSprite::~cSprite()
{
	Renderer *pRenderer = agk::GetRenderer();
	//if ( m_pInternalVertices ) pRenderer->DeleteVertices( m_pInternalVertices );
	if ( m_pInternalPipeline ) pRenderer->DeletePipeline( m_pInternalPipeline );
	if ( m_pInternalShaderConstants ) pRenderer->DeleteShaderConstants( m_pInternalShaderConstants );
	if ( m_pVertexData ) delete m_pVertexData;
	if ( m_pShaderConstantPosition ) delete m_pShaderConstantPosition;
	if ( m_pShaderConstantSize ) delete m_pShaderConstantSize;

	if ( m_pSkeleton ) m_pSkeleton->RemoveExternalSprite( this );

	// remove from any tweens
	TweenInstance::DeleteTarget( this );

	// remove from global sprite list
	if ( m_pNextSprite )
	{
		m_pNextSprite->m_pPrevSprite = m_pPrevSprite;
	}
	else
	{
		g_pLastSprite = m_pPrevSprite;
	}

	if ( m_pPrevSprite )
	{
		m_pPrevSprite->m_pNextSprite = m_pNextSprite;
	}
	else
	{
		g_pAllSprites = m_pNextSprite;
	}

	// delete any non-shared image
	if ( (m_iFlags & AGK_SPRITE_SHAREDIMAGE) == 0 )
	{
		if ( m_pImage != 0 && !m_pImage->IsDeleting() ) delete m_pImage;
		m_pImage = 0;
	}
	else
	{
		if ( m_pImage && (m_iFlags & AGK_SPRITE_MANAGE_IMAGES) ) m_pImage->RemoveSprite( this );
	}

	if ( m_iFlags & AGK_SPRITE_MANAGE_IMAGES )
	{
		for ( int i = 1; i <= m_shaderImages.GetHighestIndex(); i++ )
		{
			cImage *pImage = m_shaderImages.GetItem( i );
			if ( pImage ) pImage->RemoveSprite( this );
		}
	}

	if ( m_pFontImage ) m_pFontImage->Release();
	m_pFontImage = 0;

	// delete any animation frames
	if ( m_pFrames != 0 ) 
	{
		if ( (m_iFlags & AGK_SPRITE_MANAGE_IMAGES) )
		{
			cImage *pLast = m_pImage;
			for ( int i = 0; i < m_iFrameCount; i++ )
			{
				if ( m_pFrames[ i ].m_pFrameImage != pLast && m_pFrames[ i ].m_pFrameImage ) 
				{
					m_pFrames[ i ].m_pFrameImage->RemoveSprite( this );
					pLast = m_pFrames[ i ].m_pFrameImage;
				}
			}
		}

		delete [] m_pFrames;
	}

	if ( m_phyBody ) 
	{
		PrepareToClearPhysicsContacts();
		agk::m_phyWorld->DestroyBody( m_phyBody );
	}
	if ( m_phyShape ) delete m_phyShape;
	if ( m_phyAdditionalShapes ) 
	{
		for(int i = 0; i < (int)m_iNumAdditionalShapes; i++ ) delete m_phyAdditionalShapes[i];
		delete [] m_phyAdditionalShapes;
	}
	if ( m_colResult ) delete m_colResult;

	if ( m_fPolygonPointsTemp ) delete [] m_fPolygonPointsTemp;
}

void cSprite::ImageDeleting( cImage *pImage )
{
	if ( m_pImage == pImage ) SwitchImage(0);

	for ( int i = 0; i < m_iFrameCount; i++ )
	{
		if ( m_pFrames[ i ].m_pFrameImage == pImage ) m_pFrames[ i ].m_pFrameImage = 0;
	}
}

void cSprite::RecalcVisualRadius()
{
	// recalculate visual radius
	float furthestX = 0;
	float furthestY = 0;
	if ( m_fOffsetX < m_fWidth/2 ) furthestX = m_fWidth;
	if ( m_fOffsetY < m_fHeight/2 ) furthestY = m_fHeight;
	
	float diffX = furthestX - m_fOffsetX;
	float diffY = furthestY - m_fOffsetY;
	
	m_fVisualRadius = agk::Sqrt( diffX*diffX + diffY*diffY );
	
	if ( agk::GetStretchValue() != 1 )
	{
		diffX /= agk::GetStretchValue();
		diffY *= agk::GetStretchValue();
		float stretchedRadius = agk::Sqrt( diffX*diffX + diffY*diffY );
		if ( stretchedRadius > m_fVisualRadius ) m_fVisualRadius = stretchedRadius;
	}
}

void cSprite::RecalcColRadius()
{
	// recalculate collision radius
	if ( !m_phyShape )
	{
		float furthestX = 0;
		float furthestY = 0;
		if ( m_fOffsetX < m_fWidth/2 ) furthestX = m_fWidth;
		if ( m_fOffsetY < m_fHeight/2 ) furthestY = m_fHeight;
		
		float diffX = agk::WorldToPhyX(furthestX - m_fOffsetX);
		float diffY = agk::WorldToPhyY(furthestY - m_fOffsetY);
		
		m_fColRadius = agk::Sqrt( diffX*diffX + diffY*diffY );
	}
	else
	{
		m_fColRadius = 0;

		for( int i = -1; i < (int)m_iNumAdditionalShapes; i++ )
		{
			b2Shape *pShape;
			if ( i == -1 ) pShape = m_phyShape;
			else pShape = m_phyAdditionalShapes[i];

			switch( pShape->GetType() )
			{
				case b2Shape::e_circle:
				{
					b2CircleShape *pCircleShape = (b2CircleShape*)pShape;
					float radius = agk::Sqrt( pCircleShape->m_p.x*pCircleShape->m_p.x + pCircleShape->m_p.y*pCircleShape->m_p.y ) + pCircleShape->m_radius;
					if ( radius > m_fColRadius ) m_fColRadius = radius;
					break;
				}
				case b2Shape::e_polygon:
				{
					b2PolygonShape *pPolyShape = (b2PolygonShape*)pShape;

					float maxDist = 0;
					for ( int i = 0; i < pPolyShape->m_vertexCount; i++ )
					{
						float x = pPolyShape->m_vertices[ i ].x;// - agk::WorldToPhyX(m_fOffsetX);
						float y = pPolyShape->m_vertices[ i ].y;// - agk::WorldToPhyY(m_fOffsetY);

						float dist = x*x + y*y;
						if ( dist > maxDist ) maxDist = dist;
					}
					
					float radius = agk::Sqrt( maxDist );
					if ( radius > m_fColRadius ) m_fColRadius = radius;
					break;
				}
				case b2Shape::e_chain:
				{
					b2ChainShape *pChainShape = (b2ChainShape*)pShape;

					float maxDist = 0;
					for ( int i = 0; i < pChainShape->GetVertexCount(); i++ )
					{
						float x = pChainShape->GetVertex( i ).x;// - agk::WorldToPhyX(m_fOffsetX);
						float y = pChainShape->GetVertex( i ).y;// - agk::WorldToPhyY(m_fOffsetY);

						float dist = x*x + y*y;
						if ( dist > maxDist ) maxDist = dist;
					}
					
					float radius = agk::Sqrt( maxDist );
					if ( radius > m_fColRadius ) m_fColRadius = radius;
					break;
				}
				case b2Shape::e_edge:
				{
					b2EdgeShape *pEdgeShape = (b2EdgeShape*)pShape;
					float dist = pEdgeShape->m_vertex1.x*pEdgeShape->m_vertex1.x + pEdgeShape->m_vertex1.y*pEdgeShape->m_vertex1.y;
					float distY = pEdgeShape->m_vertex2.x*pEdgeShape->m_vertex2.x + pEdgeShape->m_vertex2.y*pEdgeShape->m_vertex2.y;
					if ( distY > dist ) dist = distY;
					float radius = agk::Sqrt( dist );
					if ( radius > m_fColRadius ) m_fColRadius = radius;
					break;
				}
				default: agk::Error( "Unsupported Box2D shape" );
			}
		}
	}
}

bool cSprite::GetInScreen( void )
{ 
	float fX2 = m_fX;
	float fY2 = m_fY;
	float radius = m_fVisualRadius;

	if ( m_pBone )
	{
		fX2 = m_pBone->m00 * m_fX + m_pBone->m01 * m_fY + m_pBone->worldX;
		fY2 = m_pBone->m10 * m_fX + m_pBone->m11 * m_fY + m_pBone->worldY;

		float largestScale = m_pBone->worldSX;
		if ( m_pBone->worldSY > m_pBone->worldSX ) largestScale = m_pBone->worldSY;
		radius = m_fVisualRadius * largestScale;
	}
	
	if ( (m_iFlags & AGK_SPRITE_SCROLL) > 0 )
	{
		float x = agk::WorldToScreenX( fX2 + radius );
		if ( x < agk::GetScreenBoundsLeft() ) return false;
		
		x = agk::WorldToScreenX( fX2 - radius );
		if ( x > agk::GetScreenBoundsRight() ) return false;
		
		float y = agk::WorldToScreenY( fY2 + radius );
		if ( y < agk::GetScreenBoundsTop() ) return false;
		
		y = agk::WorldToScreenY( fY2 - radius );
		if ( y > agk::GetScreenBoundsBottom() ) return false;
	}
	else
	{
		float x = fX2 + radius;
		if ( x < agk::GetScreenBoundsLeft() ) return false;
		
		x = fX2 - radius;
		if ( x > agk::GetScreenBoundsRight() ) return false;
			
		float y = fY2 + radius;
		if ( y < agk::GetScreenBoundsTop() ) return false;
		
		y = fY2 - radius;
		if ( y > agk::GetScreenBoundsBottom() ) return false;
	}
	
	return true; 
}

bool cSprite::GetHitTest( float x, float y )
{
	// GetHitTest assumes the cordinates are in world space, so convert them to screen space if the sprite is fixed
	if ( (m_iFlags & AGK_SPRITE_SCROLL) == 0 )
	{
		x = agk::WorldToScreenX( x );
		y = agk::WorldToScreenY( y );
	}

	// scissor test
	if ( GetScissorOn() )
	{
		if ( x < m_fClipX || x > m_fClipX2 || y < m_fClipY || y > m_fClipY2 ) return false;
	}

	// quick reject
	float diffX = agk::WorldToPhyX(m_fX - x);
	float diffY = agk::WorldToPhyY(m_fY - y);
	if ( diffX*diffX + diffY*diffY > m_fColRadius*m_fColRadius ) return false;

	if ( m_phyShape )
	{
		b2Shape *pShape;
		b2Transform xf;
		xf.Set( b2Vec2(agk::WorldToPhyX(m_fX), agk::WorldToPhyY(m_fY)), m_fAngle );
		for( int i = -1; i < (int)m_iNumAdditionalShapes; i++ )
		{
			if ( i == -1 ) pShape = m_phyShape;
			else pShape = m_phyAdditionalShapes[i];

			if ( pShape->TestPoint( xf, b2Vec2(agk::WorldToPhyX(x), agk::WorldToPhyY(y)) ) ) return true;
		}

		return false;
	}
	else
	{
		float fNewX, fNewY;
		if ( m_fAngle != 0 )
		{
			float stretch = agk::m_fStretchValue;

			float fSinA = agk::SinRad(m_fAngle);
			float fCosA = agk::CosRad(m_fAngle);

			// transform point into sprite space
			float fTempX = x - m_fX;
			float fTempY = y - m_fY;

			fNewX = (fTempX*fCosA + fTempY*fSinA/stretch);
			fNewY = (fTempY*fCosA - fTempX*fSinA*stretch);
		}
		else
		{
			fNewX = x - m_fX;
			fNewY = y - m_fY;
		}

		fNewX += m_fOffsetX;
		fNewY += m_fOffsetY;

		return (fNewX >= 0) && (fNewY >= 0) && (fNewX <= m_fWidth) && (fNewY <= m_fHeight);
	}
}

bool cSprite::GetShouldCollide( cSprite *pSprite2 )
{
	assert(pSprite2);

	if ( m_iGroup != 0 && m_iGroup == pSprite2->m_iGroup ) return m_iGroup > 0;

	bool collide = (m_iCategories & pSprite2->m_iCategoryMask) != 0 && (pSprite2->m_iCategories & m_iCategoryMask) != 0;
	return collide;
}

float cSprite::GetXFromPixel( int x )
{
	if ( !m_pImage ) return 0;

	return x * GetWidth() / m_pImage->GetWidth();
}

float cSprite::GetYFromPixel( int y )
{
	if ( !m_pImage ) return 0;

	return y * GetHeight() / m_pImage->GetHeight();
}

int cSprite::GetPixelFromX( float x )
{
	if ( !m_pImage ) return 0;

	return agk::Round( x * m_pImage->GetWidth() / GetWidth() );
}

int cSprite::GetPixelFromY( float y )
{
	if ( !m_pImage ) return 0;

	return agk::Round( y * m_pImage->GetHeight() / GetHeight() );
}

float cSprite::GetWorldXFromPoint( float x, float y )
{
	// test
	float fSinA = agk::SinRad(m_fAngle);
	float fCosA = agk::CosRad(m_fAngle);
	
	float worldX = m_fX + (x*fCosA - y*fSinA);
	return worldX;
}

float cSprite::GetWorldYFromPoint( float x, float y )
{
	// test
	float fSinA = agk::SinRad(m_fAngle);
	float fCosA = agk::CosRad(m_fAngle);
	
	float worldY = m_fY + (y*fCosA + x*fSinA);
	return worldY;
}

float cSprite::GetXFromWorld( float x, float y )
{
	// test
	x -= m_fX;
	y -= m_fY;

	float fSinA = agk::SinRad(m_fAngle);
	float fCosA = agk::CosRad(m_fAngle);

	float spriteX = (x*fCosA + y*fSinA);
	return spriteX;
}

float cSprite::GetYFromWorld( float x, float y )
{
	// test
	x -= m_fX;
	y -= m_fY;

	float fSinA = agk::SinRad(m_fAngle);
	float fCosA = agk::CosRad(m_fAngle);

	float spriteY = (y*fCosA - x*fSinA);
	return spriteY;
}


void cSprite::SetScale( float x, float y )
{
	// maintain the top left corner position
	float oldX = GetX();
	float oldY = GetY();
	SetScaleByOffset( x, y );
	SetPosition( oldX, oldY );
}

void cSprite::SetScaleByOffset( float x, float y )
{
	// must not be 0, dividing by scale would cause problems.
	if ( x < 0.00001f ) x = 0.00001f;
	if ( y < 0.00001f ) y = 0.00001f;

	float oldWidth = m_fWidth;
	float oldHeight = m_fHeight;

	m_fWidth = m_fOrigWidth * x;
	m_fHeight = m_fOrigHeight * y;

	// shapes use a relative scale factor, this *may* make the shape slowly drift out of sync with 
	// the true scale size if lots of scaling is done.
	float diffX = m_fWidth / oldWidth;
	float diffY = m_fHeight / oldHeight;

	m_fOffsetX = m_fOffsetX * diffX;
	m_fOffsetY = m_fOffsetY * diffY;
	
	RecalcVisualRadius();

	// scale any sprite shape
	if ( !m_phyShape ) 
	{
		RecalcColRadius();
		return;
	}

	float max = x;
	if ( y > x ) max = y;

	float diffRadius = 1.0f;
	for( int i = -1; i < (int)m_iNumAdditionalShapes; i++ )
	{
		b2Shape *pShape;
		if( i == -1 ) pShape = m_phyShape;
		else pShape = m_phyAdditionalShapes[i];

		switch( pShape->GetType() )
		{
			case b2Shape::e_polygon:
			{
				int vertexCount = ((b2PolygonShape*)pShape)->m_vertexCount;
				b2Vec2 *vertices = ((b2PolygonShape*)pShape)->m_vertices;

				for ( int i = 0; i < vertexCount; i++ )
				{
					vertices[ i ].x *= diffX;
					vertices[ i ].y *= diffY;
				}
				break;
			}
			case b2Shape::e_chain:
			{
				int vertexCount = ((b2ChainShape*)pShape)->GetVertexCount();
				b2Vec2 *vertices = ((b2ChainShape*)pShape)->GetVerticesRW();

				for ( int i = 0; i < vertexCount; i++ )
				{
					vertices[ i ].x *= diffX;
					vertices[ i ].y *= diffY;
				}
				break;
			}
			case b2Shape::e_circle:
			{
				((b2CircleShape*)pShape)->m_p.x *= diffX;
				((b2CircleShape*)pShape)->m_p.y *= diffY;

				if ( i == -1 )
				{
					float oldRadius = pShape->m_radius;
					pShape->m_radius = m_fOrigRadius * max;
					diffRadius = pShape->m_radius / oldRadius;
				}
				else
				{
					pShape->m_radius *= diffRadius;
				}
				break;
			}
			default: break;
		}
	}

	if ( m_phyBody ) m_phyBody->SetAwake(true);

	RecalcColRadius();
}

//****************************
// Set functions
//****************************

void cSprite::FixToSkeleton( Skeleton2D *skeleton, Bone2D *bone, int zorder )
{
	if ( m_pSkeleton ) m_pSkeleton->RemoveExternalSprite( this );
	m_pSkeleton = skeleton;
	if ( m_pSkeleton ) m_pSkeleton->AddExternalSprite( this, zorder );
	m_pBone = bone;
}

void cSprite::SetColorF( float fRed, float fGreen, float fBlue, float fAlpha )
{
	m_fColorRed = fRed;
	m_fColorGreen = fGreen;
	m_fColorBlue = fBlue;
	m_fColorAlpha = fAlpha;

	m_iFlags |= AGK_SPRITE_VERTEX_COLOR_NEEDS_UPDATE;
	
	CheckTransparency();
}

void cSprite::SetRedF( float fRed )
{
	m_fColorRed = fRed;
	m_iFlags |= AGK_SPRITE_VERTEX_COLOR_NEEDS_UPDATE;
}

void cSprite::SetGreenF( float fGreen )
{
	m_fColorGreen = fGreen;
	m_iFlags |= AGK_SPRITE_VERTEX_COLOR_NEEDS_UPDATE;
}

void cSprite::SetBlueF( float fBlue )
{
	m_fColorBlue = fBlue;
	m_iFlags |= AGK_SPRITE_VERTEX_COLOR_NEEDS_UPDATE;
}

void cSprite::SetAlphaF( float fAlpha )
{
	m_fColorAlpha = fAlpha;
	m_iFlags |= AGK_SPRITE_VERTEX_COLOR_NEEDS_UPDATE;

	CheckTransparency();
}

void cSprite::SetColor ( uint32_t iRed, uint32_t iGreen, uint32_t iBlue, uint32_t iAlpha )
{
	m_fColorRed = iRed / 255.0f;
	m_fColorGreen = iGreen / 255.0f;
	m_fColorBlue = iBlue / 255.0f;
	m_fColorAlpha = iAlpha / 255.0f;

	m_iFlags |= AGK_SPRITE_VERTEX_COLOR_NEEDS_UPDATE;
	
	CheckTransparency();
}

void cSprite::SetRed ( uint32_t iRed )
{
	m_fColorRed = iRed / 255.0f;
	m_iFlags |= AGK_SPRITE_VERTEX_COLOR_NEEDS_UPDATE;
}

void cSprite::SetGreen ( uint32_t iGreen )
{
	m_fColorGreen = iGreen / 255.0f;
	m_iFlags |= AGK_SPRITE_VERTEX_COLOR_NEEDS_UPDATE;
}

void cSprite::SetBlue ( uint32_t iBlue )
{
	m_fColorBlue = iBlue / 255.0f;
	m_iFlags |= AGK_SPRITE_VERTEX_COLOR_NEEDS_UPDATE;
}

void cSprite::SetAlpha ( uint32_t iAlpha )
{
	m_fColorAlpha = iAlpha / 255.0f;
	m_iFlags |= AGK_SPRITE_VERTEX_COLOR_NEEDS_UPDATE;

	CheckTransparency();
}

void cSprite::SetFlip( int horz, int vert )
{
	int currHorz = (m_iFlags & AGK_SPRITE_FLIPH) > 0 ? 1 : 0;
	int currVert = (m_iFlags & AGK_SPRITE_FLIPV) > 0 ? 1 : 0;

	if ( horz != 0 ) horz = 1;
	if ( vert != 0 ) vert = 1;

	if ( (horz ^ currHorz) == 0 && (vert ^ currVert) == 0 ) return;

	if ( horz == 0 ) m_iFlags &= ~AGK_SPRITE_FLIPH;
	else m_iFlags |= AGK_SPRITE_FLIPH;

	if ( vert == 0 ) m_iFlags &= ~AGK_SPRITE_FLIPV;
	else m_iFlags |= AGK_SPRITE_FLIPV;

	// move offset point
	if ( (horz ^ currHorz) != 0 ) m_fOffsetX = m_fWidth - m_fOffsetX;
	if ( (vert ^ currVert) != 0 ) m_fOffsetY = m_fHeight - m_fOffsetY;

	if ( !m_phyShape ) return;

	for( int i = -1; i < (int)m_iNumAdditionalShapes; i++ )
	{
		b2Shape *pShape;
		if( i == -1 ) pShape = m_phyShape;
		else pShape = m_phyAdditionalShapes[i];

		switch( pShape->GetType() )
		{
			case b2Shape::e_polygon:
			{
				int vertexCount = ((b2PolygonShape*)pShape)->m_vertexCount;
				b2Vec2 *vertices = ((b2PolygonShape*)pShape)->m_vertices;
		
				if ( ((horz ^ currHorz) ^ (vert ^ currVert)) != 0 )
				{
					// change winding order
					float temp;
					for ( int i = 0; i < vertexCount/2; i++ )
					{
						temp = vertices[ i ].x;
						vertices[ i ].x = vertices[ (vertexCount-i)-1 ].x;
						vertices[ (vertexCount-i)-1 ].x = temp;

						temp = vertices[ i ].y;
						vertices[ i ].y = vertices[ (vertexCount-i)-1 ].y;
						vertices[ (vertexCount-i)-1 ].y = temp;
					}
				}

		
				for ( int i = 0; i < vertexCount; i++ )
				{
					if ( (horz ^ currHorz) != 0 ) vertices[ i ].x = -vertices[ i ].x;
					if ( (vert ^ currVert) != 0 ) vertices[ i ].y = -vertices[ i ].y;
				}

				((b2PolygonShape*)pShape)->Set( vertices, vertexCount ); 
				break;
			}

			case b2Shape::e_chain:
			{
				int vertexCount = ((b2ChainShape*)pShape)->GetVertexCount();
				b2Vec2 *vertices = ((b2ChainShape*)pShape)->GetVerticesRW();
		
				for ( int i = 0; i < vertexCount; i++ )
				{
					if ( (horz ^ currHorz) != 0 ) vertices[ i ].x = -vertices[ i ].x;
					if ( (vert ^ currVert) != 0 ) vertices[ i ].y = -vertices[ i ].y;
				}

				break;
			}

			case b2Shape::e_circle:
			{
				if ( (horz ^ currHorz) != 0 ) ((b2CircleShape*)pShape)->m_p.x = -((b2CircleShape*)pShape)->m_p.x;
				if ( (vert ^ currVert) != 0 ) ((b2CircleShape*)pShape)->m_p.y = -((b2CircleShape*)pShape)->m_p.y;
			}
		}
	}

	// scale all additional physics shapes
	if ( m_phyBody ) m_phyBody->SetAwake(true);
}

void cSprite::SetManageImages( int mode )
{
	if ( mode == 0 && (m_iFlags & AGK_SPRITE_MANAGE_IMAGES) )
	{
		if ( m_pFrames != 0 ) 
		{
			cImage *pLast = m_pImage;
			for ( int i = 0; i < m_iFrameCount; i++ )
			{
				if ( m_pFrames[ i ].m_pFrameImage != pLast && m_pFrames[ i ].m_pFrameImage ) 
				{
					if ( m_pFrames[ i ].m_pFrameImage ) m_pFrames[ i ].m_pFrameImage->RemoveSprite( this );
					pLast = m_pFrames[ i ].m_pFrameImage;
				}
			}
		}

		if ( m_pImage ) m_pImage->RemoveSprite( this );
	}

	if ( mode > 0 && !(m_iFlags & AGK_SPRITE_MANAGE_IMAGES) )
	{
		if ( m_pFrames != 0 ) 
		{
			cImage *pLast = m_pImage;
			for ( int i = 0; i < m_iFrameCount; i++ )
			{
				if ( m_pFrames[ i ].m_pFrameImage != pLast && m_pFrames[ i ].m_pFrameImage ) 
				{
					if ( m_pFrames[ i ].m_pFrameImage ) m_pFrames[ i ].m_pFrameImage->AddSprite( this );
					pLast = m_pFrames[ i ].m_pFrameImage;
				}
			}
		}

		if ( m_pImage ) m_pImage->AddSprite( this );
	}

	if ( mode > 0 ) m_iFlags = m_iFlags | AGK_SPRITE_MANAGE_IMAGES;
	else m_iFlags = m_iFlags & (~AGK_SPRITE_MANAGE_IMAGES);
}

void cSprite::SetPosition ( float fX, float fY )
{
	m_fX = fX + m_fOffsetX;
	m_fY = fY + m_fOffsetY;

	m_iFlags |= AGK_SPRITE_POSTOPLEFT;

	if ( m_phyBody )
	{
		//m_phyBody->SetTransform( b2Vec2(agk::WorldToPhyX(m_fX), agk::WorldToPhyY(m_fY)), m_fAngle );
		bool bIsActive = m_phyBody->IsActive();
		PrepareToClearPhysicsContacts();
		m_phyBody->SetActive( false );
		m_phyBody->m_xf.p.Set( agk::WorldToPhyX(m_fX), agk::WorldToPhyY(m_fY) );
		m_phyBody->m_sweep.c0 = m_phyBody->m_sweep.c = b2Mul(m_phyBody->m_xf, m_phyBody->m_sweep.localCenter);
		m_phyBody->m_linearVelocity.Set( 0, 0 );
		if ( bIsActive ) 
		{
			m_phyBody->SetActive( true );
			m_phyBody->SetAwake( true );
		}
	}
}

void cSprite::SetPositionByOffset ( float fX, float fY )
{
	m_fX = fX;
	m_fY = fY;

	m_iFlags &= ~AGK_SPRITE_POSTOPLEFT;

	if ( m_phyBody )
	{
		//m_phyBody->SetTransform( b2Vec2(agk::WorldToPhyX(m_fX), agk::WorldToPhyY(m_fY)), m_fAngle );
		bool bIsActive = m_phyBody->IsActive();
		PrepareToClearPhysicsContacts();
		m_phyBody->SetActive( false );
		m_phyBody->m_xf.p.Set( agk::WorldToPhyX(m_fX), agk::WorldToPhyY(m_fY) );
		m_phyBody->m_sweep.c0 = m_phyBody->m_sweep.c = b2Mul(m_phyBody->m_xf, m_phyBody->m_sweep.localCenter);
		m_phyBody->m_linearVelocity.Set( 0, 0 );
		if ( bIsActive ) 
		{
			m_phyBody->SetActive( true );
			m_phyBody->SetAwake( true );
		}
	}
}

void cSprite::SetX ( float fX )
{
	m_fX = fX + m_fOffsetX;

	m_iFlags |= AGK_SPRITE_POSTOPLEFT;

	if ( m_phyBody )
	{
		bool bIsActive = m_phyBody->IsActive();
		PrepareToClearPhysicsContacts();
		m_phyBody->SetActive( false );
		m_phyBody->m_xf.p.Set( agk::WorldToPhyX(m_fX), agk::WorldToPhyY(m_fY) );
		m_phyBody->m_sweep.c0 = m_phyBody->m_sweep.c = b2Mul(m_phyBody->m_xf, m_phyBody->m_sweep.localCenter);
		m_phyBody->m_linearVelocity.Set( 0, 0 );
		if ( bIsActive ) 
		{
			m_phyBody->SetActive( true );
			m_phyBody->SetAwake( true );
		}
	}
}

void cSprite::SetY ( float fY )
{
	m_fY = fY + m_fOffsetY;

	m_iFlags |= AGK_SPRITE_POSTOPLEFT;

	if ( m_phyBody )
	{
		bool bIsActive = m_phyBody->IsActive();
		PrepareToClearPhysicsContacts();
		m_phyBody->SetActive( false );
		m_phyBody->m_xf.p.Set( agk::WorldToPhyX(m_fX), agk::WorldToPhyY(m_fY) );
		m_phyBody->m_sweep.c0 = m_phyBody->m_sweep.c = b2Mul(m_phyBody->m_xf, m_phyBody->m_sweep.localCenter);
		m_phyBody->m_linearVelocity.Set( 0, 0 );
		if ( bIsActive ) 
		{
			m_phyBody->SetActive( true );
			m_phyBody->SetAwake( true );
		}
	}
}

void cSprite::SetZ ( float fZ )
{
	m_fZ = fZ;
	m_iDepth = agk::Round((m_fZ+0.000001f)*10000);
}

void cSprite::SetAngle( float fAngle )
{
	m_fAngle = fAngle * (PI / 180.0f);
	m_fAngle = agk::FMod( m_fAngle, 2*PI );
	if ( m_fAngle < 0 ) m_fAngle += 2*PI;

	if ( m_phyBody )
	{
		bool bIsActive = m_phyBody->IsActive();
		PrepareToClearPhysicsContacts();
		m_phyBody->SetActive( false );
		m_phyBody->m_xf.q.Set( m_fAngle );
		m_phyBody->m_sweep.a0 = m_phyBody->m_sweep.a = m_fAngle;
		m_phyBody->m_angularVelocity = 0;
		if ( bIsActive ) 
		{
			m_phyBody->SetActive( true );
			m_phyBody->SetAwake( true );
		}
	}
}

void cSprite::SetAngleRad( float fAngle )
{
	m_fAngle = fAngle;
	m_fAngle = agk::FMod( m_fAngle, 2*PI );
	if ( m_fAngle < 0 ) m_fAngle += 2*PI;

	if ( m_phyBody )
	{
		bool bIsActive = m_phyBody->IsActive();
		PrepareToClearPhysicsContacts();
		m_phyBody->SetActive( false );
		m_phyBody->m_xf.q.Set( m_fAngle );
		m_phyBody->m_sweep.a0 = m_phyBody->m_sweep.a = m_fAngle;
		m_phyBody->m_angularVelocity = 0;
		if ( bIsActive ) 
		{
			m_phyBody->SetActive( true );
			m_phyBody->SetAwake( true );
		}
	}
}

void cSprite::SetDepth ( int iDepth )
{
	float fNewZ = iDepth / 10000.0f;
	if ( fNewZ == m_fZ ) return;

	m_iDepth = iDepth;
	m_fZ = fNewZ;
}

void cSprite::SetSize ( float fWidth, float fHeight, bool bUpdateShape )
{
	m_iFlags &= ~AGK_SPRITE_WIDTHCALC;
	m_iFlags &= ~AGK_SPRITE_HEIGHTCALC;

	// default scenario, width and height are set to 10% of device, may cause stretching
	if ( fWidth < 0 && fHeight < 0 )
	{
		if ( !m_pImage )
		{
			fWidth = 10;
		}
		else
		{
			if ( m_iFrameCount > 0 ) fWidth = (float) m_iFrameWidth;
			else fWidth = (float) m_pImage->GetWidth();// * agk::GetVirtualWidth() / (float) agk::m_iRenderWidth;
		}
		
		fHeight = -1;
		m_iFlags |= AGK_SPRITE_WIDTHCALC;
		m_iFlags |= AGK_SPRITE_HEIGHTCALC;
	}

	// width defined, height not, set height to aspect ratio of image, no stretching
	if ( fHeight < 0 )
	{
		if ( fWidth < 0.00001f ) fWidth = 0.00001f;

		float imageAspect = 1;
		if ( m_pImage ) imageAspect = m_pImage->GetWidth() / (float) m_pImage->GetHeight();
		// image aspect is different for animated sprites
		if ( m_iFrameCount > 0 ) imageAspect = m_iFrameWidth / (float) m_iFrameHeight;
		fHeight = fWidth / imageAspect;

		// adjust for a non-pixel based aspect ratio
		float realAspect = agk::GetVirtualWidth() / (float) agk::GetVirtualHeight();
		fHeight = fHeight * (agk::GetDisplayAspect() / realAspect);

		m_iFlags |= AGK_SPRITE_HEIGHTCALC;
	}

	// height defined, width not, set width to aspect ratio of image, no stretching
	if ( fWidth < 0 )
	{
		if ( fHeight < 0.00001f ) fHeight = 0.00001f;

		float imageAspect = 1;
		if ( m_pImage ) imageAspect = m_pImage->GetWidth() / (float) m_pImage->GetHeight();
		// image aspect is different for animated sprites
		if ( m_iFrameCount > 0 ) imageAspect = m_iFrameWidth / (float) m_iFrameHeight;
		fWidth = fHeight * imageAspect;

		// adjust for a non-pixel based aspect ratio
		float realAspect = agk::GetVirtualWidth() / (float) agk::GetVirtualHeight();
		fWidth = fWidth / (agk::GetDisplayAspect() / realAspect);

		m_iFlags |= AGK_SPRITE_WIDTHCALC;
	}

	if ( fWidth < 0.00001f ) fWidth = 0.00001f;
	if ( fHeight < 0.00001f ) fHeight = 0.00001f;

	float diffX = fWidth / m_fWidth;
	float diffY = fHeight / m_fHeight;

	float oldX = 0;
	float oldY = 0;
	if ( m_iFlags & AGK_SPRITE_POSTOPLEFT )
	{
		oldX = GetX();
		oldY = GetY();
	}

	// set sprite width and height
	m_fWidth  = fWidth;
	m_fHeight = fHeight;
	m_fOrigWidth = fWidth;
	m_fOrigHeight = fHeight;

	if ( m_iFlags & AGK_SPRITE_MANUALOFFSET )
	{
		m_fOffsetX *= diffX;
		m_fOffsetY *= diffY;
	}
	else 
	{
		m_fOffsetX = m_fWidth / 2.0f;
		m_fOffsetY = m_fHeight / 2.0f;
	}

	if ( m_iFlags & AGK_SPRITE_POSTOPLEFT )
	{
		SetPosition( oldX, oldY );
	}
	
	RecalcVisualRadius();
	
	if ( bUpdateShape ) SetShape( m_eShape );
	if ( !m_phyShape ) RecalcColRadius();
}

void cSprite::SetImage( const uString &szImage, bool bUpdateCollisionShape )
{
	// delete any existing non-shared image
	if ( (m_iFlags & AGK_SPRITE_SHAREDIMAGE) == 0 )
	{
		if ( m_pImage != 0 && !m_pImage->IsDeleting() ) delete m_pImage;
		m_pImage = 0;
	}
	
	cImage* pOldImage = m_pImage;

	// set sprite to use image named, unshared image
	m_iImageID = 0;
	m_pImage = new cImage( szImage );
	m_iFlags &= ~AGK_SPRITE_SHAREDIMAGE;
	CheckAndRemoveImage( pOldImage );

	m_shaderImages.SetItem( 0, m_pImage );

	CheckTransparency();
	
	if ( m_pImage->HasParent() )
	{
		if ( m_fUVBorder < 0.5f ) m_fUVBorder = 0.5f;
	}
	else
	{
		m_fUVBorder = 0;
	}

	if ( !(m_iFlags & AGK_SPRITE_CUSTOM_SHADER) ) SetShader( 0 );
	
	if ( bUpdateCollisionShape && (m_eShape == eCircle || m_eShape == ePolygon) ) SetShape( m_eShape );
}

void cSprite::CheckAndRemoveImage( cImage* pImage )
{
	// check if image is being used by this sprite in any way, if not then tell image
	if ( (m_iFlags & AGK_SPRITE_MANAGE_IMAGES) == 0 ) return;
	if ( !pImage ) return;
	if ( pImage == m_pImage ) return;

	bool bFound = false;
	for ( int i = 0; i < m_iFrameCount; i++ )
	{
		if ( m_pFrames[ i ].m_pFrameImage == pImage )
		{
			bFound = true;
			break;
		}
	}

	if ( !bFound )
	{
		for ( int i = 1; i <= m_shaderImages.GetHighestIndex(); i++ )
		{
			if ( m_shaderImages.GetItem(i) == pImage )
			{
				bFound = true;
				break;
			}
		}
	}

	if ( !bFound ) pImage->RemoveSprite( this );
}

void cSprite::CheckTransparency()
{
	// if the user has not specified a transparency mode set it based on the image
	if ( (m_iFlags & AGK_SPRITE_FORCE_TRANSPARENCY) != 0 ) return;

	bool bFound = false;

	// check color first
	if ( m_fColorAlpha < 1.0f ) bFound = true;		
	
	if ( !bFound && m_pImage && m_pImage->IsTransparent() ) bFound = true;

	if ( !bFound )
	{
		for ( int i = 0; i < m_iFrameCount; i++ )
		{
			if ( m_pFrames[ i ].m_pFrameImage && m_pFrames[ i ].m_pFrameImage->IsTransparent() )
			{
				bFound = true;
				break;
			}
		}
	}

	if ( bFound ) 
	{
		if ( m_iTransparencyMode != 1 ) SetTransparencyInternal( 1 );
	}
	else 
	{
		if ( m_iTransparencyMode != 0 ) SetTransparencyInternal( 0 );
	}
}

void cSprite::SetImage( cImage* pImage, bool bUpdateCollisionShape )
{
	// setting an image with an existing set of frames can cause strange effects, either the user should clear them or we do.
	ClearAnimationFrames();

	// delete any existing non-shared image
	if ( (m_iFlags & AGK_SPRITE_SHAREDIMAGE) == 0 )
	{
		if ( m_pImage != 0 && !m_pImage->IsDeleting() ) delete m_pImage;
		m_pImage = 0;
	}

	cImage *pOldImage = m_pImage;

	if ( pImage && (m_iFlags & AGK_SPRITE_MANAGE_IMAGES) ) pImage->AddSprite( this );

	m_iImageID = 0;
	if ( pImage ) m_iImageID = pImage->GetID();
	m_pImage = pImage;
	m_iFlags |= AGK_SPRITE_SHAREDIMAGE;

	m_shaderImages.SetItem( 0, m_pImage );

	CheckAndRemoveImage( pOldImage );

	CheckTransparency();

	if ( m_pImage && (/*m_pImage->IsResized() ||*/ m_pImage->HasParent()) )
	{
		if ( m_fUVBorder < 0.5f ) m_fUVBorder = 0.5f;
	}
	else
	{
		m_fUVBorder = 0;
	}

	if ( !(m_iFlags & AGK_SPRITE_CUSTOM_SHADER) ) SetShader( 0 );

	if ( bUpdateCollisionShape && (m_eShape == eCircle || m_eShape == ePolygon) ) SetShape( m_eShape );
	
	// change sprite dimensions to image width and height
	//m_fWidth  = (float) m_pImage->GetWidth();
	//m_fHeight = (float) m_pImage->GetHeight();
}

void cSprite::SetAdditionalImage( cImage *pImage, int stage )
{
	if ( stage < 1 || stage > 7 ) return;

	cImage *pOldImage = m_shaderImages.GetItem( stage );
	if ( pImage && (m_iFlags & AGK_SPRITE_MANAGE_IMAGES) ) pImage->AddSprite( this );

	m_shaderImages.SetItem( stage, pImage );
	CheckAndRemoveImage( pOldImage );
}

void cSprite::SwitchImage( cImage* pImage, bool bUpdateCollisionShape )
{
	// delete any existing non-shared image
	if ( (m_iFlags & AGK_SPRITE_SHAREDIMAGE) == 0 )
	{
		if ( m_pImage != 0 && !m_pImage->IsDeleting() ) delete m_pImage;
		m_pImage = 0;
	}

	m_iImageID = 0;
	if ( pImage ) m_iImageID = pImage->GetID();
	m_pImage = pImage;
	m_iFlags |= AGK_SPRITE_SHAREDIMAGE;

	m_shaderImages.SetItem( 0, m_pImage );

	CheckTransparency();

	if ( m_pImage && (/*m_pImage->IsResized() ||*/ m_pImage->HasParent()) )
	{
		if ( m_fUVBorder < 0.5f ) m_fUVBorder = 0.5f;
	}
	else
	{
		m_fUVBorder = 0;
	}

	if ( !(m_iFlags & AGK_SPRITE_CUSTOM_SHADER) ) SetShader( 0 );

	if ( bUpdateCollisionShape && (m_eShape == eCircle || m_eShape == ePolygon) ) SetShape( m_eShape );
}

void cSprite::SetShader( AGKShader* shader )
{
	if ( m_pShader == shader ) return;
	m_pShader = shader;
	m_iFlags |= AGK_SPRITE_VERTEX_LAYOUT_NEEDS_UPDATE | AGK_SPRITE_CUSTOM_SHADER;

	if ( !m_pShader )
	{
		m_iFlags &= ~AGK_SPRITE_CUSTOM_SHADER;
		if ( m_pImage ) m_pShader = AGKShader::g_pShaderTexColor;
		else m_pShader = AGKShader::g_pShaderColor;
	}
		
	if ( !m_pShader->HasPerSpriteUniforms() )
	{
		if ( m_pShaderConstantPosition ) delete m_pShaderConstantPosition;
		if ( m_pShaderConstantSize ) delete m_pShaderConstantSize;
		m_pShaderConstantPosition = 0;
		m_pShaderConstantSize = 0;
	}
	else
	{
		if ( !m_pShaderConstantPosition ) 
		{
			m_pShaderConstantPosition = new AGKShaderConstantValue();
			m_pShaderConstantPosition->SetKnownName( AGK_SHADER_CONSTANT_NAME_SPRITE_POS );
		}
		if ( !m_pShaderConstantSize ) 
		{
			m_pShaderConstantSize = new AGKShaderConstantValue();
			m_pShaderConstantSize->SetKnownName( AGK_SHADER_CONSTANT_NAME_SPRITE_SIZE );
		}
	}
}

void cSprite::SetUserInt( int index, int value )
{
	if ( index > 127 )
	{
		agk::Error( "Cannot store more than 128 integers in a sprite" );
		return;
	}

	if ( index >= m_iNumUserInts ) 
	{
		int newSize = m_iNumUserInts + m_iNumUserInts/2;
		if ( newSize < 4 ) newSize = 4;
		if ( newSize > 128 ) newSize = 128;
		int *newData = new int[ newSize ];
		for ( int i = 0; i < m_iNumUserInts; i++ ) newData[ i ] = m_pUserInts[ i ];
		for ( int i = m_iNumUserInts; i < newSize; i++ ) newData[ i ] = 0;
		if ( m_pUserInts ) delete [] m_pUserInts;
		m_pUserInts = newData;
		m_iNumUserInts = newSize;
	}

	m_pUserInts[ index ] = value;
}

int cSprite::GetUserInt( int index )
{
	if ( index < 0 || index >= m_iNumUserInts ) return 0;

	return m_pUserInts[ index ];
}

void cSprite::SetUserFloat( int index, float value )
{
	if ( index > 127 )
	{
		agk::Error( "Cannot store more than 128 floats in a sprite" );
		return;
	}

	if ( index >= m_iNumUserFloats ) 
	{
		int newSize = m_iNumUserFloats + m_iNumUserFloats/2;
		if ( newSize < 4 ) newSize = 4;
		if ( newSize > 128 ) newSize = 128;
		float *newData = new float[ newSize ];
		for ( int i = 0; i < m_iNumUserFloats; i++ ) newData[ i ] = m_pUserFloats[ i ];
		for ( int i = m_iNumUserFloats; i < newSize; i++ ) newData[ i ] = 0;
		if ( m_pUserFloats ) delete [] m_pUserFloats;
		m_pUserFloats = newData;
		m_iNumUserFloats = newSize;
	}

	m_pUserFloats[ index ] = value;
}

float cSprite::GetUserFloat( int index )
{
	if ( index < 0 || index >= m_iNumUserFloats ) return 0;

	return m_pUserFloats[ index ];
}

void cSprite::SetUserString( int index, const uString& value )
{
	if ( index > 127 )
	{
		agk::Error( "Cannot store more than 128 strings in a sprite" );
		return;
	}

	if ( index >= m_iNumUserStrings ) 
	{
		int newSize = m_iNumUserStrings + m_iNumUserStrings/2;
		if ( newSize < 4 ) newSize = 4;
		if ( newSize > 128 ) newSize = 128;
		uString *newData = new uString[ newSize ];
		for ( int i = 0; i < m_iNumUserStrings; i++ ) newData[ i ].SetStr( m_pUserStrings[ i ] );
		if ( m_pUserStrings ) delete [] m_pUserStrings;
		m_pUserStrings = newData;
		m_iNumUserStrings = newSize;
	}

	m_pUserStrings[ index ] = value;
}

const uString& cSprite::GetUserString( int index )
{
	static const uString g_EmptyString;
	if ( index < 0 || index >= m_iNumUserStrings ) return g_EmptyString;

	return m_pUserStrings[ index ];
}

void cSprite::SetFontImage( AGKFontImage *pFontImage, float scale )
{
	if ( m_pFontImage == pFontImage ) return;

	if ( m_pFontImage ) m_pFontImage->Release();
	m_pFontImage = pFontImage;
	if ( m_pFontImage ) 
	{
		m_pFontImage->AddRef();

		SetImage( m_pFontImage->m_pImage );

		if ( m_pFontImage->m_pImage )
		{
			float VwDw = agk::DeviceToDisplayRatioX();
			float VhDh = agk::DeviceToDisplayRatioY();
			
			float width = m_pFontImage->m_pImage->GetWidth() * VwDw * scale;
			float height = m_pFontImage->m_pImage->GetHeight() * VhDh * scale;
			SetSize( width, height );
		}
		else
		{
			SetSize(0,0);
		}
	}
	else
	{
		SetImage( 0 );
		SetSize( 0,0 );
	}
}

void cSprite::SetAnimation ( int iFrameWidth, int iFrameHeight, int iFrameCount )
{
	// delete old frames
	if ( m_pFrames != 0 ) 
	{
		if ( (m_iFlags & AGK_SPRITE_MANAGE_IMAGES) )
		{
			for ( int i = 0; i < m_iFrameCount; i++ )
			{
				if ( m_pFrames[i].m_pFrameImage && m_pFrames[i].m_pFrameImage != m_pImage ) m_pFrames[i].m_pFrameImage->RemoveSprite( this );
			}
		}

		delete [] m_pFrames;
	}
	m_pFrames = 0;
	m_iFrameCount = 0;
	m_iFrameArraySize = 0;

	if ( iFrameCount == 0 ) return;
	if ( m_pImage == 0 ) return;
	
	// total size is the size of any atlas texture being used, used for calculating UV coords
	int imageTotalWidth = m_pImage->GetTotalWidth();
	int imageTotalHeight = m_pImage->GetTotalHeight();

	int x = (int) (m_pImage->GetU1() * imageTotalWidth);
	int y = (int) (m_pImage->GetV1() * imageTotalHeight);

	int endX = (int) (m_pImage->GetU2() * imageTotalWidth);
	int endY = (int) (m_pImage->GetV2() * imageTotalHeight);

	/*
	if ( m_pImage->HasParent() )
	{
		x = (int) ((m_pImage->GetU1() * imageTotalWidth) - (0.5f / imageTotalWidth));
		y = (int) ((m_pImage->GetV1() * imageTotalHeight) - (0.5f / imageTotalHeight));

		//endX = (int) ((m_pImage->GetU2() * imageTotalWidth) + (0.5f / imageTotalWidth));
		//endY = (int) ((m_pImage->GetV2() * imageTotalHeight) + (0.5f / imageTotalHeight));
	}
	*/

	if ( iFrameWidth > m_pImage->GetWidth() || iFrameHeight > m_pImage->GetHeight() ) 
	{
#ifdef _AGK_ERROR_CHECK
		uString errStr( "Image is not big enough to have that many animation frames ", 100 ); errStr.Append( m_pImage->GetPath() );
		agk::Error( errStr );
#endif
		return;
	}

	m_iFrameCount = iFrameCount;
	m_iFrameArraySize = iFrameCount;

	// load new frames
	m_pFrames = new cSpriteFrame[ m_iFrameArraySize ];
	
	int count = 0;
	for ( int i = 0; i < iFrameCount; i++ )
	{
		count++;
		/*
		m_pFrames[ i ].m_fU1 = (x / (float) imageTotalWidth) + (0.5f / imageTotalWidth);
		m_pFrames[ i ].m_fV1 = (y / (float) imageTotalHeight) + (0.5f / imageTotalHeight);

		m_pFrames[ i ].m_fU2 = ((x+iFrameWidth) / (float) imageTotalWidth) - (0.5f / imageTotalWidth);
		m_pFrames[ i ].m_fV2 = ((y+iFrameHeight) / (float) imageTotalHeight) - (0.5f / imageTotalHeight);
		*/

		m_pFrames[ i ].m_fU1 = (x / (float) imageTotalWidth);
		m_pFrames[ i ].m_fV1 = (y / (float) imageTotalHeight);

		m_pFrames[ i ].m_fU2 = ((x+iFrameWidth) / (float) imageTotalWidth);
		m_pFrames[ i ].m_fV2 = ((y+iFrameHeight) / (float) imageTotalHeight);

		m_pFrames[ i ].m_iWidth = iFrameWidth;
		m_pFrames[ i ].m_iHeight = iFrameHeight;

		m_pFrames[ i ].m_pFrameImage = m_pImage;

		x += iFrameWidth;

		if ( x+iFrameWidth > endX )
		{
			x = (int) (m_pImage->GetU1() * imageTotalWidth);
			y = y+iFrameHeight;

			if ( y+iFrameHeight > endY ) break;
		}
	}

	if ( iFrameCount != count )
	{
		uString errStr( "Image is not big enough to have that many animation frames ", 100 ); errStr.Append( m_pImage->GetPath() );
		agk::Error( errStr );
	}

	// may not have loaded all frames
	m_iFrameCount = count;
	m_iFrameWidth = iFrameWidth;
	m_iFrameHeight = iFrameHeight;

	// sprite may need resizing
	float fNewWidth = m_fOrigWidth;
	float fNewHeight = m_fOrigHeight;

	if ( (m_iFlags & AGK_SPRITE_WIDTHCALC) != 0 ) fNewWidth = -1;
	if ( (m_iFlags & AGK_SPRITE_HEIGHTCALC) != 0 ) fNewHeight = -1;

	float oldScaleX = m_fWidth / m_fOrigWidth;
	float oldScaleY = m_fHeight / m_fOrigHeight;
	SetSize( fNewWidth, fNewHeight );
	if ( oldScaleX != 1 || oldScaleY != 1 ) 
	{
		if ( m_iFlags & AGK_SPRITE_POSTOPLEFT ) SetScale( oldScaleX, oldScaleY );
		else SetScaleByOffset( oldScaleX, oldScaleY );
	}
}

void cSprite::AppendAnimation ( cImage *pImage, int iFrameWidth, int iFrameHeight, int iFrameCount )
{
	if ( iFrameCount == 0 ) return;
	if ( pImage == 0 ) return;
		
	// total size is the size of any atlas texture being used, used for calculating UV coords
	int imageTotalWidth = pImage->GetTotalWidth();
	int imageTotalHeight = pImage->GetTotalHeight();

	int x = (int) (pImage->GetU1() * imageTotalWidth);
	int y = (int) (pImage->GetV1() * imageTotalHeight);

	int endX = (int) (pImage->GetU2() * imageTotalWidth);
	int endY = (int) (pImage->GetV2() * imageTotalHeight);

	/*
	if ( iFrameWidth > pImage->GetWidth() || iFrameHeight > pImage->GetHeight() ) 
	{
#ifdef _AGK_ERROR_CHECK
		uString errStr( "Image does not contain enough animation frames ", 100 ); errStr.Append( pImage->GetPath() );
		agk::Error( errStr );
#endif
		return;
	}
	*/

	if ( (m_iFlags & AGK_SPRITE_MANAGE_IMAGES) ) pImage->AddSprite( this );

	int newFrameCount = m_iFrameCount + iFrameCount;

	if ( newFrameCount > m_iFrameArraySize )
	{
		cSpriteFrame *pNewFrames = new cSpriteFrame[ newFrameCount ];

		if ( m_pFrames != 0 )
		{
			memcpy( pNewFrames, m_pFrames, sizeof(cSpriteFrame)*m_iFrameCount );
			delete [] m_pFrames;
		}

		// load new frames
		m_pFrames = pNewFrames;
		m_iFrameArraySize = newFrameCount;
	}
	
	int count = 0;
	for ( int i = m_iFrameCount; i < newFrameCount; i++ )
	{
		count++;
		/*
		m_pFrames[ i ].m_fU1 = (x / (float) imageTotalWidth) + (0.5f / imageTotalWidth);
		m_pFrames[ i ].m_fV1 = (y / (float) imageTotalHeight) + (0.5f / imageTotalHeight);

		m_pFrames[ i ].m_fU2 = ((x+iFrameWidth) / (float) imageTotalWidth) - (0.5f / imageTotalWidth);
		m_pFrames[ i ].m_fV2 = ((y+iFrameHeight) / (float) imageTotalHeight) - (0.5f / imageTotalHeight);
		*/

		m_pFrames[ i ].m_fU1 = (x / (float) imageTotalWidth);
		m_pFrames[ i ].m_fV1 = (y / (float) imageTotalHeight);

		m_pFrames[ i ].m_fU2 = ((x+iFrameWidth) / (float) imageTotalWidth);
		m_pFrames[ i ].m_fV2 = ((y+iFrameHeight) / (float) imageTotalHeight);

		m_pFrames[ i ].m_iWidth = iFrameWidth;
		m_pFrames[ i ].m_iHeight = iFrameHeight;

		m_pFrames[ i ].m_pFrameImage = pImage;

		x += iFrameWidth;

		if ( x+iFrameWidth > endX )
		{
			x = (int) (pImage->GetU1() * imageTotalWidth);
			y = y+iFrameHeight;

			if ( y+iFrameHeight > endY ) break;
		}
	}

	// may not have loaded all frames
	m_iFrameCount = m_iFrameCount + count;
	//m_iFrameWidth = iFrameWidth;
	//m_iFrameHeight = iFrameHeight;

	CheckTransparency();
}

void cSprite::ExpandAnimationArray ( int newTotalFrames )
{
	if ( newTotalFrames > m_iFrameArraySize )
	{
		cSpriteFrame *pNewFrames = new cSpriteFrame[ newTotalFrames ];

		// copy old frames
		if ( m_iFrameCount > 0 )
		{
			memcpy( pNewFrames, m_pFrames, sizeof(cSpriteFrame)*m_iFrameCount );
		}

		delete [] m_pFrames;
		m_pFrames = pNewFrames;
		m_iFrameArraySize = newTotalFrames;
	}
}

void cSprite::AddAnimationFrame( cImage *pImage )
{
	if ( !pImage ) return;

	if ( m_iFrameCount+1 > m_iFrameArraySize )
	{
		cSpriteFrame *pNewFrames = new cSpriteFrame[ m_iFrameCount+1 ];

		// copy old frames
		if ( m_iFrameCount > 0 )
		{
			memcpy( pNewFrames, m_pFrames, sizeof(cSpriteFrame)*m_iFrameCount );
		}

		delete [] m_pFrames;
		m_pFrames = pNewFrames;
		m_iFrameArraySize = m_iFrameCount+1;
	}

	if ( (m_iFlags & AGK_SPRITE_MANAGE_IMAGES) ) pImage->AddSprite( this );

	m_pFrames[ m_iFrameCount ].m_fU1 = pImage->GetU1();
	m_pFrames[ m_iFrameCount ].m_fV1 = pImage->GetV1();
	m_pFrames[ m_iFrameCount ].m_fU2 = pImage->GetU2();
	m_pFrames[ m_iFrameCount ].m_fV2 = pImage->GetV2();
	m_pFrames[ m_iFrameCount ].m_iWidth = pImage->GetWidth();
	m_pFrames[ m_iFrameCount ].m_iHeight = pImage->GetHeight();
	m_pFrames[ m_iFrameCount ].m_pFrameImage = pImage;

	if ( !m_pImage ) SwitchImage( pImage );

    m_iFrameCount++;
	
	if ( m_iFrameCount == 1 && (m_iFrameWidth != pImage->GetWidth() || m_iFrameHeight != pImage->GetHeight()) )
	{
		m_iFrameWidth = pImage->GetWidth();
		m_iFrameHeight = pImage->GetHeight();

		// sprite may need resizing
		float fNewWidth = m_fWidth;
		float fNewHeight = m_fHeight;

		if ( (m_iFlags & AGK_SPRITE_WIDTHCALC) != 0 ) fNewWidth = -1;
		if ( (m_iFlags & AGK_SPRITE_HEIGHTCALC) != 0 ) fNewHeight = -1;

		float oldScaleX = m_fWidth / m_fOrigWidth;
		float oldScaleY = m_fHeight / m_fOrigHeight;
		SetSize( fNewWidth, fNewHeight );
		if ( oldScaleX != 1 || oldScaleY != 1 ) SetScaleByOffset( oldScaleX, oldScaleY );
	}

	CheckTransparency();
}

void cSprite::SetFrame( int iFrame )
{
	if ( m_iFrameCount == 0 )
	{
#ifdef _AGK_ERROR_CHECK
		uString err;
		err.Format( "Tried to set an animation frame on a sprite (%d) that has no animation", GetID() );
		agk::Error( err );
#endif
		return;
	}

	if ( iFrame < 1 ) 
	{
		//iFrame = 1;
#ifdef _AGK_ERROR_CHECK
		uString err;
		err.Format( "Invalid frame number %d for sprite (%d), should be in the range 1 to %d.", iFrame, GetID(), m_iFrameCount );
		agk::Error( err );
#endif
		iFrame = 1;
	}

	if ( iFrame > m_iFrameCount ) 
	{
		//iFrame = m_iFrameCount;
#ifdef _AGK_ERROR_CHECK
		uString err;
		err.Format( "Invalid frame number %d for sprite (%d), should be in the range 1 to %d.", iFrame, GetID(), m_iFrameCount );
		agk::Error( err );
#endif
		iFrame = m_iFrameCount;
	}

	// shift from base 1 to base 0 indexing, internally frames start at 0, externally they start at 1
	m_iCurrentFrame = iFrame - 1;

	if ( m_pFrames[ m_iCurrentFrame ].m_pFrameImage != m_pImage ) 
	{
		SwitchImage( m_pFrames[ m_iCurrentFrame ].m_pFrameImage, false );
		
		if ( m_iFrameWidth != m_pFrames[ m_iCurrentFrame ].m_iWidth || m_iFrameHeight != m_pFrames[ m_iCurrentFrame ].m_iHeight )
		{
			m_iFrameWidth = m_pFrames[ m_iCurrentFrame ].m_iWidth;
			m_iFrameHeight = m_pFrames[ m_iCurrentFrame ].m_iHeight;

			// sprite may need resizing
			float fNewWidth = m_fWidth;
			float fNewHeight = m_fHeight;

			if ( (m_iFlags & AGK_SPRITE_WIDTHCALC) != 0 ) fNewWidth = -1;
			if ( (m_iFlags & AGK_SPRITE_HEIGHTCALC) != 0 ) fNewHeight = -1;

			float oldScaleX = m_fWidth / m_fOrigWidth;
			float oldScaleY = m_fHeight / m_fOrigHeight;
			SetSize( fNewWidth, fNewHeight, false );
			if ( oldScaleX != 1 || oldScaleY != 1 ) SetScaleByOffset( oldScaleX, oldScaleY );
		}
	}
}

void cSprite::ClearAnimationFrames()
{
	if ( m_pFrames ) 
	{
		if ( (m_iFlags & AGK_SPRITE_MANAGE_IMAGES) )
		{
			for ( int i = 0; i < m_iFrameCount; i++ )
			{
				if ( m_pFrames[i].m_pFrameImage && m_pFrames[i].m_pFrameImage != m_pImage ) m_pFrames[i].m_pFrameImage->RemoveSprite( this );
			}
		}

		delete [] m_pFrames;
	}
	m_pFrames = 0;
	m_iCurrentFrame = 0;
	m_iFrameArraySize = 0;
	m_iFrameCount = 0;
	m_iFlags &= ~AGK_SPRITE_PLAYING;
}

void cSprite::SetSpeed ( float fFps )
{
	// minimum speed of 1 frame every 1000 seconds
	if ( fFps <= 0 ) fFps = 0.001f;

	m_fFrameChangeTime = 1.0f / fFps;
	
	// If the change in speed would cause a lot of frame skipping only move 1 frame before continuing
	if ( m_fFrameTimer > m_fFrameChangeTime ) m_fFrameTimer = m_fFrameChangeTime;
}

void cSprite::Play ( float fFps, bool bLoop, int iStart, int iEnd )
{
	if ( m_iFrameCount == 0 || m_pFrames == 0 ) return;

	// minimum speed of 1 frame every 1000 seconds
	if ( fFps <= 0 ) fFps = 0.001f;
	
	m_fFrameChangeTime = 1.0f / fFps;
	if ( bLoop ) m_iFlags |= AGK_SPRITE_LOOP;
	else m_iFlags &= ~AGK_SPRITE_LOOP;
	
	if ( iStart < 1 ) m_iFrameStart = 0;
	else if ( iStart > m_iFrameCount ) m_iFrameStart = m_iFrameCount - 1;
	else m_iFrameStart = iStart - 1;

	if ( iEnd < 1 ) m_iFrameEnd = m_iFrameCount - 1;
	else if ( iEnd > m_iFrameCount ) m_iFrameEnd = m_iFrameCount - 1;
	else m_iFrameEnd = iEnd - 1;

	m_fFrameTimer = 0.0f;
	m_iCurrentFrame = m_iFrameStart;
	m_iFlags |= AGK_SPRITE_PLAYING;

	if ( m_pFrames[ m_iCurrentFrame ].m_pFrameImage != m_pImage ) SwitchImage( m_pFrames[ m_iCurrentFrame ].m_pFrameImage, false );
}

void cSprite::Resume()
{
	if ( m_fFrameChangeTime <= 0 )
	{
		m_fFrameChangeTime = 1.0f / 60.0f;
		agk::Error( "ResumeSprite called without first calling PlaySprite" );
	}

	m_iFlags |= AGK_SPRITE_PLAYING;
}

void cSprite::SetBlendFunction( int src, int dst )
{
	m_iUserBlendFunc = ((src & 0xF) << 4) | (dst & 0xF);
	
	if ( m_iTransparencyMode == 3 ) 
	{
		m_renderState.SetBlendFunc( src, dst );
	}
}

void cSprite::SetTransparencyInternal( int mode )
{
	if ( m_iTransparencyMode == mode ) return;
	m_iTransparencyMode = mode;
	
	switch( m_iTransparencyMode )
	{
		case 0: 
		{
			m_renderState.SetBlendEnable( 0 ); 
			m_renderState.SetDepthWriteEnable( 1 );
			m_renderState.SetDepthFunc( AGK_DEPTH_FUNC_LESS );
			break;
		}
		case 1: // alpha blending 
		{
			m_renderState.SetBlendEnable( 1 );
			m_renderState.SetBlendFunc( AGK_BLEND_SRC_ALPHA, AGK_BLEND_ONE_MINUS_SRC_ALPHA );
			m_renderState.SetDepthWriteEnable( 0 );
			m_renderState.SetDepthFunc( AGK_DEPTH_FUNC_LEQUAL );
			break;
		}
		case 2: // additive blending
		{
			m_renderState.SetBlendEnable( 1 );
			m_renderState.SetBlendFunc( AGK_BLEND_ONE, AGK_BLEND_ONE );
			m_renderState.SetDepthWriteEnable( 0 );
			m_renderState.SetDepthFunc( AGK_DEPTH_FUNC_LEQUAL );
			break;
		}
		case 3: // custom blending
		{
			m_renderState.SetBlendEnable( 1 );
			m_renderState.SetBlendFunc( m_iUserBlendFunc >> 4, m_iUserBlendFunc & 0xF );
			m_renderState.SetDepthWriteEnable( 0 );
			m_renderState.SetDepthFunc( AGK_DEPTH_FUNC_LEQUAL );
			break;
		}
	}
}

void cSprite::SetTransparency ( int mode )
{
	m_iFlags |= AGK_SPRITE_FORCE_TRANSPARENCY;
	SetTransparencyInternal( mode );
}

void cSprite::SetOffset ( float x, float y )
{
	float oldX;
	float oldY;
	if ( m_iFlags & AGK_SPRITE_POSTOPLEFT )
	{
		oldX = GetX();
		oldY = GetY();
	}

	m_fOffsetX = x;
	m_fOffsetY = y;
	m_iFlags |= AGK_SPRITE_MANUALOFFSET;
	
	RecalcVisualRadius();
	if ( !m_phyShape ) RecalcColRadius();

	if ( m_iFlags & AGK_SPRITE_POSTOPLEFT )
	{
		SetPosition( oldX, oldY );
	}

	//SetShape( m_eShape );
}

void cSprite::SetUVScale( float scaleU, float scaleV )
{
	if ( scaleU < 0.0001f ) scaleU = 0.0001f;
	if ( scaleV < 0.0001f ) scaleV = 0.0001f;

	m_fUScale = scaleU;
	m_fVScale = scaleV;
}

void cSprite::SetManualScissor( int x, int y, int width, int height )
{
	if ( x == 0 && y == 0 && width == 0 && height == 0 )
	{
		m_iFlags &= ~AGK_SPRITE_MANUAL_SCISSOR;
		if ( (m_iFlags & AGK_SPRITE_SCISSOR_ON) == 0 ) m_renderState.OverrideScissor( 0 );
	}
	else
	{
		m_iFlags |= AGK_SPRITE_MANUAL_SCISSOR;
		m_renderState.OverrideScissor( 1 );
		m_renderState.SetScissor( x, y, width, height );
	}
}

void cSprite::SetScissor( float x, float y, float x2, float y2 )
{
	if ( x == 0 && y == 0 && x2 == 0 && y2 == 0 )
	{
		m_fClipX = 0;
		m_fClipY = 0;
		m_fClipX2 = 0;
		m_fClipY2 = 0;
		m_iFlags &= ~AGK_SPRITE_SCISSOR_ON;
		if ( (m_iFlags & AGK_SPRITE_MANUAL_SCISSOR) == 0 ) m_renderState.OverrideScissor( 0 );
		return;
	}

	float temp;
	if ( x2 < x ) 
	{
		temp = x2;
		x2 = x;
		x = temp;
	}

	if ( y2 < y )
	{
		temp = y2;
		y2 = y;
		y = temp;
	}

	m_fClipX = x;
	m_fClipY = y;
	m_fClipX2 = x2;
	m_fClipY2 = y2;
	m_iFlags |= AGK_SPRITE_SCISSOR_ON;
	m_renderState.OverrideScissor( 1 ); // scissor values will be determine at draw time
}

void cSprite::GetClipValues( int &x, int &y, int &width, int &height )
{
	if ( m_fClipX2 == 0 && m_fClipX == 0 && m_fClipY == 0 && m_fClipY2 == 0 )
	{
		x = 0;
		y = 0;
		width = 0;
		height = 0;
		return;
	}

	float fX = m_fClipX;
	float fY = m_fClipY;
	float fX2 = m_fClipX2;
	float fY2 = m_fClipY2;

	if ( (m_iFlags & AGK_SPRITE_SCROLL) > 0 )
	{
		fX = agk::WorldToScreenX( fX );
		fY = agk::WorldToScreenY( fY );

		fX2 = agk::WorldToScreenX( fX2 );
		fY2 = agk::WorldToScreenY( fY2 );
	}

	// swap Y values for viewport
	uint32_t flipped = 0;
	if ( !agk::GetRenderer()->IsTopLeftOrigin() ) flipped = 1 - flipped;
	if ( agk::GetRenderer()->IsFBOFlipped() && AGKFrameBuffer::g_pCurrFrameBuffer ) flipped = 1 - flipped;
	
	if ( flipped )
	{
		x = agk::ScreenToViewportX( fX );
		y = agk::ScreenToViewportY( fY2 );
		width = agk::ScreenToViewportX( fX2 ) - x;
		height = agk::ScreenToViewportY( fY ) - y;
	}
	else 
	{
		x = agk::ScreenToViewportX( fX );
		y = agk::ScreenToViewportY( fY );
		width = agk::ScreenToViewportX( fX2 ) - x;
		height = agk::ScreenToViewportY( fY2 ) - y;
	}
}

int cSprite::HasAdditionalImages()
{
	if ( m_shaderImages.GetHighestIndex() > 0 ) return 1;
	else return 0;
}

void cSprite::SetUV( float u1, float v1, float u2, float v2, float u3, float v3, float u4, float v4 )
{
	m_bUVOverride = true;
	m_fU1 = u1;
	m_fV1 = v1;

	m_fU2 = u2;
	m_fV2 = v2;

	m_fU3 = u3;
	m_fV3 = v3;

	m_fU4 = u4;
	m_fV4 = v4;
}

void cSprite::CheckImages()
{
	// can only check ID'ed images
	if ( m_iImageID == 0 ) return;

	// check if the image has been deleted or changed
	cImage *pRealImage = agk::GetImagePtr( m_iImageID );
	if ( pRealImage == 0 ) SwitchImage( 0, false );
	if ( pRealImage != m_pImage ) SwitchImage( pRealImage, false );
}

int cSprite::UpdateVertexLayout( AGKVertexLayout *pLayout, AGKShader *pShader )
{
	if ( !pLayout ) return 1;
	if ( !pShader ) return 1;

	int error = 0;

	int numShaderAttribs = pShader->GetNumAttributes();
	pLayout->SetNumOffsets( numShaderAttribs );
	
	pLayout->m_iVertexSize = (uint16_t) GetSpriteVertexFloats() * sizeof(float); // pos + uv + color

	for( int i = 0; i < numShaderAttribs; i++ )
	{
		AGKShaderAttrib *pShaderAttrib = pShader->GetAttribute( i );
		int offset = -1;
		switch( pShaderAttrib->m_iKnownName )
		{
			case AGK_SHADER_ATTRIB_NAME_POS:	offset = GetSpriteVertexPosOffset() * sizeof(float); break;
			case AGK_SHADER_ATTRIB_NAME_UV0:	offset = GetSpriteVertexUVOffset() * sizeof(float); break;
			case AGK_SHADER_ATTRIB_NAME_COLOR:	offset = GetSpriteVertexColorOffset() * sizeof(float); break;
		}

		if ( offset >= 0 ) 
		{
			pLayout->m_pOffsets[ i ] = offset;
		} 
		else 
		{
			pLayout->m_pOffsets[ i ] = 0;
			error = 1;
		}
	}

	return error;
}

void cSprite::Draw()
{
	// only if sprite visible
	if ( (m_iFlags & AGK_SPRITE_VISIBLE) == 0 ) return;
	if ( !GetInScreen() ) return;

	int pixelsW = agk::Round( m_fWidth * (agk::GetDeviceWidth() / (float) agk::GetVirtualWidth()) );
	int pixelsH = agk::Round( m_fHeight * (agk::GetDeviceHeight() / (float) agk::GetVirtualHeight()) );
	g_iPixelsDrawn += (pixelsW * pixelsH);

	uint32_t iVertexStride = GetSpriteVertexFloats(); // position, color, UV
	if ( !m_pVertexData ) m_pVertexData = new float[ iVertexStride * 4 ]; // 4 vertices, triangle strip
	uint32_t colorOffset = GetSpriteVertexColorOffset();
	uint32_t uvOffset = GetSpriteVertexUVOffset();

	// position
	float boneScaleX = 1;
	float boneScaleY = 1;
	float boneScaleXInv = 1;
	float boneScaleYInv = 1;
	if ( m_pBone && (m_pBone->m_bFlags & AGK_BONE_PRE_SCALE) != 0 )
	{
		boneScaleX = m_pBone->worldSX;
		boneScaleY = m_pBone->worldSY;
		boneScaleXInv = 1.0f / boneScaleX;
		boneScaleYInv = 1.0f / boneScaleY;
	}

	float x1,x2,x3,x4;
	float y1,y2,y3,y4;
		
	if ( m_fAngle != 0 )
	{
		float boneScaleX = 1;
		float boneScaleY = 1;
		float boneScaleXInv = 1;
		float boneScaleYInv = 1;
		if ( m_pBone && (m_pBone->m_bFlags & AGK_BONE_PRE_SCALE) != 0 )
		{
			boneScaleX = m_pBone->worldSX;
			boneScaleY = m_pBone->worldSY;
			boneScaleXInv = 1.0f / boneScaleX;
			boneScaleYInv = 1.0f / boneScaleY;
		}

		float stretch = agk::m_fStretchValue;
		float fSinA = agk::SinRad(m_fAngle);
		float fCosA = agk::CosRad(m_fAngle);
		float fSinA1 = fSinA/stretch;
		float fSinA2 = fSinA*stretch;
			
		float fTempX = (-m_fOffsetX)*boneScaleX;
		float fTempY = (-m_fOffsetY)*boneScaleY;
		x1 = m_fX + (fTempX*fCosA - fTempY*fSinA1)*boneScaleXInv;
		y1 = m_fY + (fTempY*fCosA + fTempX*fSinA2)*boneScaleYInv;

		fTempX = (-m_fOffsetX)*boneScaleX;
		fTempY = (-m_fOffsetY + m_fHeight)*boneScaleY;
		x2 = m_fX + (fTempX*fCosA - fTempY*fSinA1)*boneScaleXInv;
		y2 = m_fY + (fTempY*fCosA + fTempX*fSinA2)*boneScaleYInv;

		fTempX = (-m_fOffsetX + m_fWidth)*boneScaleX;
		fTempY = (-m_fOffsetY)*boneScaleY;
		x3 = m_fX + (fTempX*fCosA - fTempY*fSinA1)*boneScaleXInv;
		y3 = m_fY + (fTempY*fCosA + fTempX*fSinA2)*boneScaleYInv;

		fTempX = (-m_fOffsetX + m_fWidth)*boneScaleX;
		fTempY = (-m_fOffsetY + m_fHeight)*boneScaleY;
		x4 = m_fX + (fTempX*fCosA - fTempY*fSinA1)*boneScaleXInv;
		y4 = m_fY + (fTempY*fCosA + fTempX*fSinA2)*boneScaleYInv;
	}
	else
	{
		x1 = m_fX - m_fOffsetX;
		y1 = m_fY - m_fOffsetY;
			
		x2 = m_fX - m_fOffsetX;
		y2 = m_fY - m_fOffsetY + m_fHeight;
			
		x3 = m_fX - m_fOffsetX + m_fWidth;
		y3 = m_fY - m_fOffsetY;
			
		x4 = m_fX - m_fOffsetX + m_fWidth;
		y4 = m_fY - m_fOffsetY + m_fHeight;
	}

	// check for bone transforms
	if ( m_pBone )
	{
		float x1t = m_pBone->m00 * x1 + m_pBone->m01 * y1 + m_pBone->worldX;
		float y1t = m_pBone->m10 * x1 + m_pBone->m11 * y1 + m_pBone->worldY;

		float x2t = m_pBone->m00 * x2 + m_pBone->m01 * y2 + m_pBone->worldX;
		float y2t = m_pBone->m10 * x2 + m_pBone->m11 * y2 + m_pBone->worldY;

		float x3t = m_pBone->m00 * x3 + m_pBone->m01 * y3 + m_pBone->worldX;
		float y3t = m_pBone->m10 * x3 + m_pBone->m11 * y3 + m_pBone->worldY;

		float x4t = m_pBone->m00 * x4 + m_pBone->m01 * y4 + m_pBone->worldX;
		float y4t = m_pBone->m10 * x4 + m_pBone->m11 * y4 + m_pBone->worldY;

		x1 = x1t; y1 = y1t;
		x2 = x2t; y2 = y2t;
		x3 = x3t; y3 = y3t;
		x4 = x4t; y4 = y4t;
	}
		
	if ( m_iFlags & AGK_SPRITE_SCROLL )
	{
		x1 = agk::WorldToScreenX( x1 );
		y1 = agk::WorldToScreenY( y1 );
			
		x2 = agk::WorldToScreenX( x2 );
		y2 = agk::WorldToScreenY( y2 );
			
		x3 = agk::WorldToScreenX( x3 );
		y3 = agk::WorldToScreenY( y3 );
			
		x4 = agk::WorldToScreenX( x4 );
		y4 = agk::WorldToScreenY( y4 );
	}
		
	if ( m_iFlags & AGK_SPRITE_SNAP )
	{
		// correct for floating point pixel positions
		// m_fTargetViewportHeight has sub pixel accuracy, round to a whole pixel first
		float VwDw = agk::DeviceToDisplayRatioX();
		float VhDh = agk::DeviceToDisplayRatioY();
			
		int x1i = agk::Floor( x1/VwDw + 0.5f );
		int y1i = agk::Floor( y1/VhDh + 0.5f );
		x1 = (x1i) * VwDw;
		y1 = (y1i) * VhDh;
			
		int x2i = agk::Floor( x2/VwDw + 0.5f );
		int y2i = agk::Floor( y2/VhDh + 0.5f );
		x2 = (x2i) * VwDw;
		y2 = (y2i) * VhDh;
			
		int x3i = agk::Floor( x3/VwDw + 0.5f );
		int y3i = agk::Floor( y3/VhDh + 0.5f );
		x3 = (x3i) * VwDw;
		y3 = (y3i) * VhDh;
			
		int x4i = agk::Floor( x4/VwDw + 0.5f );
		int y4i = agk::Floor( y4/VhDh + 0.5f );
		x4 = (x4i) * VwDw;
		y4 = (y4i) * VhDh;
	}
		
	// position
	m_pVertexData[ 0 ] = x1;
	m_pVertexData[ 1 ] = y1;
	m_pVertexData[ 2 ] = m_fZ;
	
	m_pVertexData[ iVertexStride ] = x2;
	m_pVertexData[ iVertexStride + 1 ] = y2;
	m_pVertexData[ iVertexStride + 2 ] = m_fZ;
	
	m_pVertexData[ 2*iVertexStride ] = x3;
	m_pVertexData[ 2*iVertexStride + 1 ] = y3;
	m_pVertexData[ 2*iVertexStride + 2 ] = m_fZ;
	
	m_pVertexData[ 3*iVertexStride ] = x4;
	m_pVertexData[ 3*iVertexStride + 1 ] = y4;
	m_pVertexData[ 3*iVertexStride + 2 ] = m_fZ;
	
	// color
	if ( m_iFlags & AGK_SPRITE_VERTEX_COLOR_NEEDS_UPDATE )
	{
		m_pVertexData[ colorOffset ] = m_fColorRed;		
		m_pVertexData[ colorOffset + 1 ] = m_fColorGreen;	
		m_pVertexData[ colorOffset + 2 ] = m_fColorBlue;	
		m_pVertexData[ colorOffset + 3 ] = m_fColorAlpha;

		m_pVertexData[ iVertexStride + colorOffset ] = m_fColorRed;		
		m_pVertexData[ iVertexStride + colorOffset + 1 ] = m_fColorGreen;	
		m_pVertexData[ iVertexStride + colorOffset + 2 ] = m_fColorBlue;	
		m_pVertexData[ iVertexStride + colorOffset + 3 ] = m_fColorAlpha;

		m_pVertexData[ 2*iVertexStride + colorOffset ] = m_fColorRed;		
		m_pVertexData[ 2*iVertexStride + colorOffset + 1 ] = m_fColorGreen;	
		m_pVertexData[ 2*iVertexStride + colorOffset + 2 ] = m_fColorBlue;	
		m_pVertexData[ 2*iVertexStride + colorOffset + 3 ] = m_fColorAlpha;

		m_pVertexData[ 3*iVertexStride + colorOffset ] = m_fColorRed;		
		m_pVertexData[ 3*iVertexStride + colorOffset + 1 ] = m_fColorGreen;	
		m_pVertexData[ 3*iVertexStride + colorOffset + 2 ] = m_fColorBlue;	
		m_pVertexData[ 3*iVertexStride + colorOffset + 3 ] = m_fColorAlpha;
		m_iFlags &= ~AGK_SPRITE_VERTEX_COLOR_NEEDS_UPDATE;
	}

	// UV
	if ( m_pImage != 0 )
	{
        if ( m_bUVOverride )
		{
            m_pVertexData[ uvOffset     ] =	m_fU1;	
			m_pVertexData[ uvOffset + 1 ] =	m_fV1;
			m_pVertexData[ iVertexStride + uvOffset	    ] =	m_fU2;	
			m_pVertexData[ iVertexStride + uvOffset + 1 ] =	m_fV2;
			m_pVertexData[ 2*iVertexStride + uvOffset     ] = m_fU3;		
			m_pVertexData[ 2*iVertexStride + uvOffset + 1 ] = m_fV3;
			m_pVertexData[ 3*iVertexStride + uvOffset     ] = m_fU4;		
			m_pVertexData[ 3*iVertexStride + uvOffset + 1 ] = m_fV4;
		}
		else
		{
			float fU1 = m_pImage->GetU1();
			float fV1 = m_pImage->GetV1();
			float fU2 = m_pImage->GetU2();
			float fV2 = m_pImage->GetV2();

			// modify UVs if animated
			if ( m_iFrameCount > 0 )
			{
				fU1 = m_pFrames[ m_iCurrentFrame ].m_fU1;
				fV1 = m_pFrames[ m_iCurrentFrame ].m_fV1;
				fU2 = m_pFrames[ m_iCurrentFrame ].m_fU2;
				fV2 = m_pFrames[ m_iCurrentFrame ].m_fV2;
			}

			float diff = fU2 - fU1;
			diff /= m_fUScale;
			fU2 = fU1 + diff + m_fUOffset;
			fU1 += m_fUOffset;

			diff = fV2 - fV1;
			diff /= m_fVScale;
			fV2 = fV1 + diff + m_fVOffset;
			fV1 += m_fVOffset;

			if ( m_fUVBorder > 0 )
			{
				fU1 += m_fUVBorder / m_pImage->GetTotalWidth();
				fV1 += m_fUVBorder / m_pImage->GetTotalHeight();
				fU2 -= m_fUVBorder / m_pImage->GetTotalWidth();
				fV2 -= m_fUVBorder / m_pImage->GetTotalHeight();
			}

			if ( (m_iFlags & AGK_SPRITE_FLIPH) > 0 )
			{
				float temp = fU1;
				fU1 = fU2;
				fU2 = temp;
			}

			if ( (m_iFlags & AGK_SPRITE_FLIPV) > 0 )
			{
				float temp = fV1;
				fV1 = fV2;
				fV2 = temp;
			}
			
			m_pVertexData[ uvOffset     ] =	fU1;
			m_pVertexData[ uvOffset + 1 ] =	fV1;
			m_pVertexData[ iVertexStride + uvOffset	    ] =	fU1;
			m_pVertexData[ iVertexStride + uvOffset + 1 ] =	fV2;
			m_pVertexData[ 2*iVertexStride + uvOffset     ] = fU2;
			m_pVertexData[ 2*iVertexStride + uvOffset + 1 ] = fV1;
			m_pVertexData[ 3*iVertexStride + uvOffset     ] = fU2;
			m_pVertexData[ 3*iVertexStride + uvOffset + 1 ] = fV2;
		}
	}

	Renderer *pRenderer = agk::GetRenderer();

	// vertices
	if ( !m_pInternalVertices ) pRenderer->CreateVertices( iVertexStride*4, &m_pInternalVertices );
	pRenderer->SetVertices( m_pInternalVertices, m_pVertexData, 4 );

	// pipeline
	if ( m_iFlags & AGK_SPRITE_VERTEX_LAYOUT_NEEDS_UPDATE )
	{
		UpdateVertexLayout( &m_vertexLayout, m_pShader );
		m_vertexLayout.m_iPrimitiveType = AGK_PRIMITIVE_TRIANGLE_STRIP;
		m_iFlags &= ~AGK_SPRITE_VERTEX_LAYOUT_NEEDS_UPDATE;
	}

	if ( (m_iFlags & AGK_SPRITE_MANUAL_SCISSOR) == 0 && GetScissorOn() )
	{
		int iScissorX, iScissorY, iScissorWidth, iScissorHeight;
		GetClipValues( iScissorX, iScissorY, iScissorWidth, iScissorHeight );
		m_renderState.SetScissor( iScissorX, iScissorY, iScissorWidth, iScissorHeight );
	}
	
	pRenderer->BindPipeline( 0, m_pShader, &m_renderState, &m_vertexLayout, &m_pInternalPipeline );

	// variables
	if ( m_pShader->HasPerSpriteUniforms() )
	{
		AGKUnorderedArray<AGKShaderConstantValue*> shaderConstants( 2 );
		if ( m_pShaderConstantPosition )
		{
			m_pShaderConstantPosition->SetVector2( m_fX, m_fY );
			shaderConstants.AddItem( m_pShaderConstantPosition );
		}
		if ( m_pShaderConstantSize )
		{
			m_pShaderConstantSize->SetVector2( m_fWidth, m_fHeight );
			shaderConstants.AddItem( m_pShaderConstantSize );
		}
		pRenderer->BindShaderConstants( 0, m_pShader, &shaderConstants, 0, &m_shaderImages, &m_pInternalShaderConstants );
	}
	else
	{
		pRenderer->BindShaderConstants( 0, m_pShader, 0, 0, &m_shaderImages, &m_pInternalShaderConstants );
	}
		
	pRenderer->DrawVertices( 0, m_pInternalVertices );
}

void cSprite::BatchDrawQuad( float *pVertexData )
{
	// only if sprite visible, GetInScreen should already have been checked by calling function
	if ( (m_iFlags & AGK_SPRITE_VISIBLE) == 0 ) return;

	int pixelsW = agk::Round( m_fWidth * (agk::GetDeviceWidth() / (float) agk::GetVirtualWidth()) );
	int pixelsH = agk::Round( m_fHeight * (agk::GetDeviceHeight() / (float) agk::GetVirtualHeight()) );
	g_iPixelsDrawn += (pixelsW * pixelsH);
	
	uint32_t iVertexStride = GetSpriteVertexFloats(); // position, color, UV
	uint32_t colorOffset = GetSpriteVertexColorOffset();
	uint32_t uvOffset = GetSpriteVertexUVOffset();
	
	// position
	float boneScaleX = 1;
	float boneScaleY = 1;
	float boneScaleXInv = 1;
	float boneScaleYInv = 1;
	if ( m_pBone && (m_pBone->m_bFlags & AGK_BONE_PRE_SCALE) != 0 )
	{
		boneScaleX = m_pBone->worldSX;
		boneScaleY = m_pBone->worldSY;
		boneScaleXInv = 1.0f / boneScaleX;
		boneScaleYInv = 1.0f / boneScaleY;
	}

	float x1,x2,x3,x4;
	float y1,y2,y3,y4;
		
	if ( m_fAngle != 0 )
	{
		float boneScaleX = 1;
		float boneScaleY = 1;
		float boneScaleXInv = 1;
		float boneScaleYInv = 1;
		if ( m_pBone && (m_pBone->m_bFlags & AGK_BONE_PRE_SCALE) != 0 )
		{
			boneScaleX = m_pBone->worldSX;
			boneScaleY = m_pBone->worldSY;
			boneScaleXInv = 1.0f / boneScaleX;
			boneScaleYInv = 1.0f / boneScaleY;
		}

		float stretch = agk::m_fStretchValue;
		float fSinA = agk::SinRad(m_fAngle);
		float fCosA = agk::CosRad(m_fAngle);
		float fSinA1 = fSinA/stretch;
		float fSinA2 = fSinA*stretch;
			
		float fTempX = (-m_fOffsetX)*boneScaleX;
		float fTempY = (-m_fOffsetY)*boneScaleY;
		x1 = m_fX + (fTempX*fCosA - fTempY*fSinA1)*boneScaleXInv;
		y1 = m_fY + (fTempY*fCosA + fTempX*fSinA2)*boneScaleYInv;

		fTempX = (-m_fOffsetX)*boneScaleX;
		fTempY = (-m_fOffsetY + m_fHeight)*boneScaleY;
		x2 = m_fX + (fTempX*fCosA - fTempY*fSinA1)*boneScaleXInv;
		y2 = m_fY + (fTempY*fCosA + fTempX*fSinA2)*boneScaleYInv;

		fTempX = (-m_fOffsetX + m_fWidth)*boneScaleX;
		fTempY = (-m_fOffsetY)*boneScaleY;
		x3 = m_fX + (fTempX*fCosA - fTempY*fSinA1)*boneScaleXInv;
		y3 = m_fY + (fTempY*fCosA + fTempX*fSinA2)*boneScaleYInv;

		fTempX = (-m_fOffsetX + m_fWidth)*boneScaleX;
		fTempY = (-m_fOffsetY + m_fHeight)*boneScaleY;
		x4 = m_fX + (fTempX*fCosA - fTempY*fSinA1)*boneScaleXInv;
		y4 = m_fY + (fTempY*fCosA + fTempX*fSinA2)*boneScaleYInv;
	}
	else
	{
		x1 = m_fX - m_fOffsetX;
		y1 = m_fY - m_fOffsetY;
			
		x2 = m_fX - m_fOffsetX;
		y2 = m_fY - m_fOffsetY + m_fHeight;
			
		x3 = m_fX - m_fOffsetX + m_fWidth;
		y3 = m_fY - m_fOffsetY;
			
		x4 = m_fX - m_fOffsetX + m_fWidth;
		y4 = m_fY - m_fOffsetY + m_fHeight;
	}

	// check for bone transforms
	if ( m_pBone )
	{
		float x1t = m_pBone->m00 * x1 + m_pBone->m01 * y1 + m_pBone->worldX;
		float y1t = m_pBone->m10 * x1 + m_pBone->m11 * y1 + m_pBone->worldY;

		float x2t = m_pBone->m00 * x2 + m_pBone->m01 * y2 + m_pBone->worldX;
		float y2t = m_pBone->m10 * x2 + m_pBone->m11 * y2 + m_pBone->worldY;

		float x3t = m_pBone->m00 * x3 + m_pBone->m01 * y3 + m_pBone->worldX;
		float y3t = m_pBone->m10 * x3 + m_pBone->m11 * y3 + m_pBone->worldY;

		float x4t = m_pBone->m00 * x4 + m_pBone->m01 * y4 + m_pBone->worldX;
		float y4t = m_pBone->m10 * x4 + m_pBone->m11 * y4 + m_pBone->worldY;

		x1 = x1t; y1 = y1t;
		x2 = x2t; y2 = y2t;
		x3 = x3t; y3 = y3t;
		x4 = x4t; y4 = y4t;
	}
		
	if ( m_iFlags & AGK_SPRITE_SCROLL )
	{
		x1 = agk::WorldToScreenX( x1 );
		y1 = agk::WorldToScreenY( y1 );
			
		x2 = agk::WorldToScreenX( x2 );
		y2 = agk::WorldToScreenY( y2 );
			
		x3 = agk::WorldToScreenX( x3 );
		y3 = agk::WorldToScreenY( y3 );
			
		x4 = agk::WorldToScreenX( x4 );
		y4 = agk::WorldToScreenY( y4 );
	}
		
	if ( m_iFlags & AGK_SPRITE_SNAP )
	{
		// correct for floating point pixel positions
		// m_fTargetViewportHeight has sub pixel accuracy, round to a whole pixel first
		float VwDw = agk::DeviceToDisplayRatioX();
		float VhDh = agk::DeviceToDisplayRatioY();
			
		int x1i = agk::Floor( x1/VwDw + 0.5f );
		int y1i = agk::Floor( y1/VhDh + 0.5f );
		x1 = (x1i) * VwDw;
		y1 = (y1i) * VhDh;
			
		int x2i = agk::Floor( x2/VwDw + 0.5f );
		int y2i = agk::Floor( y2/VhDh + 0.5f );
		x2 = (x2i) * VwDw;
		y2 = (y2i) * VhDh;
			
		int x3i = agk::Floor( x3/VwDw + 0.5f );
		int y3i = agk::Floor( y3/VhDh + 0.5f );
		x3 = (x3i) * VwDw;
		y3 = (y3i) * VhDh;
			
		int x4i = agk::Floor( x4/VwDw + 0.5f );
		int y4i = agk::Floor( y4/VhDh + 0.5f );
		x4 = (x4i) * VwDw;
		y4 = (y4i) * VhDh;
	}
		
	// position
	pVertexData[ 0 ] = x1;
	pVertexData[ 1 ] = y1;
	pVertexData[ 2 ] = m_fZ;
	
	pVertexData[ iVertexStride ] = x2;
	pVertexData[ iVertexStride + 1 ] = y2;
	pVertexData[ iVertexStride + 2 ] = m_fZ;
	
	pVertexData[ 2*iVertexStride ] = x3;
	pVertexData[ 2*iVertexStride + 1 ] = y3;
	pVertexData[ 2*iVertexStride + 2 ] = m_fZ;
	
	pVertexData[ 3*iVertexStride ] = x4;
	pVertexData[ 3*iVertexStride + 1 ] = y4;
	pVertexData[ 3*iVertexStride + 2 ] = m_fZ;
	
	// color
	pVertexData[ colorOffset ] = m_fColorRed;		
	pVertexData[ colorOffset + 1 ] = m_fColorGreen;	
	pVertexData[ colorOffset + 2 ] = m_fColorBlue;	
	pVertexData[ colorOffset + 3 ] = m_fColorAlpha;

	pVertexData[ iVertexStride + colorOffset ] = m_fColorRed;		
	pVertexData[ iVertexStride + colorOffset + 1 ] = m_fColorGreen;	
	pVertexData[ iVertexStride + colorOffset + 2 ] = m_fColorBlue;	
	pVertexData[ iVertexStride + colorOffset + 3 ] = m_fColorAlpha;

	pVertexData[ 2*iVertexStride + colorOffset ] = m_fColorRed;		
	pVertexData[ 2*iVertexStride + colorOffset + 1 ] = m_fColorGreen;	
	pVertexData[ 2*iVertexStride + colorOffset + 2 ] = m_fColorBlue;	
	pVertexData[ 2*iVertexStride + colorOffset + 3 ] = m_fColorAlpha;

	pVertexData[ 3*iVertexStride + colorOffset ] = m_fColorRed;		
	pVertexData[ 3*iVertexStride + colorOffset + 1 ] = m_fColorGreen;	
	pVertexData[ 3*iVertexStride + colorOffset + 2 ] = m_fColorBlue;	
	pVertexData[ 3*iVertexStride + colorOffset + 3 ] = m_fColorAlpha;
	
	// UV
	if ( m_pImage != 0 )
	{
        if ( m_bUVOverride )
		{
            pVertexData[ uvOffset     ] = m_fU1;	
			pVertexData[ uvOffset + 1 ] = m_fV1;
			pVertexData[ iVertexStride + uvOffset	  ] = m_fU2;	
			pVertexData[ iVertexStride + uvOffset + 1 ] = m_fV2;
			pVertexData[ 2*iVertexStride + uvOffset     ] = m_fU3;		
			pVertexData[ 2*iVertexStride + uvOffset + 1 ] = m_fV3;
			pVertexData[ 3*iVertexStride + uvOffset     ] = m_fU4;		
			pVertexData[ 3*iVertexStride + uvOffset + 1 ] = m_fV4;
		}
		else
		{
			float fU1 = m_pImage->GetU1();
			float fV1 = m_pImage->GetV1();
			float fU2 = m_pImage->GetU2();
			float fV2 = m_pImage->GetV2();

			// modify UVs if animated
			if ( m_iFrameCount > 0 )
			{
				fU1 = m_pFrames[ m_iCurrentFrame ].m_fU1;
				fV1 = m_pFrames[ m_iCurrentFrame ].m_fV1;
				fU2 = m_pFrames[ m_iCurrentFrame ].m_fU2;
				fV2 = m_pFrames[ m_iCurrentFrame ].m_fV2;
			}

			float diff = fU2 - fU1;
			diff /= m_fUScale;
			fU2 = fU1 + diff + m_fUOffset;
			fU1 += m_fUOffset;

			diff = fV2 - fV1;
			diff /= m_fVScale;
			fV2 = fV1 + diff + m_fVOffset;
			fV1 += m_fVOffset;

			if ( m_fUVBorder > 0 )
			{
				fU1 += m_fUVBorder / m_pImage->GetTotalWidth();
				fV1 += m_fUVBorder / m_pImage->GetTotalHeight();
				fU2 -= m_fUVBorder / m_pImage->GetTotalWidth();
				fV2 -= m_fUVBorder / m_pImage->GetTotalHeight();
			}

			if ( (m_iFlags & AGK_SPRITE_FLIPH) > 0 )
			{
				float temp = fU1;
				fU1 = fU2;
				fU2 = temp;
			}

			if ( (m_iFlags & AGK_SPRITE_FLIPV) > 0 )
			{
				float temp = fV1;
				fV1 = fV2;
				fV2 = temp;
			}
			
			pVertexData[ uvOffset     ] = fU1;
			pVertexData[ uvOffset + 1 ] = fV1;
			pVertexData[ iVertexStride + uvOffset	  ] = fU1;
			pVertexData[ iVertexStride + uvOffset + 1 ] = fV2;
			pVertexData[ 2*iVertexStride + uvOffset     ] = fU2;
			pVertexData[ 2*iVertexStride + uvOffset + 1 ] = fV1;
			pVertexData[ 3*iVertexStride + uvOffset     ] = fU2;
			pVertexData[ 3*iVertexStride + uvOffset + 1 ] = fV2;
		}
	}
}

// Update sprite animation, physics position, etc
void cSprite::Update( float time )
{
	if ( (m_iFlags & AGK_SPRITE_ACTIVE) != 0 )
	{
		UpdatePhysics();
		UpdateAnimation( time );
	}
}

// Advance the sprite animation by the necessary frames to keep up with frame time 
void cSprite::UpdateAnimation( float time )
{
	if ( m_iFrameCount > 0 && (m_iFlags & AGK_SPRITE_PLAYING) != 0 )
	{
		m_fFrameTimer += time;
		while ( m_fFrameTimer > m_fFrameChangeTime )
		{
			// advance one frame
			m_fFrameTimer -= m_fFrameChangeTime;

			if ( m_iFrameEnd >= m_iFrameStart ) 
			{
				m_iCurrentFrame++;

				if ( m_iCurrentFrame > m_iFrameEnd )
				{
					// reached the end
					if ( (m_iFlags & AGK_SPRITE_LOOP) != 0 ) m_iCurrentFrame = m_iFrameStart;
					else 
					{
						m_iCurrentFrame = m_iFrameEnd;
						m_iFlags &= ~AGK_SPRITE_PLAYING;
						break;
					}
				}
			}
			else 
			{
				m_iCurrentFrame--;

				if ( m_iCurrentFrame < m_iFrameEnd )
				{
					// reached the end
					if ( (m_iFlags & AGK_SPRITE_LOOP) != 0 ) m_iCurrentFrame = m_iFrameStart;
					else 
					{
						m_iCurrentFrame = m_iFrameEnd;
						m_iFlags &= ~AGK_SPRITE_PLAYING;
						break;
					}
				}
			}			

			if ( m_pFrames[ m_iCurrentFrame ].m_pFrameImage != m_pImage ) 
			{
				SwitchImage( m_pFrames[ m_iCurrentFrame ].m_pFrameImage, false );

				if ( m_iFrameWidth != m_pFrames[ m_iCurrentFrame ].m_iWidth || m_iFrameHeight != m_pFrames[ m_iCurrentFrame ].m_iHeight )
				{
					m_iFrameWidth = m_pFrames[ m_iCurrentFrame ].m_iWidth;
					m_iFrameHeight = m_pFrames[ m_iCurrentFrame ].m_iHeight;

					// sprite may need resizing
					float fNewWidth = m_fWidth;
					float fNewHeight = m_fHeight;

					if ( (m_iFlags & AGK_SPRITE_WIDTHCALC) != 0 ) fNewWidth = -1;
					if ( (m_iFlags & AGK_SPRITE_HEIGHTCALC) != 0 ) fNewHeight = -1;

					float oldScaleX = m_fWidth / m_fOrigWidth;
					float oldScaleY = m_fHeight / m_fOrigHeight;
					SetSize( fNewWidth, fNewHeight, false );
					if ( oldScaleX != 1 || oldScaleY != 1 ) SetScaleByOffset( oldScaleX, oldScaleY );
				}
			}
		}
	}
}

// Sprite physics functions
void cSprite::ReplacePhysicsShape( b2Shape *pOldShape, b2Shape *pNewShape )
{
	if ( !pOldShape && !pNewShape ) return;

	if ( pNewShape && pNewShape == m_phyShape )
	{
		float scale = m_fWidth / m_fOrigWidth;
		float scaleY = m_fHeight / m_fOrigHeight;
		if ( scaleY > scale ) scale = scaleY;
		m_fOrigRadius = m_phyShape->m_radius / scale;
	}
	
	if ( !m_phyBody ) 
	{
		if ( pOldShape ) delete pOldShape;
		return;
	}

	b2FixtureDef sFixDef;
	sFixDef.shape = pNewShape;
	sFixDef.localShapeMemory = false;
	if ( pOldShape )
	{
		// replace fixtures
		b2Fixture *pFix = m_phyBody->GetFixtureList();
		b2Fixture *pNext = 0;
		while( pFix )
		{
			pNext = pFix->GetNext();
			if ( pFix->GetShape() == pOldShape )
			{
				if ( !pNewShape ) m_phyBody->DestroyFixture( pFix );
				else
				{
					sFixDef.density = pFix->GetDensity();
					sFixDef.filter = pFix->GetFilterData();
					sFixDef.friction = pFix->GetFriction();
					sFixDef.isSensor = pFix->IsSensor();
					sFixDef.restitution = pFix->GetRestitution();
					sFixDef.userData = pFix->GetUserData();

					m_phyBody->DestroyFixture( pFix );
					m_phyBody->CreateFixture( &sFixDef );
				}
			}
			pFix = pNext;
		}
		delete pOldShape;
	}
	else
	{
		// new fixture
		b2Fixture *pFix = m_phyBody->GetFixtureList();
		if ( pFix )
		{
			// copy from other fixture
			sFixDef.density = pFix->GetDensity();
			sFixDef.filter = pFix->GetFilterData();
			sFixDef.friction = pFix->GetFriction();
			sFixDef.isSensor = pFix->IsSensor();
			sFixDef.restitution = pFix->GetRestitution();
		}
		else
		{
			// use default values
			sFixDef.density = 1.0f;
			sFixDef.friction = 0.3f;
			sFixDef.restitution = 0.1f;
			sFixDef.isSensor = 0;
			sFixDef.filter.categoryBits = m_iCategories & 0x0000ffff;
			sFixDef.filter.maskBits = m_iCategoryMask & 0x0000ffff;
			sFixDef.filter.groupIndex = m_iGroup;
		}

		m_phyBody->CreateFixture( &sFixDef );
	}
}

void cSprite::CalculatePhysicsCOM()
{
	if ( !m_phyBody ) return;
		
	m_phyBody->ResetMassData();
}

void cSprite::SetPhysicsCOM( float x, float y )
{
	if ( !m_phyBody ) return;
		
	b2MassData mass;
	mass.center.Set( agk::WorldToPhyX(x),agk::WorldToPhyY(y) );
	mass.mass = m_phyBody->GetMass();
	mass.I = m_phyBody->GetInertia();
	m_phyBody->SetMassData( &mass );
}

float cSprite::GetPhysicsCOMX()
{
	if ( !m_phyBody ) return 0;
	return agk::PhyToWorldX( m_phyBody->m_sweep.localCenter.x );
}

float cSprite::GetPhysicsCOMY()
{
	if ( !m_phyBody ) return 0;
	return agk::PhyToWorldY( m_phyBody->m_sweep.localCenter.y );
}

bool cSprite::InBox( float x1, float y1, float x2, float y2 )
{
	if ( !m_phyShape ) 
	{
		SetShape( eBox );
	}

	// quick reject
	if ( m_fX + agk::PhyToWorldX(m_fColRadius) < x1 ) return false;
	if ( m_fX - agk::PhyToWorldX(m_fColRadius) > x2 ) return false;
	if ( m_fY + agk::PhyToWorldY(m_fColRadius) < y1 ) return false;
	if ( m_fY - agk::PhyToWorldY(m_fColRadius) > y2 ) return false;

	b2Transform transform1; transform1.Set( b2Vec2(agk::WorldToPhyX(m_fX),agk::WorldToPhyY(m_fY)), m_fAngle );
	b2Transform transform2; transform2.SetIdentity();

	if ( x1 > x2 )
	{
		float temp = x2;
		x2 = x1;
		x1 = temp;
	}

	if ( y1 > y2 )
	{
		float temp = y2;
		y2 = y1;
		y1 = temp;
	}

	x1 = agk::WorldToPhyX( x1 );
	y1 = agk::WorldToPhyY( y1 );
	x2 = agk::WorldToPhyX( x2 );
	y2 = agk::WorldToPhyY( y2 );

	b2PolygonShape poly;
	poly.SetAsBox( (x2-x1)/2.0f, (y2-y1)/2.0f, b2Vec2((x2+x1)/2.0f,(y2+y1)/2.0f), 0 );

	// box-polygon
	if ( m_phyShape->GetType() == b2Shape::e_polygon )
	{
		b2Manifold manifold;
		b2CollidePolygons( &manifold, (b2PolygonShape*)m_phyShape, transform1, &poly, transform2 );
		return (manifold.pointCount > 0);
	}

	// must be box-circle
	b2PolygonShape *polygonA = &poly;
	b2CircleShape *circleB = (b2CircleShape*)m_phyShape;
		
	// Compute circle position in the frame of the polygon.
	b2Vec2 c = b2Mul(transform1, circleB->m_p);
	b2Vec2 cLocal = b2MulT(transform2, c);

	// Find the min separating edge.
	int32 normalIndex = 0;
	float32 separation = -b2_maxFloat;
	float32 radius = polygonA->m_radius + circleB->m_radius;
	int32 vertexCount = polygonA->m_vertexCount;
	const b2Vec2* vertices = polygonA->m_vertices;
	const b2Vec2* normals = polygonA->m_normals;

	for (int32 i = 0; i < vertexCount; ++i)
	{
		float32 s = b2Dot(normals[i], cLocal - vertices[i]);

		// Early out.
		if (s > radius) return false;

		if (s > separation)
		{
			separation = s;
			normalIndex = i;
		}
	}

	// Vertices that subtend the incident face.
	int32 vertIndex1 = normalIndex;
	int32 vertIndex2 = vertIndex1 + 1 < vertexCount ? vertIndex1 + 1 : 0;
	b2Vec2 v1 = vertices[vertIndex1];
	b2Vec2 v2 = vertices[vertIndex2];

	// If the center is inside the polygon ...
	if (separation < b2_epsilon)
	{
		return true;
	}

	// Compute barycentric coordinates
	float32 u1 = b2Dot(cLocal - v1, v2 - v1);
	float32 u2 = b2Dot(cLocal - v2, v1 - v2);
	if (u1 <= 0.0f)
	{
		return (b2DistanceSquared(cLocal, v1) <= radius * radius);
	}
	else if (u2 <= 0.0f)
	{
		return (b2DistanceSquared(cLocal, v2) <= radius * radius);
	}
	else
	{
		b2Vec2 faceCenter = 0.5f * (v1 + v2);
		float32 separation = b2Dot(cLocal - faceCenter, normals[vertIndex1]);
		return (separation <= radius);
	}
}

bool cSprite::InCircle( float x1, float y1, float radius )
{
	if ( !m_phyShape ) 
	{
		SetShape( eBox );
	}

	// quick reject
	float diffX = agk::WorldToPhyX(m_fX - x1);
	float diffY = agk::WorldToPhyY(m_fY - y1);
	float radius2 = agk::WorldToPhyX(radius);
	float radiusSqr = m_fColRadius*m_fColRadius + radius2*radius2 + 2*m_fColRadius*radius2;
	if ( diffX*diffX + diffY*diffY > radiusSqr ) return false;

	b2Transform transform1; transform1.Set( b2Vec2(agk::WorldToPhyX(m_fX),agk::WorldToPhyY(m_fY)), m_fAngle );
	b2Transform transform2; transform2.SetIdentity();

	x1 = agk::WorldToPhyX( x1 );
	y1 = agk::WorldToPhyY( y1 );
	radius = agk::WorldToPhyX( radius );

	b2CircleShape circle;
	circle.m_p.Set( x1, y1 );
	circle.m_radius = radius;

	// circle-circle
	if ( m_phyShape->GetType() == b2Shape::e_circle )
	{
		b2Vec2 pA = b2Mul(transform1, ((b2CircleShape*)m_phyShape)->m_p);
		b2Vec2 pB = b2Mul(transform2, (circle.m_p));

		b2Vec2 d = pB - pA;
		float32 distSqr = b2Dot(d, d);
		float32 rA = m_phyShape->m_radius, rB = circle.m_radius;
		float32 radius = rA + rB;
		return (distSqr < radius * radius);
	}

	// must be polygon-circle
	b2PolygonShape *polygonA = (b2PolygonShape*)m_phyShape;
	b2CircleShape *circleB = &circle;;
		
	// Compute circle position in the frame of the polygon.
	b2Vec2 c = b2Mul(transform2, circleB->m_p);
	b2Vec2 cLocal = b2MulT(transform1, c);

	// Find the min separating edge.
	int32 normalIndex = 0;
	float32 separation = -b2_maxFloat;
	radius = polygonA->m_radius + circleB->m_radius;
	int32 vertexCount = polygonA->m_vertexCount;
	const b2Vec2* vertices = polygonA->m_vertices;
	const b2Vec2* normals = polygonA->m_normals;

	for (int32 i = 0; i < vertexCount; ++i)
	{
		float32 s = b2Dot(normals[i], cLocal - vertices[i]);

		// Early out.
		if (s > radius) return false;

		if (s > separation)
		{
			separation = s;
			normalIndex = i;
		}
	}

	// Vertices that subtend the incident face.
	int32 vertIndex1 = normalIndex;
	int32 vertIndex2 = vertIndex1 + 1 < vertexCount ? vertIndex1 + 1 : 0;
	b2Vec2 v1 = vertices[vertIndex1];
	b2Vec2 v2 = vertices[vertIndex2];

	// If the center is inside the polygon ...
	if (separation < b2_epsilon) return true;

	// Compute barycentric coordinates
	float32 u1 = b2Dot(cLocal - v1, v2 - v1);
	float32 u2 = b2Dot(cLocal - v2, v1 - v2);
	if (u1 <= 0.0f)
	{
		return (b2DistanceSquared(cLocal, v1) <= radius * radius);
	}
	else if (u2 <= 0.0f)
	{
		return (b2DistanceSquared(cLocal, v2) <= radius * radius);
	}
	else
	{
		b2Vec2 faceCenter = 0.5f * (v1 + v2);
		float32 separation = b2Dot(cLocal - faceCenter, normals[vertIndex1]);
		return (separation <= radius);
	}
}

float cSprite::GetDistancePoint1X() { return agk::PhyToWorldX(m_colResult->pointA.x); }
float cSprite::GetDistancePoint1Y() { return agk::PhyToWorldY(m_colResult->pointA.y); }
float cSprite::GetDistancePoint2X() { return agk::PhyToWorldX(m_colResult->pointB.x); }
float cSprite::GetDistancePoint2Y() { return agk::PhyToWorldY(m_colResult->pointB.y); }

bool cSprite::GetCollision( cSprite *pSprite2 )
{
	if ( !m_phyShape ) 
	{
		// create a default box
		SetShape( eBox );
	}

	if ( !pSprite2->m_phyShape ) 
	{
		// create a default box
		pSprite2->SetShape( eBox );
	}

	float fX1 = m_fX;
	float fY1 = m_fY;
	float fX2 = pSprite2->m_fX;
	float fY2 = pSprite2->m_fY;
	float fColRadius1 = m_fColRadius;
	float fColRadius2 = pSprite2->m_fColRadius;

	b2Shape* shape1 = 0;
	switch( m_phyShape->GetType() )
	{
		case b2Shape::e_polygon:
		{
			shape1 = new b2PolygonShape();
			*((b2PolygonShape*)shape1) = *((b2PolygonShape*)m_phyShape);
			break;
		}

		case b2Shape::e_circle:
		{
			shape1 = new b2CircleShape();
			*((b2CircleShape*)shape1) = *((b2CircleShape*)m_phyShape);
			break;
		}
        default: agk::Error( "Unsupported Box2D shape" );
	}

	b2Shape* shape2 = 0;
	switch( pSprite2->m_phyShape->GetType() )
	{
		case b2Shape::e_polygon:
		{
			shape2 = new b2PolygonShape();
			*((b2PolygonShape*)shape2) = *((b2PolygonShape*)pSprite2->m_phyShape);
			break;
		}

		case b2Shape::e_circle:
		{
			shape2 = new b2CircleShape();
			*((b2CircleShape*)shape2) = *((b2CircleShape*)pSprite2->m_phyShape);
			break;
		}
        default: agk::Error( "Unsupported Box2D shape" );
	}

	if ( !shape1 || !shape2 ) 
	{
		if ( shape1 ) delete shape1;
		if ( shape2 ) delete shape2;
		return false;
	}

	if ( (m_iFlags & AGK_SPRITE_SCROLL) == 0 )
	{
		fX1 = agk::ScreenToWorldX( fX1 );
		fY1 = agk::ScreenToWorldY( fY1 );
		fColRadius1 /= agk::GetViewZoom();

		if ( shape1->GetType() == b2Shape::e_polygon )
		{
			int vertexCount = ((b2PolygonShape*)shape1)->m_vertexCount;
			b2Vec2 *vertices = ((b2PolygonShape*)shape1)->m_vertices;

			for ( int i = 0; i < vertexCount; i++ )
			{
				vertices[ i ].x /= agk::GetViewZoom();
				vertices[ i ].y /= agk::GetViewZoom();
			}
		}
			
		if ( shape1->GetType() == b2Shape::e_circle )
		{
			shape1->m_radius /= agk::GetViewZoom();
		}
	}

	if ( (pSprite2->m_iFlags & AGK_SPRITE_SCROLL) == 0 )
	{
		fX2 = agk::ScreenToWorldX( fX2 );
		fY2 = agk::ScreenToWorldY( fY2 );
		fColRadius2 /= agk::GetViewZoom();

		if ( shape2->GetType() == b2Shape::e_polygon )
		{
			int vertexCount = ((b2PolygonShape*)shape2)->m_vertexCount;
			b2Vec2 *vertices = ((b2PolygonShape*)shape2)->m_vertices;

			for ( int i = 0; i < vertexCount; i++ )
			{
				vertices[ i ].x /= agk::GetViewZoom();
				vertices[ i ].y /= agk::GetViewZoom();
			}
		}
			
		if ( shape2->GetType() == b2Shape::e_circle )
		{
			shape2->m_radius /= agk::GetViewZoom();
		}
	}

	// quick reject
	float diffX = agk::WorldToPhyX(fX1 - fX2);
	float diffY = agk::WorldToPhyY(fY1 - fY2);
	float radiusSqr = fColRadius1*fColRadius1 + fColRadius2*fColRadius2 + 2*fColRadius1*fColRadius2;
	if ( diffX*diffX + diffY*diffY > radiusSqr ) 
	{
		delete shape1;
		delete shape2;
		return false;
	}

	b2Transform transform1; transform1.Set( b2Vec2(agk::WorldToPhyX(fX1),agk::WorldToPhyY(fY1)), m_fAngle );
	b2Transform transform2; transform2.Set( b2Vec2(agk::WorldToPhyX(fX2),agk::WorldToPhyY(fY2)), pSprite2->m_fAngle );

	// circle-circle
	if ( shape1->GetType() == b2Shape::e_circle && shape2->GetType() == b2Shape::e_circle )
	{
		b2Vec2 pA = b2Mul(transform1, ((b2CircleShape*)shape1)->m_p);
		b2Vec2 pB = b2Mul(transform2, ((b2CircleShape*)shape2)->m_p);

		b2Vec2 d = pB - pA;
		float32 distSqr = b2Dot(d, d);
		float32 rA = shape1->m_radius, rB = shape2->m_radius;
		float32 radius = rA + rB;
		delete shape1;
		delete shape2;
		return (distSqr < radius * radius);
	}

	// polygon-polygon
	if ( shape1->GetType() == b2Shape::e_polygon && shape2->GetType() == b2Shape::e_polygon )
	{
		b2Manifold manifold;
		b2CollidePolygons( &manifold, (b2PolygonShape*)shape1, transform1, (b2PolygonShape*)shape2, transform2 );
		delete shape1;
		delete shape2;
		return (manifold.pointCount > 0);
	}

	// must be polygon-circle
	b2PolygonShape *polygonA;
	b2CircleShape *circleB;
	if ( shape1->GetType() == b2Shape::e_polygon )
	{
		polygonA = (b2PolygonShape*)shape1;
		circleB = (b2CircleShape*)(shape2);

		transform1.Set( b2Vec2(agk::WorldToPhyX(fX1),agk::WorldToPhyY(fY1)), m_fAngle );
		transform2.Set( b2Vec2(agk::WorldToPhyX(fX2),agk::WorldToPhyY(fY2)), pSprite2->m_fAngle );
	}
	else
	{
		polygonA = (b2PolygonShape*)(shape2);
		circleB = (b2CircleShape*)shape1;

		transform1.Set( b2Vec2(agk::WorldToPhyX(fX2),agk::WorldToPhyY(fY2)), pSprite2->m_fAngle );
		transform2.Set( b2Vec2(agk::WorldToPhyX(fX1),agk::WorldToPhyY(fY1)), m_fAngle );
	}

	// Compute circle position in the frame of the polygon.
	b2Vec2 c = b2Mul(transform2, circleB->m_p);
	b2Vec2 cLocal = b2MulT(transform1, c);

	// Find the min separating edge.
	int32 normalIndex = 0;
	float32 separation = -b2_maxFloat;
	float32 radius = polygonA->m_radius + circleB->m_radius;
	int32 vertexCount = polygonA->m_vertexCount;
	const b2Vec2* vertices = polygonA->m_vertices;
	const b2Vec2* normals = polygonA->m_normals;

	for (int32 i = 0; i < vertexCount; ++i)
	{
		float32 s = b2Dot(normals[i], cLocal - vertices[i]);

		// Early out.
		if (s > radius) 
		{
			delete shape1;
			delete shape2;
			return false;
		}

		if (s > separation)
		{
			separation = s;
			normalIndex = i;
		}
	}

	// Vertices that subtend the incident face.
	int32 vertIndex1 = normalIndex;
	int32 vertIndex2 = vertIndex1 + 1 < vertexCount ? vertIndex1 + 1 : 0;
	b2Vec2 v1 = vertices[vertIndex1];
	b2Vec2 v2 = vertices[vertIndex2];

	// If the center is inside the polygon ...
	if (separation < b2_epsilon)
	{
		delete shape1;
		delete shape2;
		return true;
	}

	// Compute barycentric coordinates
	float32 u1 = b2Dot(cLocal - v1, v2 - v1);
	float32 u2 = b2Dot(cLocal - v2, v1 - v2);
	if (u1 <= 0.0f)
	{
		delete shape1;
		delete shape2;
		return (b2DistanceSquared(cLocal, v1) <= radius * radius);
	}
	else if (u2 <= 0.0f)
	{
		delete shape1;
		delete shape2;
		return (b2DistanceSquared(cLocal, v2) <= radius * radius);
	}
	else
	{
		b2Vec2 faceCenter = 0.5f * (v1 + v2);
		float32 separation = b2Dot(cLocal - faceCenter, normals[vertIndex1]);
		delete shape1;
		delete shape2;
		return (separation <= radius);
	}
}

float cSprite::GetDistance( cSprite *pSprite2 )
{
	if ( !m_phyShape ) 
	{
		// create a box for them
		SetShape( eBox );
	}

	if ( !pSprite2->m_phyShape ) 
	{
		pSprite2->SetShape( eBox );
	}

	b2DistanceInput input;
	input.proxyA.Set(m_phyShape,0);
	input.proxyB.Set(pSprite2->m_phyShape,0);
	input.transformA.Set( b2Vec2(agk::WorldToPhyX(m_fX),agk::WorldToPhyY(m_fY)), m_fAngle );
	input.transformB.Set( b2Vec2(agk::WorldToPhyX(pSprite2->m_fX),agk::WorldToPhyY(pSprite2->m_fY)), pSprite2->m_fAngle );
	input.useRadii = true;

	b2SimplexCache cache;
	cache.count = 0;

	b2Distance(m_colResult, &cache, &input);

	return agk::PhyToWorldX(m_colResult->distance);
}

bool cSprite::GetPhysicsCollision( cSprite *pSprite2 )
{
	m_lastContact = 0;

	if ( !pSprite2 ) return false;

	if ( !m_phyBody ) return false;
	if ( !pSprite2->m_phyBody ) return false;
		
	b2ContactEdge *pContact = m_phyBody->GetContactList();
	while ( pContact )
	{
		if ( pContact->contact->IsTouching() && pContact->other == pSprite2->m_phyBody ) 
		{
			m_lastContact = pContact->contact;
			return true;
		}
		pContact = pContact->next;
	}

	return false;
}

float cSprite::GetPhysicsCollisionX( )
{
	if ( m_lastContact ) return GetXFromWorld( GetPhysicsCollisionWorldX(), GetPhysicsCollisionWorldY() );
	return 0;
}

float cSprite::GetPhysicsCollisionY( )
{
	if ( m_lastContact ) return GetYFromWorld( GetPhysicsCollisionWorldX(), GetPhysicsCollisionWorldY() );
	return 0;
}

float cSprite::GetPhysicsCollisionWorldX( )
{
	if ( m_lastContact ) 
	{
		b2WorldManifold manifold;
		m_lastContact->GetWorldManifold( &manifold );
		return agk::PhyToWorldX(manifold.points[0].x);
	}
	return 0;
}

float cSprite::GetPhysicsCollisionWorldY( )
{
	if ( m_lastContact ) 
	{
		b2WorldManifold manifold;
		m_lastContact->GetWorldManifold( &manifold );
		return agk::PhyToWorldY(manifold.points[0].y);
	}
	return 0;
}

void cSprite::SetShape( ePhysicsShape shape, int shapeID )
{
	if ( shape == eManual ) return;

	if ( shapeID > (int)m_iNumAdditionalShapes )
	{
		uString err; err.Format( "Cannot set the shape, shapeID %d does not exist", shapeID+1 );
		agk::Error( err );
		return;
	}

	// legacy functionality, shapeID of -1 replaces all shapes with just this one
	if ( shapeID < 0 ) 
	{
		ClearAdditionalShapes();
		shapeID = 0; // set the main shape
	}

	b2Shape **ppShapePtr = &m_phyShape;
	if ( shapeID > 0 ) ppShapePtr = m_phyAdditionalShapes + (shapeID-1);

	b2Shape *pOldShape = *ppShapePtr;
	*ppShapePtr = 0;

	if ( shape == eNone ) 
	{
		ReplacePhysicsShape( pOldShape, 0 );
		return;
	}

	// can't create a polygon without an image.
	if ( (!m_pImage || !m_pImage->GetInternalResource()) && shape == ePolygon ) shape = eBox;

	switch( shape )
	{
		case cSprite::ePolygon:
		{
			const Point2D *pPoints = 0;
			if ( m_pImage )
			{
				if ( m_iFrameCount == 0 ) pPoints = m_pImage->GetBoundingPoints( );
				else pPoints = m_pImage->GetBoundingPoints( m_pFrames[m_iCurrentFrame].m_fU1, m_pFrames[m_iCurrentFrame].m_fV1, m_pFrames[m_iCurrentFrame].m_fU2, m_pFrames[m_iCurrentFrame].m_fV2 );
			}

			b2PolygonShape *polyShape = new b2PolygonShape();

			int count = Point2D::Count( pPoints );
			if ( count < 3 )
			{
				// not enough points for convex hull
				polyShape->SetAsBox( agk::WorldToPhyX(m_fWidth/2.0f), agk::WorldToPhyY(m_fHeight/2.0f), b2Vec2(agk::WorldToPhyX(m_fWidth/2.0f-m_fOffsetX),agk::WorldToPhyY(m_fHeight/2.0f-m_fOffsetY)), 0.0f );
			}
			else
			{
				b2Vec2 *vertices = new b2Vec2[ count ];

				int flipH = (m_iFlags & AGK_SPRITE_FLIPH) > 0 ? 1 : 0;
				int flipV = (m_iFlags & AGK_SPRITE_FLIPV) > 0 ? 1 : 0;
				int reverse = flipH ^ flipV;
				float maxDist = 0;

				int index = reverse ? count-1 : 0;
				const Point2D *pPoint = pPoints;
				while ( pPoint )
				{
					float x = pPoint->x;
					float y = pPoint->y;
					if ( m_iFrameCount > 0 )
					{
						x = x * m_fWidth / m_iFrameWidth;
						y = y * m_fHeight / m_iFrameHeight;
					}
					else
					{
						x = x * m_fWidth / m_pImage->GetWidth();
						y = y * m_fHeight / m_pImage->GetHeight();
					}

						
					x = x - m_fOffsetX;
					y = y - m_fOffsetY;

					x = agk::WorldToPhyX(x);
					y = agk::WorldToPhyY(y);

					float dist = x*x + y*y;
					if ( dist > maxDist ) maxDist = dist;

					if ( flipH ) x = -x;
					if ( flipV ) y = -y;

					vertices[ index ].Set( x, y );

					if ( reverse ) index--;
					else index++;
					pPoint = pPoint->pNext;
				}

				polyShape->Set( vertices, count ); 

				delete [] vertices;
			}
			*ppShapePtr = (b2Shape*) polyShape;
			break;
		}

		case cSprite::eBox:
		{
			b2PolygonShape *polyShape = new b2PolygonShape();
			polyShape->SetAsBox( agk::WorldToPhyX(m_fWidth/2.0f), agk::WorldToPhyY(m_fHeight/2.0f), b2Vec2(agk::WorldToPhyX(m_fWidth/2.0f-m_fOffsetX),agk::WorldToPhyY(m_fHeight/2.0f-m_fOffsetY)), 0.0f );
			*ppShapePtr = (b2Shape*) polyShape;
			break;
		}

		case cSprite::eCircle:
		{
			b2CircleShape *circleShape = new b2CircleShape();

			float maxDist = 0;
			if ( !m_pImage )
			{
				float maxX = m_fOffsetX;
				float maxY = m_fOffsetY;
				if ( m_fOffsetX < m_fWidth/2 ) maxX = m_fWidth - m_fOffsetX;
				if ( m_fOffsetY < m_fHeight/2 ) maxY = m_fHeight - m_fOffsetY;

				maxX = agk::WorldToPhyX(maxX);
				maxY = agk::WorldToPhyY(maxY);
				maxDist = maxX*maxX + maxY*maxY;
			}
			else
			{
				// get the convex hull points and work out which one is furthest from the offset point
				const Point2D *pPoints = 0;
				if ( m_pImage )
				{
					if ( m_iFrameCount == 0 ) pPoints = m_pImage->GetBoundingPoints( );
					else pPoints = m_pImage->GetBoundingPoints( m_pFrames[m_iCurrentFrame].m_fU1, m_pFrames[m_iCurrentFrame].m_fV1, m_pFrames[m_iCurrentFrame].m_fU2, m_pFrames[m_iCurrentFrame].m_fV2 );
				}

				const Point2D *pPoint = pPoints;
				while ( pPoint )
				{
					float x = pPoint->x;
					float y = pPoint->y;

					if ( m_iFrameCount > 0 )
					{
						x = x * m_fOrigWidth / m_iFrameWidth;
						y = y * m_fOrigHeight / m_iFrameHeight;
					}
					else
					{
						x = x * m_fOrigWidth / m_pImage->GetWidth();
						y = y * m_fOrigHeight / m_pImage->GetHeight();
					}

					x = x - m_fOffsetX;
					y = y - m_fOffsetY;

					x = agk::WorldToPhyX(x);
					y = agk::WorldToPhyY(y);

					float dist = x*x + y*y;
					if ( dist > maxDist ) maxDist = dist;
					pPoint = pPoint->pNext;
				}
			}

			circleShape->m_radius = agk::Sqrt( maxDist );
			circleShape->m_p.Set(0,0);
			*ppShapePtr = (b2Shape*) circleShape;
			break;
		}

		default: 
		{
#ifdef _AGK_ERROR_CHECK
			agk::Error( "Unrecognised physics shape for sprite in SetSpriteShape()" );
#endif
			ReplacePhysicsShape( pOldShape, 0 );
			return;
		}
	}

	m_eShape = shape;
	ReplacePhysicsShape( pOldShape, *ppShapePtr );
	RecalcColRadius();
}

void cSprite::SetShapeBox( float x, float y, float x2, float y2, float angle, int shapeID )
{
	if ( shapeID > (int)m_iNumAdditionalShapes )
	{
		uString err; err.Format( "Cannot set the shape, shapeID %d does not exist", shapeID+1 );
		agk::Error( err );
		return;
	}

	// legacy functionality, shapeID of -1 replaces all shapes with just this one
	if ( shapeID < 0 ) 
	{
		ClearAdditionalShapes();
		shapeID = 0; // set the main shape
	}

	b2Shape **ppShapePtr = &m_phyShape;
	if ( shapeID > 0 ) ppShapePtr = m_phyAdditionalShapes + (shapeID-1);

	b2Shape *pOldShape = *ppShapePtr;
	*ppShapePtr = 0;

	if ( x2 < x )
	{
		// swap
		float temp = x;
		x = x2; 
		x2 = temp;
	}

	if ( y2 < y )
	{
		// swap
		float temp = y;
		y = y2; 
		y2 = temp;
	}

	float width = x2 - x;
	float height = y2 - y;
	float posX = (x+x2)/2.0f;// - m_fOffsetX;
	float posY = (y+y2)/2.0f;// - m_fOffsetY;

	b2PolygonShape* polyShape = new b2PolygonShape();
	polyShape->SetAsBox( agk::WorldToPhyX(width/2.0f), agk::WorldToPhyY(height/2.0f), b2Vec2(agk::WorldToPhyX(posX),agk::WorldToPhyY(posY)), angle );
	*ppShapePtr = (b2Shape*) polyShape;
		
	m_eShape = eManual;
	ReplacePhysicsShape( pOldShape, *ppShapePtr );
	RecalcColRadius();
}

void cSprite::SetShapeCircle( float x, float y, float radius, int shapeID )
{
	if ( shapeID > (int)m_iNumAdditionalShapes )
	{
		uString err; err.Format( "Cannot set the shape, shapeID %d does not exist", shapeID+1 );
		agk::Error( err );
		return;
	}

	// legacy functionality, shapeID of -1 replaces all shapes with just this one
	if ( shapeID < 0 ) 
	{
		ClearAdditionalShapes();
		shapeID = 0; // set the main shape
	}

	b2Shape **ppShapePtr = &m_phyShape;
	if ( shapeID > 0 ) ppShapePtr = m_phyAdditionalShapes + (shapeID-1);

	b2Shape *pOldShape = *ppShapePtr;
	*ppShapePtr = 0;

	b2CircleShape* circleShape = new b2CircleShape();
	circleShape->m_p.Set( agk::WorldToPhyX(x), agk::WorldToPhyY(y) );
	circleShape->m_radius = agk::WorldToPhyX(radius);
	*ppShapePtr = (b2Shape*) circleShape;
		
	m_eShape = eManual;
	ReplacePhysicsShape( pOldShape, *ppShapePtr );
	RecalcColRadius();
}

void cSprite::SetShapePolygon( uint32_t numPoints, float *pPoints, int shapeID )
{
	if ( !pPoints ) return;
	if ( numPoints < 2 ) return;
	if ( numPoints > b2_maxPolygonVertices ) 
	{
#ifdef _AGK_ERROR_CHECK
		agk::Error( "Too many points for a physics polygon" );
#endif
		return;
	}

	if ( shapeID > (int)m_iNumAdditionalShapes )
	{
		uString err; err.Format( "Cannot set the shape, shapeID %d does not exist", shapeID+1 );
		agk::Error( err );
		return;
	}

	// legacy functionality, shapeID of -1 replaces all shapes with just this one
	if ( shapeID < 0 ) 
	{
		ClearAdditionalShapes();
		shapeID = 0; // set the main shape
	}

	b2Shape **ppShapePtr = &m_phyShape;
	if ( shapeID > 0 ) ppShapePtr = m_phyAdditionalShapes + (shapeID-1);

	b2Shape *pOldShape = *ppShapePtr;
	*ppShapePtr = 0;

	b2Vec2 *vertices = new b2Vec2[ numPoints ];
	for ( uint32_t i = 0; i < numPoints; i++ )
	{
		vertices[ i ].x = agk::WorldToPhyX( pPoints[ i*2 ] );
		vertices[ i ].y = agk::WorldToPhyY( pPoints[ i*2 + 1 ] );
	}

	b2PolygonShape* polyShape = new b2PolygonShape();
	polyShape->Set( vertices, (int)numPoints );
	*ppShapePtr = (b2Shape*) polyShape;
			
	m_eShape = eManual;
	ReplacePhysicsShape( pOldShape, *ppShapePtr );
	RecalcColRadius();
}

void cSprite::SetShapePolygon( uint32_t numPoints, uint32_t index, float x, float y, int shapeID )
{
	if ( numPoints > b2_maxPolygonVertices )
	{
#ifdef _AGK_ERROR_CHECK
		uString errStr;
		errStr.Format( "Could not add polygon point, polygon shapes have a maximum of %d points", b2_maxPolygonVertices );
		agk::Error( errStr );
#endif
		return;
	}

	if ( m_iPolygonPointsNum < numPoints )
	{
		if ( m_fPolygonPointsTemp ) delete [] m_fPolygonPointsTemp;
		m_fPolygonPointsTemp = new float[ numPoints*2 ];
		for ( uint32_t i = 0; i < numPoints*2; i++ ) m_fPolygonPointsTemp[ i ] = 0;
		m_iPolygonPointsNum = numPoints;
	}

	if ( index >= numPoints )
	{
#ifdef _AGK_ERROR_CHECK
		agk::Error( uString("Could not add point to polygon shape, index must be less than the number of points. index starts at 0.") );
#endif
		return;
	}

	m_fPolygonPointsTemp[ index*2 ] = x;
	m_fPolygonPointsTemp[ index*2 + 1 ] = y;

	if ( numPoints == index+1 ) 
	{
		SetShapePolygon( numPoints, m_fPolygonPointsTemp, shapeID );
		delete [] m_fPolygonPointsTemp;
		m_fPolygonPointsTemp = 0;
		m_iPolygonPointsNum = 0;
	}
}

void cSprite::SetShapeChain( uint32_t numPoints, float *pPoints, int loop, int shapeID )
{
	if ( !pPoints ) return;
	if ( numPoints < 2 ) return;
	
	if ( shapeID > (int)m_iNumAdditionalShapes )
	{
		uString err; err.Format( "Cannot set the shape, shapeID %d does not exist", shapeID+1 );
		agk::Error( err );
		return;
	}

	// legacy functionality, shapeID of -1 replaces all shapes with just this one
	if ( shapeID < 0 ) 
	{
		ClearAdditionalShapes();
		shapeID = 0; // set the main shape
	}

	b2Shape **ppShapePtr = &m_phyShape;
	if ( shapeID > 0 ) ppShapePtr = m_phyAdditionalShapes + (shapeID-1);

	b2Shape *pOldShape = *ppShapePtr;
	*ppShapePtr = 0;

	b2Vec2 *vertices = new b2Vec2[ numPoints ];
	for ( uint32_t i = 0; i < numPoints; i++ )
	{
		vertices[ i ].x = agk::WorldToPhyX( pPoints[ i*2 ] );
		vertices[ i ].y = agk::WorldToPhyY( pPoints[ i*2 + 1 ] );
	}

	b2ChainShape* chainShape = new b2ChainShape();
	if ( loop == 1 ) chainShape->CreateLoop( vertices, (int)numPoints );
	else chainShape->CreateChain( vertices, (int)numPoints );
	*ppShapePtr = (b2Shape*) chainShape;
			
	m_eShape = eManual;
	ReplacePhysicsShape( pOldShape, *ppShapePtr );
	RecalcColRadius();
}

void cSprite::SetShapeChain( uint32_t numPoints, uint32_t index, int loop, float x, float y, int shapeID )
{
	if ( m_iPolygonPointsNum < numPoints )
	{
		if ( m_fPolygonPointsTemp ) delete [] m_fPolygonPointsTemp;
		m_fPolygonPointsTemp = new float[ numPoints*2 ];
		for ( uint32_t i = 0; i < numPoints*2; i++ ) m_fPolygonPointsTemp[ i ] = 0;
		m_iPolygonPointsNum = numPoints;
	}

	if ( index >= numPoints )
	{
#ifdef _AGK_ERROR_CHECK
		agk::Error( uString("Could not add point to chain shape, index must be less than the number of points. index starts at 0.") );
#endif
		return;
	}

	m_fPolygonPointsTemp[ index*2 ] = x;
	m_fPolygonPointsTemp[ index*2 + 1 ] = y;

	if ( numPoints == index+1 ) 
	{
		SetShapeChain( numPoints, m_fPolygonPointsTemp, loop, shapeID );
		delete [] m_fPolygonPointsTemp;
		m_fPolygonPointsTemp = 0;
		m_iPolygonPointsNum = 0;
	}
}

void cSprite::AddShapeInternal( b2Shape* pShape )
{
	if ( m_iNumAdditionalShapes > 0 )
	{
		b2Shape **pNewShapes = new b2Shape*[ m_iNumAdditionalShapes+1 ];
		for( uint32_t i = 0; i < m_iNumAdditionalShapes; i++ ) pNewShapes[i] = m_phyAdditionalShapes[i];
		delete [] m_phyAdditionalShapes;
		m_phyAdditionalShapes = pNewShapes;
	}
	else
	{
		m_phyAdditionalShapes = new b2Shape*[1];
	}
	m_phyAdditionalShapes[ m_iNumAdditionalShapes ] = pShape;
	m_iNumAdditionalShapes++;
}

// add shapes
void cSprite::AddShapeBox( float x, float y, float x2, float y2, float angle )
{
	if ( x2 < x )
	{
		// swap
		float temp = x;
		x = x2; 
		x2 = temp;
	}

	if ( y2 < y )
	{
		// swap
		float temp = y;
		y = y2; 
		y2 = temp;
	}

	float width = x2 - x;
	float height = y2 - y;
	float posX = (x+x2)/2.0f;// - m_fOffsetX;
	float posY = (y+y2)/2.0f;// - m_fOffsetY;

	b2PolygonShape *polyShape = new b2PolygonShape();
	polyShape->SetAsBox( agk::WorldToPhyX(width/2.0f), agk::WorldToPhyY(height/2.0f), b2Vec2(agk::WorldToPhyX(posX),agk::WorldToPhyY(posY)), angle );

	AddShapeInternal( polyShape );
	ReplacePhysicsShape( 0, polyShape );
	RecalcColRadius();
}

void cSprite::AddShapeCircle( float x, float y, float radius )
{
	b2CircleShape *circleShape = new b2CircleShape();
	circleShape->m_p.Set( agk::WorldToPhyX(x), agk::WorldToPhyY(y) );
	circleShape->m_radius = agk::WorldToPhyX(radius);
		
	AddShapeInternal( circleShape );
	ReplacePhysicsShape( 0, circleShape );
	RecalcColRadius();
}

void cSprite::AddShapePolygon( uint32_t numPoints, float *pPoints )
{
	if ( !pPoints ) return;
	if ( numPoints < 2 ) return;
	if ( numPoints > b2_maxPolygonVertices ) 
	{
#ifdef _AGK_ERROR_CHECK
		agk::Error( "Too many points for a physics polygon" );
#endif
		return;
	}

	b2Vec2 *vertices = new b2Vec2[ numPoints ];
	for ( uint32_t i = 0; i < numPoints; i++ )
	{
		vertices[ i ].x = agk::WorldToPhyX( pPoints[ i*2 ] );
		vertices[ i ].y = agk::WorldToPhyY( pPoints[ i*2 + 1 ] );
	}

	b2PolygonShape *polyShape = new b2PolygonShape();
	polyShape->Set( vertices, (int)numPoints );

	AddShapeInternal( polyShape );
	ReplacePhysicsShape( 0, polyShape );
	RecalcColRadius();
}

void cSprite::AddShapePolygon( uint32_t numPoints, uint32_t index, float x, float y )
{
	if ( numPoints > b2_maxPolygonVertices )
	{
#ifdef _AGK_ERROR_CHECK
		uString errStr;
		errStr.Format( "Could not add polygon point, polygon shapes have a maximum of %d points", b2_maxPolygonVertices );
		agk::Error( errStr );
#endif
		return;
	}

	if ( m_iPolygonPointsNum < numPoints )
	{
		if ( m_fPolygonPointsTemp ) delete [] m_fPolygonPointsTemp;
		m_fPolygonPointsTemp = new float[ numPoints*2 ];
		for ( uint32_t i = 0; i < numPoints*2; i++ ) m_fPolygonPointsTemp[ i ] = 0;
		m_iPolygonPointsNum = numPoints;
	}

	if ( index >= numPoints )
	{
#ifdef _AGK_ERROR_CHECK
		agk::Error( uString("Could not add point to polygon shape, index must be less than the number of points. index starts at 0.") );
#endif
		return;
	}

	m_fPolygonPointsTemp[ index*2 ] = x;
	m_fPolygonPointsTemp[ index*2 + 1 ] = y;

	if ( numPoints == index+1 ) 
	{
		AddShapePolygon( numPoints, m_fPolygonPointsTemp );
		delete [] m_fPolygonPointsTemp;
		m_fPolygonPointsTemp = 0;
		m_iPolygonPointsNum = 0;
	}
}

void cSprite::AddShapeChain( uint32_t numPoints, float *pPoints, int loop )
{
	if ( !pPoints ) return;
	if ( numPoints < 2 ) return;
	
	b2Vec2 *vertices = new b2Vec2[ numPoints ];
	for ( uint32_t i = 0; i < numPoints; i++ )
	{
		vertices[ i ].x = agk::WorldToPhyX( pPoints[ i*2 ] );
		vertices[ i ].y = agk::WorldToPhyY( pPoints[ i*2 + 1 ] );
	}

	b2ChainShape *chainShape = new b2ChainShape();
	if ( loop == 1 ) chainShape->CreateLoop( vertices, (int)numPoints );
	else chainShape->CreateChain( vertices, (int)numPoints );

	AddShapeInternal( chainShape );
	ReplacePhysicsShape( 0, chainShape );
	RecalcColRadius();
}

void cSprite::AddShapeChain( uint32_t numPoints, uint32_t index, int loop, float x, float y )
{
	if ( m_iPolygonPointsNum < numPoints )
	{
		if ( m_fPolygonPointsTemp ) delete [] m_fPolygonPointsTemp;
		m_fPolygonPointsTemp = new float[ numPoints*2 ];
		for ( uint32_t i = 0; i < numPoints*2; i++ ) m_fPolygonPointsTemp[ i ] = 0;
		m_iPolygonPointsNum = numPoints;
	}

	if ( index >= numPoints )
	{
#ifdef _AGK_ERROR_CHECK
		agk::Error( uString("Could not add point to chain shape, index must be less than the number of points. index starts at 0.") );
#endif
		return;
	}

	m_fPolygonPointsTemp[ index*2 ] = x;
	m_fPolygonPointsTemp[ index*2 + 1 ] = y;

	if ( numPoints == index+1 ) 
	{
		AddShapeChain( numPoints, m_fPolygonPointsTemp, loop );
		delete [] m_fPolygonPointsTemp;
		m_fPolygonPointsTemp = 0;
		m_iPolygonPointsNum = 0;
	}
}

void cSprite::ClearAdditionalShapes()
{
	if ( m_phyBody )
	{
		// destory additional fixtures
		b2Fixture* pFix = m_phyBody->GetFixtureList();
		b2Fixture *pNext = 0;
		while ( pFix )
		{
			pNext = pFix->GetNext();

			if ( pFix->GetShape() != m_phyShape ) m_phyBody->DestroyFixture( pFix );
				
			pFix = pNext;
		}
	}

	for( uint32_t i = 0; i < m_iNumAdditionalShapes; i++ )
	{
		if( m_phyAdditionalShapes[i] ) delete m_phyAdditionalShapes[i];
	}
	if ( m_phyAdditionalShapes ) delete [] m_phyAdditionalShapes;
	m_iNumAdditionalShapes = 0;
	m_phyAdditionalShapes = 0;

	RecalcColRadius();
}

int cSprite::GetShapeNumVertices( int shapeID )
{
	if ( shapeID > (int)m_iNumAdditionalShapes )
	{
		uString err; err.Format( "Cannot get shape vertex, shapeID %d does not exist", shapeID+1 );
		agk::Error( err );
		return 0;
	}

	const b2Shape *pShape = m_phyShape;
	if ( shapeID > 0 ) pShape = m_phyAdditionalShapes[ shapeID-1 ];

	if ( pShape == 0 ) return 0;
	if ( pShape->GetType() == b2Shape::e_polygon )
	{
		const b2PolygonShape *pPoly = (b2PolygonShape*)pShape;
		return pPoly->GetVertexCount();
	}
	else if ( pShape->GetType() == b2Shape::e_chain ) 
	{
		const b2ChainShape *pChain = (b2ChainShape*)pShape;
		return pChain->GetVertexCount();
	}
	else return 0;
}

float cSprite::GetShapeVertexX( int shapeID, int vertex )
{
	if ( shapeID > (int)m_iNumAdditionalShapes )
	{
		uString err; err.Format( "Cannot get shape vertex, shapeID %d does not exist", shapeID+1 );
		agk::Error( err );
		return 0;
	}

	const b2Shape *pShape = m_phyShape;
	if ( shapeID > 0 ) pShape = m_phyAdditionalShapes[ shapeID-1 ];

	if ( pShape == 0 ) return 0;
	if ( pShape->GetType() == b2Shape::e_polygon )
	{
		const b2PolygonShape *pPoly = (b2PolygonShape*)pShape;
		if ( vertex < 0 || vertex >= pPoly->GetVertexCount() )
		{
			uString err; err.Format( "Cannot get shape vertex, vertex %d does not exist", vertex+1 );
			agk::Error( err );
			return 0;
		}
		return agk::PhyToWorldX( pPoly->GetVertex( vertex ).x );
	}
	else if ( pShape->GetType() == b2Shape::e_chain ) 
	{
		const b2ChainShape *pChain = (b2ChainShape*)pShape;
		if ( vertex < 0 || vertex >= pChain->GetVertexCount() )
		{
			uString err; err.Format( "Cannot get shape vertex, vertex %d does not exist", vertex+1 );
			agk::Error( err );
			return 0;
		}
		return agk::PhyToWorldX( pChain->GetVertex( vertex ).x );
	}
	else return 0;	
}

float cSprite::GetShapeVertexY( int shapeID, int vertex )
{
	if ( shapeID > (int)m_iNumAdditionalShapes )
	{
		uString err; err.Format( "Cannot get shape vertex, shapeID %d does not exist", shapeID+1 );
		agk::Error( err );
		return 0;
	}

	const b2Shape *pShape = m_phyShape;
	if ( shapeID > 0 ) pShape = m_phyAdditionalShapes[ shapeID-1 ];

	if ( pShape == 0 ) return 0;
	if ( pShape->GetType() == b2Shape::e_polygon )
	{
		const b2PolygonShape *pPoly = (b2PolygonShape*)pShape;
		if ( vertex < 0 || vertex >= pPoly->GetVertexCount() )
		{
			uString err; err.Format( "Cannot get shape vertex, vertex %d does not exist", vertex+1 );
			agk::Error( err );
			return 0;
		}
		return agk::PhyToWorldY( pPoly->GetVertex( vertex ).y );
	}
	else if ( pShape->GetType() == b2Shape::e_chain ) 
	{
		const b2ChainShape *pChain = (b2ChainShape*)pShape;
		if ( vertex < 0 || vertex >= pChain->GetVertexCount() )
		{
			uString err; err.Format( "Cannot get shape vertex, vertex %d does not exist", vertex+1 );
			agk::Error( err );
			return 0;
		}
		return agk::PhyToWorldY( pChain->GetVertex( vertex ).y );
	}
	else return 0;	
}

// physics setters
void cSprite::SetPhysicsOn( ePhysicsMode mode )
{
	if ( m_phyBody ) 
	{
		m_phyBody->SetActive( true );
		return;
	}
		
	if ( m_phyShape == 0 ) 
	{
		if ( m_eShape == eNone || m_eShape == eManual ) SetShape( eBox );
		else SetShape( m_eShape );
	}

	b2BodyDef bodyDef;
	switch( mode )
	{
		case cSprite::eDynamic: bodyDef.type = b2_dynamicBody; break;
		case cSprite::eStatic: bodyDef.type = b2_staticBody; break;
		case cSprite::eKinematic: bodyDef.type = b2_kinematicBody; break;
#ifdef _AGK_ERROR_CHECK
		default: agk::Error( "Unrecognised physics mode for sprite in SetPhysicsOn()" );
#endif
		return;
	}

	bodyDef.position.Set( agk::WorldToPhyX(m_fX), agk::WorldToPhyY(m_fY) );
	bodyDef.angle = m_fAngle;
	bodyDef.userData = (void*) this;
	bodyDef.gravityScale = m_fGravityScale;
	bodyDef.awake = m_bAwake;
	bodyDef.allowSleep = m_bSleep;

	//bodyDef.linearVelocity.x = 10.0f;
	//bodyDef.angularVelocity = 4.0f;
	m_phyBody = agk::m_phyWorld->CreateBody( &bodyDef );
		
	if ( m_phyShape )
	{
		b2FixtureDef fixtureDef;
		fixtureDef.shape = m_phyShape;
		fixtureDef.localShapeMemory = false;
		fixtureDef.density = 1.0f;
		fixtureDef.friction = 0.3f;
		fixtureDef.restitution = 0.1f;
		fixtureDef.isSensor = 0;
		fixtureDef.filter.categoryBits = m_iCategories & 0x0000ffff;
		fixtureDef.filter.maskBits = m_iCategoryMask & 0x0000ffff;
		fixtureDef.filter.groupIndex = m_iGroup;
		m_phyBody->CreateFixture( &fixtureDef );

		for( uint32_t i = 0; i < m_iNumAdditionalShapes; i++ )
		{
			fixtureDef.shape = m_phyAdditionalShapes[i];
			m_phyBody->CreateFixture( &fixtureDef );
		}
	}

	b2MassData mass;
	mass.center.Set( 0,0 );
	mass.mass = m_phyBody->GetMass();
	mass.I = m_phyBody->GetInertia();
	m_phyBody->SetMassData( &mass );
}


void cSprite::SetPhysicsGravityScale ( float scale )
{
	// can set it before or after creation
	m_fGravityScale = scale;

	if ( !m_phyBody )
		return;

	m_phyBody->SetGravityScale ( scale );
}

float cSprite::GetPhysicsGravityScale ( void )
{
	return m_fGravityScale;
}

void cSprite::SetPhysicsInitiallyAwake ( bool awake )
{
	m_bAwake = awake;
}

void cSprite::SetPhysicsAllowSleep ( bool sleep )
{
	// can set it before or after creation
	m_bSleep = sleep;

	if ( !m_phyBody )
		return;

	m_phyBody->SetSleepingAllowed ( sleep );
}

float cSprite::GetInertia ( void )
{
	if ( !m_phyBody )
		return 0.0f;

	return m_phyBody->GetInertia ( );
}

bool cSprite::GetIsAwake ( void )
{
	if ( !m_phyBody )
		return false;

	return m_phyBody->IsAwake ( );
}

void cSprite::PrepareToClearPhysicsContacts()
{
	agk::PrepareToDeleteSpriteContacts( this );

	// check if any other sprites are iterating on these contacts
	b2ContactEdge* ce = m_phyBody->m_contactList;
	while (ce)
	{
		cSprite* pOther = (cSprite*) ce->other->GetUserData();
		if ( pOther )
		{
			while ( pOther->m_pContactIter && pOther->m_pContactIter->other == m_phyBody ) 
			{
				// sprite is iterating and this contact is going to get deleted so move it along
				pOther->m_pContactIter = pOther->m_pContactIter->next;
			}
		}
		ce = ce->next;
	}

	if ( m_pContactIter ) m_pContactIter = 0;
}

void cSprite::SetPhysicsOff( )
{
	if ( !m_phyBody ) return;

	PrepareToClearPhysicsContacts();
	m_phyBody->SetActive( false );
}

void cSprite::SetPhysicsDelete( )
{
	if ( !m_phyBody ) return;
		
	PrepareToClearPhysicsContacts();
	agk::m_phyWorld->DestroyBody( m_phyBody );
	m_phyBody = 0;
}

void cSprite::SetPhysicsFriction( float friction, int shapeID )
{
	if ( !m_phyBody ) return;
	if ( shapeID > (int)m_iNumAdditionalShapes ) 
	{
		uString err; err.Format( "Failed to set shape friction, shape %d does not exist", shapeID+1 );
		return;
	}

	b2Shape *pShape = m_phyShape;
	if ( shapeID > 0 ) pShape = m_phyAdditionalShapes[ shapeID-1 ];
		
	b2Fixture *pFix = m_phyBody->GetFixtureList();
	while ( pFix )
	{
		if ( shapeID < 0 || pFix->GetShape() == pShape ) 
		{
			pFix->SetFriction( friction );
			if ( shapeID >= 0 ) break;
		}
		pFix = pFix->GetNext();
	}

	for ( b2ContactEdge* contactEdge = m_phyBody->GetContactList(); contactEdge; contactEdge = contactEdge->next )
	{
		if ( shapeID < 0 
		  || contactEdge->contact->GetFixtureA() == pFix 
	      || contactEdge->contact->GetFixtureB() == pFix ) 
		{
			contactEdge->contact->ResetFriction();
		}
	}
}

void cSprite::SetPhysicsRestitution( float restitution, int shapeID )
{
	if ( !m_phyBody ) return;
	if ( shapeID > (int)m_iNumAdditionalShapes ) 
	{
		uString err; err.Format( "Failed to set shape restitution, shape %d does not exist", shapeID+1 );
		return;
	}

	b2Shape *pShape = m_phyShape;
	if ( shapeID > 0 ) pShape = m_phyAdditionalShapes[ shapeID-1 ];

	b2Fixture *pFix = m_phyBody->GetFixtureList();
	while ( pFix )
	{
		if ( shapeID < 0 || pFix->GetShape() == pShape ) 
		{
			pFix->SetRestitution( restitution );
			if ( shapeID >= 0 ) break;
		}
		pFix = pFix->GetNext();
	}

	for ( b2ContactEdge* contactEdge = m_phyBody->GetContactList(); contactEdge; contactEdge = contactEdge->next )
	{
		if ( shapeID < 0 
		  || contactEdge->contact->GetFixtureA() == pFix 
	      || contactEdge->contact->GetFixtureB() == pFix ) 
		{
			contactEdge->contact->ResetRestitution();
		}
	}
}

void cSprite::SetPhysicsDensity( float density, int shapeID )
{
	if ( !m_phyBody ) return;
	if ( shapeID > (int)m_iNumAdditionalShapes ) 
	{
		uString err; err.Format( "Failed to set shape density, shape %d does not exist", shapeID+1 );
		return;
	}

	b2Shape *pShape = m_phyShape;
	if ( shapeID > 0 ) pShape = m_phyAdditionalShapes[ shapeID-1 ];

	b2Fixture *pFix = m_phyBody->GetFixtureList();
	while ( pFix )
	{
		if ( shapeID < 0 || pFix->GetShape() == pShape ) 
		{
			pFix->SetDensity( density );
			if ( shapeID >= 0 ) break;
		}
		pFix = pFix->GetNext();
	}

	m_phyBody->ResetMassData();
}

void cSprite::SetPhysicsCanRotate( bool rotate )
{
	if ( !m_phyBody ) return;

	// command names indicate negative of each other
	m_phyBody->SetFixedRotation( !rotate );
}

void cSprite::SetPhysicsVelocity( float vx, float vy )
{
	if ( !m_phyBody ) return;

	m_phyBody->SetLinearVelocity( b2Vec2( agk::WorldToPhyX(vx), agk::WorldToPhyY(vy) ) );
}

void cSprite::SetPhysicsAngularVelocity( float va )
{
	if ( !m_phyBody ) return;

	m_phyBody->SetAngularVelocity( va );
}

void cSprite::SetPhysicsDamping( float damp )
{
	if ( !m_phyBody ) return;

	m_phyBody->SetLinearDamping( damp );
}

void cSprite::SetPhysicsAngularDamping( float damp )
{
	if ( !m_phyBody ) return;

	m_phyBody->SetAngularDamping( damp );
}

void cSprite::SetPhysicsIsBullet( bool bullet )
{
	if ( !m_phyBody ) return;

	m_phyBody->SetBullet( bullet );
}

bool cSprite::GetIsBullet ( void )
{
	if ( !m_phyBody ) return false;

	return m_phyBody->IsBullet ( );
}

void cSprite::SetPhysicsMass( float mass )
{
	if ( !m_phyBody ) return;

	b2MassData massData;
	m_phyBody->GetMassData( &massData );
	massData.mass = mass;
	m_phyBody->SetMassData( &massData );
}

void cSprite::SetPhysicsIsSensor( bool sensor, int shapeID )
{
	if ( !m_phyBody ) return;
	if ( shapeID > (int)m_iNumAdditionalShapes ) 
	{
		uString err; err.Format( "Failed to set shape sensor, shape %d does not exist", shapeID+1 );
		return;
	}

	b2Shape *pShape = m_phyShape;
	if ( shapeID > 0 ) pShape = m_phyAdditionalShapes[ shapeID-1 ];

	b2Fixture *pFix = m_phyBody->GetFixtureList();
	while ( pFix )
	{
		if ( shapeID < 0 || pFix->GetShape() == pShape ) pFix->SetSensor( sensor );
		pFix = pFix->GetNext();
	}
}

float cSprite::GetPhysicsVelocityX()
{
	if ( !m_phyBody ) return 0;

	return agk::PhyToWorldX( m_phyBody->GetLinearVelocity().x );
}

float cSprite::GetPhysicsVelocityY()
{
	if ( !m_phyBody ) return 0;

	return agk::PhyToWorldY( m_phyBody->GetLinearVelocity().y );
}

float cSprite::GetPhysicsAngularVelocity()
{
	if ( !m_phyBody ) return 0;

	return m_phyBody->GetAngularVelocity();
}

float cSprite::GetPhysicsMass( )
{
	if ( !m_phyBody ) return 0;

	b2MassData massData;
	m_phyBody->GetMassData( &massData );
	return massData.mass;
}

void cSprite::SetGroup( int group, int shapeID )
{
	if ( shapeID <= 0 ) m_iGroup = group;

	if ( !m_phyBody ) return;
	if ( shapeID > (int)m_iNumAdditionalShapes ) 
	{
		uString err; err.Format( "Failed to set shape group, shape %d does not exist", shapeID+1 );
		return;
	}

	b2Shape *pShape = m_phyShape;
	if ( shapeID > 0 ) pShape = m_phyAdditionalShapes[ shapeID-1 ];

	if ( group < -32767 ) group = 0;
	if ( group > 32767 ) group = 0;

	b2Fixture *pFix = m_phyBody->GetFixtureList();
	while ( pFix )
	{
		if ( shapeID < 0 || pFix->GetShape() == pShape )
		{
			const b2Filter pFilter = pFix->GetFilterData();
			
			b2Filter pNewFilter;
			pNewFilter.categoryBits = pFilter.categoryBits;
			pNewFilter.maskBits = pFilter.maskBits;
			pNewFilter.groupIndex = group;
			pFix->SetFilterData( pNewFilter );
		}

		pFix = pFix->GetNext();
	}
}

void cSprite::SetCategoryBits( uint32_t categories, int shapeID )
{
	if ( shapeID <= 0 ) m_iCategories = categories;

	if ( !m_phyBody ) return;
	if ( shapeID > (int)m_iNumAdditionalShapes ) 
	{
		uString err; err.Format( "Failed to set shape category bits, shape %d does not exist", shapeID+1 );
		return;
	}

	b2Shape *pShape = m_phyShape;
	if ( shapeID > 0 ) pShape = m_phyAdditionalShapes[ shapeID-1 ];

	b2Fixture *pFix = m_phyBody->GetFixtureList();
	while ( pFix )
	{
		if ( shapeID < 0 || pFix->GetShape() == pShape )
		{
			const b2Filter pFilter = pFix->GetFilterData();
			
			b2Filter pNewFilter;
			pNewFilter.categoryBits = categories & 0x0000ffff;
			pNewFilter.maskBits = pFilter.maskBits;
			pNewFilter.groupIndex = pFilter.groupIndex;
			pFix->SetFilterData( pNewFilter );
		}

		pFix = pFix->GetNext();
	}
}

void cSprite::SetCategoryBit( uint32_t category, int flag, int shapeID )
{
	if ( category == 0 ) return;
	if ( category > 16 ) return;

	int mask = 1 << (category-1);
	if ( shapeID <= 0 ) 
	{
		if ( flag ) m_iCategories |= mask;
		else m_iCategories &= ~mask;
	}
		
	if ( !m_phyBody ) return;
	if ( shapeID > (int)m_iNumAdditionalShapes ) 
	{
		uString err; err.Format( "Failed to set shape category bit, shape %d does not exist", shapeID+1 );
		return;
	}

	b2Shape *pShape = m_phyShape;
	if ( shapeID > 0 ) pShape = m_phyAdditionalShapes[ shapeID-1 ];

	b2Fixture *pFix = m_phyBody->GetFixtureList();
	while ( pFix )
	{
		if ( shapeID < 0 || pFix->GetShape() == pShape )
		{
			const b2Filter pFilter = pFix->GetFilterData();
			b2Filter pNewFilter;

			if ( flag ) pNewFilter.categoryBits = pFilter.categoryBits | mask;
			else pNewFilter.categoryBits = pFilter.categoryBits & ~mask;
			
			pNewFilter.maskBits = pFilter.maskBits;
			pNewFilter.groupIndex = pFilter.groupIndex;
			pFix->SetFilterData( pNewFilter );
		}

		pFix = pFix->GetNext();
	}
}

void cSprite::SetCollideBits( uint32_t mask, int shapeID )
{
	if ( shapeID <= 0 ) m_iCategoryMask = mask;

	if ( !m_phyBody ) return;
	if ( shapeID > (int)m_iNumAdditionalShapes ) 
	{
		uString err; err.Format( "Failed to set shape collide bits, shape %d does not exist", shapeID+1 );
		return;
	}

	b2Shape *pShape = m_phyShape;
	if ( shapeID > 0 ) pShape = m_phyAdditionalShapes[ shapeID-1 ];

	b2Fixture *pFix = m_phyBody->GetFixtureList();
	while ( pFix )
	{
		if ( shapeID < 0 || pFix->GetShape() == pShape )
		{
			const b2Filter pFilter = pFix->GetFilterData();
			
			b2Filter pNewFilter;
			pNewFilter.categoryBits = pFilter.categoryBits;
			pNewFilter.maskBits = mask & 0x0000ffff;
			pNewFilter.groupIndex = pFilter.groupIndex;
			pFix->SetFilterData( pNewFilter );
		}

		pFix = pFix->GetNext();
	}
}

void cSprite::SetCollideBit( uint32_t category, int flag, int shapeID )
{
	if ( category == 0 ) return;
	if ( category > 16 ) return;

	int mask = 1 << (category-1);
	if ( shapeID <= 0 ) 
	{
		if ( flag ) m_iCategoryMask |= mask;
		else m_iCategoryMask &= ~mask;
	}

	if ( !m_phyBody ) return;
	if ( shapeID > (int)m_iNumAdditionalShapes ) 
	{
		uString err; err.Format( "Failed to set shape collide bit, shape %d does not exist", shapeID+1 );
		return;
	}

	b2Shape *pShape = m_phyShape;
	if ( shapeID > 0 ) pShape = m_phyAdditionalShapes[ shapeID-1 ];

	b2Fixture *pFix = m_phyBody->GetFixtureList();
	while ( pFix )
	{
		if ( shapeID < 0 || pFix->GetShape() == pShape )
		{
			const b2Filter pFilter = pFix->GetFilterData();
			b2Filter pNewFilter;
			
			if ( flag ) pNewFilter.maskBits = pFilter.maskBits | mask;
			else pNewFilter.maskBits = pFilter.maskBits & ~mask;

			pNewFilter.categoryBits = pFilter.categoryBits;
			pNewFilter.groupIndex = pFilter.groupIndex;
			pFix->SetFilterData( pNewFilter );
		}

		pFix = pFix->GetNext();
	}
}

void cSprite::SetPhysicsForce( float x, float y, float vx, float vy )
{
	if ( !m_phyBody ) return;

	x = agk::WorldToPhyX( x );
	y = agk::WorldToPhyY( y );
	vx = agk::WorldToPhyX( vx );
	vy = agk::WorldToPhyY( vy );

	m_phyBody->ApplyForce( b2Vec2(vx,vy), b2Vec2(x,y), true );
}

void cSprite::SetPhysicsTorque( float a )
{
	if ( !m_phyBody ) return;

	m_phyBody->ApplyTorque( a, true );
}

void cSprite::SetPhysicsLinearImpulse( float x, float y, float vx, float vy )
{
	if ( !m_phyBody ) return;

	x = agk::WorldToPhyX( x );
	y = agk::WorldToPhyY( y );
	vx = agk::WorldToPhyX( vx );
	vy = agk::WorldToPhyY( vy );

	m_phyBody->ApplyLinearImpulse( b2Vec2(vx,vy), b2Vec2(x,y), true );
}

void cSprite::SetPhysicsAngularImpulse( float a )
{
	if ( !m_phyBody ) return;

	m_phyBody->ApplyAngularImpulse( a, true );
}

int cSprite::GetFirstContact()
{
	if ( !m_phyBody ) return 0;
	m_pContactIter = m_phyBody->GetContactList();
	while ( m_pContactIter && !m_pContactIter->contact->IsTouching() ) m_pContactIter = m_pContactIter->next;
	return m_pContactIter ? 1 : 0;
}

int cSprite::GetNextContact()
{
	if ( !m_pContactIter ) return 0;
	m_pContactIter = m_pContactIter->next;
	while ( m_pContactIter && !m_pContactIter->contact->IsTouching() ) m_pContactIter = m_pContactIter->next;
	return m_pContactIter ? 1 : 0;
}

float cSprite::GetContactWorldX()
{
	if ( !m_pContactIter ) return 0;
	b2WorldManifold manifold;
	m_pContactIter->contact->GetWorldManifold(&manifold);
	return agk::PhyToWorldX(manifold.points[0].x);
}

float cSprite::GetContactWorldY()
{
	if ( !m_pContactIter ) return 0;
	b2WorldManifold manifold;
	m_pContactIter->contact->GetWorldManifold(&manifold);
	return agk::PhyToWorldY(manifold.points[0].y);
}

cSprite* cSprite::GetContactSprite2()
{
	if ( !m_pContactIter ) return 0;
	cSprite *pSprite = (cSprite*) m_pContactIter->contact->GetFixtureA()->GetBody()->GetUserData();
	if ( pSprite == this ) pSprite = (cSprite*) m_pContactIter->contact->GetFixtureB()->GetBody()->GetUserData();
	return pSprite;
}

void cSprite::UpdatePhysics()
{
	m_lastContact = 0;

	if ( !m_phyBody ) return;

	b2Vec2 position = m_phyBody->GetPosition();
	float32 angle = m_phyBody->GetAngle();

	m_fX = agk::PhyToWorldX(position.x);
	m_fY = agk::PhyToWorldY(position.y);
	m_fAngle = angle;
}
