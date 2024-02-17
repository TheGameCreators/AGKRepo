#ifndef _H_TEXTENTRY_
#define _H_TEXTENTRY_

// Common includes
#include "Common.h"
#include "uString.h"

#define MAX_EDITBOXES 0x7fffffff

#define AGK_EDITBOX_USING_NEW_FONT		0x0001
#define AGK_EDITBOX_SCISSOR_ON			0x0002
#define AGK_EDITBOX_VISIBLE				0x0004
#define AGK_EDITBOX_ACTIVE				0x0008
#define AGK_EDITBOX_PREV_ACTIVE			0x0010
#define AGK_EDITBOX_ENABLED				0x0020
#define AGK_EDITBOX_MULTILINE			0x0040
#define AGK_EDITBOX_FIXED				0x0080
#define AGK_EDITBOX_IS_PASSWORD			0x0100
#define AGK_EDITBOX_ALTERNATE_INPUT		0x0200
#define AGK_EDITBOX_SUPPORTS_EXT_ASCII	0x0400
#define AGK_EDITBOX_SUPPORT_WARNED		0x0800

// Namespace
namespace AGK
{
	class cSprite;

	class cEditBox
	{
		protected:
			uint32_t m_iID;
			float m_fX;
			float m_fY;
			float m_fZ;
			int m_iDepth;
			float m_fWidth;
			float m_fHeight;
			float m_fBorder;
			uString m_sCurrInput;
			float m_fTextSize;
			int m_iWrapMode;
			int m_iInputType;
			char m_iTextChanged;
            int m_iLastLength;
			uint32_t m_iFlags;
			
			int m_iCursorPos;
			float m_fCursorBlinkTime;
			float m_fCursorBlinkDelay;
			int m_iShowCursor;
			uint32_t m_iMaxChars;
			uint32_t m_iMaxLines;
			int m_iLines;

			float m_fScissorX;
			float m_fScissorY;
			float m_fScissorX2;
			float m_fScissorY2;

			cSprite *m_pInputCursor;
			cSprite *m_pInputBorder;
			cSprite *m_pInputArea;
			cText *m_pInputText;

			int m_iOldLength;
			int m_iOldCursorPos;

			static cImage *g_pDefaultFont;
			static cEditBox *g_pCurrentEditBox;

			void ParametersChanged();

			void PlatformStartText();
			void PlatformUpdateExternal();
			bool PlatformUpdateText();
			void PlatformUpdateTextEnd();
			void PlatformEndText();

		public:
			static cEditBox* GetCurrentFocus() { return g_pCurrentEditBox; }
			static void DeleteDefaultFont() { if ( g_pDefaultFont ) delete g_pDefaultFont; g_pDefaultFont = 0; }

			cEditBox();
			~cEditBox();

			void SetID( uint32_t iID );
			void SetPosition( float x, float y );
			void SetSize( float width, float height );
			void SetDepth( int depth );
			void SetBorderSize( float size );
			void SetBorderColor( uint32_t red, uint32_t green, uint32_t blue, uint32_t alpha );
			void SetBackgroundColor( uint32_t red, uint32_t green, uint32_t blue, uint32_t alpha );
			void SetTextColor( uint32_t red, uint32_t green, uint32_t blue );
			void SetCursorColor( uint32_t red, uint32_t green, uint32_t blue );
			void SetFontImage( cImage* image );
            void SetExtendedFontImage( cImage* image );
			void SetFont( AGKFont *pFont );
			void SetTextSize( float size );
			void SetActive( bool active );
			void SetFocus( bool active );
			void SetVisible( bool active );
			void SetCursorLocation( float x, float y );
			void SetCursorPosition( int pos );
			void SetBorderImage( cImage* image );
			void SetBackgroundImage( cImage* image );
			void SetCursorBlinkTime( float time );
			void SetCursorWidth( float width );
			void SetText( const char *str );
			void SetMaxChars( uint32_t max );
			void SetMultiLine( bool multiline );
			void SetMaxLines( uint32_t lines );
			void SetScissor( float x, float y ,float x2, float y2 );
			void FixToScreen( bool fix );
			void SetPasswordMode( int mode ) { if (mode==1) m_iFlags |= AGK_EDITBOX_IS_PASSWORD; else m_iFlags &= ~AGK_EDITBOX_IS_PASSWORD; }
            void SetUseAlternateInput( int mode ) { if (mode==1) m_iFlags |= AGK_EDITBOX_ALTERNATE_INPUT; else m_iFlags &= ~AGK_EDITBOX_ALTERNATE_INPUT; }
			void SetWrapMode( int mode );
			void SetInputType( int inputType );

			uint32_t GetID() { return m_iID; }
			float GetX() { return m_fX; }
			float GetY() { return m_fY; }
			float GetZ() { return m_fZ; }
			int GetDepth() { return m_iDepth; }
			bool GetFixed() { return (m_iFlags & AGK_EDITBOX_FIXED) != 0; }
			float GetWidth() { return m_fWidth; }
			float GetHeight() { return m_fHeight; }
			int GetText( uString &text ) { m_pInputText->GetText( text ); return text.GetLength(); }
			char* GetText(); // must delete returned string
			bool GetHitTest( float x, float y );
			bool GetActive() { return (m_iFlags & AGK_EDITBOX_ENABLED) != 0; }
			bool GetTextChanged() { return (m_iTextChanged>0); }
			void ResetChangedFlag() { m_iTextChanged = 0; }
			bool GetVisible() { return (m_iFlags & AGK_EDITBOX_VISIBLE) != 0; }
			bool HasFocus() { return (m_iFlags & AGK_EDITBOX_ACTIVE) != 0; }
			int GetLines() { return m_pInputText->GetLines(); }
            bool GetMultiLine() { return (m_iFlags & AGK_EDITBOX_MULTILINE) != 0; }
			int GetInputType() { return m_iInputType; }
			int GetCursorPosition() { return m_iCursorPos; }
			bool GetScissorOn() { return (m_iFlags & AGK_EDITBOX_SCISSOR_ON) != 0; }
			bool GetPrevFocus() { return (m_iFlags & AGK_EDITBOX_PREV_ACTIVE) != 0; }
			bool IsPassword() { return (m_iFlags & AGK_EDITBOX_IS_PASSWORD) != 0; }
			bool IsAlternateInput() { return (m_iFlags & AGK_EDITBOX_ALTERNATE_INPUT) != 0; }
			bool SupportsExtAscii() { return (m_iFlags & AGK_EDITBOX_SUPPORTS_EXT_ASCII) != 0; }
			bool GetSupportWarned() { return (m_iFlags & AGK_EDITBOX_SUPPORT_WARNED) != 0; }

			void Update();
			void Draw();
	};
}

#endif
