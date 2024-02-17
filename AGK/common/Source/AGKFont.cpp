#include "agk.h"
#include "zlib.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include "DefaultFont.h"

FT_Library AGKSizedFont::g_pFTLibrary = 0;
AGKFont* AGKFont::g_pDefaultFont = 0;

AGKFontImage::AGKFontImage() 
{ 
	m_pImage = 0; 
	m_pData = 0;
	m_iRefCount = 0; 
	m_iDataWidth = 0;
	m_iDataHeight = 0;
	m_iImageX = 0;
	m_iImageY = 0;
	m_iOffsetX = 0; 
	m_iOffsetY = 0; 
	m_iAdvanceX = 0; 
	m_iAdvanceY = 0; 
	m_iUnicodeValue = 0;
	m_iCharIndex = 0;
	m_iCreatedOrder = 0;
}

AGKFontImage::~AGKFontImage()
{
	if ( m_iRefCount > 0 ) agk::Error( "Deleted font image whilst it is still in use" );

	if ( m_pData ) delete [] m_pData;
	m_pData = 0;

	if ( m_pImage ) delete m_pImage;
}

void AGKFontImage::Release() 
{ 
	m_iRefCount--; 
	if ( m_iRefCount < 0 ) agk::Warning("Released font image too many times"); 
}

float AGKFontImage::GetDisplayAdvanceX() const
{
	return m_iAdvanceX * agk::DeviceToDisplayRatioX();
}

float AGKFontImage::GetDisplayOffsetX() const
{
	return m_iOffsetX * agk::DeviceToDisplayRatioX();
}

float AGKFontImage::GetDisplayOffsetY() const
{
	return m_iOffsetY * agk::DeviceToDisplayRatioY();
}

// Sized Font

AGKSizedFont::AGKSizedFont( AGKFont *parent ) : m_CharImages(512)
{
	if ( g_pFTLibrary == 0 )
	{
		if ( FT_Init_FreeType( &g_pFTLibrary ) != 0 )
		{
			agk::Error( "Failed to initialise FreeType library" );
		}
	}

	m_pParent = parent;
	m_iRefCount = 0;
	m_pFTFace = 0;
	m_iDesiredSize = 0;
	m_iActualSize = 0;
	m_iCurrX = 0;
	m_iCurrY = 0;
	m_iMaxRowY = 0;
	m_iUnusedCount = 0;
	m_iCreatedCount = 0;

	m_iImageWidth = 0;
	m_iNumMainImages = 1;
	m_pMainImages = new cImage*[ m_iNumMainImages ];
	m_pMainImages[ 0 ] = new cImage();
	m_pCurrImage = m_pMainImages[0];

	m_pCurrImage->LoadFromData( 1, 1, 0, 2 );
}

AGKSizedFont::~AGKSizedFont()
{
	if ( m_iRefCount > 0 )
	{
		agk::Error( "Deleted font whilst it is still in use, this could cause a crash" );
	}

	AGKFontImage *pFontImage = m_CharImages.GetFirst();
	while( pFontImage )
	{
		delete pFontImage;
		pFontImage = m_CharImages.GetNext();
	}
	m_CharImages.ClearAll();

	if ( m_pMainImages ) 
	{
		for ( unsigned int i = 0; i < m_iNumMainImages; i++ ) 
		{
			if ( m_pMainImages[i] ) delete m_pMainImages[i];
		}

		delete [] m_pMainImages;
	}

	if ( m_pFTFace ) FT_Done_Face( m_pFTFace );
}

void AGKSizedFont::Release() 
{ 
	m_iRefCount--; 
	if ( m_iRefCount < 0 ) agk::Warning("Released font too many times"); 
}

