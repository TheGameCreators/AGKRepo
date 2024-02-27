// Image Header
#ifndef _H_IMAGE_
#define _H_IMAGE_

// Common includes
#include "Common.h"
#include "uString.h"
#include "Network.h"

#define MAX_IMAGES 0x7fffffff

struct GifFileType;

// Namespace
namespace AGK
{
	bool loadJpegImage(const char *name, int &outWidth, int &outHeight, bool &outHasAlpha, unsigned char **outData);
	void WriteJPEG(const char * filename, int quality, unsigned char *bits, int width, int height);
	bool loadPngImageFromMemory(unsigned char *name, int &outWidth, int &outHeight, bool &outHasAlpha, unsigned char **outData);
	bool loadPngImage(const char *name, int &outWidth, int &outHeight, bool &outHasAlpha, unsigned char **outData);
	void write_png(const char *file_name, int width, int height, UINT* bits);

	int ReadGifFile( GifFileType *file, unsigned char *buffer, int length );

	class Point2D;
	class cImage;
	class cSprite;
	class cText;
	class ImageCacher;
	class cFile;

	class cSubImage
	{
		public:

			uString szFilename;
			float x;
			float y;
			float width;
			float height;
			cSubImage *pNextSubImage;

			cSubImage() { x=0; y=0; width=0; height=0; pNextSubImage=UNDEF; }
			~cSubImage() {}
	};

	class cImageLink
	{
		public:
			cImage *m_pImage;
			cImageLink *m_pNext;

			cImageLink() { m_pImage = 0; m_pNext = 0; }
			~cImageLink() {}
	};

	class cSpriteLink
	{
		public:
			cSprite *m_pSprite;
			cSpriteLink *m_pNext;

			cSpriteLink() { m_pSprite = 0; m_pNext = 0; }
			~cSpriteLink() {}
	};

	/*
	class cTextLink
	{
		public:
			cText *m_pText;
			cTextLink *m_pNext;

			cTextLink() { m_pText = 0; m_pNext = 0; }
			~cTextLink() {}
	};
	*/

	class cGifFrame
	{
		public:
			unsigned char* pData;
			unsigned int iCompressedSize;
			float fDelay;

			cGifFrame() { pData = 0; iCompressedSize = 0; fDelay = 0; }
			~cGifFrame() { if ( pData ) delete [] pData; }
	};

	class _EXPORT_ cImage
	{
		friend class agk;
		friend class cText;
		friend class ImageCacher;

		public:
			UINT				m_iID;

		protected:

			static UINT			g_iCreated;

			UINT				m_iCreated;
			UINT				m_iTextureID;
			UINT				m_iWidth; // the width/height of the image stored in OpenGL with padding
			UINT				m_iHeight;
			UINT				m_iOrigWidth; // the width and height of the image stored in the filesystem without padding.
			UINT				m_iOrigHeight;
			uString				m_szFile;
            int                 m_iTextureOverridden;
			
			// if parent image is not null then this image is a sub image stored on a parent atlas texture
			cImage*				m_pParentImage;
			// for power of 2 images this will be 0.0 -> 1.0, for sub images this will specify where on the texture atlas this image is
			float				m_fU1;
			float				m_fV1;
			float				m_fU2;
			float				m_fV2;
			cSubImage*			m_pSubImages;
			// points creating a convex hull around the image, only generated when a sprite needs it.
			Point2D*			m_pBoundingPoints;
			int					m_iNumPoints;
			int					m_iMaxNumPoints;
			unsigned char*		m_pAlphaPixels;
			float				m_fLastU1;
			float				m_fLastV1;
			float				m_fLastU2;
			float				m_fLastV2;
			bool				m_bMipmapped;
			bool				m_bResized;
			float				m_fScaledAmount;
            int                 m_iSpecialLoadMode;
			int					m_iNoMissingImage;
			bool				m_bIsCubeMap;
			bool				m_bHasAlpha;

			unsigned char* volatile m_pCompressedPixelData;
			volatile UINT		m_iCompressedLength;
			volatile int		m_iCompressed;

			int					m_iMinFilter;
			int					m_iMagFilter;
			int					m_iWrapU;
			int					m_iWrapV;

			unsigned int		m_iImageMode; // 0 = RGBA, 1 = Depth, 2 = Alpha

			cImageLink*			m_pChildren;
			bool				m_bDeleting;
			bool				m_bDontLink;

			cImage **m_pLetterImages;
			cImage **m_pLetterImagesExt;

			cGifFrame **m_pGifFrames;
			unsigned int m_iGifNumFrames;
			unsigned int m_iGifCurrFrame;
			unsigned int m_iGifFramesArraySize;
			float m_fGifTime;
			cImage * m_pPrevGifImage;
			cImage * m_pNextGifImage;
			static cImage* g_pAllGifImages;

			//cSpriteLink*		m_pCurrentSprites;
			//cTextLink*			m_pCurrentTextObjects;
			cHashedList<cSprite>* m_pCurrentSprites;
			cHashedList<cText>*  m_pCurrentTextObjects;
			
			cImage*				m_pPrevImage;
			cImage*				m_pNextImage;
			static cImage*		g_pAllImages;
			
			static UINT iCurrTexture[8];
			static int g_iAlphaColThreshold;
            static int g_iSavePixels;
			static int g_iLosingContext;

			static ImageCacher* g_pCachingThread;

			// internal
			void Reset();

			// platform specific
			void PlatformReloadFromData();
			void PlatformDelete();
			void PlatformLoadFromData( int width, int height, UINT *bits );
			void PlatformSetSubData( int x, int y, int width, int height, unsigned char* pData );
			void PlatformCreateBlank( int width, int height, UINT format, UINT mipamp );

