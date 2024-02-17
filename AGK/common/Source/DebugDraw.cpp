#include "DebugDraw.h"
#include "agk.h"

using namespace AGK;

DebugDraw::DebugDraw()
{
	m_pShader = AGKShader::g_pShaderColor;
	UpdateVertexLayout( &m_triVertexLayout, m_pShader );
	UpdateVertexLayout( &m_lineVertexLayout, m_pShader );

	m_renderState.SetDepthEnable( 0 );
	m_renderState.SetCullMode( AGK_CULL_NONE );
	m_renderState.SetBlendEnable( 1 );
	m_renderState.SetBlendFunc( AGK_BLEND_SRC_ALPHA, AGK_BLEND_ONE_MINUS_SRC_ALPHA );

	m_triVertexLayout.m_iPrimitiveType = AGK_PRIMITIVE_TRIANGLES;
	m_lineVertexLayout.m_iPrimitiveType = AGK_PRIMITIVE_LINES;

	ExpandTriVertexArray( 256 );
	ExpandTriIndexArray( 1024 );

	ExpandLineVertexArray( 256 );
	ExpandLineIndexArray( 512 );
}

DebugDraw::~DebugDraw()
{
	if ( m_pTriVertexData ) delete [] m_pTriVertexData;
	if ( m_pTriIndices ) delete [] m_pTriIndices;
	if ( m_pLineVertexData ) delete [] m_pLineVertexData;
	if ( m_pLineIndices ) delete [] m_pLineIndices;

	Renderer *pRenderer = agk::GetRenderer();
	if ( !pRenderer ) return;

	if ( m_pInternalTriVertices ) pRenderer->DeleteVertices( m_pInternalTriVertices );
	if ( m_pInternalLineVertices ) pRenderer->DeleteVertices( m_pInternalLineVertices );
	if ( m_pInternalTriPipeline ) pRenderer->DeletePipeline( m_pInternalTriPipeline );
	if ( m_pInternalLinePipeline ) pRenderer->DeletePipeline( m_pInternalLinePipeline );
	if ( m_pInternalTriConstants ) pRenderer->DeleteShaderConstants( m_pInternalTriConstants );
	if ( m_pInternalLineConstants ) pRenderer->DeleteShaderConstants( m_pInternalLineConstants );
}