void AGKSizedFont::SetSize( int height )
{
	if ( height < 3 ) height = 3;
	m_iDesiredSize = height;
	if ( m_iDesiredSize < 30 ) m_iImageWidth = 1024;
	else m_iImageWidth = 2048;

	// calculate required scale to fit all font glyphs in the desired height
	float fontMaxY = m_pFTFace->bbox.yMax / 64.0f;
	float fontMinY = m_pFTFace->bbox.yMin / 64.0f;
	float fontHeight = fontMaxY - fontMinY;
	float fontEM = m_pFTFace->units_per_EM / 64.0f;

	float scale = height / fontHeight;
	int newHeight = agk::Round(fontEM * scale);
	
	// check new height value is suitable
	int ascender, descender;
	while(1)
	{
		float newScale = newHeight / fontEM;
		ascender = agk::Round(fontMaxY * newScale);
		descender = -agk::Round(fontMinY * newScale);
		if ( ascender+descender <= height ) break;
		newHeight--;
	}

	if ( FT_Set_Pixel_Sizes( m_pFTFace, 0, newHeight ) ) return;
	m_iActualSize = newHeight;
	m_iBaseLine = m_iDesiredSize - descender;
}

int AGKSizedFont::LoadFromFile( const uString& filename, int height )
{
	if ( m_pFTFace ) FT_Done_Face( m_pFTFace );
	m_pFTFace = 0;

	int result;
	if ( (result = FT_New_Face( g_pFTLibrary, filename.GetStr(), 0, &m_pFTFace )) != 0 )
	{
		uString err; err.Format( "Failed to create new font from file %s, error: %d", filename.GetStr(), result );
		agk::Error( err );
		return 0;
	}

	SetSize( height );
	return 1;
}

int AGKSizedFont::LoadFromData( const unsigned char *pData, unsigned int dataSize, int height )
{
	if ( m_pFTFace ) FT_Done_Face( m_pFTFace );
	m_pFTFace = 0;

	int result;
	if ( (result = FT_New_Memory_Face( g_pFTLibrary, pData, dataSize, 0, &m_pFTFace )) != 0 )
	{
		uString err;
		err.Format( "Failed to create new default font, error: %d", result );
		agk::Error( err );
		return 0;
	}

	SetSize( height );
	return 1;
}

void AGKSizedFont::RebuildImages()
{
	for( uint32_t i = 0; i < m_iNumMainImages; i++ )
	{
		int width = m_pMainImages[ i ]->GetWidth();
		int height = m_pMainImages[ i ]->GetHeight();
		unsigned char *data = new unsigned char[ width * height ];
		memset( data, 0, width * height );
		
		AGKFontImage *pFontImage = m_CharImages.GetFirst();
		while ( pFontImage )
		{
			if ( pFontImage->m_pImage->GetParent() == m_pMainImages[ i ] )
			{
				// copy font character into image data
				for( int y = 0; y < pFontImage->m_iDataHeight; y++ )
				{
					int index = y*pFontImage->m_iDataWidth;
					int index2 = (y+pFontImage->m_iImageY)*width + pFontImage->m_iImageX;

					for( int x = 0; x < pFontImage->m_iDataWidth; x++ )
					{
						data[ index2++ ] = pFontImage->m_pData[ index++ ];
					}
				}
			}

			pFontImage = m_CharImages.GetNext();
		}

		m_pMainImages[ i ]->LoadFromData( width, height, (unsigned int*)data, 2 );
		delete [] data;
	}
}

void AGKSizedFont::DeleteImages()
{
	for( uint32_t i = 0; i < m_iNumMainImages; i++ )
	{
		agk::GetRenderer()->DeleteImage( m_pMainImages[ i ]->m_pInternalResource );
		m_pMainImages[ i ]->m_pInternalResource = 0;
	}
}

//#define AGK_FONT_MAX_IMAGE_SIZE 2048

