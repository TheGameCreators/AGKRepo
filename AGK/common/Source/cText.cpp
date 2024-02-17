//#include "cText.h"
//#include "cSprite.h"
//#include "Wrapper.h"
#include "agk.h"

namespace AGK
{

// ASCII new line character
#define TEXT_NEWLINE 10
#define TEXT_SPACE 32

cImage* cText::m_pDefaultFontOrig = 0;
cImage* cText::m_pDefaultFontExtOrig = 0;

cImage* cText::m_pDefaultFont = 0;
cImage** cText::m_pDefaultLetters = 0;

cImage* cText::m_pDefaultFontExt = 0;
cImage** cText::m_pDefaultLettersExt = 0;


uint32_t cText::g_iCreated = 0;

void cText::SetDefaultMinFilter( uint32_t mode )
{
	if ( m_pDefaultFont ) m_pDefaultFont->SetMinFilter( mode );
	if ( m_pDefaultFontExt ) m_pDefaultFontExt->SetMinFilter( mode );
}

void cText::SetDefaultMagFilter( uint32_t mode )
{
	if ( m_pDefaultFont ) m_pDefaultFont->SetMagFilter( mode );
	if ( m_pDefaultFontExt ) m_pDefaultFontExt->SetMagFilter( mode );
}

void cText::SetDefaultFontImage( cImage *pImage )
{
	if ( pImage == m_pDefaultFont ) return;

	//if ( !pImage ) return;
	if ( m_pDefaultLetters ) m_pDefaultLetters = 0;

	// don't delete the main image as it was loaded by the user, they must delete it.
	//if ( m_pDefaultFont ) delete m_pDefaultFont;

	if ( !pImage )
	{
		// don't reload as this is a memory leak
		m_pDefaultFont = m_pDefaultFontOrig;
	}
	else
	{
		// load image used by all text objects
		m_pDefaultFont = pImage;
	}

	m_pDefaultLetters = m_pDefaultFont->GetFontImages();
}

void cText::SetDefaultExtendedFontImage( cImage *pImage )
{
	if ( m_pDefaultFontExt == pImage ) return;

	//if ( !pImage ) return;
	if ( m_pDefaultLettersExt ) m_pDefaultLettersExt = 0;

	if ( !pImage )
	{
		// don't reload as this is a memory leak
		m_pDefaultFontExt = m_pDefaultFontExtOrig;
	}
	else
	{
		// load image used by all text objects
		m_pDefaultFontExt = pImage;

	}

	m_pDefaultLettersExt = m_pDefaultFontExt->GetExtendedFontImages();
}

cText::cText( int iLength ) : m_cShaderConstantsByName(32), m_cShaderConstantArray(8)
{
	m_iCreated = g_iCreated;
	g_iCreated++;

	if ( !m_pDefaultFontOrig )
	{
		// load image used by all text objects
		uString sPath( "ascii.png" );
		m_pDefaultFontOrig = new cImage( sPath );
		m_pDefaultFontOrig->SetWrapU( 0 );
		m_pDefaultFontOrig->SetWrapV( 0 );
		m_iImageID = m_pDefaultFontOrig->GetID();

		m_pDefaultLetters = m_pDefaultFontOrig->GetFontImages();
	}

	if ( !m_pDefaultFontExtOrig )
	{
		// load image used by all text objects
		uString sPath( "asciiExt.png" );
		m_pDefaultFontExtOrig = new cImage( sPath );
		m_pDefaultFontExtOrig->SetWrapU( 0 );
		m_pDefaultFontExtOrig->SetWrapV( 0 );

		m_pDefaultLettersExt = m_pDefaultFontExtOrig->GetExtendedFontImages();
	}

	if ( !m_pDefaultFont ) m_pDefaultFont = m_pDefaultFontOrig;
	if ( !m_pDefaultFontExt ) m_pDefaultFontExt = m_pDefaultFontExtOrig;

	m_pUsingDefaultFont = m_pDefaultFont;
	m_pUsingDefaultFontExt = m_pDefaultFontExt;

	m_pShader = AGKShader::g_pShaderTexColor;

	// new fonts
	m_pFTFont = 0;
	m_pFTSizedFont = 0;
	if ( agk::m_iUseNewDefaultFonts == 1 )
	{
		m_pFTFont = AGKFont::GetDefaultFont();

		m_fHorizontalRatio = agk::DeviceToDisplayRatioX();
		m_fVerticalRatio = agk::DeviceToDisplayRatioY();

		int pixelSize = agk::Round( m_fOrigSize/m_fVerticalRatio );
		m_fSize = pixelSize * m_fVerticalRatio;
		m_pFTSizedFont = m_pFTFont->GetSizedFont( pixelSize );
		if ( m_pFTSizedFont ) 
		{
			m_fFontScale = pixelSize / (float) m_pFTSizedFont->GetSize();
			m_pFTSizedFont->AddRef();
		}

		m_pShader = AGKShader::g_pShaderFont;

		m_iFlags |= AGK_TEXT_SNAP_TO_PIXELS;
	}

	m_renderState.SetDepthEnable( 0 );
	m_renderState.SetCullMode( 0 );
	m_renderState.SetBlendEnable( 0 );
	SetTransparency( 1 );

	int iVertexFloats = cSprite::GetSpriteVertexFloats(); // position + color + uv

	cSprite::UpdateVertexLayout( &m_vertexLayout, m_pShader );
	m_vertexLayout.m_iPrimitiveType = AGK_PRIMITIVE_TRIANGLES;
	
	if ( iLength > 0 )
	{
		int vertexLength = iLength;
		if ( vertexLength > 15000 ) vertexLength = 15000; // renderer may be limited to 65535 vertices

		// assign arrays
		m_pVertexData = new float[ vertexLength*iVertexFloats*4 ]; // 4 vertices per sprite
		m_pIndices = new unsigned short[ vertexLength*6 ];
		
		for ( int i = 0; i < vertexLength; i++ )
		{
			m_pIndices[ i*6 + 0 ] = i*4 + 0;
			m_pIndices[ i*6 + 1 ] = i*4 + 1;
			m_pIndices[ i*6 + 2 ] = i*4 + 2;
			
			m_pIndices[ i*6 + 3 ] = i*4 + 2;
			m_pIndices[ i*6 + 4 ] = i*4 + 1;
			m_pIndices[ i*6 + 5 ] = i*4 + 3;
		}

		agk::GetRenderer()->CreateVertices( m_vertexLayout.m_iVertexSize, &m_pInternalVertices );
		agk::GetRenderer()->SetIndices( m_pInternalVertices, m_pIndices, vertexLength*6 );

		m_iNumSprites = iLength;
		m_pSprites = new cSprite*[ iLength ];
		m_pCharStyles = new unsigned char[ iLength ];
		for ( int i = 0; i < iLength; i++ )
		{
			m_pCharStyles[ i ] = 0;

			m_pSprites[ i ] = new cSprite( );
			m_pSprites[ i ]->SetManageImages( 0 );
			//m_pSprites[ i ]->SetOffset( 0,0 );
			m_pSprites[ i ]->SetDepth( m_iDepth );
			m_pSprites[ i ]->SetTransparency( 1 );
			m_pSprites[ i ]->SetColorF( m_fRed, m_fGreen, m_fBlue, m_fAlpha );
			if ( m_pFTSizedFont ) 
			{
				if ( m_iFlags & AGK_TEXT_SNAP_TO_PIXELS ) m_pSprites[ i ]->SetSnap( 1 );
				AGKFontImage *pFontImage = m_pFTSizedFont->GetCharImage( 32, 0 );
				m_pSprites[ i ]->SetFontImage( pFontImage, m_fFontScale );
			}
			else
			{
				if ( m_pDefaultLetters )
				{
					m_pSprites[ i ]->SetImage( m_pDefaultLetters[0] );
				}
				else
				{
					m_pSprites[ i ]->SetImage( m_pDefaultFont );
					m_pSprites[ i ]->SetAnimation( m_pDefaultFont->GetWidth()/16, m_pDefaultFont->GetHeight()/6, 96 );

					if ( !m_pDefaultLettersExt )
					{
						m_pSprites[ i ]->AppendAnimation( m_pDefaultFontExt, m_pDefaultFontExt->GetWidth()/16, m_pDefaultFontExt->GetHeight()/8, 128 );
					}
				}

				m_pSprites[ i ]->SetSize( -1, m_fSize );
			}

			m_pSprites[ i ]->SetUVBorder( 0 );
		}

		// re-align the sprites
		ReAlignSprites();
	}
}

cText::~cText()
{
	// remove from any tweens
	TweenInstance::DeleteTarget( this );

	// lee - does this need cleaning up?
	// paul - no as it is a global value that could be used by multiple objects
	// if ( m_pDefaultFont ) { }

	if ( m_bManagedDrawing )
	{
		// remove sprites from sprite manager
		if ( m_pSpriteManager )
		{
			m_pSpriteManager->RemoveText( this );
		}

		m_bManagedDrawing = false;
	}

	// don't delete letter images as they are a pointer to a global array

	if ( m_pFontImage ) m_pFontImage->RemoveText( this );
	if ( m_pFontImageExt ) m_pFontImageExt->RemoveText( this );

	if ( m_pSprites ) 
	{
		for ( int i = 0; i < (int)m_iNumSprites; i++ )
		{
			delete m_pSprites[ i ];
		}

		delete [] m_pSprites;
	}

	if ( m_pCharStyles ) delete [] m_pCharStyles;

	if ( m_pVertexData ) delete [] m_pVertexData;
	if ( m_pIndices ) delete [] m_pIndices;

	Renderer *pRenderer = agk::GetRenderer();
	if ( m_pInternalVertices ) pRenderer->DeleteVertices( m_pInternalVertices );
	if ( m_pInternalPipeline ) pRenderer->DeletePipeline( m_pInternalPipeline );
	if ( m_pInternalShaderConstants ) pRenderer->DeleteShaderConstants( m_pInternalShaderConstants );
	
	if ( m_pFTSizedFont ) m_pFTSizedFont->Release();
	m_pFTSizedFont = 0;
}

void cText::SetSpriteManager( cSpriteMgrEx *pMgr )
{
	if ( m_pSpriteManager == pMgr ) return;

	// remove from any old manager
	if ( m_bManagedDrawing && m_pSpriteManager ) 
	{
		m_pSpriteManager->RemoveText( this );
	}

	m_bManagedDrawing = false;
	m_pSpriteManager = pMgr;

	if ( m_pSpriteManager )
	{
		m_pSpriteManager->AddText( this );
		m_bManagedDrawing = true;
	}
}

void cText::GlobalImageDeleting( cImage *pImage )
{
	if ( m_pDefaultFont == pImage )
	{
		SetDefaultFontImage( 0 );
	}

	if ( m_pDefaultFontExt == pImage )
	{
		SetDefaultExtendedFontImage( 0 );
	}
}

void cText::ImageDeleting( cImage *pImage )
{
	if ( m_pFontImage == pImage )
	{
		SetFontImage( 0 );
		/*
		for ( int i = 0; i < (int)m_iNumSprites; i++ )
		{
			m_pSprites[ i ]->ImageDeleting( pImage );
			if ( m_pLetterImages )
			{
				int c = m_pSprites[ i ]->GetCurrentFrame() - 1;
				if ( c >= 96 || c < 0 ) c = 0;
				if ( m_pLetterImages[ c ] == 0 ) c = 0;
				m_pSprites[ i ]->ImageDeleting( m_pLetterImages[ c ] );
			}
		}
		m_pLetterImages = 0;
		m_pFontImage = 0;
		*/
	}

	if ( m_pFontImageExt == pImage )
	{
		SetExtendedFontImage( 0 );
		/*
		for ( int i = 0; i < (int)m_iNumSprites; i++ )
		{
			m_pSprites[ i ]->ImageDeleting( pImage );
			if ( m_pLetterImagesExt )
			{
				int c = m_pSprites[ i ]->GetCurrentFrame() - 1;
				if ( c >= 128 || c < 0 ) c = 0;
				if ( m_pLetterImagesExt[ c ] == 0 ) c = 0;
				m_pSprites[ i ]->ImageDeleting( m_pLetterImagesExt[ c ] );
			}
		}
		m_pLetterImagesExt = 0;
		m_pFontImageExt = 0;
		*/
	}
}

void cText::SetString( const char* szString )
{
	m_sText.SetStr( szString );
	uint32_t iLength = m_sText.GetNumChars();
	if ( iLength > m_iNumSprites )
	{
		if ( m_iNumSprites < 15000 )
		{
			if ( m_pVertexData ) delete [] m_pVertexData;
			if ( m_pIndices ) delete [] m_pIndices;

			uint32_t vertexLength = iLength;
			if ( vertexLength > 15000 ) vertexLength = 15000;

			// assign arrays
			int iVertexFloats = cSprite::GetSpriteVertexFloats(); // position + color + uv
			m_pVertexData = new float[ vertexLength*iVertexFloats*4 ]; // 4 vertices per sprite
			m_pIndices = new unsigned short[ vertexLength*6 ];
		
			for ( uint32_t i = 0; i < vertexLength; i++ )
			{
				m_pIndices[ i*6 + 0 ] = i*4 + 0;
				m_pIndices[ i*6 + 1 ] = i*4 + 1;
				m_pIndices[ i*6 + 2 ] = i*4 + 2;
			
				m_pIndices[ i*6 + 3 ] = i*4 + 2;
				m_pIndices[ i*6 + 4 ] = i*4 + 1;
				m_pIndices[ i*6 + 5 ] = i*4 + 3;
			}

			if ( !m_pInternalVertices ) agk::GetRenderer()->CreateVertices( iVertexFloats*sizeof(float), &m_pInternalVertices );
			agk::GetRenderer()->SetIndices( m_pInternalVertices, m_pIndices, vertexLength*6 );
		}

		// expand styles array
		unsigned char *newStyles = new unsigned char[ iLength ];
		for ( uint32_t i = 0; i < m_iNumSprites; i++ ) newStyles[ i ] = m_pCharStyles[ i ];
		for ( uint32_t i = m_iNumSprites; i < iLength; i++ ) newStyles[ i ] = 0;
		if ( m_pCharStyles ) delete [] m_pCharStyles;
		m_pCharStyles = newStyles;

		// expands sprites array
		cSprite **pNewSpriteList = new cSprite*[ iLength ];
		for ( uint32_t i = 0; i < m_iNumSprites; i++ )
		{
			pNewSpriteList[ i ] = m_pSprites[ i ];
		}

		for ( uint32_t i = m_iNumSprites; i < iLength; i++ )
		{
			pNewSpriteList[ i ] = new cSprite( );
			pNewSpriteList[ i ]->SetManageImages( 0 );
			pNewSpriteList[ i ]->SetDepth( m_iDepth );
			pNewSpriteList[ i ]->SetTransparency( 1 );
			pNewSpriteList[ i ]->SetColorF( m_fRed, m_fGreen, m_fBlue, m_fAlpha );
			pNewSpriteList[ i ]->FixToScreen( m_iFixed ); 

			if ( m_pFTSizedFont ) 
			{
				if ( m_iFlags & AGK_TEXT_SNAP_TO_PIXELS ) pNewSpriteList[ i ]->SetSnap( 1 );
				AGKFontImage *pFontImage = m_pFTSizedFont->GetCharImage( 32, 0 );
				pNewSpriteList[ i ]->SetFontImage( pFontImage, m_fFontScale );
			}
			else
			{
				if ( m_pFontImage )
				{
					if ( m_pLetterImages )
					{
						pNewSpriteList[ i ]->SetImage( m_pLetterImages[0] );
					}
					else
					{
						pNewSpriteList[ i ]->SetImage( m_pFontImage );
						pNewSpriteList[ i ]->SetAnimation( m_pFontImage->GetWidth()/16, m_pFontImage->GetHeight()/6, 96 );

						if ( m_pFontImageExt )
						{
							if ( !m_pLetterImagesExt )
							{
								pNewSpriteList[ i ]->AppendAnimation( m_pFontImageExt, m_pFontImageExt->GetWidth()/16, m_pFontImageExt->GetHeight()/8, 128 );
							}
						}
						else
						{
							if ( !m_pDefaultLettersExt )
							{
								pNewSpriteList[ i ]->AppendAnimation( m_pDefaultFontExt, m_pDefaultFontExt->GetWidth()/16, m_pDefaultFontExt->GetHeight()/8, 128 );
							}
						}
					}
				}
				else
				{
					if ( m_pDefaultLetters )
					{
						pNewSpriteList[ i ]->SetImage( m_pDefaultLetters[0] );
					}
					else
					{
						pNewSpriteList[ i ]->SetImage( m_pDefaultFont );
						pNewSpriteList[ i ]->SetAnimation( m_pDefaultFont->GetWidth()/16, m_pDefaultFont->GetHeight()/6, 96 );

						if ( m_pFontImageExt )
						{
							if ( !m_pLetterImagesExt )
							{
								pNewSpriteList[ i ]->AppendAnimation( m_pFontImageExt, m_pFontImageExt->GetWidth()/16, m_pFontImageExt->GetHeight()/8, 128 );
							}
						}
						else
						{
							if ( !m_pDefaultLettersExt )
							{
								pNewSpriteList[ i ]->AppendAnimation( m_pDefaultFontExt, m_pDefaultFontExt->GetWidth()/16, m_pDefaultFontExt->GetHeight()/8, 128 );
							}
						}
					}
				}

				pNewSpriteList[ i ]->SetSize( -1, m_fSize );
			}
		}

		if ( m_pSprites ) delete [] m_pSprites;
		m_pSprites = pNewSpriteList;
		m_iNumSprites = iLength;
	}

	if ( (m_iFlags & AGK_TEXT_REFRESHING) == 0 )
	{
		for ( uint32_t i = 0; i < m_iNumSprites; i++ ) m_pCharStyles[ i ] = 0;
	}

	if ( !m_pFontImage || !m_pFontImageExt )
	{
		// check if default font image has changed
		if ( m_pUsingDefaultFont != m_pDefaultFont || m_pUsingDefaultFontExt != m_pDefaultFontExt )
		{
			m_pUsingDefaultFont = m_pDefaultFont;
			m_pUsingDefaultFontExt = m_pDefaultFontExt;
			InternalRefresh();
		}
	}

	for( uint32_t i = 0; i < iLength; i++ )
	{
		int c = m_sText.CharAt(i);
		int orig = c;
		if ( c < 32 ) c = 32;
		if ( m_pFTSizedFont )
		{
			if ( orig < 32 )
			{
				m_pSprites[ i ]->SetFontImage( 0, 1 );
				if ( orig != 9 ) m_pSprites[ i ]->SetSize( 0, 0 );
				else 
				{
					// tab character
					float VwDw = agk::DeviceToDisplayRatioX();
					float width = m_pFTSizedFont->GetCharImage( 32, 0 )->GetDisplayAdvanceX() * VwDw * m_fFontScale;
					m_pSprites[ i ]->SetSize( width*4, 0 );
				}
			}
			else
			{
				int style = m_pCharStyles[i];
				if ( m_iFlags & AGK_TEXT_BOLD ) style |= 0x01;

				AGKFontImage *pFontImage = m_pFTSizedFont->GetCharImage( c, style );
				if ( !pFontImage ) pFontImage = m_pFTSizedFont->GetCharImage( 32, 0 );
				m_pSprites[ i ]->SetFontImage( pFontImage, m_fFontScale );
			}
		}
		else
		{
			if ( c > 0xFF )
			{
				// convert UTF8 back into Windows-1252 for old font image
				switch( c )
				{
					case 0x20AC: c = 0x80; break;
					case 0x201A: c = 0x82; break;
					case 0x192: c = 0x83; break;
					case 0x201E: c = 0x84; break;
					case 0x2026: c = 0x85; break;
					case 0x2020: c = 0x86; break;
					case 0x2021: c = 0x87; break;
					case 0x2C6: c = 0x88; break;
					case 0x2030: c = 0x89; break;
					case 0x160: c = 0x8A; break;
					case 0x2039: c = 0x8B; break;
					case 0x152: c = 0x8C; break;
					case 0x17D: c = 0x8E; break;
					case 0x2018: c = 0x91; break;
					case 0x2019: c = 0x92; break;
					case 0x201C: c = 0x93; break;
					case 0x201D: c = 0x94; break;
					case 0x2022: c = 0x95; break;
					case 0x2013: c = 0x96; break;
					case 0x2014: c = 0x97; break;
					case 0x2DC: c = 0x98; break;
					case 0x2122: c = 0x99; break;
					case 0x161: c = 0x9A; break;
					case 0x203A: c = 0x9B; break;
					case 0x153: c = 0x9C; break;
					case 0x17E: c = 0x9E; break;
					case 0x178: c = 0x9F; break;
					default: c = 63;
				}
			}
			c -= 32;

			if ( c < 96 )
			{
				if ( m_pFontImage )
				{
					if ( m_pLetterImages )
					{
						if ( m_pLetterImages[c] ) m_pSprites[ i ]->SetImage( m_pLetterImages[c] );
						else m_pSprites[ i ]->SetImage( m_pLetterImages[0] );
						m_pSprites[ i ]->SetSize( -1, m_fSize );
					}
					else
					{
						if ( m_pSprites[ i ]->GetFrameCount() > 0 ) m_pSprites[ i ]->SetFrame( c+1 );
					}
				}
				else
				{
					if ( m_pDefaultLetters )
					{
						if ( m_pDefaultLetters[c] ) m_pSprites[ i ]->SetImage( m_pDefaultLetters[c] );
						else m_pSprites[ i ]->SetImage( m_pDefaultLetters[0] );
						m_pSprites[ i ]->SetSize( -1, m_fSize );
					}
					else
					{
						if ( m_pSprites[ i ]->GetFrameCount() > 0 ) m_pSprites[ i ]->SetFrame( c+1 );
					}
				}
			}
			else
			{
				c -= 96;

				// extended letters
				if ( m_pFontImageExt )
				{
					if ( m_pLetterImagesExt )
					{
						if ( m_pLetterImagesExt[c] ) m_pSprites[ i ]->SetImage( m_pLetterImagesExt[c] );
						else m_pSprites[ i ]->SetImage( m_pLetterImagesExt[0] );
						m_pSprites[ i ]->SetSize( -1, m_fSize );
					}
					else
					{
						if ( m_pSprites[ i ]->GetFrameCount() > 0 ) m_pSprites[ i ]->SetFrame( c+96+1 );
					}
				}
				else
				{
					if ( m_pDefaultLettersExt )
					{
						if ( m_pDefaultLettersExt[c] ) m_pSprites[ i ]->SetImage( m_pDefaultLettersExt[c] );
						else m_pSprites[ i ]->SetImage( m_pDefaultLettersExt[0] );
						m_pSprites[ i ]->SetSize( -1, m_fSize );
					}
					else
					{
						if ( m_pSprites[ i ]->GetFrameCount() > 0 ) m_pSprites[ i ]->SetFrame( c+96+1 );
					}
				}
			}

			if ( orig < 32 ) m_pSprites[ i ]->SetSize( 0, m_pSprites[ i ]->GetHeight() );
			else m_pSprites[ i ]->SetSize( -1, m_pSprites[ i ]->GetHeight() );
		}
		
		m_pSprites[ i ]->SetVisible( (m_iFlags & AGK_TEXT_VISIBLE) != 0 );
		m_pSprites[ i ]->SetUVBorder( 0 );
	}

	// re-align the sprites
	ReAlignSprites();

	for ( uint32_t i = iLength; i < m_iNumSprites; i++ )
	{
		m_pSprites[ i ]->SetVisible( false );
	}
}

void cText::InternalRefresh()
{
	// doesn't apply to new FT fonts
	if ( m_pFTSizedFont ) return;

	for ( uint32_t i = 0; i < m_iNumSprites; i++ )
	{
		m_pSprites[ i ]->ClearAnimationFrames();
		m_pSprites[ i ]->SetImage( 0 ); // must set to 0 first to avoid deleted image pointer problem
	}
		
	for ( uint32_t i = 0; i < m_iNumSprites; i++ )
	{
		if ( m_pFontImage )
		{
			if ( !m_pLetterImages )
			{
				m_pSprites[ i ]->SetImage( m_pFontImage );
				m_pSprites[ i ]->SetAnimation( m_pFontImage->GetWidth()/16, m_pFontImage->GetHeight()/6, 96 );

				if ( m_pFontImageExt )
				{
					if ( !m_pLetterImagesExt )
					{
						m_pSprites[ i ]->AppendAnimation( m_pFontImageExt, m_pFontImageExt->GetWidth()/16, m_pFontImageExt->GetHeight()/8, 128 );
					}
				}
				else
				{
					if ( !m_pDefaultLettersExt )
					{
						m_pSprites[ i ]->AppendAnimation( m_pDefaultFontExt, m_pDefaultFontExt->GetWidth()/16, m_pDefaultFontExt->GetHeight()/8, 128 );
					}
				}
			}
		}
		else
		{
			if ( !m_pDefaultLetters )
			{
				m_pSprites[ i ]->SetImage( m_pDefaultFont );
				m_pSprites[ i ]->SetAnimation( m_pDefaultFont->GetWidth()/16, m_pDefaultFont->GetHeight()/6, 96 );

				if ( m_pFontImageExt )
				{
					if ( !m_pLetterImagesExt )
					{
						m_pSprites[ i ]->AppendAnimation( m_pFontImageExt, m_pFontImageExt->GetWidth()/16, m_pFontImageExt->GetHeight()/8, 128 );
					}
				}
				else
				{
					if ( !m_pDefaultLettersExt )
					{
						m_pSprites[ i ]->AppendAnimation( m_pDefaultFontExt, m_pDefaultFontExt->GetWidth()/16, m_pDefaultFontExt->GetHeight()/8, 128 );
					}
				}
			}
		}
	}
}

void cText::Refresh()
{
	m_iFlags |= AGK_TEXT_REFRESHING;
	InternalRefresh();
	SetString( m_sText.GetStr() );
	m_iFlags &= ~AGK_TEXT_REFRESHING;
}

void cText::SetPosition( float fX, float fY )
{
	float oldX = m_fSpriteX;
	float oldY = m_fSpriteY;
	m_fX = fX;
	m_fY = fY;
	ShiftPosition( fX - oldX, fY - oldY );
}

void cText::ReAlignSprites()
{
	uint32_t letters = m_sText.GetNumChars();

	if ( m_iFlags & AGK_TEXT_SNAP_TO_PIXELS )
	{
		int iSpriteX = agk::Round(m_fX / agk::DeviceToDisplayRatioX());
		m_fSpriteX = iSpriteX * agk::DeviceToDisplayRatioX();

		int iSpriteY = agk::Round(m_fY / agk::DeviceToDisplayRatioY());
		m_fSpriteY = iSpriteY * agk::DeviceToDisplayRatioY();
	}
	else
	{
		m_fSpriteX = m_fX;
		m_fSpriteY = m_fY;
	}

	m_iLines = 1;
	m_fTotalWidth = 0;
	m_fTotalHeight = 0;
	float currWidth = 0;
	float lineWidth = 0;
	float letterWidth = 0;

	float stretch = agk::m_fStretchValue;
	float fSinA = 0;
	float fCosA = 1;
	float fSinA1 = 0;
	float fSinA2 = 0;

	if ( m_fAngle != 0 )
	{
		stretch = agk::m_fStretchValue;
		fSinA = agk::SinRad(m_fAngle);
		fCosA = agk::CosRad(m_fAngle);
		fSinA1 = fSinA/stretch;
		fSinA2 = fSinA*stretch;
	}

	const AGKFontImage *pFontImage = 0;
	if ( m_iAlign < 0 ) m_iAlign = 0;
	if ( m_iAlign > 2 ) m_iAlign = 2;
	switch ( m_iAlign )
	{
		case 0:
		{
			// left
			uint32_t first = 0;
			uint32_t curr = 0;
			int lastSpace = -1;
			float y = m_fSpriteY;
			if ( letters > 0 ) 
			{
				letterWidth = m_pSprites[ 0 ]->GetFontImage() ? m_pSprites[ 0 ]->GetFontImage()->GetDisplayAdvanceX()*m_fFontScale : m_pSprites[ 0 ]->GetWidth();
				lineWidth = letterWidth;
			}
			
			// look for new line characters and position line by line
			while ( curr < letters )
			{
				if ( curr < letters-1 ) 
				{
					letterWidth = m_pSprites[ curr+1 ]->GetFontImage() ? m_pSprites[ curr+1 ]->GetFontImage()->GetDisplayAdvanceX()*m_fFontScale : m_pSprites[ curr+1 ]->GetWidth();
					lineWidth += m_fSpacing + letterWidth;
				}

				bool bLineBreak = false;
				if ( m_sText.CharAt( curr ) == TEXT_SPACE ) lastSpace = curr;
				if ( (m_fMaxWidth > 0 && lineWidth > m_fMaxWidth) || (m_fMaxWidth < 0 && (curr-first) > abs(m_fMaxWidth)) )
				{
					if ( m_sText.CharAt( curr ) != TEXT_NEWLINE )
					{
						if ( lastSpace > (int)first ) curr = lastSpace;
					}
					bLineBreak = true;
				}

				if ( m_sText.CharAt( curr ) == TEXT_NEWLINE || bLineBreak )
				{
					// position this line, indices first->curr
					for ( uint32_t i = first; i <= curr; i++ )
					{
						float offsetX = m_pSprites[ i ]->GetFontImage() ? m_pSprites[ i ]->GetFontImage()->GetDisplayOffsetX()*m_fFontScale : 0;
						float offsetY = m_pSprites[ i ]->GetFontImage() ? m_pSprites[ i ]->GetFontImage()->GetDisplayOffsetY()*m_fFontScale : 0;

						if ( m_fAngle == 0 )
						{
							m_pSprites[ i ]->SetPosition( m_fSpriteX + currWidth + offsetX, y + offsetY );
						}
						else
						{
							float fTempX = currWidth + m_pSprites[ i ]->GetOffsetX() + offsetX;
							float fTempY = y - m_fSpriteY + m_pSprites[ i ]->GetOffsetY() + offsetY;
							
							float fNewX = fTempX*fCosA - fTempY*fSinA1 - m_pSprites[ i ]->GetOffsetX();
							float fNewY = fTempY*fCosA + fTempX*fSinA2 - m_pSprites[ i ]->GetOffsetY();

							m_pSprites[ i ]->SetPosition( m_fSpriteX + fNewX, m_fSpriteY + fNewY );
						}

						letterWidth = m_pSprites[ i ]->GetFontImage() ? m_pSprites[ i ]->GetFontImage()->GetDisplayAdvanceX()*m_fFontScale : m_pSprites[ i ]->GetWidth();
						currWidth += letterWidth + m_fSpacing;
					}

					if ( currWidth > 0 ) currWidth -= m_fSpacing;
					if ( currWidth > m_fTotalWidth ) m_fTotalWidth = currWidth;
					currWidth = 0;

					// setup next line
					lastSpace = -1;
					first = curr+1;
					y += m_fSize+m_fVSpacing;
					lineWidth = 0;
					if ( first < letters ) 
					{
						letterWidth = m_pSprites[ first ]->GetFontImage() ? m_pSprites[ first ]->GetFontImage()->GetDisplayAdvanceX()*m_fFontScale : m_pSprites[ first ]->GetWidth();
						lineWidth = letterWidth;
					}
					m_iLines++;
				}

				curr++;
			}

			if ( first < curr ) 
			{
				// final line (and first if no new line characters found)
				for ( uint32_t i = first; i < curr; i++ )
				{
					float offsetX = m_pSprites[ i ]->GetFontImage() ? m_pSprites[ i ]->GetFontImage()->GetDisplayOffsetX()*m_fFontScale : 0;
					float offsetY = m_pSprites[ i ]->GetFontImage() ? m_pSprites[ i ]->GetFontImage()->GetDisplayOffsetY()*m_fFontScale : 0;

					if ( m_fAngle == 0 )
					{
						m_pSprites[ i ]->SetPosition( m_fSpriteX + currWidth + offsetX, y + offsetY );
					}
					else
					{
						float fTempX = currWidth + m_pSprites[ i ]->GetOffsetX() + offsetX;
						float fTempY = y - m_fSpriteY + m_pSprites[ i ]->GetOffsetY() + offsetY;
						
						float fNewX = fTempX*fCosA - fTempY*fSinA1 - m_pSprites[ i ]->GetOffsetX();
						float fNewY = fTempY*fCosA + fTempX*fSinA2 - m_pSprites[ i ]->GetOffsetY();

						m_pSprites[ i ]->SetPosition( m_fSpriteX + fNewX, m_fSpriteY + fNewY );
					}

					letterWidth = m_pSprites[ i ]->GetFontImage() ? m_pSprites[ i ]->GetFontImage()->GetDisplayAdvanceX()*m_fFontScale : m_pSprites[ i ]->GetWidth();
					currWidth += letterWidth + m_fSpacing;
				}

				currWidth -= m_fSpacing;
				if ( currWidth > m_fTotalWidth ) m_fTotalWidth = currWidth;
				m_fTotalHeight = (y - m_fSpriteY) + m_fSize;
			}
			else 
			{
				m_fTotalHeight = (y - m_fSpriteY) - m_fVSpacing;
			}
			break;
		}
		case 1:
		{
			// center
			uint32_t first = 0;
			uint32_t curr = 0;
			int lastSpace = -1;
			float y = m_fSpriteY;
			if ( letters > 0 ) 
			{
				letterWidth = m_pSprites[ 0 ]->GetFontImage() ? m_pSprites[ 0 ]->GetFontImage()->GetDisplayAdvanceX()*m_fFontScale : m_pSprites[ 0 ]->GetWidth();
				lineWidth = letterWidth;
			}

			// look for new line characters and position line by line
			while ( curr < letters )
			{
				if ( curr < letters-1 ) 
				{
					letterWidth = m_pSprites[ curr+1 ]->GetFontImage() ? m_pSprites[ curr+1 ]->GetFontImage()->GetDisplayAdvanceX()*m_fFontScale : m_pSprites[ curr+1 ]->GetWidth();
					lineWidth += m_fSpacing + letterWidth;
				}

				bool bLineBreak = false;
				if ( m_sText.CharAt( curr ) == TEXT_SPACE ) lastSpace = curr;
				if ( (m_fMaxWidth > 0 && lineWidth > m_fMaxWidth) || (m_fMaxWidth < 0 && (curr-first) > abs(m_fMaxWidth)) )
				{
					if ( m_sText.CharAt( curr ) != TEXT_NEWLINE )
					{
						if ( lastSpace > (int)first ) curr = lastSpace;
					}
					bLineBreak = true;
				}

				if ( m_sText.CharAt( curr ) == TEXT_NEWLINE || bLineBreak )
				{
					currWidth = 0;
					for( uint32_t i = first; i <= curr; i++ )
					{
						letterWidth = m_pSprites[ i ]->GetFontImage() ? m_pSprites[ i ]->GetFontImage()->GetDisplayAdvanceX()*m_fFontScale : m_pSprites[ i ]->GetWidth();
						currWidth += letterWidth + m_fSpacing;
					}

					if ( currWidth > 0 ) currWidth -= m_fSpacing;
					if ( currWidth > m_fTotalWidth ) m_fTotalWidth = currWidth;

					// position this line, indices first->curr
					float pos = -currWidth/2.0f;
					if ( m_sText.CharAt( curr ) == TEXT_NEWLINE || m_sText.CharAt( curr ) == TEXT_SPACE ) 
					{
						letterWidth = m_pSprites[ curr ]->GetFontImage() ? m_pSprites[ curr ]->GetFontImage()->GetDisplayAdvanceX()*m_fFontScale : m_pSprites[ curr ]->GetWidth();
						pos += (letterWidth + m_fSpacing)/2.0f;
					}

					for ( uint32_t i = first; i <= curr; i++ )
					{
						float offsetX = m_pSprites[ i ]->GetFontImage() ? m_pSprites[ i ]->GetFontImage()->GetDisplayOffsetX()*m_fFontScale : 0;
						float offsetY = m_pSprites[ i ]->GetFontImage() ? m_pSprites[ i ]->GetFontImage()->GetDisplayOffsetY()*m_fFontScale : 0;

						if ( m_fAngle == 0 )
						{
							m_pSprites[ i ]->SetPosition( m_fSpriteX + pos + offsetX, y + offsetY );
						}
						else
						{
							float fTempX = pos + m_pSprites[ i ]->GetOffsetX() + offsetX;
							float fTempY = y - m_fSpriteY + m_pSprites[ i ]->GetOffsetY() + offsetY;
							
							float fNewX = fTempX*fCosA - fTempY*fSinA1 - m_pSprites[ i ]->GetOffsetX();
							float fNewY = fTempY*fCosA + fTempX*fSinA2 - m_pSprites[ i ]->GetOffsetY();

							m_pSprites[ i ]->SetPosition( m_fSpriteX + fNewX, m_fSpriteY + fNewY );
						}

						letterWidth = m_pSprites[ i ]->GetFontImage() ? m_pSprites[ i ]->GetFontImage()->GetDisplayAdvanceX()*m_fFontScale : m_pSprites[ i ]->GetWidth();
						pos += letterWidth + m_fSpacing;
					}

					// setup next line
					lastSpace = -1;
					first = curr+1;
					y += m_fSize+m_fVSpacing;
					currWidth = 0;
					lineWidth = 0;
					if ( first < letters ) 
					{
						letterWidth = m_pSprites[ first ]->GetFontImage() ? m_pSprites[ first ]->GetFontImage()->GetDisplayAdvanceX()*m_fFontScale : m_pSprites[ first ]->GetWidth();
						lineWidth = letterWidth;
					}
					m_iLines++;
				}
				else
				{
					letterWidth = m_pSprites[ curr ]->GetFontImage() ? m_pSprites[ curr ]->GetFontImage()->GetDisplayAdvanceX()*m_fFontScale : m_pSprites[ curr ]->GetWidth();
					currWidth += letterWidth + m_fSpacing;
				}

				curr++;
			}

			if ( currWidth > 0 ) 
			{
				currWidth -= m_fSpacing;
				if ( currWidth > m_fTotalWidth ) m_fTotalWidth = currWidth;

				// final line (and first if no new line characters found)
				float pos = -currWidth/2.0f;
				for ( uint32_t i = first; i < curr; i++ )
				{
					float offsetX = m_pSprites[ i ]->GetFontImage() ? m_pSprites[ i ]->GetFontImage()->GetDisplayOffsetX()*m_fFontScale : 0;
					float offsetY = m_pSprites[ i ]->GetFontImage() ? m_pSprites[ i ]->GetFontImage()->GetDisplayOffsetY()*m_fFontScale : 0;

					if ( m_fAngle == 0 )
					{
						m_pSprites[ i ]->SetPosition( m_fSpriteX + pos + offsetX, y + offsetY );
					}
					else
					{
						float fTempX = pos + m_pSprites[ i ]->GetOffsetX() + offsetX;
						float fTempY = y - m_fSpriteY + m_pSprites[ i ]->GetOffsetY() + offsetY;
						
						float fNewX = fTempX*fCosA - fTempY*fSinA1 - m_pSprites[ i ]->GetOffsetX();
						float fNewY = fTempY*fCosA + fTempX*fSinA2 - m_pSprites[ i ]->GetOffsetY();

						m_pSprites[ i ]->SetPosition( m_fSpriteX + fNewX, m_fSpriteY + fNewY );
					}

					letterWidth = m_pSprites[ i ]->GetFontImage() ? m_pSprites[ i ]->GetFontImage()->GetDisplayAdvanceX()*m_fFontScale : m_pSprites[ i ]->GetWidth();
					pos += letterWidth + m_fSpacing;
				}

				m_fTotalHeight = (y - m_fSpriteY) + m_fSize;
			}
			else
			{
				m_fTotalHeight = (y - m_fSpriteY) - m_fVSpacing;
			}
			
			break;
		}
		case 2:
		{
			// right
			uint32_t first = 0;
			uint32_t curr = 0;
			int lastSpace = -1;
			float y = m_fSpriteY;
			if ( letters > 0 ) 
			{
				letterWidth = m_pSprites[ 0 ]->GetFontImage() ? m_pSprites[ 0 ]->GetFontImage()->GetDisplayAdvanceX()*m_fFontScale : m_pSprites[ 0 ]->GetWidth();
				lineWidth = letterWidth;
			}

			// look for new line characters and position line by line
			while ( curr < letters )
			{
				if ( curr < letters-1 ) 
				{
					letterWidth = m_pSprites[ curr+1 ]->GetFontImage() ? m_pSprites[ curr+1 ]->GetFontImage()->GetDisplayAdvanceX()*m_fFontScale : m_pSprites[ curr+1 ]->GetWidth();
					lineWidth += m_fSpacing + letterWidth;
				}

				bool bLineBreak = false;
				if ( m_sText.CharAt( curr ) == TEXT_SPACE ) lastSpace = curr;
				if ( (m_fMaxWidth > 0 && lineWidth > m_fMaxWidth) || (m_fMaxWidth < 0 && (curr-first) > abs(m_fMaxWidth)) )
				{
					if ( m_sText.CharAt( curr ) != TEXT_NEWLINE )
					{
						if ( lastSpace > (int)first ) curr = lastSpace;
					}
					bLineBreak = true;
				}

				if ( m_sText.CharAt( curr ) == TEXT_NEWLINE || bLineBreak )
				{
					currWidth = 0;
					for( uint32_t i = first; i <= curr; i++ )
					{
						letterWidth = m_pSprites[ i ]->GetFontImage() ? m_pSprites[ i ]->GetFontImage()->GetDisplayAdvanceX()*m_fFontScale : m_pSprites[ i ]->GetWidth();
						currWidth += letterWidth + m_fSpacing;
					}

					if ( currWidth > 0 ) currWidth -= m_fSpacing;
					if ( currWidth > m_fTotalWidth ) m_fTotalWidth = currWidth;

					// position this line, indices first->curr
					float pos = -currWidth;
					if ( m_sText.CharAt( curr ) == TEXT_NEWLINE || m_sText.CharAt( curr ) == TEXT_SPACE ) 
					{
						letterWidth = m_pSprites[ curr ]->GetFontImage() ? m_pSprites[ curr ]->GetFontImage()->GetDisplayAdvanceX()*m_fFontScale : m_pSprites[ curr ]->GetWidth();
						pos += letterWidth + m_fSpacing;
					}

					for ( uint32_t i = first; i <= curr; i++ )
					{
						float offsetX = m_pSprites[ i ]->GetFontImage() ? m_pSprites[ i ]->GetFontImage()->GetDisplayOffsetX()*m_fFontScale : 0;
						float offsetY = m_pSprites[ i ]->GetFontImage() ? m_pSprites[ i ]->GetFontImage()->GetDisplayOffsetY()*m_fFontScale : 0;

						if ( m_fAngle == 0 )
						{
							m_pSprites[ i ]->SetPosition( m_fSpriteX + pos + offsetX, y + offsetY );
						}
						else
						{
							float fTempX = pos + m_pSprites[ i ]->GetOffsetX() + offsetX;
							float fTempY = y - m_fSpriteY + m_pSprites[ i ]->GetOffsetY() + offsetY;
							
							float fNewX = fTempX*fCosA - fTempY*fSinA1 - m_pSprites[ i ]->GetOffsetX();
							float fNewY = fTempY*fCosA + fTempX*fSinA2 - m_pSprites[ i ]->GetOffsetY();

							m_pSprites[ i ]->SetPosition( m_fSpriteX + fNewX, m_fSpriteY + fNewY );
						}

						letterWidth = m_pSprites[ i ]->GetFontImage() ? m_pSprites[ i ]->GetFontImage()->GetDisplayAdvanceX()*m_fFontScale : m_pSprites[ i ]->GetWidth();
						pos += letterWidth + m_fSpacing;
					}

					// setup next line
					lastSpace = -1;
					first = curr+1;
					y += m_fSize+m_fVSpacing;
					currWidth = 0;
					lineWidth = 0;
					if ( first < letters ) 
					{
						letterWidth = m_pSprites[ first ]->GetFontImage() ? m_pSprites[ first ]->GetFontImage()->GetDisplayAdvanceX()*m_fFontScale : m_pSprites[ first ]->GetWidth();
						lineWidth = letterWidth;
					}
					m_iLines++;
				}
				else
				{
					letterWidth = m_pSprites[ curr ]->GetFontImage() ? m_pSprites[ curr ]->GetFontImage()->GetDisplayAdvanceX()*m_fFontScale : m_pSprites[ curr ]->GetWidth();
					currWidth += letterWidth + m_fSpacing;
				}

				curr++;
			}

			if ( currWidth > 0 ) 
			{
				currWidth -= m_fSpacing;
				if ( currWidth > m_fTotalWidth ) m_fTotalWidth = currWidth;

				// final line (and first if no new line characters found)
				float pos = -currWidth;
				for ( uint32_t i = first; i < curr; i++ )
				{
					float offsetX = m_pSprites[ i ]->GetFontImage() ? m_pSprites[ i ]->GetFontImage()->GetDisplayOffsetX()*m_fFontScale : 0;
					float offsetY = m_pSprites[ i ]->GetFontImage() ? m_pSprites[ i ]->GetFontImage()->GetDisplayOffsetY()*m_fFontScale : 0;

					if ( m_fAngle == 0 )
					{
						m_pSprites[ i ]->SetPosition( m_fSpriteX + pos + offsetX, y + offsetY );
					}
					else
					{
						float fTempX = pos + m_pSprites[ i ]->GetOffsetX() + offsetX;
						float fTempY = y - m_fSpriteY + m_pSprites[ i ]->GetOffsetY() + offsetY;
						
						float fNewX = fTempX*fCosA - fTempY*fSinA1 - m_pSprites[ i ]->GetOffsetX();
						float fNewY = fTempY*fCosA + fTempX*fSinA2 - m_pSprites[ i ]->GetOffsetY();

						m_pSprites[ i ]->SetPosition( m_fSpriteX + fNewX, m_fSpriteY + fNewY );
					}

					letterWidth = m_pSprites[ i ]->GetFontImage() ? m_pSprites[ i ]->GetFontImage()->GetDisplayAdvanceX()*m_fFontScale : m_pSprites[ i ]->GetWidth();
					pos += letterWidth + m_fSpacing;
				}

				m_fTotalHeight = (y - m_fSpriteY) + m_fSize;
			}
			else
			{
				m_fTotalHeight = (y - m_fSpriteY) - m_fVSpacing;
			}
			break;
		}
	}

	for ( uint32_t i = 0; i < m_iNumSprites; i++ )
	{
		m_pSprites[ i ]->SetAngleRad( m_fAngle );
	}
}

void cText::SetX( float fX )
{
	float oldX = m_fSpriteX;
	m_fX = fX;
	// don't do a full rebuild of position data
	ShiftPosition( fX - oldX, 0 );
}

void cText::SetY( float fY )
{
	float oldY = m_fSpriteY;
	m_fY = fY;
	ShiftPosition( 0, fY - oldY );
}

void cText::SetAngle( float fA )
{
	m_fAngle = fA * PI / 180.0f;
	ReAlignSprites();
}

void cText::SetAngleRad( float fA )
{
	m_fAngle = fA;
	ReAlignSprites();
}

void cText::ChangedAspect()
{
	SetSize( m_fOrigSize );
}

void cText::ShiftPosition( float fDiffX, float fDiffY )
{
	if ( m_iFlags & AGK_TEXT_SNAP_TO_PIXELS )
	{
		int iDiffX = agk::Round(fDiffX / agk::DeviceToDisplayRatioX());
		int iDiffY = agk::Round(fDiffY / agk::DeviceToDisplayRatioY());
		if ( iDiffX == 0 && iDiffY == 0 ) return;

		fDiffX = iDiffX * agk::DeviceToDisplayRatioX();
		fDiffY = iDiffY * agk::DeviceToDisplayRatioY();
	}

	m_fSpriteX += fDiffX;
	m_fSpriteY += fDiffY;

	for ( uint32_t i = 0; i < m_iNumSprites; i++ )
	{
		m_pSprites[ i ]->SetPosition( m_pSprites[ i ]->GetX() + fDiffX, m_pSprites[ i ]->GetY() + fDiffY );
	}
}

void cText::SetSize( float fSize )
{
	if ( fSize < 0 ) fSize = 0;
	m_fOrigSize = fSize;
	m_fSize = fSize;
	int updateRequired = 0;

	if ( m_pFTFont )
	{
		if ( m_fHorizontalRatio != agk::DeviceToDisplayRatioX() || m_fVerticalRatio != agk::DeviceToDisplayRatioY() ) updateRequired = 1;

		m_fHorizontalRatio = agk::DeviceToDisplayRatioX();
		m_fVerticalRatio = agk::DeviceToDisplayRatioY();
		
		int pixelSize = agk::Round( m_fOrigSize/m_fVerticalRatio );
		m_fSize = pixelSize * m_fVerticalRatio;

		AGKSizedFont *pNewFont = m_pFTFont->GetSizedFont( pixelSize );
		float newScale = m_fFontScale;
		if ( pNewFont ) newScale = pixelSize / (float) pNewFont->GetSize();

		if ( pNewFont != m_pFTSizedFont ) 
		{
			m_fFontScale = newScale;

			for ( uint32_t i = 0; i < m_iNumSprites; i++ )
			{
				m_pSprites[ i ]->SetFontImage( 0, 1 );
			}
			
			if ( m_pFTSizedFont ) m_pFTSizedFont->Release();
			m_pFTSizedFont = pNewFont;
			if ( m_pFTSizedFont ) m_pFTSizedFont->AddRef();

			// have to reassign all sprite images
			Refresh();
		}
		else
		{
			if ( m_fFontScale != newScale || updateRequired )
			{
				for ( uint32_t i = 0; i < m_iNumSprites; i++ )
				{
					if ( m_pSprites[ i ]->GetImagePtr() )
					{
						float width = m_pSprites[ i ]->GetImagePtr()->GetWidth() * m_fHorizontalRatio * newScale;
						float height = m_pSprites[ i ]->GetImagePtr()->GetHeight() * m_fVerticalRatio * newScale;
						m_pSprites[ i ]->SetSize( width, height );
					}
					else
					{
						float adjust = 1;
						if ( m_fFontScale != 0 ) adjust = newScale / m_fFontScale;
						m_pSprites[ i ]->SetSize( m_pSprites[i]->GetWidth()*adjust, m_pSprites[i]->GetHeight()*adjust );
					}
				}

				m_fFontScale = newScale;
				ReAlignSprites();
			}
		}
	}

	// if for some reason we failed to get a sized font then fall back to bitmap fonts
	if ( !m_pFTSizedFont )
	{
		for ( uint32_t i = 0; i < m_iNumSprites; i++ )
		{
			m_pSprites[ i ]->SetSize( -1, m_fSize );
		}

		ReAlignSprites();
	}
}

void cText::SetMaxWidth( float width )
{
	if ( width < 0 ) 
	{
		// LEE: new feature, when negative, will wrap on character count, not pixel width
		m_fMaxWidth = width;
	}
	else
	{
		m_fMaxWidth = width;
	}
	
	ReAlignSprites();
}

void cText::SetBold( uint32_t bold )
{
	int changed = (m_iFlags & AGK_TEXT_BOLD) ^ (bold ? AGK_TEXT_BOLD : 0);
	if ( !changed ) return;

	if ( bold ) m_iFlags |= AGK_TEXT_BOLD;
	else m_iFlags &= ~AGK_TEXT_BOLD;

	Refresh();
}

void cText::SetSnap( int mode ) 
{ 
	if ( mode ) m_iFlags |= AGK_TEXT_SNAP_TO_PIXELS; 
	else m_iFlags &= ~AGK_TEXT_SNAP_TO_PIXELS; 

	for ( uint32_t i = 0; i < m_iNumSprites; i++ )
	{
		m_pSprites[ i ]->SetSnap( mode );
	}
}

void cText::SetSpacing( float fSpacing )
{
	m_fSpacing = fSpacing;
	ReAlignSprites();
}

void cText::SetLineSpacing( float fSpacing )
{
	m_fVSpacing = fSpacing;
	ReAlignSprites();
}

// 0=left, 1=center, 2=right
void cText::SetAlignment( int iMode )
{
	if ( iMode < 0 ) iMode = 0;
	if ( iMode > 2 ) iMode = 2;

	m_iAlign = iMode;
	ReAlignSprites();
}

void cText::SetDepth( int iDepth )
{
	m_iDepth = iDepth;

	for ( uint32_t i = 0; i < m_iNumSprites; i++ )
	{
		m_pSprites[ i ]->SetDepth( m_iDepth );
	}
}

void cText::SetVisible( bool bVisible )
{
	if ( bVisible ) m_iFlags |= AGK_TEXT_VISIBLE;
	else m_iFlags &= ~AGK_TEXT_VISIBLE;

	for ( uint32_t i = 0; i < m_sText.GetNumChars(); i++ )
	{
		m_pSprites[ i ]->SetVisible( bVisible );
	}
}

void cText::SetColor( uint32_t iRed, uint32_t iGreen, uint32_t iBlue, uint32_t iAlpha )
{
	m_fRed = iRed / 255.0f;
	m_fGreen = iGreen / 255.0f;
	m_fBlue = iBlue / 255.0f; 
	m_fAlpha = iAlpha / 255.0f;

	for ( uint32_t i = 0; i < m_iNumSprites; i++ )
	{
		m_pSprites[ i ]->SetColorF( m_fRed, m_fGreen, m_fBlue, m_fAlpha );
	}
}

void cText::SetRed( uint32_t iRed )
{
	m_fRed = iRed / 255.0f;
	
	for ( uint32_t i = 0; i < m_iNumSprites; i++ )
	{
		m_pSprites[ i ]->SetRedF( m_fRed );
	}
}

void cText::SetGreen( uint32_t iGreen )
{
	m_fGreen = iGreen / 255.0f;
	
	for ( uint32_t i = 0; i < m_iNumSprites; i++ )
	{
		m_pSprites[ i ]->SetGreenF( m_fGreen );
	}
}

void cText::SetBlue( uint32_t iBlue )
{
	m_fBlue = iBlue / 255.0f;
	
	for ( uint32_t i = 0; i < m_iNumSprites; i++ )
	{
		m_pSprites[ i ]->SetBlueF( m_fBlue );
	}
}

void cText::SetAlpha( uint32_t iAlpha )
{
	m_fAlpha = iAlpha / 255.0f;
	
	for ( uint32_t i = 0; i < m_iNumSprites; i++ )
	{
		m_pSprites[ i ]->SetAlphaF( m_fAlpha );
	}
}


void cText::SetTransparency( int iMode )
{
	if ( iMode == m_iTransparency ) return;
	m_iTransparency = iMode;

	switch( m_iTransparency )
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

void cText::SetFontImage( cImage *pImage )
{
	// clear out freetype font
	if ( m_pFTFont )
	{
		if ( m_pFTSizedFont )
		{
			for ( uint32_t i = 0; i < m_iNumSprites; i++ )
			{
				m_pSprites[ i ]->SetFontImage( 0, 1 );
			}

			m_pFTSizedFont->Release();
			m_pFTSizedFont = 0;
		}
		m_pFTFont = 0;

		// switching from new fonts to bitmap fonts
		SetSnap( 0 );
	}

	if ( m_pLetterImages ) m_pLetterImages = 0;

	if ( m_pFontImage != pImage ) 
	{
		if ( m_pFontImage ) m_pFontImage->RemoveText( this );
		if ( pImage ) pImage->AddText( this );
	}

	if ( !pImage )
	{
		m_pFontImage = 0;
		m_iImageID = 0;
	}
	else
	{	
		m_pFontImage = pImage;
		m_iImageID = m_pFontImage->GetID();
		m_pLetterImages = m_pFontImage->GetFontImages();
	}
	
	Refresh();

	if ( !(m_iFlags & AGK_TEXT_CUSTOM_SHADER) ) InternalSetShader( 0 );
}

void cText::SetExtendedFontImage( cImage *pImage )
{
	if ( m_pLetterImagesExt ) m_pLetterImagesExt = 0;

	if ( m_pFontImageExt != pImage ) 
	{
		if ( m_pFontImageExt ) m_pFontImageExt->RemoveText( this );
		if ( pImage ) pImage->AddText( this );
	}

	if ( !pImage )
	{
		m_pFontImageExt = 0;
	}
	else
	{
		m_pFontImageExt = pImage;
		m_pLetterImagesExt = m_pFontImageExt->GetExtendedFontImages();
	}
	
	Refresh();
}

void cText::SetFont( AGKFont *pFont )
{
	// clear out bitmap font
	if ( m_pLetterImages ) m_pLetterImages = 0;
	if ( m_pFontImage ) m_pFontImage->RemoveText( this );

	m_pFontImage = 0;
	m_iImageID = 0;

	if ( !pFont ) pFont = AGKFont::GetDefaultFont();
	if ( m_pFTFont == pFont ) return;
	if ( m_pFTFont )
	{
		if ( m_pFTSizedFont )
		{
			for ( uint32_t i = 0; i < m_iNumSprites; i++ )
			{
				m_pSprites[ i ]->SetFontImage( 0, 1 );
			}

			m_pFTSizedFont->Release();
			m_pFTSizedFont = 0;
		}
	}
	else
	{
		// switching from bitmap fonts to new fonts
		SetSnap( 1 );
	}

	m_pFTFont = pFont;

	if ( m_pFTFont )
	{
		SetSize( m_fOrigSize ); // this regenerates the font for all sprites
	}

	if ( !(m_iFlags & AGK_TEXT_CUSTOM_SHADER) ) InternalSetShader( 0 );
}

void cText::SetCharPosition( uint32_t iIndex, float x, float y )
{
	if ( iIndex >= m_iNumSprites ) return;

	float offsetX = m_pSprites[ iIndex ]->GetFontImage() ? m_pSprites[ iIndex ]->GetFontImage()->GetDisplayOffsetX()*m_fFontScale : 0;
	float offsetY = m_pSprites[ iIndex ]->GetFontImage() ? m_pSprites[ iIndex ]->GetFontImage()->GetDisplayOffsetY()*m_fFontScale : 0;
	m_pSprites[ iIndex ]->SetPosition( m_fX + x + offsetX, m_fY + y + offsetY );
}

void cText::SetCharX( uint32_t iIndex, float x )
{
	if ( iIndex >= m_iNumSprites ) return;

	float offsetX = m_pSprites[ iIndex ]->GetFontImage() ? m_pSprites[ iIndex ]->GetFontImage()->GetDisplayOffsetX()*m_fFontScale : 0;
	m_pSprites[ iIndex ]->SetX( m_fX + x + offsetX );
}

void cText::SetCharY( uint32_t iIndex, float y )
{
	if ( iIndex >= m_iNumSprites ) return;

	float offsetY = m_pSprites[ iIndex ]->GetFontImage() ? m_pSprites[ iIndex ]->GetFontImage()->GetDisplayOffsetY()*m_fFontScale : 0;
	m_pSprites[ iIndex ]->SetY( m_fY + y + offsetY );
}

void cText::SetCharAngle( uint32_t iIndex, float angle )
{
	if ( iIndex >= m_iNumSprites ) return;

	m_pSprites[ iIndex ]->SetAngle( angle );
}

void cText::SetCharAngleRad( uint32_t iIndex, float angle )
{
	if ( iIndex >= m_iNumSprites ) return;

	m_pSprites[ iIndex ]->SetAngleRad( angle );
}

void cText::SetCharColor( uint32_t iIndex, uint32_t red, uint32_t green, uint32_t blue, uint32_t alpha )
{
	if ( iIndex >= m_iNumSprites ) return;

	m_pSprites[ iIndex ]->SetColor( red, green, blue, alpha );
}

void cText::SetCharRed( uint32_t iIndex, uint32_t red )
{
	if ( iIndex >= m_iNumSprites ) return;

	m_pSprites[ iIndex ]->SetRed( red );
}

void cText::SetCharGreen( uint32_t iIndex, uint32_t green )
{
	if ( iIndex >= m_iNumSprites ) return;

	m_pSprites[ iIndex ]->SetGreen( green );
}

void cText::SetCharBlue( uint32_t iIndex, uint32_t blue )
{
	if ( iIndex >= m_iNumSprites ) return;

	m_pSprites[ iIndex ]->SetBlue( blue );
}

void cText::SetCharAlpha( uint32_t iIndex, uint32_t alpha )
{
	if ( iIndex >= m_iNumSprites ) return;

	m_pSprites[ iIndex ]->SetAlpha( alpha );
}

void cText::SetCharBold( uint32_t iIndex, uint32_t bold )
{
	if ( iIndex >= m_sText.GetNumChars() ) return;

	int changed = (m_pCharStyles[ iIndex ] & 0x1) ^ (bold ? 1 : 0);
	if ( !changed ) return;

	if ( bold ) m_pCharStyles[ iIndex ] |= 0x01;
	else m_pCharStyles[ iIndex ] &= ~0x01;

	if ( m_pFTSizedFont )
	{
		int c = m_sText.CharAt(iIndex);
		if ( c >= 32 )
		{
			int style = m_pCharStyles[iIndex];
			if ( m_iFlags & AGK_TEXT_BOLD ) style |= 0x01;

			AGKFontImage *pFontImage = m_pFTSizedFont->GetCharImage( c, style );
			if ( !pFontImage ) pFontImage = m_pFTSizedFont->GetCharImage( 32, 0 );
			m_pSprites[ iIndex ]->SetFontImage( pFontImage, m_fFontScale );
			m_pSprites[ iIndex ]->SetUVBorder( 0 );

			ReAlignSprites();
		}
	}
}

void cText::InternalSetShader( AGKShader* shader )
{
	m_pShader = shader;

	if ( !m_pShader )
	{
		m_iFlags &= ~AGK_TEXT_CUSTOM_SHADER;

		if ( m_pFTSizedFont ) m_pShader = AGKShader::g_pShaderFont;
		else m_pShader = AGKShader::g_pShaderTexColor;
	}
	else 
	{
		m_iFlags |= AGK_TEXT_CUSTOM_SHADER;
	}

	cSprite::UpdateVertexLayout( &m_vertexLayout, m_pShader );
	m_vertexLayout.m_iPrimitiveType = AGK_PRIMITIVE_TRIANGLES;
}

void cText::SetShader ( AGKShader* shader )
{
	InternalSetShader( shader );
}

void cText::SetShaderConstantByName( const char *name, float v1, float v2, float v3, float v4 )
{
	int oldNum = m_cShaderConstantsByName.GetCount();
	AGKShaderConstantValue *pConstant = m_cShaderConstantsByName.GetOrCreate( name );
	if ( pConstant->IsArray() ) return;
	pConstant->SetVector4( v1, v2, v3, v4 );
	if ( oldNum != m_cShaderConstantsByName.GetCount() ) m_iFlags |= AGK_TEXT_SHADER_ARRAY_NEEDS_UPDATE;
}

void cText::SetShaderConstantArrayByName( const char *name, uint32_t index, uint32_t numValues, float v1, float v2, float v3, float v4 )
{
	int oldNum = m_cShaderConstantsByName.GetCount();
	AGKShaderConstantValue *pConstant = m_cShaderConstantsByName.GetOrCreate( name );
	if ( !pConstant->IsArray() ) return;
	AGKShaderConstantArray *pConstantArray = (AGKShaderConstantArray*) pConstant;

	switch( numValues )
	{
		case 1: pConstantArray->SetArrayFloat( index, v1 ); break;
		case 2: pConstantArray->SetArrayVector2( index, v1, v2 ); break;
		case 3: pConstantArray->SetArrayVector3( index, v1, v2, v3 ); break;
		case 4: pConstantArray->SetArrayVector4( index, v1, v2, v3, v4 ); break;
	}
	
	if ( oldNum != m_cShaderConstantsByName.GetCount() ) m_iFlags |= AGK_TEXT_SHADER_ARRAY_NEEDS_UPDATE;
}

void cText::SetShaderConstantDefault( const char *name )
{
	int oldNum = m_cShaderConstantsByName.GetCount();
	m_cShaderConstantsByName.Remove( name );
	if ( oldNum != m_cShaderConstantsByName.GetCount() ) m_iFlags |= AGK_TEXT_SHADER_ARRAY_NEEDS_UPDATE;
}

// Get Char

float cText::GetCharX( uint32_t iIndex )
{
	if ( iIndex >= m_iNumSprites ) return 0;

	float offsetX = m_pSprites[ iIndex ]->GetFontImage() ? m_pSprites[ iIndex ]->GetFontImage()->GetDisplayOffsetX()*m_fFontScale : 0;
	return m_pSprites[ iIndex ]->GetX() - m_fX - offsetX;
}

float cText::GetCharY( uint32_t iIndex )
{
	if ( iIndex >= m_iNumSprites ) return 0;

	float offsetY = m_pSprites[ iIndex ]->GetFontImage() ? m_pSprites[ iIndex ]->GetFontImage()->GetDisplayOffsetY()*m_fFontScale : 0;
	return m_pSprites[ iIndex ]->GetY() - m_fY - offsetY;
}

float cText::GetCharWidth( uint32_t iIndex )
{
	if ( iIndex >= m_iNumSprites ) return 0;

	float width = m_pSprites[ iIndex ]->GetFontImage() ? m_pSprites[ iIndex ]->GetFontImage()->GetDisplayAdvanceX()*m_fFontScale : m_pSprites[ iIndex ]->GetWidth();
	return width;
}

float cText::GetCharAngle( uint32_t iIndex )
{
	if ( iIndex >= m_iNumSprites ) return 0;

	return m_pSprites[ iIndex ]->GetAngle();
}

float cText::GetCharAngleRad( uint32_t iIndex )
{
	if ( iIndex >= m_iNumSprites ) return 0;

	return m_pSprites[ iIndex ]->GetAngleRad();
}


uint32_t cText::GetCharRed( uint32_t iIndex )
{
	if ( iIndex >= m_iNumSprites ) return 0;

	return m_pSprites[ iIndex ]->GetColorRed();
}


uint32_t cText::GetCharGreen( uint32_t iIndex )
{
	if ( iIndex >= m_iNumSprites ) return 0;

	return m_pSprites[ iIndex ]->GetColorGreen();
}


uint32_t cText::GetCharBlue( uint32_t iIndex )
{
	if ( iIndex >= m_iNumSprites ) return 0;

	return m_pSprites[ iIndex ]->GetColorBlue();
}


uint32_t cText::GetCharAlpha( uint32_t iIndex )
{
	if ( iIndex >= m_iNumSprites ) return 0;

	return m_pSprites[ iIndex ]->GetColorAlpha();
}

unsigned char cText::GetChar( uint32_t iIndex )
{
	if ( iIndex >= m_sText.GetNumChars() ) return 0;

	return m_sText.CharAt( iIndex );
}

void cText::FixToScreen( int mode )
{
	m_iFixed = mode != 0 ? 1 : 0;
	
	for ( uint32_t i = 0; i < m_iNumSprites; i++ )
	{
		m_pSprites[ i ]->FixToScreen( mode );
	}
}

bool cText::GetHitTest( float x, float y )
{
	if ( m_iFixed != 0 )
	{
		x = agk::WorldToScreenX( x );
		y = agk::WorldToScreenY( y );
	}

	// scissor test
	if ( m_fClipX2 != 0 || m_fClipX != 0 || m_fClipY != 0 || m_fClipY2 != 0 )
	{
		if ( x < m_fClipX || x > m_fClipX2 || y < m_fClipY || y > m_fClipY2 ) return false;
	}

	float fTempX;
	float fTempY;

	switch( m_iAlign )
	{
		case 0:
		{
			fTempX = x - m_fX;
			fTempY = y - m_fY;
			break;
		}

		case 1:
		{
			fTempX = (x - m_fX) + (m_fTotalWidth/2.0f);
			fTempY = y - m_fY;
			break;
		}

		case 2:
		{
			fTempX = (x - m_fX) + m_fTotalWidth;
			fTempY = y - m_fY;
			break;
		}
	}

	return (fTempX > 0) && (fTempY > 0) && (fTempX < m_fTotalWidth) && (fTempY < m_fTotalHeight);
}

void cText::SetManualScissor( int x, int y, int width, int height )
{
	if ( x == 0 && y == 0 && width == 0 && height == 0 )
	{
		m_iFlags &= ~AGK_TEXT_MANUAL_SCISSOR;
		if ( (m_iFlags & AGK_TEXT_SCISSOR_ON) == 0 ) m_renderState.OverrideScissor( 0 );
	}
	else
	{
		m_iFlags |= AGK_TEXT_MANUAL_SCISSOR;
		m_renderState.OverrideScissor( 1 );
		m_renderState.SetScissor( x, y, width, height );
	}
}

void cText::SetScissor( float x, float y, float x2, float y2 )
{
	if ( x == 0 && y == 0 && x2 == 0 && y2 == 0 )
	{
		m_fClipX = 0;
		m_fClipY = 0;
		m_fClipX2 = 0;
		m_fClipY2 = 0;
		m_iFlags &= ~AGK_TEXT_SCISSOR_ON;
		if ( (m_iFlags & AGK_TEXT_MANUAL_SCISSOR) == 0 ) m_renderState.OverrideScissor( 0 );
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
	m_iFlags |= AGK_TEXT_SCISSOR_ON;
	m_renderState.OverrideScissor( 1 ); // actual scissor values will be calculated at draw time
}

void cText::GetClipValues( int &x, int &y, int &width, int &height )
{
	if ( (m_iFlags & AGK_TEXT_SCISSOR_ON) == 0 )
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

	if ( m_iFixed == 0 )
	{
		fX = agk::WorldToScreenX( fX );
		fY = agk::WorldToScreenY( fY );

		fX2 = agk::WorldToScreenX( fX2 );
		fY2 = agk::WorldToScreenY( fY2 );
	}

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

void cText::Update( float time )
{
	for ( uint32_t i = 0; i < m_sText.GetNumChars(); i++ )
	{
		m_pSprites[ i ]->Update( time );
	}
}

void cText::Draw()
{
	if ( !GetVisible() ) return;

	if ( m_pFTFont )
	{
		float hRatio = agk::DeviceToDisplayRatioX();
		float vRatio = agk::DeviceToDisplayRatioY();

		if ( m_fHorizontalRatio != hRatio || m_fVerticalRatio != vRatio )
		{
			SetSize( m_fOrigSize );
		}
	}
	else
	{
		if ( !m_pFontImage || !m_pFontImageExt )
		{
			// check if default font image has changed
			if ( m_pUsingDefaultFont != m_pDefaultFont || m_pUsingDefaultFontExt != m_pDefaultFontExt )
			{
				m_pUsingDefaultFont = m_pDefaultFont;
				m_pUsingDefaultFontExt = m_pDefaultFontExt;
				Refresh();
			}
		}
	}

	if ( (m_iFlags & AGK_TEXT_MANUAL_SCISSOR) == 0 && GetScissorOn() )
	{
		int iScissorX, iScissorY, iScissorWidth, iScissorHeight;
		GetClipValues( iScissorX, iScissorY, iScissorWidth, iScissorHeight );
		
		m_renderState.SetScissor( iScissorX, iScissorY, iScissorWidth, iScissorHeight );
	}

	if ( m_pFTSizedFont ) PlatformDrawFT();
	else PlatformDraw();
}

AGKUnorderedArray<AGKShaderConstantValue*>* cText::GetShaderConstants()
{
	if ( m_iFlags & AGK_TEXT_SHADER_ARRAY_NEEDS_UPDATE )
	{
		m_cShaderConstantArray.Clear();
		
		AGKShaderConstantValue *pValue = m_cShaderConstantsByName.m_cConstantsByName.GetFirst();
		while( pValue )
		{
			m_cShaderConstantArray.AddItem( pValue );
			pValue = m_cShaderConstantsByName.m_cConstantsByName.GetNext();
		}
	}

	return &m_cShaderConstantArray;
}

void cText::PlatformDraw()
{
	// extended characters
	cImage* pCurrImage = 0;
	if ( m_pFontImageExt ) pCurrImage = m_pFontImageExt->GetRootImage();
	else if ( m_pDefaultFontExt ) pCurrImage = m_pDefaultFontExt->GetRootImage();

	Renderer *pRenderer = agk::GetRenderer();

	int iVertexFloats = cSprite::GetSpriteVertexFloats(); // position + color + uv;
	AGKDynamicArray<cImage*> shaderImages(1);
	shaderImages.SetItem( 0, pCurrImage );

	int iLength = m_sText.GetNumChars();
	int count = 0;
	int boundPipeline = 0;
	int boundConstants = 0;
	for ( int i = 0; i < iLength; i++ )
	{
		if ( m_pSprites[ i ]->GetImagePtr() && m_pSprites[ i ]->GetImagePtr()->GetRootImage() == pCurrImage ) 
		{
			if ( m_pSprites[ i ]->GetInScreen() )
			{
				m_pSprites[ i ]->BatchDrawQuad( m_pVertexData + count*iVertexFloats*4 ); // 4 vertices per sprite
				count++;

				// vertex array limited to 15000 sprites
				if ( count > 14900 )
				{
					if ( !boundPipeline )
					{
						pRenderer->BindPipeline( 0, m_pShader, &m_renderState, &m_vertexLayout, &m_pInternalPipeline );
						boundPipeline = 1;
					}
					if ( !boundConstants )
					{
						pRenderer->BindShaderConstants( 0, m_pShader, GetShaderConstants(), 0, &shaderImages, &m_pInternalShaderConstants );
						boundConstants = 1;
					}
					pRenderer->SetVertices( m_pInternalVertices, m_pVertexData, count * 4 ); // 4 vertices per sprite
					pRenderer->DrawVertices( 0, m_pInternalVertices, count*6 );
					count = 0;
				}
			}
		}
	}
	
	if ( count > 0 ) 
	{
		if ( !boundPipeline )
		{
			pRenderer->BindPipeline( 0, m_pShader, &m_renderState, &m_vertexLayout, &m_pInternalPipeline );
			boundPipeline = 1;
		}
		if ( !boundConstants )
		{
			pRenderer->BindShaderConstants( 0, m_pShader, GetShaderConstants(), 0, &shaderImages, &m_pInternalShaderConstants );
			boundConstants = 1;
		}
		pRenderer->SetVertices( m_pInternalVertices, m_pVertexData, count * 4 ); // 4 vertices per sprite
		pRenderer->DrawVertices( 0, m_pInternalVertices, count*6 );
	}

	cImage* oldImage = pCurrImage;

	// normal characters
	pCurrImage = 0;
	if ( m_pFontImage ) pCurrImage = m_pFontImage->GetRootImage();
	else if ( m_pDefaultFont ) pCurrImage = m_pDefaultFont->GetRootImage();

	if ( oldImage == pCurrImage ) return;

	shaderImages.SetItem( 0, pCurrImage );
	boundConstants = 0;

	count = 0;
	for ( int i = 0; i < iLength; i++ )
	{
		if ( m_pSprites[ i ]->GetImagePtr() && m_pSprites[ i ]->GetImagePtr()->GetRootImage() == pCurrImage ) 
		{
			if ( m_pSprites[ i ]->GetInScreen() )
			{
				m_pSprites[ i ]->BatchDrawQuad( m_pVertexData + count*iVertexFloats*4 ); // 4 vertices per sprite
				count++;

				// vertex array limited to 15000 sprites
				if ( count > 14900 )
				{
					if ( !boundPipeline )
					{
						pRenderer->BindPipeline( 0, m_pShader, &m_renderState, &m_vertexLayout, &m_pInternalPipeline );
						boundPipeline = 1;
					}
					if ( !boundConstants )
					{
						pRenderer->BindShaderConstants( 0, m_pShader, GetShaderConstants(), 0, &shaderImages, &m_pInternalShaderConstants );
						boundConstants = 1;
					}
					pRenderer->SetVertices( m_pInternalVertices, m_pVertexData, count * 4 ); // 4 vertices per sprite
					pRenderer->DrawVertices( 0, m_pInternalVertices, count*6 );
					count = 0;
				}
			}
		}
	}

	if ( count > 0 ) 
	{
		if ( !boundPipeline )
		{
			pRenderer->BindPipeline( 0, m_pShader, &m_renderState, &m_vertexLayout, &m_pInternalPipeline );
			boundPipeline = 1;
		}
		if ( !boundConstants )
		{
			pRenderer->BindShaderConstants( 0, m_pShader, GetShaderConstants(), 0, &shaderImages, &m_pInternalShaderConstants );
			boundConstants = 1;
		}
		pRenderer->SetVertices( m_pInternalVertices, m_pVertexData, count * 4 ); // 4 vertices per sprite
		pRenderer->DrawVertices( 0, m_pInternalVertices, count*6 );
	}
}

void cText::PlatformDrawFT()
{
	Renderer *pRenderer = agk::GetRenderer();

	int iVertexFloats = cSprite::GetSpriteVertexFloats(); // position + color + uv;
	AGKDynamicArray<cImage*> shaderImages(1);

	int boundPipeline = 0;
			
	for( unsigned int image = 0; image < m_pFTSizedFont->GetNumMainImages(); image++ )
	{
		cImage* pCurrImage = m_pFTSizedFont->GetMainImage( image )->GetRootImage();
		shaderImages.SetItem( 0, pCurrImage );
		int boundConstants = 0;

		int iLength = m_sText.GetNumChars();
		int count = 0;
		for ( int i = 0; i < iLength; i++ )
		{
			if ( m_pSprites[ i ]->GetImagePtr() && m_pSprites[ i ]->GetImagePtr()->GetRootImage() == pCurrImage && m_pSprites[ i ]->GetInScreen() ) 
			{
				m_pSprites[ i ]->BatchDrawQuad( m_pVertexData + count*iVertexFloats*4 ); // 4 vertices per sprite
				count++;

				// vertex array limited to 15000 sprites
				if ( count > 14900 )
				{
					if ( !boundPipeline )
					{
						pRenderer->BindPipeline( 0, m_pShader, &m_renderState, &m_vertexLayout, &m_pInternalPipeline );
						boundPipeline = 1;
					}
					if ( !boundConstants )
					{
						pRenderer->BindShaderConstants( 0, m_pShader, GetShaderConstants(), 0, &shaderImages, &m_pInternalShaderConstants );
						boundConstants = 1;
					}
					pRenderer->SetVertices( m_pInternalVertices, m_pVertexData, count * 4 ); // 4 vertices per sprite
					pRenderer->DrawVertices( 0, m_pInternalVertices, count*6 );
					count = 0;
				}
			}
		}

		if ( count > 0 ) 
		{
			if ( !boundPipeline )
			{
				pRenderer->BindPipeline( 0, m_pShader, &m_renderState, &m_vertexLayout, &m_pInternalPipeline );
				boundPipeline = 1;
			}
			if ( !boundConstants )
			{
				pRenderer->BindShaderConstants( 0, m_pShader, GetShaderConstants(), 0, &shaderImages, &m_pInternalShaderConstants );
				boundConstants = 1;
			}
			pRenderer->SetVertices( m_pInternalVertices, m_pVertexData, count * 4 ); // 4 vertices per sprite
			pRenderer->DrawVertices( 0, m_pInternalVertices, count*6 );
		}
	}
}

}