int DebugDraw::UpdateVertexLayout( AGKVertexLayout *pLayout, AGKShader *pShader )
{
	if ( !pLayout ) return 1;
	if ( !pShader ) return 1;

	int error = 0;

	int numShaderAttribs = pShader->GetNumAttributes();
	pLayout->SetNumOffsets( numShaderAttribs );
	
	pLayout->m_iVertexSize = (uint16_t) GetVertexNumFloats() * sizeof(float); // pos + color

	for( int i = 0; i < numShaderAttribs; i++ )
	{
		AGKShaderAttrib *pShaderAttrib = pShader->GetAttribute( i );
		int offset = -1;
		switch( pShaderAttrib->m_iKnownName )
		{
			case AGK_SHADER_ATTRIB_NAME_POS:	offset = GetVertexPositionOffset() * sizeof(float); break;
			case AGK_SHADER_ATTRIB_NAME_COLOR:	offset = GetVertexColorOffset() * sizeof(float); break;
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

void DebugDraw::SetShader( AGKShader *pShader )
{
	if ( pShader == m_pShader ) return;

	m_pShader = pShader;
	UpdateVertexLayout( &m_triVertexLayout, pShader );
	UpdateVertexLayout( &m_lineVertexLayout, pShader );

	m_triVertexLayout.m_iPrimitiveType = AGK_PRIMITIVE_TRIANGLES;
	m_lineVertexLayout.m_iPrimitiveType = AGK_PRIMITIVE_LINES;
}

void DebugDraw::ExpandTriVertexArray( uint32_t newSize )
{
	if ( newSize <= m_iMaxNumTriVertices ) return;

	m_iMaxNumTriVertices = newSize + newSize/2;
	uint32_t iVertexFloats = GetVertexNumFloats();
	float* newVertices = new float[ m_iMaxNumTriVertices * iVertexFloats ];

	if ( m_pTriVertexData )
	{
		memcpy( newVertices, m_pTriVertexData, m_iNumTriVertices * iVertexFloats * sizeof(float) );
		delete [] m_pTriVertexData;
	}

	m_pTriVertexData = newVertices;
}

void DebugDraw::ExpandTriIndexArray( uint32_t newSize )
{
	if ( newSize <= m_iMaxNumTriIndices ) return;

	m_iMaxNumTriIndices = newSize + newSize/2;
	uint32_t* newIndices = new uint32_t[ m_iMaxNumTriIndices ];

	if ( m_pTriIndices )
	{
		memcpy( newIndices, m_pTriIndices, m_iNumTriIndices * sizeof(uint32_t) );
		delete [] m_pTriIndices;
	}

	m_pTriIndices = newIndices;
}

void DebugDraw::ExpandLineVertexArray( uint32_t newSize )
{
	if ( newSize <= m_iMaxNumLineVertices ) return;

	m_iMaxNumLineVertices = newSize + newSize/2;
	uint32_t iVertexFloats = GetVertexNumFloats();
	float* newVertices = new float[ m_iMaxNumLineVertices * iVertexFloats ];

	if ( m_pLineVertexData )
	{
		memcpy( newVertices, m_pLineVertexData, m_iNumLineVertices * iVertexFloats * sizeof(float) );
		delete [] m_pLineVertexData;
	}

	m_pLineVertexData = newVertices;
}

void DebugDraw::ExpandLineIndexArray( uint32_t newSize )
{
	if ( newSize <= m_iMaxNumLineIndices ) return;

	m_iMaxNumLineIndices = newSize + newSize/2;
	uint32_t* newIndices = new uint32_t[ m_iMaxNumLineIndices ];

	if ( m_pLineIndices )
	{
		memcpy( newIndices, m_pLineIndices, m_iNumLineIndices * sizeof(uint32_t) );
		delete [] m_pLineIndices;
	}

	m_pLineIndices = newIndices;
}

void DebugDraw::BeginRender()
{
	m_iNumTriVertices = 0;
	m_iNumTriIndices = 0;
	m_iNumLineVertices = 0;
	m_iNumLineIndices = 0;
}

void DebugDraw::EndRender()
{
	Renderer *pRenderer = agk::GetRenderer();

	if ( m_iNumTriVertices > 0 )
	{
		if ( !m_pInternalTriVertices ) pRenderer->CreateVertices( m_triVertexLayout.m_iVertexSize, &m_pInternalTriVertices );
		pRenderer->SetVertices( m_pInternalTriVertices, m_pTriVertexData, m_iNumTriVertices );

		if ( pRenderer->SupportsIntIndices() )
		{
			pRenderer->SetIndicesInt( m_pInternalTriVertices, m_pTriIndices, m_iNumTriIndices );
		}
		else
		{
			uint16_t *shortIndices = new uint16_t[ m_iNumTriIndices ];
			for( uint32_t i = 0; i < m_iNumTriIndices; i++ ) shortIndices[ i ] = m_pTriIndices[ i ];
			pRenderer->SetIndices( m_pInternalTriVertices, shortIndices, m_iNumTriIndices );
			delete [] shortIndices;
		}

		pRenderer->BindPipeline( 0, m_pShader, &m_renderState, &m_triVertexLayout, &m_pInternalTriPipeline );
		pRenderer->BindShaderConstants( 0, m_pShader, 0, 0, 0, &m_pInternalTriConstants );
		pRenderer->DrawVertices( 0, m_pInternalTriVertices, m_iNumTriIndices );
	}

	if ( m_iNumLineVertices > 0 )
	{
		if ( !m_pInternalLineVertices ) pRenderer->CreateVertices( m_lineVertexLayout.m_iVertexSize, &m_pInternalLineVertices );
		pRenderer->SetVertices( m_pInternalLineVertices, m_pLineVertexData, m_iNumLineVertices );

		if ( pRenderer->SupportsIntIndices() )
		{
			pRenderer->SetIndicesInt( m_pInternalLineVertices, m_pLineIndices, m_iNumLineIndices );
		}
		else
		{
			uint16_t *shortIndices = new uint16_t[ m_iNumLineIndices ];
			for( uint32_t i = 0; i < m_iNumLineIndices; i++ ) shortIndices[ i ] = m_pLineIndices[ i ];
			pRenderer->SetIndices( m_pInternalLineVertices, shortIndices, m_iNumLineIndices );
			delete [] shortIndices;
		}

		pRenderer->BindPipeline( 0, m_pShader, &m_renderState, &m_lineVertexLayout, &m_pInternalLinePipeline );
		pRenderer->BindShaderConstants( 0, m_pShader, 0, 0, 0, &m_pInternalLineConstants );
		pRenderer->DrawVertices( 0, m_pInternalLineVertices, m_iNumLineIndices );
	}
}

void DebugDraw::DrawString(int x, int y, const char *string, ...)
{
	
}

void DebugDraw::DrawAABB(b2AABB* aabb, const b2Color& c)
{
	
}

void DebugDraw::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
	if ( !m_pShader ) return;

	float fMinX = 10000000;
	float fMinY = 10000000;
	float fMaxX = -10000000;
	float fMaxY = -10000000;

	for (int32 i = 0; i < vertexCount; ++i)
	{
		if ( vertices[i].x < fMinX ) fMinX = vertices[i].x;
		if ( vertices[i].y < fMinY ) fMinY = vertices[i].y;
		if ( vertices[i].x > fMaxX ) fMaxX = vertices[i].x;
		if ( vertices[i].y > fMaxY ) fMaxY = vertices[i].y;
	}

	// check if the polygon is outside the view
	fMinX = agk::WorldToScreenX( agk::PhyToWorldX( fMinX ) );
	if ( fMinX > agk::GetScreenBoundsRight() ) return;
	fMinY = agk::WorldToScreenY( agk::PhyToWorldY( fMinY ) );
	if ( fMinY > agk::GetScreenBoundsBottom() ) return;

	fMaxX = agk::WorldToScreenX( agk::PhyToWorldX( fMaxX ) );
	if ( fMaxX < agk::GetScreenBoundsLeft() ) return;
	fMaxY = agk::WorldToScreenY( agk::PhyToWorldY( fMaxY ) );
	if ( fMaxX < agk::GetScreenBoundsTop()  ) return;
	
	// check if we can draw more vertices
	if ( !agk::GetRenderer()->SupportsIntIndices() && m_iNumLineVertices + vertexCount > 65535 )
	{
		EndRender();
		BeginRender();
	}

	ExpandLineVertexArray( m_iNumLineVertices + vertexCount );
	ExpandLineIndexArray( m_iNumLineIndices + vertexCount*2 );

	uint32_t iVertexFloats = GetVertexNumFloats();
	float *pVertexPtr = m_pLineVertexData + (m_iNumLineVertices * iVertexFloats);
	uint32_t *pIndexPtr = m_pLineIndices + m_iNumLineIndices;
		
	uint32_t firstIndex = m_iNumLineVertices;
	for (int32 i = 0; i < vertexCount; ++i)
	{
		float x = agk::PhyToWorldX( vertices[ i ].x );
		float y = agk::PhyToWorldY( vertices[ i ].y );
		x = agk::WorldToScreenX( x );
		y = agk::WorldToScreenY( y );

		pVertexPtr[0] = x;
		pVertexPtr[1] = y;
		pVertexPtr[2] = 0;

		pVertexPtr[3] = color.r;
		pVertexPtr[4] = color.g;
		pVertexPtr[5] = color.b;
		pVertexPtr[6] = color.a;

		*pIndexPtr = m_iNumLineVertices;	
		pIndexPtr++;
		
		if ( i > 0 )
		{
			*pIndexPtr = m_iNumLineVertices;	
			pIndexPtr++;
		}

		m_iNumLineVertices++;
		pVertexPtr += iVertexFloats;
	}

	// loop back to first vertex
	*pIndexPtr = firstIndex;	
	m_iNumLineIndices += vertexCount * 2;
}

void DebugDraw::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
	if ( !m_pShader ) return;

	float fMinX = 10000000;
	float fMinY = 10000000;
	float fMaxX = -10000000;
	float fMaxY = -10000000;

	for (int32 i = 0; i < vertexCount; ++i)
	{
		if ( vertices[i].x < fMinX ) fMinX = vertices[i].x;
		if ( vertices[i].y < fMinY ) fMinY = vertices[i].y;
		if ( vertices[i].x > fMaxX ) fMaxX = vertices[i].x;
		if ( vertices[i].y > fMaxY ) fMaxY = vertices[i].y;
	}

	// check if the polygon is outside the view
	fMinX = agk::WorldToScreenX( agk::PhyToWorldX( fMinX ) );
	if ( fMinX > agk::GetScreenBoundsRight() ) return;
	fMinY = agk::WorldToScreenY( agk::PhyToWorldY( fMinY ) );
	if ( fMinY > agk::GetScreenBoundsBottom() ) return;

	fMaxX = agk::WorldToScreenX( agk::PhyToWorldX( fMaxX ) );
	if ( fMaxX < agk::GetScreenBoundsLeft() ) return;
	fMaxY = agk::WorldToScreenY( agk::PhyToWorldY( fMaxY ) );
	if ( fMaxY < agk::GetScreenBoundsTop()  ) return;

	// check if we can draw more vertices
	if ( !agk::GetRenderer()->SupportsIntIndices() && (m_iNumLineVertices + vertexCount > 65535 || m_iNumTriVertices + vertexCount > 65535) )
	{
		EndRender();
		BeginRender();
	}

	ExpandTriVertexArray( m_iNumTriVertices + vertexCount );
	ExpandTriIndexArray( m_iNumTriIndices + (vertexCount-2)*3 );

	ExpandLineVertexArray( m_iNumLineVertices + vertexCount );
	ExpandLineIndexArray( m_iNumLineIndices + vertexCount*2 );

	uint32_t iVertexFloats = GetVertexNumFloats();

	float *pTriVertexPtr = m_pTriVertexData + (m_iNumTriVertices * iVertexFloats);
	uint32_t *pTriIndexPtr = m_pTriIndices + m_iNumTriIndices;

	float *pLineVertexPtr = m_pLineVertexData + (m_iNumLineVertices * iVertexFloats);
	uint32_t *pLineIndexPtr = m_pLineIndices + m_iNumLineIndices;
	
	uint32_t firstLineIndex = m_iNumLineVertices;
	uint32_t firstTriIndex = m_iNumTriVertices;
	for (int32 i = 0; i < vertexCount; ++i)
	{
		float x = agk::PhyToWorldX( vertices[ i ].x );
		float y = agk::PhyToWorldY( vertices[ i ].y );
		x = agk::WorldToScreenX( x );
		y = agk::WorldToScreenY( y );

		// triangles
		pTriVertexPtr[0] = x;			
		pTriVertexPtr[1] = y;			
		pTriVertexPtr[2] = 0;			

		pTriVertexPtr[3] = color.r*0.5f;
		pTriVertexPtr[4] = color.g*0.5f;
		pTriVertexPtr[5] = color.b*0.5f;
		pTriVertexPtr[6] = color.a*0.5f;

		if ( i < 3 )
		{
			// first triangle
			*pTriIndexPtr = m_iNumTriVertices;
			pTriIndexPtr++;
		}
		else
		{
			pTriIndexPtr[0] = firstTriIndex;			
			pTriIndexPtr[1] = m_iNumTriVertices - 1;	
			pTriIndexPtr[2] = m_iNumTriVertices;		
			pTriIndexPtr += 3;
		}
		m_iNumTriVertices++;

		// lines
		pLineVertexPtr[0] = x;			
		pLineVertexPtr[1] = y;			
		pLineVertexPtr[2] = 0;			
		 
		pLineVertexPtr[3] = color.r;	
		pLineVertexPtr[4] = color.g;	
		pLineVertexPtr[5] = color.b;	
		pLineVertexPtr[6] = color.a;	

		*pLineIndexPtr = m_iNumLineVertices;	
		pLineIndexPtr++;
		
		if ( i > 0 )
		{
			*pLineIndexPtr = m_iNumLineVertices;	
			pLineIndexPtr++;
		}

		m_iNumLineVertices++;
		pTriVertexPtr += iVertexFloats;
		pLineVertexPtr += iVertexFloats;
	}
	
	m_iNumTriIndices += (vertexCount - 2) * 3;
	
	// loop back to first line vertex
	*pLineIndexPtr = firstLineIndex;	
	m_iNumLineIndices += vertexCount * 2;
}

void DebugDraw::DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color)
{
	if ( !m_pShader ) return;

	float fMinX = agk::WorldToScreenX( agk::PhyToWorldX(center.x - radius) );
	float fMinY = agk::WorldToScreenY( agk::PhyToWorldY(center.y - radius) );
	float fMaxX = agk::WorldToScreenX( agk::PhyToWorldX(center.x + radius) );
	float fMaxY = agk::WorldToScreenY( agk::PhyToWorldY(center.y + radius) );

	if ( fMaxX < agk::GetScreenBoundsLeft() || fMaxY < agk::GetScreenBoundsTop() ) return;
	if ( fMinX > agk::GetScreenBoundsRight() || fMinY > agk::GetScreenBoundsBottom() ) return;

	const uint32_t k_segments = 16;
	const float32 k_increment = 2.0f * b2_pi / k_segments;
	float32 theta = 0.0f;

	// check if we can draw more vertices
	if ( !agk::GetRenderer()->SupportsIntIndices() && m_iNumLineVertices + k_segments > 65535 )
	{
		EndRender();
		BeginRender();
	}

	ExpandLineVertexArray( m_iNumLineVertices + k_segments );
	ExpandLineIndexArray( m_iNumLineIndices + k_segments*2 );

	uint32_t iVertexFloats = GetVertexNumFloats();
	float *pVertexPtr = m_pLineVertexData + (m_iNumLineVertices * iVertexFloats);
	uint32_t *pIndexPtr = m_pLineIndices + m_iNumLineIndices;
		
	uint32_t firstIndex = m_iNumLineVertices;
	for (int32 i = 0; i < k_segments; ++i)
	{
		b2Vec2 v = center + radius * b2Vec2(cosf(theta), sinf(theta));

		float x = agk::WorldToScreenX( agk::PhyToWorldX( v.x ) );
		float y = agk::WorldToScreenY( agk::PhyToWorldY( v.y ) );

		pVertexPtr[0] = x;		
		pVertexPtr[1] = y;		
		pVertexPtr[2] = 0;		

		pVertexPtr[3] = color.r;
		pVertexPtr[4] = color.g;
		pVertexPtr[5] = color.b;
		pVertexPtr[6] = color.a;

		*pIndexPtr = m_iNumLineVertices;	
		pIndexPtr++;
		
		if ( i > 0 )
		{
			*pIndexPtr = m_iNumLineVertices;	
			pIndexPtr++;
		}

		m_iNumLineVertices++;
		theta += k_increment;
		pVertexPtr += iVertexFloats;
	}

	// loop back to first vertex
	*pIndexPtr = firstIndex;	
	m_iNumLineIndices += k_segments * 2;	
}