int AGKSizedFont::AddToMainImage( AGKFontImage* pNewFontImage )
{
	if ( m_iImageWidth == 0 ) return 0;

	int border = 2;
	int offset = border/2;

	if ( !pNewFontImage ) return 0;
	if ( pNewFontImage->m_iDataWidth+border > m_iImageWidth || pNewFontImage->m_iDataHeight+border > m_iImageWidth ) return 0;

	if ( m_pCurrImage->GetWidth() > 0 && m_iCurrX + pNewFontImage->m_iDataWidth+border > m_pCurrImage->GetWidth() )
	{
		m_iCurrY += m_iMaxRowY;
		m_iMaxRowY = 0;
		m_iCurrX = 0;
	}

	int maxY = pNewFontImage->m_iDataHeight + border;
	if ( m_iDesiredSize + 5 + border > maxY ) maxY = m_iDesiredSize + 5 + border; // + 5 to allow for some characters that exceed the bounds
	if ( m_iCurrY + maxY > m_pCurrImage->GetHeight() )
	{
		// not enough room, regenerate the main images with new sizes
		int newHeight = m_iCurrY + maxY;
		if ( newHeight > m_iImageWidth )
		{
			// need a new image
			int newNumImages = m_iNumMainImages + 1;
			cImage **pNewImages = new cImage*[ newNumImages ];
			for ( unsigned int i = 0; i < m_iNumMainImages; i++ ) pNewImages[ i ] = m_pMainImages[ i ];
			pNewImages[ m_iNumMainImages ] = new cImage();
			m_pCurrImage = pNewImages[ m_iNumMainImages ];
			m_pCurrImage->LoadFromData( 1, 1, 0, 2 );

			delete [] m_pMainImages;
			m_pMainImages = pNewImages;
			m_iNumMainImages = newNumImages;

			m_iCurrX = 0;
			m_iCurrY = 0;
			m_iMaxRowY = 0;

			// restart process with new image
			return AddToMainImage( pNewFontImage );
		}
		else
		{
			// expand current image, all characters on this image must be redrawn to resize the image
			unsigned char *data = new unsigned char[ m_iImageWidth * newHeight ];
			memset( data, 0, m_iImageWidth * newHeight );
			
			int posX = 0;
			int posY = 0;
			int rowY = 0;

			// images must be added in the order they were created, otherwise a row of tall chars could get split across multiple rows and accidentally overflow the image height
			int imageCount = m_CharImages.GetCount();
			AGKSortValue *ppFontImages = new AGKSortValue[ imageCount ];
			AGKFontImage *pFontImage = m_CharImages.GetFirst();
			int count = 0;
			while ( pFontImage )
			{
				ppFontImages[ count ].iValue = pFontImage->m_iCreatedOrder;
				ppFontImages[ count ].ptr = pFontImage;
				count++;
				if ( count > imageCount ) 
				{
					agk::Error( "Number of font images is greater than expected" );
					break;
				}

				pFontImage = m_CharImages.GetNext();
			}

			agk::SortArray( ppFontImages, imageCount );

			for( int i = 0; i < imageCount; i++ )
			{
				pFontImage = (AGKFontImage*) ppFontImages[ i ].ptr;

				if ( pFontImage->m_pImage->GetParent() == m_pCurrImage )
				{
					if ( pFontImage->m_iRefCount <= 0 && pFontImage->m_iUnicodeValue > 127 )
					{
						// not being used, so get rid of it
						m_CharImages.RemoveItem( pFontImage->m_iCharIndex );
						delete pFontImage;
					}
					else
					{
						if ( posX + pFontImage->m_iDataWidth + border > m_iImageWidth )
						{
							posY += rowY;
							rowY = 0;
							posX = 0;
						}

						// copy font character into image data
						for( int y = 0; y < pFontImage->m_iDataHeight; y++ )
						{
							int index = y*pFontImage->m_iDataWidth;
							int index2 = (y+posY+offset)*m_iImageWidth + posX + offset;

							for( int x = 0; x < pFontImage->m_iDataWidth; x++ )
							{
								data[ index2++ ] = pFontImage->m_pData[ index++ ];
							}
						}

						pFontImage->m_iImageX = posX + offset;
						pFontImage->m_iImageY = posY + offset;
						float u1 = (posX + offset) / (float)m_iImageWidth;
						float v1 = (posY + offset) / (float)newHeight;
						float u2 = (posX+offset+pFontImage->m_iDataWidth) / (float)m_iImageWidth;
						float v2 = (posY+offset+pFontImage->m_iDataHeight) / (float)newHeight;
						pFontImage->m_pImage->OverrideSubImage( m_pCurrImage, pFontImage->m_iDataWidth, pFontImage->m_iDataHeight, u1, v1, u2, v2 );

						posX += pFontImage->m_iDataWidth + border;
						if ( pFontImage->m_iDataHeight+border > rowY ) rowY = pFontImage->m_iDataHeight + border;
					}
				}
			}

			delete [] ppFontImages;

			m_pCurrImage->LoadFromData( m_iImageWidth, newHeight, (unsigned int*)data, 2 );
			delete [] data;

			m_iCurrX = posX;
			m_iCurrY = posY;
			m_iMaxRowY = rowY;

			if ( m_iCurrX + pNewFontImage->m_iDataWidth+border > m_pCurrImage->GetWidth() )
			{
				m_iCurrY += m_iMaxRowY;
				m_iMaxRowY = 0;
				m_iCurrX = 0;
			}
		}
	}

	// paste new font character onto current image
	m_pCurrImage->LoadSubRegionFromData( m_iCurrX+offset, m_iCurrY+offset, pNewFontImage->m_iDataWidth, pNewFontImage->m_iDataHeight, pNewFontImage->m_pData );
	
	if ( !pNewFontImage->m_pImage ) pNewFontImage->m_pImage = new cImage();
	pNewFontImage->m_pImage->ManualSubImage( m_pCurrImage, m_iCurrX+offset, m_iCurrY+offset, pNewFontImage->m_iDataWidth, pNewFontImage->m_iDataHeight );
	pNewFontImage->m_iImageX = m_iCurrX + offset;
	pNewFontImage->m_iImageY = m_iCurrY + offset;

	m_iCurrX += pNewFontImage->m_iDataWidth + border;
	if ( pNewFontImage->m_iDataHeight + border > m_iMaxRowY ) m_iMaxRowY = pNewFontImage->m_iDataHeight + border;

	return 1;
}

