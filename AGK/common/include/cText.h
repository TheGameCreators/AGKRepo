// Text Header
#ifndef _H_TEXT_
#define _H_TEXT_

// Common includes
#include "Common.h"
#include "uString.h"

#define MAX_TEXTS 0x7fffffff

#define AGK_TEXT_REFRESHING			        0x01
#define AGK_TEXT_BOLD				        0x02
#define AGK_TEXT_MANUAL_SCISSOR		        0x04
#define AGK_TEXT_VISIBLE			        0x08
#define AGK_TEXT_SCISSOR_ON			        0x10
#define AGK_TEXT_SNAP_TO_PIXELS		        0x20
#define AGK_TEXT_SHADER_ARRAY_NEEDS_UPDATE  0x40
#define AGK_TEXT_CUSTOM_SHADER              0x80

// Namespace
namespace AGK
{
	class cSprite;
	class cSpriteMgrEx;
	class AGKFont;
	class AGKSizedFont;

	class cText
	{
		protected:
			
			friend class agk;

			static uint32_t g_iCreated;

			uint32_t m_iCreated = 0;
			uint32_t m_iID = 0;
			
			uString m_sText;
			unsigned char *m_pCharStyles = 0;
			cSprite **m_pSprites = 0;
			uint32_t m_iNumSprites = 0;
						
			float m_fX = 0;
			float m_fY = 0;
			float m_fSpriteX = 0;
			float m_fSpriteY = 0;
			float m_fAngle = 0;
			int m_iDepth = 9;
			float m_fOrigSize = 4;
			float m_fSize = 4;
			float m_fTotalWidth = 0;
			float m_fTotalHeight = 0;
			float m_fSpacing = 0;
			float m_fVSpacing = 0;
			float m_fRed = 1.0f;
			float m_fGreen = 1.0f;
			float m_fBlue = 1.0f;
			float m_fAlpha = 1.0f;
			int m_iAlign = 0;
			int m_iVAlign = 0;
			int m_iTransparency = 0;
			cSpriteMgrEx* m_pSpriteManager = 0;
			bool m_bManagedDrawing = false; // true if the drawing is handled by a sprite manager
						
			int m_iFixed = 0;
			float m_fMaxWidth = 0;
			uint32_t m_iLines;
			
			float m_fClipX = 0;
			float m_fClipY = 0;
			float m_fClipX2 = 0;
			float m_fClipY2 = 0;

			void *m_pInternalVertices = 0;
			void *m_pInternalPipeline = 0;
			void *m_pInternalShaderConstants = 0;
			AGKRenderState m_renderState;
			AGKVertexLayout m_vertexLayout;
			AGKShader *m_pShader = 0;

			AGKShaderConstantsByName m_cShaderConstantsByName;
			AGKUnorderedArray<AGKShaderConstantValue*> m_cShaderConstantArray;

			uint32_t m_iFlags = AGK_TEXT_VISIBLE;
			uint32_t m_iImageID = 0;

			float *m_pVertexData = 0;
			unsigned short *m_pIndices = 0;
			
			cImage *m_pFontImage = 0;
			cImage **m_pLetterImages = 0;

			cImage *m_pFontImageExt = 0;
			cImage **m_pLetterImagesExt = 0;

			static cImage *m_pDefaultFontOrig;
			static cImage *m_pDefaultFontExtOrig;

			static cImage *m_pDefaultFont;
			static cImage **m_pDefaultLetters;

			static cImage *m_pDefaultFontExt;
			static cImage **m_pDefaultLettersExt;

			cImage *m_pUsingDefaultFont = 0;
			cImage *m_pUsingDefaultFontExt = 0;

			// new fonts
			AGKFont *m_pFTFont = 0;
			AGKSizedFont *m_pFTSizedFont = 0;
			float m_fHorizontalRatio = 0;
			float m_fVerticalRatio = 0;
			float m_fFontScale = 1;
			
			void PlatformDraw();
			void PlatformDrawFT(); // FreeType
			void InternalRefresh();
			void InternalSetShader ( AGKShader* shader );

			AGKUnorderedArray<AGKShaderConstantValue*>* GetShaderConstants();

		public:

			static void SetDefaultMinFilter( uint32_t mode );
			static void SetDefaultMagFilter( uint32_t mode );
			static void SetDefaultFontImage( cImage *pImage ); 
			static void SetDefaultExtendedFontImage( cImage *pImage ); 

			static const cImage* GetDefaultFontImage() { return m_pDefaultFont; }
			static const cImage* GetDefaultFontExtImage() { return m_pDefaultFontExt; }

			AGKShader* GetShader ( ) { return m_pShader; }
			void SetShader ( AGKShader* shader );
			void SetShaderConstantByName ( const char* name, float v1, float v2, float v3, float v4 );
			void SetShaderConstantArrayByName ( const char* name, uint32_t index, uint32_t numValues, float v1, float v2, float v3, float v4 );
			void SetShaderConstantDefault ( const char* name );

