#include "agk.h"

using namespace AGK;

int AGK3DParticleEmitter::UpdateVertexLayout( AGKVertexLayout *pLayout, AGKShader *pShader )
{
	if ( !pLayout ) return 1;
	if ( !pShader ) return 1;

	int error = 0;

	int numShaderAttribs = pShader->GetNumAttributes();
	pLayout->SetNumOffsets( numShaderAttribs );
	
	pLayout->m_iVertexSize = (uint16_t)GetParticleVertexFloats() * sizeof(float); // pos + uv + color

	for( int i = 0; i < numShaderAttribs; i++ )
	{
		AGKShaderAttrib *pShaderAttrib = pShader->GetAttribute( i );
		int offset = -1;
		switch( pShaderAttrib->m_iKnownName )
		{
			case AGK_SHADER_ATTRIB_NAME_POS:	offset = GetParticleVertexPosOffset() * sizeof(float); break;
			case AGK_SHADER_ATTRIB_NAME_UV0:	offset = GetParticleVertexUVOffset() * sizeof(float); break;
			case AGK_SHADER_ATTRIB_NAME_COLOR:	offset = GetParticleVertexColorOffset() * sizeof(float); break;
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

// Emitter

int AGK3DParticleEmitter::m_iParticlesDrawn = 0;

AGK3DParticleEmitter::AGK3DParticleEmitter()
{
	if ( m_iNumParticles > 0 )
	{
		m_pParticles = new AGK3DParticle*[ m_iNumParticles ];
		for ( uint32_t i = 0; i < m_iNumParticles; i++ )
		{
			m_pParticles[ i ] = new AGK3DParticle();
		}
	}

	m_renderState.SetCullMode( AGK_CULL_BACK );
	m_renderState.SetBlendEnable( 0 );
	SetTransparency( 1 );

	m_vertexLayout.m_iVertexSize = (uint16_t) GetParticleVertexFloats() * sizeof(float); 
	m_vertexLayout.m_iPrimitiveType = AGK_PRIMITIVE_TRIANGLES;

	m_pShader = AGKShader::g_pShader3DParticlesColor;
	UpdateVertexLayout( &m_vertexLayout, m_pShader );
}

AGK3DParticleEmitter::~AGK3DParticleEmitter()
{
	if ( m_pParticles ) 
	{
		for ( uint32_t i = 0; i < m_iNumParticles; i++ ) if ( m_pParticles[ i ] ) delete m_pParticles[ i ];
		delete [] m_pParticles;
	}
	if ( m_pForces ) delete [] m_pForces;
	if ( m_pColors ) delete [] m_pColors;
	if ( m_pSizes ) delete [] m_pSizes;

	if ( m_pVertexData ) delete [] m_pVertexData;
	if ( m_pIndices ) 
	{
		if ( m_iFlags & AGK_PARTICLES_INT_INDICES ) delete [] (uint32_t*)m_pIndices;
		else delete [] (uint16_t*)m_pIndices;
	}

	Renderer *pRenderer = agk::GetRenderer();
	if ( m_pInternalVertices ) pRenderer->DeleteVertices( m_pInternalVertices );
	if ( m_pInternalPipeline ) pRenderer->DeletePipeline( m_pInternalPipeline );
	if ( m_pInternalConstants ) pRenderer->DeleteShaderConstants( m_pInternalConstants );
}

void AGK3DParticleEmitter::UpdateNumParticles()
{
	uint32_t iNewNum = agk::Ceil( m_fFreq * m_fLife ) + 2;  //final addition is a buffer to leave some room for error.
	if ( iNewNum > m_iNumParticles )
	{
		AGK3DParticle **pNewParticles = new AGK3DParticle*[ iNewNum ];
		if ( m_iNumParticles > 0 && m_pParticles )
		{
			// copy old particles to new list
			for ( uint32_t i = 0; i < m_iCurrParticle; i++ )
			{
				pNewParticles[ i ] = m_pParticles[ i ];
			}

			// important to create new particles in the gap between oldest and newest particle
			// m_iCurrParticle points to oldest particle and will be used in next emission.
			uint32_t remaining = m_iNumParticles - m_iCurrParticle;
			for ( uint32_t i = m_iCurrParticle; i < iNewNum-remaining; i++ )
			{
				pNewParticles[ i ] = new AGK3DParticle();
			}

			// add oldest particles after new ones
			for ( uint32_t i = iNewNum-remaining; i < iNewNum; i++ )
			{
				pNewParticles[ i ] = m_pParticles[ i - (iNewNum - m_iNumParticles) ];
			}
		}
		else
		{
			for ( uint32_t i = 0; i < iNewNum; i++ )
			{
				pNewParticles[ i ] = new AGK3DParticle();
			}
		}

		if ( m_pParticles ) delete [] m_pParticles;
		m_pParticles = pNewParticles;
		m_iNumParticles = iNewNum;
	}
}

void AGK3DParticleEmitter::SetPosition( float x, float y, float z )
{
	m_fX = x;
	m_fY = y;
	m_fZ = z;
}

void AGK3DParticleEmitter::SetFrequency( float freq )
{
	//if ( freq < 0.1f ) freq = 0.1f;
	if ( freq < 0 ) freq = 0;
	if ( freq > 500.0f ) freq = 500.0f;

	m_fFreq = freq;

	// adjust number of particles based on life and frequency
	UpdateNumParticles();
}

void AGK3DParticleEmitter::SetStartZone( float x1, float y1, float z1, float x2, float y2, float z2 )
{
	if ( x1 > x2 ) 
	{
		float temp = x1;
		x1 = x2;
		x2 = temp;
	}

	if ( y1 > y2 ) 
	{
		float temp = y1;
		y1 = y2;
		y2 = temp;
	}

	if ( z1 > z2 ) 
	{
		float temp = z1;
		z1 = z2;
		z2 = temp;
	}

	m_fStartX1 = x1;
	m_fStartY1 = y1;
	m_fStartZ1 = z1;
	m_fStartX2 = x2;
	m_fStartY2 = y2;
	m_fStartZ2 = z2;
}

void AGK3DParticleEmitter::SetDirection( float vx, float vy, float vz, float roll )
{
	m_fVX = vx;
	m_fVY = vy;
	m_fVZ = vz;
	m_fRoll = roll;
}

void AGK3DParticleEmitter::SetVelocityRange( float v1, float v2 )
{
	if ( v1 < 0.001f ) v1 = 0.001f;
	if ( v2 < 0.001f ) v2 = 0.001f;

	if ( v2 < v1 )
	{
		float temp = v1;
		v1 = v2;
		v2 = temp;
	}

	m_fVMin = v1;
	m_fVMax = v2;
}

void AGK3DParticleEmitter::SetDirectionRange( float angle1, float angle2 )
{
	if ( angle1 < 0 ) angle1 = 0;
	if ( angle1 > 360 ) angle1 = 360;
	m_fAngle1 = angle1;

	if ( angle2 < 0 ) angle2 = 0;
	if ( angle2 > 180 ) angle2 = 180;
	m_fAngle2 = angle2;
}

void AGK3DParticleEmitter::SetSize( float size )
{
	if ( size < 0.0f ) size = 0.0f;
	m_fSize = size;
}

void AGK3DParticleEmitter::SetVisible( int visible )
{
	if ( visible ) m_iFlags |= AGK_3DPARTICLE_VISIBLE;
	else m_iFlags &= ~AGK_3DPARTICLE_VISIBLE;
}

void AGK3DParticleEmitter::SetActive( int active )
{
	if ( active ) m_iFlags |= AGK_3DPARTICLE_ACTIVE;
	else m_iFlags &= ~AGK_3DPARTICLE_ACTIVE;
}
/*
void AGK3DParticleEmitter::SetRotationRate( float a1, float a2 )
{
	a1 = a1 * PI / 180.0f;
	a2 = a2 * PI / 180.0f;
	SetRotationRateRad( a1, a2 );
}

void AGK3DParticleEmitter::SetRotationRateRad( float a1, float a2 )
{
	if ( a2 < a1 )
	{
		float temp = a1;
		a1 = a2;
		a2 = temp;
	}

	m_fAMin = a1;
	m_fAMax = a2;
}

void AGK3DParticleEmitter::SetFaceDirection( int flag )
{
	if ( flag ) m_iFlags |= AGK_3DPARTICLE_FACEDIR;
	else m_iFlags &= ~AGK_3DPARTICLE_FACEDIR;
}
*/
void AGK3DParticleEmitter::SetTransparency( int mode )
{
	if ( mode == m_iTransparencyMode ) return;
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
	}
}

void AGK3DParticleEmitter::SetLife( float time )
{
	if ( time < 0.001f ) time = 0.001f;
	if ( time > 120.0f ) time = 120.0f;

	m_fLife = time;

	// adjust number of particles based on life and frequency
	UpdateNumParticles();
}

void AGK3DParticleEmitter::SetImage( cImage *pImage )
{
	if ( m_pImage == pImage ) return;

	m_pImage = pImage;
	
	if ( !pImage ) 
	{
		m_pShader = AGKShader::g_pShader3DParticlesColor;
		m_iImageID = 0;
	}
	else
	{
		m_pShader = AGKShader::g_pShader3DParticlesTex;
		m_iImageID = pImage->GetID();
	}

	UpdateVertexLayout( &m_vertexLayout, m_pShader );
}

void AGK3DParticleEmitter::SetColorInterpolation( int mode )
{
	if ( mode ) m_iFlags |= AGK_3DPARTICLE_INTERP_COLOR;
	else m_iFlags &= ~AGK_3DPARTICLE_INTERP_COLOR;
}

void AGK3DParticleEmitter::SetMaxParticles( int max )
{
	m_iMaxParticles = max;
}

void AGK3DParticleEmitter::ResetParticleCount()
{
	m_iEmittedParticles = 0;
}

int AGK3DParticleEmitter::GetMaxParticlesReached()
{
	if ( m_iMaxParticles < 0 ) return false;
	return (!GetSomeAlive()) && (m_iEmittedParticles >= m_iMaxParticles);
}

void AGK3DParticleEmitter::AddForce( float starttime, float endtime, float vx, float vy, float vz )
{
	if ( starttime < 0 ) starttime = 0;
	if ( endtime <= starttime ) return;

	if ( m_iNumForces+1 > m_iMaxNumForces )
	{
		m_iMaxNumForces = m_iMaxNumForces + m_iMaxNumForces/2;
		if ( m_iMaxNumForces < 2 ) m_iMaxNumForces = 2;
		AGK3DParticleForce *newArray = new AGK3DParticleForce[ m_iMaxNumForces ];
		if ( m_pForces )
		{
			memcpy( newArray, m_pForces, m_iNumForces*sizeof(cParticleForce) );
			delete [] m_pForces;
		}
		m_pForces = newArray;
	}

	// forces are not sorted

	m_pForces[m_iNumForces].m_fStartTime = starttime;
	m_pForces[m_iNumForces].m_fEndTime = endtime;
	m_pForces[m_iNumForces].m_fX = vx;
	m_pForces[m_iNumForces].m_fY = vy;
	m_pForces[m_iNumForces].m_fZ = vz;

	m_iNumForces++;
}

void AGK3DParticleEmitter::ClearForces()
{
	if( m_pForces ) delete [] m_pForces;
	m_pForces = 0;
	m_iNumForces = 0;
	m_iMaxNumForces = 0;
}

void AGK3DParticleEmitter::AddColorKeyFrame( float time, uint32_t red, uint32_t green, uint32_t blue, uint32_t alpha )
{
	AddColorKeyFrameF( time, red/255.0f, green/255.0f, blue/255.0f, alpha/255.0f );
}

void AGK3DParticleEmitter::AddColorKeyFrameF( float time, float red, float green, float blue, float alpha )
{
	if ( time < 0 ) time = 0;

	if ( m_iNumColors+1 > m_iMaxNumColors )
	{
		m_iMaxNumColors = m_iMaxNumColors + m_iMaxNumColors/2;
		if ( m_iMaxNumColors < 2 ) m_iMaxNumColors = 2;
		AGK3DParticleColor *newArray = new AGK3DParticleColor[ m_iMaxNumColors ];
		if ( m_pColors )
		{
			memcpy( newArray, m_pColors, m_iNumColors*sizeof(AGK3DParticleColor) );
			delete [] m_pColors;
		}
		m_pColors = newArray;
	}

	// colors are sorted by time

	// could use a binary search to improve performance
	int index = 0;
	while( index < (int)m_iNumColors && m_pColors[index].m_fTime < time ) index++;
	
	for( int i = ((int)m_iNumColors)-1; i >= index; i-- ) m_pColors[i+1] = m_pColors[i];
		
	m_pColors[index].m_fTime = time;
	m_pColors[index].m_fRed = red;
	m_pColors[index].m_fGreen = green;
	m_pColors[index].m_fBlue = blue;
	m_pColors[index].m_fAlpha = alpha;

	m_iNumColors++;
}

void AGK3DParticleEmitter::ClearColors()
{
	if ( m_pColors ) delete [] m_pColors;
	m_pColors = 0;
	m_iNumColors = 0;
	m_iMaxNumColors = 0;
}

void AGK3DParticleEmitter::AddScaleKeyFrame( float time, float scale )
{
	if ( time < 0 ) time = 0;
	if ( scale < 0 ) scale = 0;
	
	if ( m_iNumSizes+1 > m_iMaxNumSizes )
	{
		m_iMaxNumSizes = m_iMaxNumSizes + m_iMaxNumSizes/2;
		if ( m_iMaxNumSizes < 2 ) m_iMaxNumSizes = 2;
		AGK3DParticleSize *newArray = new AGK3DParticleSize[ m_iMaxNumSizes ];
		if ( m_pSizes )
		{
			memcpy( newArray, m_pSizes, m_iNumSizes*sizeof(AGK3DParticleSize) );
			delete [] m_pSizes;
		}
		m_pSizes = newArray;
	}

	// sizes are sorted by time

	// could use a binary search to improve performance
	int index = 0;
	while( index < (int)m_iNumSizes && m_pSizes[index].m_fTime < time ) index++;
	
	for( int i = ((int)m_iNumSizes)-1; i >= index; i-- ) m_pSizes[i+1] = m_pSizes[i];
		
	m_pSizes[index].m_fTime = time;
	m_pSizes[index].m_fScale = scale;

	m_iNumSizes++;
}

void AGK3DParticleEmitter::ClearScales()
{
	if ( m_pSizes ) delete [] m_pSizes;
	m_pSizes = 0;
	m_iNumSizes = 0;
	m_iMaxNumSizes = 0;
}

void AGK3DParticleEmitter::Offset( float x, float y, float z )
{
	for ( uint32_t i = 0; i < m_iNumParticles; i++ )
	{
		if ( !m_pParticles[ i ]->m_bAlive ) continue;

		m_pParticles[ i ]->m_fX += x;
		m_pParticles[ i ]->m_fY += y;
		m_pParticles[ i ]->m_fZ += z;
	}
}

void AGK3DParticleEmitter::Update( float time )
{
	if ( !GetActive() ) return;
	if ( time <= 0 ) return;

	// how many particles should start this frame, builds up over many frames in the case of slow emitters
	if ( m_iMaxParticles < 0 || m_iEmittedParticles < m_iMaxParticles )
	{
		m_fNumStart += (m_fFreq * time);
	}

	float vLength = 0;
	if ( m_fNumStart >= 1 ) vLength = agk::Sqrt(m_fVX*m_fVX + m_fVY*m_fVY + m_fVZ*m_fVZ);

	while ( m_fNumStart >= 1 )
	{
		// find the start point for this particle, could be anywhere within a box
		float x = m_fStartX1;
		float y = m_fStartY1;
		float z = m_fStartZ1;
		if ( m_fStartX2 > m_fStartX1 ) x = (agk::Random() / 65535.0f) * (m_fStartX2 - m_fStartX1) + m_fStartX1;
		if ( m_fStartY2 > m_fStartY1 ) y = (agk::Random() / 65535.0f) * (m_fStartY2 - m_fStartY1) + m_fStartY1;
		if ( m_fStartZ2 > m_fStartZ1 ) z = (agk::Random() / 65535.0f) * (m_fStartZ2 - m_fStartZ1) + m_fStartZ1;

		m_pParticles[ m_iCurrParticle ]->m_fX = m_fX + x;
		m_pParticles[ m_iCurrParticle ]->m_fY = m_fY + y;
		m_pParticles[ m_iCurrParticle ]->m_fZ = m_fZ + z;

		float vx = m_fVX;
		float vy = m_fVY;
		float vz = m_fVZ;

		// find the start angle for this particle, could be anywhere +/- m_fAngle/2 radians from the emitter direction
		if ( m_fAngle1 > 0 || m_fAngle2 > 0 )
		{
			AGKQuaternion dir; dir.LookAt( m_fVX, m_fVY, m_fVZ, m_fRoll );
		
			float angleY = m_fAngle1*(agk::Random()/65535.0f - 0.5f);
			float angleX = m_fAngle2*(agk::Random()/65535.0f - 0.5f);
			float newX = agk::Sin( angleY )*agk::Cos( angleX );
			float newZ = agk::Cos( angleY )*agk::Cos( angleX );
			float newY = agk::Sin( angleX );

			AGKVector v( newX, newY, newZ );
			v.Mult( vLength );
			v.Mult( dir );

			vx = v.x;
			vy = v.y;
			vz = v.z;
		}

		// adjust the length of the direction vector (speed)
		if ( m_fVMin != 1 || m_fVMax != 1 )
		{
			float velocity = m_fVMin + (agk::Random() / 65535.0f) * (m_fVMax - m_fVMin);
			vx *= velocity;
			vy *= velocity;
			vz *= velocity;
		}

		// adjust the rotation speed
		/*
		m_pParticles[ m_iCurrParticle ]->m_fAngle = 0;
		m_pParticles[ m_iCurrParticle ]->m_fAngleDelta = 0;
		if ( m_fAMin != 0 || m_fAMax != 0 )
		{
			m_pParticles[ m_iCurrParticle ]->m_fAngleDelta = m_fAMin + (agk::Random() / 65535.0f) * (m_fAMax - m_fAMin);
		}

		if ( m_iFlags & AGK_3DPARTICLE_FACEDIR )
		{
			//m_pParticles[ m_iCurrParticle ]->m_fAngle = agk::ATan2Rad( vy, vx );
		}
		*/

		m_pParticles[ m_iCurrParticle ]->m_fVX = vx;
		m_pParticles[ m_iCurrParticle ]->m_fVY = vy;
		m_pParticles[ m_iCurrParticle ]->m_fVZ = vz;
		m_pParticles[ m_iCurrParticle ]->m_fTime = 0;

		// color
		m_pParticles[ m_iCurrParticle ]->m_iColorIndex = 0;				
		if ( m_iNumColors > 0 ) m_pParticles[ m_iCurrParticle ]->SetColor( m_pColors[ 0 ].m_fRed, m_pColors[ 0 ].m_fGreen, m_pColors[ 0 ].m_fBlue, m_pColors[ 0 ].m_fAlpha );
		else m_pParticles[ m_iCurrParticle ]->SetColor( 1,1,1,1 );

		// scale
		m_pParticles[ m_iCurrParticle ]->m_iSizeIndex = 0;
		if ( m_iNumSizes > 0 ) m_pParticles[ m_iCurrParticle ]->m_fScale = m_pSizes[0].m_fScale;
		else m_pParticles[ m_iCurrParticle ]->m_fScale = 1;

		// make it active
		m_pParticles[ m_iCurrParticle ]->m_bAlive = true;

		m_iCurrParticle++;
		if ( m_iCurrParticle >= m_iNumParticles ) m_iCurrParticle = 0;
		m_fNumStart--;

		m_iEmittedParticles++;
	}

	int iSomeAlive = 0;

	// update live particles
	for ( uint32_t i = 0; i < m_iNumParticles; i++ )
	{
		if ( !m_pParticles[ i ]->m_bAlive ) continue;

		iSomeAlive = 1;

		m_pParticles[ i ]->m_fTime += time;
		if ( m_pParticles[ i ]->m_fTime > m_fLife ) m_pParticles[ i ]->m_bAlive = false;

		// check forces
		for( uint32_t j = 0; j < m_iNumForces; j++ )
		{
			AGK3DParticleForce *pForce = m_pForces + j;
			if ( pForce->m_fStartTime <= m_pParticles[ i ]->m_fTime 
			  && pForce->m_fEndTime > m_pParticles[ i ]->m_fTime )
			{
				m_pParticles[ i ]->m_fVX += pForce->m_fX*time;
				m_pParticles[ i ]->m_fVY += pForce->m_fY*time;
				m_pParticles[ i ]->m_fVZ += pForce->m_fZ*time;
			}
		}

		/*
		// modify angle
		if ( m_iFlags & AGK_3DPARTICLE_FACEDIR )
		{
			//m_pParticles[ i ]->m_fAngle = agk::ATan2Rad( m_pParticles[ i ]->m_fVY, m_pParticles[ i ]->m_fVX ) + PI/2;
		}
		//else
		{
			m_pParticles[ i ]->m_fAngle += m_pParticles[ i ]->m_fAngleDelta*time;
		}
		*/

		// check colors
		uint32_t colorIndex = m_pParticles[i]->m_iColorIndex;
		if ( m_iNumColors > 1 && colorIndex < m_iNumColors-1 )
		{
			if ( m_pParticles[i]->m_fTime > m_pColors[ colorIndex ].m_fTime )
			{
				if ( m_pParticles[i]->m_fTime >= m_pColors[ colorIndex+1 ].m_fTime )
				{
					colorIndex++;
					m_pParticles[i]->m_iColorIndex = colorIndex;

					if ( !GetInterpolateColor() || colorIndex == m_iNumColors-1 ) 
					{
						m_pParticles[ m_iCurrParticle ]->SetColor( m_pColors[ colorIndex ].m_fRed, m_pColors[ colorIndex ].m_fGreen, m_pColors[ colorIndex ].m_fBlue, m_pColors[ colorIndex ].m_fAlpha );
					}
				}

				if ( GetInterpolateColor() && colorIndex < m_iNumColors-1 ) 
				{
					AGK3DParticleColor* pBegin = m_pColors + colorIndex;
					AGK3DParticleColor* pEnd = pBegin + 1;
					float curr = m_pParticles[ i ]->m_fTime - pBegin->m_fTime;
					float total = pEnd->m_fTime - pBegin->m_fTime;
					if ( total > 0 )
					{
						float s = curr / total;
						float fRed = pBegin->m_fRed + s*(pEnd->m_fRed - pBegin->m_fRed);
						float fGreen = pBegin->m_fGreen + s*(pEnd->m_fGreen - pBegin->m_fGreen);
						float fBlue = pBegin->m_fBlue + s*(pEnd->m_fBlue - pBegin->m_fBlue);
						float fAlpha = pBegin->m_fAlpha + s*(pEnd->m_fAlpha - pBegin->m_fAlpha);

						m_pParticles[ i ]->SetColor( fRed, fGreen, fBlue, fAlpha );
					}
				}
			}
		}

		// check size
		uint32_t sizeIndex = m_pParticles[i]->m_iSizeIndex;
		if ( m_iNumSizes > 1 && sizeIndex < m_iNumSizes-1 )
		{
			if ( m_pParticles[i]->m_fTime > m_pSizes[ sizeIndex ].m_fTime )
			{
				if ( m_pParticles[i]->m_fTime >= m_pSizes[ sizeIndex+1 ].m_fTime )
				{
					sizeIndex++;
					m_pParticles[i]->m_iSizeIndex = sizeIndex;

					if ( sizeIndex == m_iNumSizes-1 ) 
					{
						m_pParticles[ m_iCurrParticle ]->m_fScale = m_pSizes[ sizeIndex ].m_fScale;
					}
				}

				if ( sizeIndex < m_iNumSizes-1 ) 
				{
					AGK3DParticleSize* pBegin = m_pSizes + sizeIndex;
					AGK3DParticleSize* pEnd = pBegin + 1;
					float curr = m_pParticles[ i ]->m_fTime - pBegin->m_fTime;
					float total = pEnd->m_fTime - pBegin->m_fTime;
					if ( total > 0 )
					{
						float s = curr / total;
						m_pParticles[ i ]->m_fScale = pBegin->m_fScale + s*(pEnd->m_fScale - pBegin->m_fScale);
					}
				}
			}
		}

		m_pParticles[ i ]->m_fX += m_pParticles[ i ]->m_fVX*time;
		m_pParticles[ i ]->m_fY += m_pParticles[ i ]->m_fVY*time;
		m_pParticles[ i ]->m_fZ += m_pParticles[ i ]->m_fVZ*time;
	}

	if ( iSomeAlive ) m_iFlags |= AGK_3DPARTICLE_SOME_ALIVE;
	else m_iFlags &= ~AGK_3DPARTICLE_SOME_ALIVE;
}

void AGK3DParticleEmitter::DrawAll()
{
	if ( !GetVisible() ) return;
	if ( !GetSomeAlive() ) return;

	if ( m_iImageID )
	{
		// check image
		cImage *pRealPtr = agk::GetImagePtr( m_iImageID );
		if ( pRealPtr != m_pImage )
		{
			SetImage( 0 );
		}
	}

	Renderer *pRenderer = agk::GetRenderer();

	int iVertexFloats = GetParticleVertexFloats();

	// quad setup
	if ( m_iNumParticles > m_iNumVertices )
	{
		if ( m_pVertexData ) delete [] m_pVertexData;
		m_pVertexData = new float[ m_iNumParticles*iVertexFloats*4 ]; // 4 vertices per particle

		if ( m_pIndices ) 
		{
			if ( m_iFlags & AGK_PARTICLES_INT_INDICES ) delete [] (uint32_t*)m_pIndices;
			else delete [] (uint16_t*)m_pIndices;
		}

		m_iNumVertices = m_iNumParticles;
		if ( !m_pInternalVertices ) pRenderer->CreateVertices( iVertexFloats*sizeof(float), &m_pInternalVertices );

		if ( m_iNumParticles*4 > 65536 ) 
		{
			m_iFlags |= AGK_PARTICLES_INT_INDICES;
			uint32_t *newIndices = new uint32_t[ m_iNumParticles*6 ];
			m_pIndices = newIndices;

			for( uint32_t i = 0; i < m_iNumParticles; i++ )
			{
				newIndices[ i*6 + 0 ] = 0 + i*4;
				newIndices[ i*6 + 1 ] = 1 + i*4;
				newIndices[ i*6 + 2 ] = 2 + i*4;

				newIndices[ i*6 + 3 ] = 2 + i*4;
				newIndices[ i*6 + 4 ] = 1 + i*4;
				newIndices[ i*6 + 5 ] = 3 + i*4;
			}

			pRenderer->SetIndicesInt( m_pInternalVertices, newIndices, m_iNumParticles * 6 );
		}
		else
		{
			m_iFlags &= ~AGK_PARTICLES_INT_INDICES;
			uint16_t *newIndices = new uint16_t[ m_iNumParticles*6 ];
			m_pIndices = newIndices;

			for( uint32_t i = 0; i < m_iNumParticles; i++ )
			{
				newIndices[ i*6 + 0 ] = 0 + i*4;
				newIndices[ i*6 + 1 ] = 1 + i*4;
				newIndices[ i*6 + 2 ] = 2 + i*4;

				newIndices[ i*6 + 3 ] = 2 + i*4;
				newIndices[ i*6 + 4 ] = 1 + i*4;
				newIndices[ i*6 + 5 ] = 3 + i*4;
			}

			pRenderer->SetIndices( m_pInternalVertices, newIndices, m_iNumParticles * 6 );
		}
	}

	float fNewSize = m_fSize / 2.0f;

	AGKVector camHorz(1, 0, 0);
	AGKVector camVert(0, 1, 0);
	const AGKQuaternion camRot = agk::GetCurrentCamera()->rotFinal();
	camHorz.Mult( camRot );
	camVert.Mult( camRot );

	AGKVector frustumN[6];
	float frustumD[6];
	for( int i = 0; i < 6; i++ )
	{
		agk::GetCurrentCamera()->GetFrustumPlane( i, frustumN[i], frustumD[i] );
	}

	float fU1 = 0;
	float fV1 = 0;
	float fU2 = 1;
	float fV2 = 1;

	if ( m_pImage )
	{
		fU1 = m_pImage->GetU1();
		fV1 = m_pImage->GetV1();
		fU2 = m_pImage->GetU2();
		fV2 = m_pImage->GetV2();
	}

	float* pVertexPtr = m_pVertexData;
	uint32_t count = 0;
	// start with the oldest particle, this determines the draw order
	if ( m_iCurrParticle > 0 )
	{
		for( int i = ((int)m_iCurrParticle)-1; i >= 0; i-- )
		{
			if ( !m_pParticles[ i ]->m_bAlive ) continue;

			float x = m_pParticles[ i ]->m_fX;
			float y = m_pParticles[ i ]->m_fY;
			float z = m_pParticles[ i ]->m_fZ;
			float size = m_pParticles[ i ]->m_fScale*fNewSize;

			// check if particle is in view frustum
			int valid = 1;
			float limit = -1.74f*size; // 1.74 = rough distance from center to corner of a unit cube
			for( int j = 0; j < 6; j++ )
			{
				float dist = frustumN[j].x*x + frustumN[j].y*y + frustumN[j].z*z + frustumD[j];
				if ( dist < limit ) { valid = 0; break; }
			}
			if ( !valid ) continue;

			float fRed = m_pParticles[ i ]->m_fRed;
			float fGreen = m_pParticles[ i ]->m_fGreen;
			float fBlue = m_pParticles[ i ]->m_fBlue;
			float fAlpha = m_pParticles[ i ]->m_fAlpha;
			
			// vertex 1		
			pVertexPtr[ 0 ] = x + size*(-camHorz.x + camVert.x);	
			pVertexPtr[ 1 ] = y + size*(-camHorz.y + camVert.y);	
			pVertexPtr[ 2 ] = z + size*(-camHorz.z + camVert.z);	
					  
			pVertexPtr[ 3 ] = fRed;	
			pVertexPtr[ 4 ] = fGreen;
			pVertexPtr[ 5 ] = fBlue;	
			pVertexPtr[ 6 ] = fAlpha;	
					  
			pVertexPtr[ 7 ] = fU1;	
			pVertexPtr[ 8 ] = fV1;	
					  
			// vertex 2
			pVertexPtr[ 9 ] = x + size*(-camHorz.x - camVert.x);
			pVertexPtr[ 10 ] = y + size*(-camHorz.y - camVert.y);
			pVertexPtr[ 11 ] = z + size*(-camHorz.z - camVert.z);
					  
			pVertexPtr[ 12 ] = fRed;		
			pVertexPtr[ 13 ] = fGreen;	
			pVertexPtr[ 14 ] = fBlue;		
			pVertexPtr[ 15 ] = fAlpha;		
					  
			pVertexPtr[ 16 ] = fU1;		
			pVertexPtr[ 17 ] = fV2;		
					  
			// vertex 3
			pVertexPtr[ 18 ] = x + size*(camHorz.x + camVert.x);
			pVertexPtr[ 19 ] = y + size*(camHorz.y + camVert.y);
			pVertexPtr[ 20 ] = z + size*(camHorz.z + camVert.z);
					  
			pVertexPtr[ 21 ] = fRed;		
			pVertexPtr[ 22 ] = fGreen;	
			pVertexPtr[ 23 ] = fBlue;		
			pVertexPtr[ 24 ] = fAlpha;		
					  
			pVertexPtr[ 25 ] = fU2;		
			pVertexPtr[ 26 ] = fV1;		
					  
			// vertex 4
			pVertexPtr[ 27 ] = x + size*(camHorz.x - camVert.x);
			pVertexPtr[ 28 ] = y + size*(camHorz.y - camVert.y);
			pVertexPtr[ 29 ] = z + size*(camHorz.z - camVert.z);
					  
			pVertexPtr[ 30 ] = fRed;		
			pVertexPtr[ 31 ] = fGreen;	
			pVertexPtr[ 32 ] = fBlue;		
			pVertexPtr[ 33 ] = fAlpha;		
					 
			pVertexPtr[ 34 ] = fU2;		
			pVertexPtr[ 35 ] = fV2;
					 
			pVertexPtr += iVertexFloats * 4;
			count++;
		}
	}

	// wrap around the array back to oldest particle
	for ( int i = ((int)m_iNumParticles)-1; i >= (int)m_iCurrParticle ; i-- )
	{
		if ( !m_pParticles[ i ]->m_bAlive ) continue;

		float x = m_pParticles[ i ]->m_fX;
		float y = m_pParticles[ i ]->m_fY;
		float z = m_pParticles[ i ]->m_fZ;
		float size = m_pParticles[ i ]->m_fScale*fNewSize;

		// check if particle is in view frustum
		int valid = 1;
		float limit = -1.74f*size; // 1.74 = rough distance from center to corner of a unit cube
		for( int j = 0; j < 6; j++ )
		{
			float dist = frustumN[j].x*x + frustumN[j].y*y + frustumN[j].z*z + frustumD[j];
			if ( dist < limit ) { valid = 0; break; }
		}
		if ( !valid ) continue;
			
		float fRed = m_pParticles[ i ]->m_fRed;
		float fGreen = m_pParticles[ i ]->m_fGreen;
		float fBlue = m_pParticles[ i ]->m_fBlue;
		float fAlpha = m_pParticles[ i ]->m_fAlpha;
			
		// vertex 1		
		pVertexPtr[ 0 ] = x + size*(-camHorz.x + camVert.x);	
		pVertexPtr[ 1 ] = y + size*(-camHorz.y + camVert.y);	
		pVertexPtr[ 2 ] = z + size*(-camHorz.z + camVert.z);	
					  
		pVertexPtr[ 3 ] = fRed;	
		pVertexPtr[ 4 ] = fGreen;
		pVertexPtr[ 5 ] = fBlue;	
		pVertexPtr[ 6 ] = fAlpha;	
					  
		pVertexPtr[ 7 ] = fU1;	
		pVertexPtr[ 8 ] = fV1;	
					  
		// vertex 2
		pVertexPtr[ 9 ] = x + size*(-camHorz.x - camVert.x);
		pVertexPtr[ 10 ] = y + size*(-camHorz.y - camVert.y);
		pVertexPtr[ 11 ] = z + size*(-camHorz.z - camVert.z);
					  
		pVertexPtr[ 12 ] = fRed;		
		pVertexPtr[ 13 ] = fGreen;	
		pVertexPtr[ 14 ] = fBlue;		
		pVertexPtr[ 15 ] = fAlpha;		
					  
		pVertexPtr[ 16 ] = fU1;		
		pVertexPtr[ 17 ] = fV2;		
					  
		// vertex 3
		pVertexPtr[ 18 ] = x + size*(camHorz.x + camVert.x);
		pVertexPtr[ 19 ] = y + size*(camHorz.y + camVert.y);
		pVertexPtr[ 20 ] = z + size*(camHorz.z + camVert.z);
					  
		pVertexPtr[ 21 ] = fRed;		
		pVertexPtr[ 22 ] = fGreen;	
		pVertexPtr[ 23 ] = fBlue;		
		pVertexPtr[ 24 ] = fAlpha;		
					  
		pVertexPtr[ 25 ] = fU2;		
		pVertexPtr[ 26 ] = fV1;		
					  
		// vertex 4
		pVertexPtr[ 27 ] = x + size*(camHorz.x - camVert.x);
		pVertexPtr[ 28 ] = y + size*(camHorz.y - camVert.y);
		pVertexPtr[ 29 ] = z + size*(camHorz.z - camVert.z);
					  
		pVertexPtr[ 30 ] = fRed;		
		pVertexPtr[ 31 ] = fGreen;	
		pVertexPtr[ 32 ] = fBlue;		
		pVertexPtr[ 33 ] = fAlpha;		
					 
		pVertexPtr[ 34 ] = fU2;		
		pVertexPtr[ 35 ] = fV2;
					 
		pVertexPtr += iVertexFloats * 4;	
		count++;
	}

	if ( count == 0 ) return;
		
	m_iParticlesDrawn += count;

	pRenderer->BindPipeline( 0, m_pShader, &m_renderState, &m_vertexLayout, &m_pInternalPipeline );
	
	if ( m_pImage )
	{
		AGKDynamicArray<cImage*> shaderImages(1);
		shaderImages.SetItem( 0, m_pImage );
		pRenderer->BindShaderConstants( 0, m_pShader, 0, 0, &shaderImages, &m_pInternalConstants );
	}
	else
	{
		pRenderer->BindShaderConstants( 0, m_pShader, 0, 0, 0, &m_pInternalConstants );
	}
	
	pRenderer->SetVertices( m_pInternalVertices, m_pVertexData, count * 4 ); // 4 vertices per sprite
	pRenderer->DrawVertices( 0, m_pInternalVertices, count*6 );
}