AGKFontImage* AGKSizedFont::GetCharImage( unsigned int unicodeChar, unsigned int style )
{
	if ( unicodeChar > 0x10FFFF ) return 0;
	style &= 0x03;

	int charIndex = unicodeChar | (style << 21);
	AGKFontImage *pFontImage = m_CharImages.GetItem( charIndex );
	if ( pFontImage ) return pFontImage;

	// char hasn't yet been rendered, draw it now
	if ( FT_Load_Char( m_pFTFace, unicodeChar, FT_LOAD_RENDER ) ) return 0;

	// get values from the glyph
	int bmpWidth = m_pFTFace->glyph->bitmap.width;
	int pitch = m_pFTFace->glyph->bitmap.pitch;
	int bmpHeight = m_pFTFace->glyph->bitmap.rows;

	// calculate the image values
	int imgHeight = bmpHeight;	
	int imgWidth = bmpWidth;
	if ( imgWidth < 1 ) imgWidth = 1;
	if ( imgHeight < 1 ) imgHeight = 1;
	
	int padding = 0;
	if ( style & 0x01 ) 
	{
		padding = agk::Ceil( m_iDesiredSize / 30.0f );
		imgWidth += padding; 
	}

	unsigned char *data = new unsigned char[ imgWidth*imgHeight ];
	memset( data, 0, imgWidth*imgHeight );

	for( int y = 0; y < bmpHeight; y++ )
	{
		for( int x = 0; x < bmpWidth; x++ )
		{
			int index = y*imgWidth + x;
			int index2 = y*pitch + x;

			data[ index ] = m_pFTFace->glyph->bitmap.buffer[ index2 ];
		}
	}

	int pass = 1;
	while ( pass <= padding )
	{
		for( int y = 0; y < bmpHeight; y++ )
		{
			for( int x = 0; x < bmpWidth; x++ )
			{
				int index = y*imgWidth + x + pass;
				int index2 = y*pitch + x;

				int color = m_pFTFace->glyph->bitmap.buffer[ index2 ];
				if ( x < bmpWidth-1 ) color += data[ index ];
				if ( color > 255 ) color = 255;
				data[ index ] = color;
			}
		}
		pass++;
	}

	pFontImage = new AGKFontImage();
	pFontImage->m_iCreatedOrder = m_iCreatedCount++;
	pFontImage->m_pImage = 0;
	pFontImage->m_pData = data;
	pFontImage->m_iDataWidth = imgWidth;
	pFontImage->m_iDataHeight = imgHeight;
	pFontImage->m_iOffsetX = m_pFTFace->glyph->bitmap_left;
	pFontImage->m_iOffsetY = (m_iBaseLine - m_pFTFace->glyph->bitmap_top);
	pFontImage->m_iAdvanceX = (short) ((m_pFTFace->glyph->advance.x >> 6) + padding);
	pFontImage->m_iAdvanceY = 0;
	pFontImage->m_iUnicodeValue = unicodeChar;
	pFontImage->m_iCharIndex = charIndex;

	if ( !AddToMainImage( pFontImage ) )
	{
		delete pFontImage;
		return 0;
	}

	m_CharImages.AddItem( pFontImage, charIndex );

	return pFontImage;
} 


