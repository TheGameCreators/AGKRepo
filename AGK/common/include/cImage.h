// Image Header
#ifndef _H_IMAGE_
#define _H_IMAGE_

// Common includes
#include "Common.h"
#include "uString.h"
#include "AGKShader.h"

#define MAX_IMAGES 0x7fffffff

enum AGKImgFormat
{
	AGK_COLOR_FORMAT_R_8 = 0,
	AGK_COLOR_FORMAT_RG_8,
	AGK_COLOR_FORMAT_RGBA_8,
	AGK_COLOR_FORMAT_R_F16,
	AGK_COLOR_FORMAT_RG_F16,
	AGK_COLOR_FORMAT_RGBA_F16,
	AGK_COLOR_FORMAT_R_F32,
	AGK_COLOR_FORMAT_RG_F32,
	AGK_COLOR_FORMAT_RGBA_F32,

	AGK_DEPTH_FORMAT_16_INT,
	AGK_DEPTH_FORMAT_24_INT,
	AGK_DEPTH_FORMAT_32_FLOAT,

	AGK_IMAGE_FORMAT_MAX = 0x7FFFFFFF
};

// must remain within the low 16bits, internal flags are in the high 16 bits
#define AGK_IMAGE_USE_MIPMAP			0x0001 // if GEN_MIPMAP not set then mipmap levels must be uploaded by the user
#define AGK_IMAGE_GEN_MIPMAP			0x0002 // generates levels from base layer
#define AGK_IMAGE_RENDERABLE			0x0004
#define AGK_IMAGE_MIN_FILTER_NEAREST	0x0008 // default is linear
#define AGK_IMAGE_MAG_FILTER_NEAREST	0x0010 // default is linear
#define AGK_IMAGE_MIPMAP_FILTER_NEAREST	0x0020 // default is linear
#define AGK_IMAGE_WRAP_U_REPEAT			0x0040 // default is clamp
#define AGK_IMAGE_WRAP_V_REPEAT			0x0080 // default is clamp
#define AGK_IMAGE_HAS_MODIFIED_UVS		0x0100 

#define AGK_MAX_TEXTURE_STAGES 16

struct GifFileType;

// Namespace
namespace AGK
{
	bool loadJpegImage(const char *name, int &outWidth, int &outHeight, bool &outHasAlpha, unsigned char **outData);
	void WriteJPEG(const char * filename, int quality, unsigned char *bits, int width, int height);
	bool loadPngImageFromMemory(unsigned char *name, int &outWidth, int &outHeight, bool &outHasAlpha, unsigned char **outData);
	bool loadPngImage(const char *name, int &outWidth, int &outHeight, bool &outHasAlpha, unsigned char **outData);
	void write_png(const char *file_name, int width, int height, uint32_t* bits);

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

			cSubImage() { x=0; y=0; width=0; height=0; pNextSubImage=0; }
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
		friend class AGKFrameBuffer;

		public:
			uint32_t				m_iID;

		protected:

			static uint32_t		g_iCreated;

			uint32_t			m_iCreated;
			uint32_t			m_iWidth; // the width/height of the image stored in the renderer with padding
			uint32_t			m_iHeight;
			uint32_t			m_iOrigWidth; // the width and height of the image stored in the filesystem without padding.
			uint32_t			m_iOrigHeight;
			uString				m_szFile;
            
			// if parent image is not null then this image is a sub image stored on a parent atlas texture
			cImage*				m_pParentImage;
			// for normal images this will be 0.0 -> 1.0, for sub images this will specify where on the texture atlas this image is
			float				m_fU1;
			float				m_fV1;
			float				m_fU2;
			float				m_fV2;
			AGKShaderConstantValue m_shaderVarTexBounds;
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
			bool				m_bResized;
			float				m_fScaledAmount;
            int                 m_iSpecialLoadMode;
			int					m_iNoMissingImage;
			bool				m_bIsCubeMap;
			bool				m_bHasAlpha;
			unsigned int		m_iImageMode; // 0 = RGBA, 1 = Depth, 2 = Alpha
			int                 m_iTextureOverridden;

			unsigned char* volatile m_pCompressedPixelData;
			volatile uint32_t	m_iCompressedLength;
			volatile int		m_iCompressed;

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

			cHashedList<cSprite*>* m_pCurrentSprites;
			cHashedList<cText*>*  m_pCurrentTextObjects;
			
			cImage*				m_pPrevImage;
			cImage*				m_pNextImage;
			static cImage*		g_pAllImages;

			uint32_t			m_iFlags;
			
			static int g_iAlphaColThreshold;
            static int g_iSavePixels;
			static int g_iLosingContext;

			static ImageCacher* g_pCachingThread;

			void Reset();
						
