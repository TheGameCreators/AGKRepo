#ifndef _H_AGKFONT_
#define _H_AGKFONT_

// Common includes
#include "Common.h"

struct FT_LibraryRec_;
struct FT_FaceRec_;

#define AGK_MAX_FONTS 0x7fffffff
#define AGK_FONT_MAX_SIZE 200

namespace AGK
{
	class cImage;

	class AGKFontImage
	{
		public:
			cImage *m_pImage;
			unsigned char *m_pData;
			int m_iRefCount;
			unsigned int m_iUnicodeValue;
			unsigned int m_iCharIndex;
			unsigned int m_iCreatedOrder;
			short m_iOffsetX;
			short m_iOffsetY;
			short m_iAdvanceX;
			short m_iAdvanceY;
			unsigned short m_iDataWidth;
			unsigned short m_iDataHeight;
			unsigned short m_iImageX;
			unsigned short m_iImageY;
			
			AGKFontImage();
			~AGKFontImage();

			void AddRef() { m_iRefCount++; }
			void Release();

			float GetDisplayOffsetX() const;
			float GetDisplayOffsetY() const;
			float GetDisplayAdvanceX() const;
	};

	class AGKFont;

	class AGKSizedFont
	{
		protected:
			AGKFont *m_pParent;
			FT_FaceRec_ *m_pFTFace;
			
			int m_iImageWidth;
			cImage **m_pMainImages;
			cImage *m_pCurrImage;
			UINT m_iNumMainImages;
			unsigned short m_iCurrX;
			unsigned short m_iCurrY;
			unsigned short m_iMaxRowY;
			unsigned int m_iCreatedCount;

			cHashedList<AGKFontImage> m_CharImages;
			int m_iRefCount;
			int m_iUnusedCount;
			int m_iDesiredSize;
			int m_iActualSize;
			int m_iBaseLine;
			float m_fScale;

			void SetSize( int height );
			int AddToMainImage( AGKFontImage* pFontImage );

			static FT_LibraryRec_ *g_pFTLibrary;

			friend class AGKFont;

		public:
			AGKSizedFont( AGKFont *parent );
			~AGKSizedFont();
			void AddRef() { m_iRefCount++; }
			void Release();

			void RebuildImages();

			UINT GetNumMainImages() { return m_iNumMainImages; }
			cImage* GetMainImage( UINT index ) { return (index >= m_iNumMainImages) ? 0 : m_pMainImages[ index ]; }

			int GetSize() { return m_iDesiredSize; }

			int LoadFromFile( const uString& filename, int height );
			int LoadFromData( const unsigned char *pData, unsigned int dataSize, int height );

			AGKFontImage *GetCharImage( unsigned int unicodeChar, unsigned int style ); // in UTF-32. style: 0=normal, 1=bold, 2=italic, 3=both
	};

	class AGKFont
	{
		protected:
			cHashedList<AGKSizedFont> m_SizedFonts;
			uString m_sFontFile;
			unsigned char *m_pMemFont;
			unsigned int m_iMemFontSize;
			AGKFont *m_pNextFont;
			AGKFont *m_pPrevFont;

			static AGKFont *g_pDefaultFont;
			static int PlatformGetSystemFontPath( const uString &sFontName, uString &sOut );

			static AGKFont *g_pAllFonts;

		public:
			AGKFont();
			AGKFont( unsigned char* data, UINT compressedSize, UINT size );
			AGKFont( const uString &sFilename );
			~AGKFont();
			int GetLoaded() { return m_pMemFont ? 1 : 0; }

			void RebuildImages();
			static void RebuildAllFontImages();

			static AGKFont* GetDefaultFont();
			static int GetSystemFontExists( const uString &sfilename );

			int GetMaxSize() { return AGK_FONT_MAX_SIZE; }
			AGKSizedFont* GetSizedFont( int size );
	};
}

#endif