// Main Font

AGKFont *AGKFont::g_pAllFonts = 0;

AGKFont::AGKFont() : m_SizedFonts(32)
{
	// load the default font
	m_pPrevFont = 0;
	m_pNextFont = 0;
	m_iMemFontSize = g_iDefaultFontSize;
	m_pMemFont = new unsigned char[ m_iMemFontSize ];
	unsigned long size = m_iMemFontSize;
	int err = uncompress( m_pMemFont, &size, (unsigned char*)g_iDefaultFont, g_iDefaultFontCompressedSize );
	if ( err != Z_OK )
	{
		uString sErr;
		sErr.Format( "Failed to uncompress font: %d", err );
		agk::Error( sErr );
	}
	m_iMemFontSize = size;

	if ( g_pAllFonts ) g_pAllFonts->m_pPrevFont = this;
	m_pPrevFont = 0;
	m_pNextFont = g_pAllFonts;
	g_pAllFonts = this;
}

AGKFont::AGKFont( uint8_t* data, uint32_t compressedSize, uint32_t uncompressedSize ) : m_SizedFonts(32)
{
	m_pPrevFont = 0;
	m_pNextFont = 0;
	m_iMemFontSize = uncompressedSize;
	m_pMemFont = new unsigned char[ m_iMemFontSize ];
	unsigned long size = m_iMemFontSize;
	int err = uncompress( m_pMemFont, &size, data, compressedSize );
	if ( err != Z_OK )
	{
		uString sErr;
		sErr.Format( "Failed to uncompress font: %d", err );
		agk::Error( sErr );
	}
	m_iMemFontSize = size;

	if ( g_pAllFonts ) g_pAllFonts->m_pPrevFont = this;
	m_pPrevFont = 0;
	m_pNextFont = g_pAllFonts;
	g_pAllFonts = this;
}

AGKFont::AGKFont( const uString &sFilename ) : m_SizedFonts(32)
{
	m_pPrevFont = 0;
	m_pNextFont = 0;
	m_pMemFont = 0;
	m_iMemFontSize = 0;
	m_sFontFile.SetStr( sFilename );
	int mode = 0;

	cFile oFile;
	if ( cFile::Exists(sFilename) )
	{
		// if SetErrorMode is set to 2 then failing to load will cause a fatal error, so check for existence first
		if ( !oFile.OpenToRead( sFilename ) ) return;
		agk::GetRealPath( m_sFontFile );
	}
	else
	{
		if ( !PlatformGetSystemFontPath( sFilename, m_sFontFile ) )
		{
			uString err; err.Format( "Failed to load font %s, could not find it in the media folder nor in the system fonts", sFilename.GetStr() );
			agk::Error( err );
			return;
		}

		m_sFontFile.Prepend( "raw:" );
		if ( cFile::Exists(m_sFontFile) ) 
		{
			if ( !oFile.OpenToRead( m_sFontFile ) ) return;
		}
		else
		{
			uString err; err.Format( "Failed to load font %s from the system fonts", sFilename.GetStr() );
			agk::Error( err );
			return;
		}
	}

	m_iMemFontSize = oFile.GetSize();
	m_pMemFont = new unsigned char[ m_iMemFontSize ];
	oFile.ReadData( (char*)m_pMemFont, m_iMemFontSize );
	oFile.Close();
	
	if ( g_pAllFonts ) g_pAllFonts->m_pPrevFont = this;
	m_pPrevFont = 0;
	m_pNextFont = g_pAllFonts;
	g_pAllFonts = this;

	/*
	// read font file into memory, compress it, and save it out as a C++ header
	FILE *pFile = fopen( m_sFontFile, "rb" );
	fseek( pFile, 0, SEEK_END );
	uint32_t size = ftell( pFile );
	fseek( pFile, 0, SEEK_SET );

	unsigned char *pData = new unsigned char[ size ];
	int total = 0;
	do
	{
		int written = fread( pData+total, 1, size-total, pFile );
		if ( written <= 0 ) break;
		total += written;
	} while( total < size );
	fclose( pFile );

	unsigned long newSize = size;
	unsigned char *pData2 = new unsigned char[ newSize ];
	compress2( pData2, &newSize, pData, size, 9 );
	delete [] pData;

	char num[ 32 ];
	pFile = fopen( "E:\\Temp\\DefaultFont.h", "wb" );
	fputs( "// ", pFile );
	fputs( m_sFontFile.GetStr(), pFile );
	fputs( "\nunsigned int g_iDefaultFontSize = ", pFile );
	sprintf( num, "%d", size );
	fputs( num, pFile );
	fputs( ";\n", pFile );
	fputs( "unsigned int g_iDefaultFontCompressedSize = ", pFile );
	sprintf( num, "%d", newSize );
	fputs( num, pFile );
	fputs( ";\n", pFile );
	fputs( "unsigned int g_iDefaultFont[] = { ", pFile );

	int *pDataInt = (int*)pData2;
	int newSizeInt = (int)newSize / 4 + 1;
	for ( int i = 0; i < newSizeInt; i++ )
	{
		sprintf( num, "0x%x", pDataInt[i] );
		fputs( num, pFile );
		if ( i < newSizeInt - 1 ) fputs( ", ", pFile );
		if ( i > 0 && i % 16 == 0 ) fputs( "\\\n", pFile );
	}

	fputs( "};\n", pFile );
	fclose( pFile );
	delete [] pData2;
	*/
}