			void SetCompressedPixelData( unsigned char* pixels, unsigned int size );
			void SetCachePixelData( unsigned char *pixels, int width, int height, const char* filename, int compress=0 );
			static void CacheNewSize( const char* szFile, float scaleX, float scaleY, int maxTexSize );

			// get data
			bool GetGifFromFile( const char* szFile, unsigned char **pData, unsigned int *width, unsigned int *height );
			static bool PlatformGetDataFromFile( const char* szFile, unsigned char **pData, unsigned int *width, unsigned int *height );

			int GetBMPFromFile( cFile* pFile, unsigned char** pData, unsigned int* width, unsigned int* height );

			static void CommonResize( unsigned char * pSrc, int width1, int height1, unsigned char * pDest, int width2, int height2 );
			static void PlatformSaveDataToFile( const char* szFile, unsigned char *pData, unsigned int width, unsigned int height );

			void ReloadFromData();
			void UpdateGifImage();
			
		public:

			// for the renderer
			void* m_pInternalResource;
			void* GetInternalResource() { if ( m_pParentImage ) return m_pParentImage->GetInternalResource(); else return m_pInternalResource; }

			static void DeleteAllImages();
			//static void SaveAllImages();
			static void UpdateGifImages();
            static bool IsLosingContext() { return g_iLosingContext != 0; }

			static unsigned int GetImageSizeFromFile( const char* filename );

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

			AGKShaderConstantValue* GetShaderVarTexBounds() { return &m_shaderVarTexBounds; }
        
            void OverrideTexture( void* texID, int width, int height, AGKImgFormat format );

			void LoadFromData( int width, int height, unsigned int *bits, int format=0 ); // format: 0=RGBA, 1=NotUsed, 2=Alpha
			void LoadSubRegionFromData( int x, int y, int width, int height, unsigned char *bits );
			int GetRawData( unsigned char** pData );

			void SaveSubImages();
			
			void CreateFromScreen( int x, int y, int width, int height );
			void CopyFrom( cImage *pFromImage, int srcX, int srcY, int width, int height );
			bool ChooseFromSystem();
			bool CaptureFromCamera();
			int DecodeQR( uString &out );
			bool EncodeQR( const char* text, int errormode );
			void Print( float size );
			
			void CreateColorImage( uint32_t red, uint32_t green, uint32_t blue, uint32_t alpha );
			void CreateBlankImage( uint32_t width, uint32_t height, uint32_t format, uint32_t mipmap );

			uint32_t GetCreated() const { return m_iCreated; }
			uint32_t GetID() const { return m_iID; }
			float GetU1() const { return m_fU1; }
			float GetV1() const { return m_fV1; }
			float GetU2() const { return m_fU2; }
			float GetV2() const { return m_fV2; }
			int GetWidth() const { return m_iOrigWidth; }
			int GetHeight()	const { return m_iOrigHeight; }
			const char* GetPath() const { return m_szFile.GetStr(); }
			bool HasParent() const { return m_pParentImage != 0; }
			bool IsAtlas() const { return m_pSubImages != 0; }
			bool IsResized() const { return m_bResized; }
			float GetScaledAmount()	const { return m_fScaledAmount; }
			bool IsTransparent() const { if ( m_pParentImage ) return m_pParentImage->IsTransparent(); else return m_bHasAlpha; }
			bool IsDeleting() { return m_bDeleting; }
			cImage* GetParent() { return m_pParentImage; }
			cImage* GetRootImage() { if ( m_pParentImage ) return m_pParentImage->GetRootImage(); else return this; }
			bool ShouldGenerateMipmaps() { return (m_iFlags & AGK_IMAGE_GEN_MIPMAP) != 0; }
			bool HasModifiedUVs() { return (m_iFlags & AGK_IMAGE_HAS_MODIFIED_UVS) != 0; }

			int GetTotalWidth() const { if ( m_pParentImage ) return m_pParentImage->GetTotalWidth(); else return m_iWidth; }
			int GetTotalHeight() const { if ( m_pParentImage ) return m_pParentImage->GetTotalHeight(); else return m_iHeight; }
			int GetTotalWidth2() const { if ( m_pParentImage ) return m_pParentImage->GetTotalWidth2(); else return m_iOrigWidth; }
			int GetTotalHeight2() const { if ( m_pParentImage ) return m_pParentImage->GetTotalHeight2(); else return m_iOrigHeight; }
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

			void SetMagFilter( uint32_t mode );
			void SetMinFilter( uint32_t mode );

			void SetWrapU( uint32_t mode );
			void SetWrapV( uint32_t mode );

			void Save( const char* filename );
			void SetMask( cImage* pSrcImage, int dst, int src, int dstx, int dsty );
			void SetTransparentColor( int red, int green, int blue );
	};
}

#endif