			void PlatformSetMagFilter( UINT mode );
			void PlatformSetMinFilter( UINT mode );
			void PlatformSetWrapU( UINT mode );
			void PlatformSetWrapV( UINT mode );

			void SetCompressedPixelData( unsigned char* pixels, unsigned int size );
			void SetCachePixelData( unsigned char *pixels, int width, int height, const char* filename, int compress=0 );
			static void CacheNewSize( const char* szFile, float scaleX, float scaleY, int maxTexSize );

			// get data
			bool GetGifFromFile( const char* szFile, unsigned char **pData, unsigned int *width, unsigned int *height );
			static bool PlatformGetDataFromFile( const char* szFile, unsigned char **pData, unsigned int *width, unsigned int *height );
			static void PlatformGetDataFromScreen( unsigned int** pData, int x, int y, int width, int height );

			int GetBMPFromFile( cFile* pFile, unsigned char** pData, unsigned int* width, unsigned int* height );

			static void CommonResize( unsigned char * pSrc, int width1, int height1, unsigned char * pDest, int width2, int height2 );
			static void PlatformSaveDataToFile( const char* szFile, unsigned char *pData, unsigned int width, unsigned int height );

			void UpdateGifImage();
			
		public:

			static void BindTexture( UINT iTex, UINT stage=0 );
			static void ReloadAllImages();
			static void SaveAllImages();
			static void UpdateGifImages();

			static unsigned int GetImageSizeFromFile( const char* filename );

			static int GetSupportsNPOT();
			static int GetMaxTextureSize();
			
			void AddChild( cImage *pChild );
			void RemoveChild( cImage *pChild );

			void AddSprite( cSprite *pSprite );
			void RemoveSprite( cSprite *pSprite );

			void AddText( cText *pText );
			void RemoveText( cText *pText );

			// construct/destruct
			cImage ( );
			cImage ( const uString &szFile );
			cImage ( const char* szFile );
			~cImage ( );
        
            void OverrideTexture( UINT texID, int width, int height );

			void LoadFromData( int width, int height, unsigned int *bits, int format=0 ); // format: 0=RGBA, 1=NotUsed, 2=Alpha
			void LoadSubRegionFromData( int x, int y, int width, int height, unsigned char *bits, int format=0 ); // format: 0=RGBA, 1=NotUsed, 2=Alpha
			int GetRawData( unsigned char** pData );
			int GetRawDataFull( unsigned char** pData );

			void SaveSubImages();

			void Bind( UINT stage=0 );
            void UnBind();

			void CreateFromScreen( int x, int y, int width, int height );
			void CopyFrom( cImage *pFromImage, int srcX, int srcY, int width, int height );
			bool ChooseFromSystem();
			bool CaptureFromCamera();
			int DecodeQR( uString &out );
			bool EncodeQR( const char* text, int errormode );
			void Print( float size );

			void GenerateMipmaps();

			void CreateColorImage( UINT red, UINT green, UINT blue, UINT alpha );
			void CreateBlankImage( UINT width, UINT height, UINT format, UINT mipmap );

			// internal - one line
			UINT GetCreated() const { return m_iCreated; }
			UINT GetID() const;
			float GetU1() const;
			float GetV1() const;
			float GetU2() const;
			float GetV2() const;
			int GetWidth() const;
			int GetHeight()	const;
			const char* GetPath() const;
			bool HasParent() const;
			bool HasUVBounds() const;
			bool IsAtlas() const;
			bool IsResized() const { return m_bResized; }
			float GetScaledAmount()	const { return m_fScaledAmount; }
			bool IsTransparent() const { if ( m_pParentImage ) return m_pParentImage->IsTransparent(); else return m_bHasAlpha; }
			bool IsDeleting() { return m_bDeleting; }
			cImage* GetParent() { return m_pParentImage; }

			// internal - cpp file
			unsigned int GetTextureID() const;
			int GetTotalWidth() const;
			int GetTotalHeight() const;
			int GetTotalWidth2() const;
			int GetTotalHeight2() const;
			bool Load( const char* szFile, bool bBlackToAlpha=false );
			bool Load( const uString &szFile, bool bBlackToAlpha=false );
			bool LoadPNGFromMemory( unsigned int* pInData, unsigned int *pSubImageData, unsigned int iNumSubImages, bool bBlackToAlpha=false );
			bool LoadSubImage( cImage *pParent, const char* szSubFile, bool bSilent=false );
			bool LoadSubImage( cImage *pParent, const uString &sSubFile, bool bSilent=false );
			void ManualSubImage( cImage *pParent, int x, int y, int width, int height );
			void OverrideSubImage( cImage *pParent, int width, int height, float u1, float v1, float u2, float v2 );
			void SetSubImages( const char* szSubImageFile );

			static void PreCache( const char* szFile, float scaleX, float scaleY );
			static int GetCachingFinished();

			bool LoadResized( const char* szFile, float scaleX, float scaleY, int cache );
			void Resize( int width, int height );

			const Point2D* GetBoundingPoints( float u1=-1, float v1=-1, float u2=1, float v2=1 );

			cImage** GetFontImages();
			cImage** GetExtendedFontImages();

			void CheckAndBuildAlphaPixels();
			void CreateAlphaPixels( int size );
			void SetAlphaPixel( int index, int value );
			int GetAlphaPixel( int x, int y );
			int GetAlphaPixelUV( float u, float v );

			void SetMagFilter( UINT mode );
			void SetMinFilter( UINT mode );

			void SetWrapU( UINT mode );
			void SetWrapV( UINT mode );

			void Save( const char* filename );
			void SetMask( cImage* pSrcImage, int dst, int src, int dstx, int dsty );
			void SetTransparentColor( int red, int green, int blue );
	};
}

#endif
