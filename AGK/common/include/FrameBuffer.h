// Image Header
#ifndef _H_FRAMEBUFFER_
#define _H_FRAMEBUFFER_

// Common includes
#include "Common.h"

#define AGK_FBO_SHARED_COLOR	0x02
#define AGK_FBO_SHARED_DEPTH	0x04
#define AGK_FBO_FORCE_DEPTH		0x08

// Namespace
namespace AGK
{
	class cImage;

	class AGKFrameBuffer
	{
		public:
			static AGKFrameBuffer* g_pAllFrameBuffers;
			static void DeleteImage( cImage *pImage );
			static AGKFrameBuffer* FindFrameBuffer( cImage *pColor, cImage* pDepth, bool forceDepth );
			static void ClearAll();
			static void ReloadAll();

			static AGKFrameBuffer* g_pCurrFrameBuffer;
			static void BindFrameBuffer( AGKFrameBuffer *pFBO );

		protected:
			cImage *m_pColor = 0;
			cImage *m_pDepth = 0;
			int m_bFlags = 0;
			uint32_t m_iWidth = 0;
			uint32_t m_iHeight = 0;

		public:
			AGKFrameBuffer *m_pNextFBO = 0;
			void *m_pInternalResource = 0;

			AGKFrameBuffer( cImage *pColor, bool bCShared, cImage* pDepth, bool bDShared, bool forceDepth );
			~AGKFrameBuffer();
						
			cImage* GetColorImage() { return m_pColor; }
			cImage* GetDepthImage() { return m_pDepth; }
			bool ShouldForceDepth() { return (m_bFlags & AGK_FBO_FORCE_DEPTH) != 0; }
			uint32_t GetWidth() { return m_iWidth; }
			uint32_t GetHeight() { return m_iHeight; }
			void SetShadow();
	};
}

#endif