void DebugDraw::DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color)
{
	if ( !m_pShader ) return;

	float fMinX = agk::WorldToScreenX( agk::PhyToWorldX(center.x - radius) );
	float fMinY = agk::WorldToScreenY( agk::PhyToWorldY(center.y - radius) );
	float fMaxX = agk::WorldToScreenX( agk::PhyToWorldX(center.x + radius) );
	float fMaxY = agk::WorldToScreenY( agk::PhyToWorldY(center.y + radius) );

	if ( fMaxX < agk::GetScreenBoundsLeft() || fMaxY < agk::GetScreenBoundsTop() ) return;
	if ( fMinX > agk::GetScreenBoundsRight() || fMinY > agk::GetScreenBoundsBottom() ) return;

	const uint32_t k_segments = 16;
	const float32 k_increment = 2.0f * b2_pi / k_segments;
	float32 theta = 0.0f;

	// check if we can draw more vertices
	if ( !agk::GetRenderer()->SupportsIntIndices() && (m_iNumLineVertices + k_segments > 65535 || m_iNumTriVertices + k_segments > 65535) )
	{
		EndRender();
		BeginRender();
	}

	ExpandTriVertexArray( m_iNumTriVertices + k_segments );
	ExpandTriIndexArray( m_iNumTriIndices + (k_segments-2)*3 );

	ExpandLineVertexArray( m_iNumLineVertices + k_segments + 2 ); // + 2 for the center line
	ExpandLineIndexArray( m_iNumLineIndices + k_segments*2 + 2 ); // + 2 for the center line

	uint32_t iVertexFloats = GetVertexNumFloats();

	float *pTriVertexPtr = m_pTriVertexData + (m_iNumTriVertices * iVertexFloats);
	uint32_t *pTriIndexPtr = m_pTriIndices + m_iNumTriIndices;

	float *pLineVertexPtr = m_pLineVertexData + (m_iNumLineVertices * iVertexFloats);
	uint32_t *pLineIndexPtr = m_pLineIndices + m_iNumLineIndices;

	uint32_t firstLineIndex = m_iNumLineVertices;
	uint32_t firstTriIndex = m_iNumTriVertices;
	for (int32 i = 0; i < k_segments; ++i)
	{
		b2Vec2 v = center + radius * b2Vec2(cosf(theta), sinf(theta));

		float x = agk::WorldToScreenX( agk::PhyToWorldX( v.x ) );
		float y = agk::WorldToScreenY( agk::PhyToWorldY( v.y ) );
		
		// triangles
		pTriVertexPtr[0] = x;			
		pTriVertexPtr[1] = y;			
		pTriVertexPtr[2] = 0;			

		pTriVertexPtr[3] = color.r*0.5f;
		pTriVertexPtr[4] = color.g*0.5f;
		pTriVertexPtr[5] = color.b*0.5f;
		pTriVertexPtr[6] = color.a*0.5f;
		pTriVertexPtr += iVertexFloats;

		if ( i < 3 )
		{
			// first triangle
			*pTriIndexPtr = m_iNumTriVertices;
			pTriIndexPtr++;
		}
		else
		{
			pTriIndexPtr[0] = firstTriIndex;			
			pTriIndexPtr[1] = m_iNumTriVertices - 1;
			pTriIndexPtr[2] = m_iNumTriVertices;
			pTriIndexPtr += 3;
		}
		m_iNumTriVertices++;

		// lines
		pLineVertexPtr[0] = x;		
		pLineVertexPtr[1] = y;		
		pLineVertexPtr[2] = 0;		
		 
		pLineVertexPtr[3] = color.r;
		pLineVertexPtr[4] = color.g;
		pLineVertexPtr[5] = color.b;
		pLineVertexPtr[6] = color.a;
		pLineVertexPtr += iVertexFloats;

		*pLineIndexPtr = m_iNumLineVertices;	
		pLineIndexPtr++;
		
		if ( i > 0 )
		{
			*pLineIndexPtr = m_iNumLineVertices;	
			pLineIndexPtr++;
		}

		m_iNumLineVertices++;
		theta += k_increment;
	}
	
	m_iNumTriIndices += (k_segments - 2) * 3;
	
	// loop back to first line vertex
	*pLineIndexPtr = firstLineIndex;
	pLineIndexPtr++;
	m_iNumLineIndices += k_segments * 2;

	// draw the center line
	b2Vec2 p = center + radius * axis;
	pLineVertexPtr[0] = agk::WorldToScreenX( agk::PhyToWorldX(center.x) );
	pLineVertexPtr[1] = agk::WorldToScreenY( agk::PhyToWorldY(center.y) );
	pLineVertexPtr[2] = 0;
	pLineVertexPtr[3] = color.r;
	pLineVertexPtr[4] = color.g;
	pLineVertexPtr[5] = color.b;
	pLineVertexPtr[6] = color.a;
	
	pLineVertexPtr[7] = agk::WorldToScreenX( agk::PhyToWorldX(p.x) );
	pLineVertexPtr[8] = agk::WorldToScreenY( agk::PhyToWorldY(p.y) );
	pLineVertexPtr[9] = 0;
	pLineVertexPtr[10] = color.r;
	pLineVertexPtr[11] = color.g;
	pLineVertexPtr[12] = color.b;
	pLineVertexPtr[13] = color.a;

	pLineIndexPtr[0] = m_iNumLineVertices;		m_iNumLineVertices++;
	pLineIndexPtr[1] = m_iNumLineVertices;		m_iNumLineVertices++;

	m_iNumLineIndices += 2;
}