AGKFont::~AGKFont()
{
	// remove from global list
	if ( m_pPrevFont ) m_pPrevFont->m_pNextFont = m_pNextFont;
	else g_pAllFonts = m_pNextFont;
	if ( m_pNextFont ) m_pNextFont->m_pPrevFont = m_pPrevFont;

	AGKSizedFont *pSizedFont = m_SizedFonts.GetFirst();
	while( pSizedFont )
	{
		delete pSizedFont;
		pSizedFont = m_SizedFonts.GetNext();
	}
	m_SizedFonts.ClearAll();

	if ( m_pMemFont ) delete [] m_pMemFont;
}

AGKSizedFont* AGKFont::GetSizedFont( int size )
{
	if ( size < 3 ) size = 3;
	if ( size > AGK_FONT_MAX_SIZE ) size = AGK_FONT_MAX_SIZE;

	AGKSizedFont *pSizedFont = m_SizedFonts.GetItem( size );
	if ( pSizedFont ) 
	{
		pSizedFont->m_iUnusedCount = 0;
		return pSizedFont;
	}

	// delete any unused fonts
	pSizedFont = m_SizedFonts.GetFirst();
	while ( pSizedFont )
	{
		if ( pSizedFont->m_iRefCount <= 0 )
		{
			if ( pSizedFont->GetSize() * pSizedFont->m_iUnusedCount > 75 )
			{
				m_SizedFonts.RemoveItem( pSizedFont->GetSize() );
				delete pSizedFont;
			}
			else
			{
				pSizedFont->m_iUnusedCount++;
			}
		}

		pSizedFont = m_SizedFonts.GetNext();
	}

	// create new size
	pSizedFont = new AGKSizedFont( this );
	int result = 0;
	if ( m_iMemFontSize > 0 )
	{
		result = pSizedFont->LoadFromData( m_pMemFont, m_iMemFontSize, size );
	}
	else
	{
		result = pSizedFont->LoadFromFile( m_sFontFile, size );
	}

	if( !result ) 
	{
		delete pSizedFont;
		return 0;
	}

	m_SizedFonts.AddItem( pSizedFont, size );
	return pSizedFont;
}

AGKFont* AGKFont::GetDefaultFont()
{
	if ( !g_pDefaultFont ) g_pDefaultFont = new AGKFont();
	return g_pDefaultFont;
}

int AGKFont::GetSystemFontExists( const uString &sFilename )
{
	uString sOut;
	if ( !PlatformGetSystemFontPath( sFilename, sOut ) ) return 0;
	else return 1;
}
