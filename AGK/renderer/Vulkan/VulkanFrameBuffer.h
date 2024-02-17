#ifndef _H_AGK_VULKAN_FRAMEBUFFER_
#define _H_AGK_VULKAN_FRAMEBUFFER_


namespace AGK
{
	#define AGK_VK_FRAMEBUFFER_FORCE_DEPTH	0x0001
	#define AGK_VK_FRAMEBUFFER_AWAITING_USE	0x0002
	#define AGK_VK_FRAMEBUFFER_IN_USE		0x0004
	#define AGK_VK_FRAMEBUFFER_DELETE		0x0008
	#define AGK_VK_FRAMEBUFFER_CLEARED		0x0010
	#define AGK_VK_FRAMEBUFFER_IS_SHADOW	0x0020

	class VulkanFrameBuffer
	{
		public:
			//global
			static VulkanFrameBuffer *g_pCurrFrameBuffer;
			
			static VulkanFrameBuffer *g_pAllFrameBuffers;
			VulkanFrameBuffer *m_pNextFrameBuffer = 0;
			VulkanFrameBuffer *m_pPrevFrameBuffer = 0;
			void RemoveFrameBuffer();
			void AddFrameBuffer();

			// local
			VulkanImage *m_pColor = 0;
			VulkanImage *m_pDepth = 0;

			float m_fClearRed = 0;
			float m_fClearGreen = 0;
			float m_fClearBlue = 0;
			float m_fClearAlpha = 0;
			
			VkFramebuffer m_vkFrameBuffer = 0;
			uint16_t m_iFlags = 0;
			uint16_t m_iWidth = 0;
			uint16_t m_iHeight = 0;
			uint8_t m_iRenderPassIndex = 0;

			// functions
			VulkanFrameBuffer() { AddFrameBuffer(); }
			~VulkanFrameBuffer() { RemoveFrameBuffer(); }

			void Clear( float red, float green, float blue, float alpha ) { m_iFlags |= AGK_VK_FRAMEBUFFER_CLEARED; m_fClearRed = red; m_fClearGreen = green; m_fClearBlue = blue; m_fClearAlpha = alpha; }

			bool ShouldDelete() { return (m_iFlags & AGK_VK_FRAMEBUFFER_DELETE) ? 1 : 0; }
			bool IsInUse() { return (m_iFlags & AGK_VK_FRAMEBUFFER_IN_USE) ? 1 : 0; }
			bool IsCleared() { return (m_iFlags & AGK_VK_FRAMEBUFFER_CLEARED) != 0; }
			void ResetClear() { m_iFlags &= ~AGK_VK_FRAMEBUFFER_CLEARED; }
			bool IsLocalDepth() { return (m_iFlags & AGK_VK_FRAMEBUFFER_FORCE_DEPTH) != 0; }
			bool IsShadow() { return (m_iFlags & AGK_VK_FRAMEBUFFER_IS_SHADOW) != 0; }
	};
}


#endif