void DebugDraw::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
{
	if ( !m_pShader ) return;

	float fMinX = p1.x;
	float fMinY = p1.y;
	float fMaxX = p1.x;
	float fMaxY = p1.y;

	if ( p2.x < fMinX ) fMinX = p2.x;
	if ( p2.y < fMinY ) fMinY = p2.y;
	if ( p2.x > fMaxX ) fMaxX = p2.x;
	if ( p2.y > fMaxY ) fMaxY = p2.y;

	fMinX = agk::WorldToScreenX( agk::PhyToWorldX(fMinX) );
	fMaxX = agk::WorldToScreenX( agk::PhyToWorldX(fMaxX) );
	fMinY = agk::WorldToScreenY( agk::PhyToWorldY(fMinY) );
	fMaxY = agk::WorldToScreenY( agk::PhyToWorldY(fMaxY) );

	if ( fMaxX < agk::GetScreenBoundsLeft() || fMaxY < agk::GetScreenBoundsTop() ) return;
	if ( fMinX > agk::GetScreenBoundsRight() || fMinY > agk::GetScreenBoundsBottom() ) return;

	// check if we can draw more vertices
	if ( !agk::GetRenderer()->SupportsIntIndices() && m_iNumLineVertices + 2 > 65535 )
	{
		EndRender();
		BeginRender();
	}

	ExpandLineVertexArray( m_iNumLineVertices + 2 );
	ExpandLineIndexArray( m_iNumLineIndices + 2 );

	float *pLineVertexPtr = m_pLineVertexData + (m_iNumLineVertices * GetVertexNumFloats());
	uint32_t *pLineIndexPtr = m_pLineIndices + m_iNumLineIndices;
	
	pLineVertexPtr[0] = agk::WorldToScreenX( agk::PhyToWorldX(p1.x) );
	pLineVertexPtr[1] = agk::WorldToScreenY( agk::PhyToWorldY(p1.y) );
	pLineVertexPtr[2] = 0;		
	pLineVertexPtr[3] = color.r;
	pLineVertexPtr[4] = color.g;
	pLineVertexPtr[5] = color.b;
	pLineVertexPtr[6] = color.a;
	
	pLineVertexPtr[7] = agk::WorldToScreenX( agk::PhyToWorldX(p2.x) );
	pLineVertexPtr[8] = agk::WorldToScreenY( agk::PhyToWorldY(p2.y) );
	pLineVertexPtr[9] = 0;		
	pLineVertexPtr[10] = color.r;
	pLineVertexPtr[11] = color.g;
	pLineVertexPtr[12] = color.b;
	pLineVertexPtr[13] = color.a;

	pLineIndexPtr[0] = m_iNumLineVertices;	m_iNumLineVertices++;
	pLineIndexPtr[1] = m_iNumLineVertices;	m_iNumLineVertices++;

	m_iNumLineIndices += 2;
}