			cText( int iLength=0 );
			~cText();
			uint32_t GetCreated() const { return m_iCreated; }
			void SetSpriteManager( cSpriteMgrEx *pMgr );

			static void GlobalImageDeleting( cImage *pImage );
			void ImageDeleting( cImage *pImage );
			
			void ChangedAspect();
			void SetString( const uString &sString ) { SetString( sString.GetStr() ); }
			void SetString( const char* szString );
			void SetPosition( float fX, float fY );
			void ReAlignSprites();
			void ShiftPosition( float fDiffX, float fDiffY );
			void SetX( float fX );
			void SetY( float fY );
			void SetAngle( float fA );
			void SetAngleRad( float fA );
			void SetSize( float fSize );
			void SetSpacing( float fSpacing );
			void SetLineSpacing( float fSpacing );
			void SetAlignment( int iMode );
			void SetDepth( int iDepth );
			void SetVisible( bool bVisible );
			void SetColor( uint32_t iRed, uint32_t iGreen, uint32_t iBlue, uint32_t iAlpha=255 );
			void SetRed( uint32_t iRed );
			void SetGreen( uint32_t iGreen );
			void SetBlue( uint32_t iBlue );
			void SetAlpha( uint32_t iAlpha );
			void SetTransparency( int iMode );
			void SetMaxWidth( float width );
			void SetBold( uint32_t bold );
			void SetSnap( int mode );
			
			void SetCharPosition( uint32_t iIndex, float x, float y );
			void SetCharX( uint32_t iIndex, float x );
			void SetCharY( uint32_t iIndex, float y );
			void SetCharAngle( uint32_t iIndex, float angle );
			void SetCharAngleRad( uint32_t iIndex, float angle );
			void SetCharColor( uint32_t iIndex, uint32_t red, uint32_t green, uint32_t blue, uint32_t alpha );
			void SetCharRed( uint32_t iIndex, uint32_t iRed );
			void SetCharGreen( uint32_t iIndex, uint32_t iGreen );
			void SetCharBlue( uint32_t iIndex, uint32_t iBlue );
			void SetCharAlpha( uint32_t iIndex, uint32_t iAlpha );
			void SetCharBold( uint32_t index, uint32_t bold );
			float GetCharX( uint32_t iIndex );
			float GetCharY( uint32_t iIndex );
			float GetCharWidth( uint32_t iIndex );
			float GetCharAngle( uint32_t iIndex );
			float GetCharAngleRad( uint32_t iIndex );
			uint32_t GetCharRed( uint32_t iIndex );
			uint32_t GetCharGreen( uint32_t iIndex );
			uint32_t GetCharBlue( uint32_t iIndex );
			uint32_t GetCharAlpha( uint32_t iIndex );
			unsigned char GetChar( uint32_t iIndex );

			bool GetHitTest( float x, float y );
			void FixToScreen( int mode );
			void SetFontImage( cImage *pImage ); // use either SetFontImage or SetFont, not both
			void SetExtendedFontImage( cImage *pImage ); 
			void SetFont( AGKFont *pFont ); // use either SetFontImage or SetFont, not both
			void Refresh();
			
			float GetX( ) { return m_fX; }
			float GetY( ) { return m_fY; }
			float GetZ( ) { return m_iDepth / 10000.0f; }
			float GetAngle( ) { return m_fAngle * 180.0f / PI; }
			float GetAngleRad( ) { return m_fAngle; }
			uint32_t GetLength( ) { return m_sText.GetNumChars(); }
			float GetRed() { return m_fRed; }
			float GetGreen() { return m_fGreen; }
			float GetBlue() { return m_fBlue; }
			float GetAlpha() { return m_fAlpha; }
			int GetTransparency() { return m_iTransparency; }
			float GetTotalWidth() { return m_fTotalWidth; }
			float GetTotalHeight() { return m_fTotalHeight; }
			bool GetVisible() { return (m_iFlags & AGK_TEXT_VISIBLE) != 0; }
			int GetDepth() { return m_iDepth; }
			float GetSize() { return m_fSize; }
			void GetText( uString &out ) { out.SetStr( m_sText ); }
			uint32_t GetLines() { return m_iLines; }
			float GetSpacing() { return m_fSpacing; }
			float GetLineSpacing() { return m_fVSpacing; }
			int GetAlignment() { return m_iAlign; }
			bool GetScissorOn() { return (m_iFlags & AGK_TEXT_SCISSOR_ON) != 0; }

			bool IsManaged() { return m_bManagedDrawing; }

			void SetManualScissor( int x, int y, int width, int height );
			void SetScissor( float x, float y, float x2, float y2 );
			void GetClipValues( int &x, int &y, int &width, int &height );

			void Draw();
			void Update( float time );
	};
}

#endif