void DebugDraw::DrawTransform(const b2Transform& xf)
{
	
}

void DebugDraw::DrawPoint(const b2Vec2& p, float32 size, const b2Color& color)
{
	if ( !m_pShader ) return;

	float x = agk::WorldToScreenX( agk::PhyToWorldX(p.x) );
	float y = agk::WorldToScreenY( agk::PhyToWorldY(p.y) );

	if ( x < agk::GetScreenBoundsLeft()  || y < agk::GetScreenBoundsTop() ) return;
	if ( x > agk::GetScreenBoundsRight() || y > agk::GetScreenBoundsBottom() ) return;

	float diffX = agk::m_iDisplayWidth / (float) agk::Round(agk::m_fTargetViewportWidth);
	float diffY = agk::m_iDisplayHeight / (float) agk::Round(agk::m_fTargetViewportHeight);

	// check if we can draw more vertices
	if ( !agk::GetRenderer()->SupportsIntIndices() && m_iNumLineVertices + 2 > 65535 )
	{
		EndRender();
		BeginRender();
	}

	ExpandLineVertexArray( m_iNumLineVertices + 2 );
	ExpandLineIndexArray( m_iNumLineIndices + 2 );

	float *pLineVertexPtr = m_pLineVertexData + (m_iNumLineVertices * GetVertexNumFloats());
	uint32_t *pLineIndexPtr = m_pLineIndices + m_iNumLineIndices;

	pLineVertexPtr[0] = x;
	pLineVertexPtr[1] = y;
	pLineVertexPtr[2] = 0;
	pLineVertexPtr[3] = color.r;
	pLineVertexPtr[4] = color.g;
	pLineVertexPtr[5] = color.b;
	pLineVertexPtr[6] = color.a;

	pLineVertexPtr[7] = x+diffX;
	pLineVertexPtr[8] = y+diffY;
	pLineVertexPtr[9] = 0;		
	pLineVertexPtr[10] = color.r;
	pLineVertexPtr[11] = color.g;
	pLineVertexPtr[12] = color.b;
	pLineVertexPtr[13] = color.a;

	pLineIndexPtr[0] = m_iNumLineVertices;	m_iNumLineVertices++;
	pLineIndexPtr[1] = m_iNumLineVertices;	m_iNumLineVertices++;

	m_iNumLineIndices += 2;
}


// Destruction listener class

void MyDestructionListener::SayGoodbye( b2Joint* joint )
{
	agk::ClearJoint( joint );
}

// Ray cast callback
MyRayCastCallback::MyRayCastCallback()
{
	Reset();
}

void MyRayCastCallback::Reset()
{
	m_fixture = 0;
	m_fraction = 1.0f;
	m_category = 0xffff;
	m_group = 0;
	m_sprite = 0;
}

float32 MyRayCastCallback::ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float32 fraction)
{
	m_category = m_category & 0xffff;
	if ( m_category != 0 )
	{
		if ( (fixture->GetFilterData().categoryBits & m_category) == 0 ) return -1;
	}

	if ( m_group != 0 )
	{
		if ( fixture->GetFilterData().groupIndex != m_group ) return -1;
	}

	m_fixture = fixture;
	m_point = point;
	m_normal = normal;
	m_fraction = fraction;

	return fraction;
}
