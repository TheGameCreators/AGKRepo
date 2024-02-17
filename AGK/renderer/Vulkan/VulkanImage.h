#ifndef _H_AGK_VULKAN_IMAGE_
#define _H_AGK_VULKAN_IMAGE_

#include "VulkanFunctions.h"
#include "vk_mem_alloc.h"
#include "../Renderer.h"

#include "cImage.h"

// internal flags in the high 16bits
#define AGK_VK_IMAGE_IS_DEPTH			0x00010000
#define AGK_VK_IMAGE_ANY_IN_USE			0x00020000
#define AGK_VK_IMAGE_AWAITING_USE		0x00040000
#define AGK_VK_IMAGE_IN_USE				0x00080000
#define AGK_VK_IMAGE_UNUSED				0x00100000
#define AGK_VK_IMAGE_IS_VR				0x00200000
#define AGK_VK_IMAGE_RENDERED			0x00400000
#define AGK_VK_IMAGE_DELETE				0x00800000
#define AGK_VK_IMAGE_NEEDS_MIPMAPS		0x01000000
#define AGK_VK_IMAGE_CLEAN_UP_DATA		0x02000000
#define AGK_VK_IMAGE_HAS_TRANSFERS		0x04000000
#define AGK_VK_IMAGE_SAMPLER_CHANGED	0x08000000
#define AGK_VK_IMAGE_IS_DEFAULT			0x10000000
#define AGK_VK_IMAGE_GPU_MODIFIED		0x20000000
#define AGK_VK_IMAGE_GPU_PREV_MODIFIED	0x40000000

#define AGK_VK_IMAGE_TRANSFER_AWAITING		1
#define AGK_VK_IMAGE_TRANSFER_IN_PROGRESS	2
#define AGK_VK_IMAGE_TRANSFER_COMPLETED		3

#define AGK_VK_IMAGE_MODE_UNDEFINED		0
#define AGK_VK_IMAGE_MODE_READ			1
#define AGK_VK_IMAGE_MODE_DRAW			2
#define AGK_VK_IMAGE_MODE_TRANSFER_DST	3
#define AGK_VK_IMAGE_MODE_TRANSFER_SRC	4

namespace AGK
{

	class VulkanSamplerPool
	{
		public:

			struct SamplerItem
			{
				uint32_t flags = 0;
				VkSampler pSampler = 0;
			};

			SamplerItem *m_pSamplers = 0;
			uint32_t m_iNumItems = 0;
			uint32_t m_iArraySize = 0;

			VulkanSamplerPool() 
			{ 
				m_iArraySize = 8; 
				m_pSamplers = new SamplerItem[m_iArraySize];  
			}		
		
			~VulkanSamplerPool() 
			{ 
				Clear(); 
				if ( m_pSamplers ) delete [] m_pSamplers;
			}

			void Clear();

			VkSampler Get( uint32_t flags );
	};

	class VulkanImageData
	{
			static uint64_t g_iLastCreatedID;

		public:
			VulkanImageData() { m_iCreatedID = g_iLastCreatedID; g_iLastCreatedID++; }
			~VulkanImageData() {}

			uint64_t m_iCreatedID;
			VkImageView m_vkImageView = 0;
			VkImageView m_vkImageViewFrameBuffer = 0;
			VkImage m_vkImage = 0;
			VmaAllocation m_vkImageMemory = 0;
			uint32_t m_iFlags = 0;
			uint16_t m_iCurrentMode = AGK_VK_IMAGE_MODE_UNDEFINED;
			uint16_t m_iUnusedCount = 0;

			// as soon as the image is used to draw then it can no longer be changed, future changes will need to create a new VulkanImageData object
			void DidBind() { m_iFlags |= AGK_VK_IMAGE_AWAITING_USE; } 
			void DidRender() { m_iFlags |= AGK_VK_IMAGE_AWAITING_USE | AGK_VK_IMAGE_GPU_MODIFIED; m_iCurrentMode = AGK_VK_IMAGE_MODE_DRAW; }
			bool IsAwaitingUse() { return (m_iFlags & AGK_VK_IMAGE_AWAITING_USE) != 0; }
			bool IsInUse() { return (m_iFlags & AGK_VK_IMAGE_IN_USE) != 0; }

			void GenerateMipmaps() { m_iFlags |= AGK_VK_IMAGE_NEEDS_MIPMAPS; } 
			bool NeedsMipmaps() { return (m_iFlags & AGK_VK_IMAGE_NEEDS_MIPMAPS) != 0; }
			void MipmapsDone() { m_iFlags &= ~AGK_VK_IMAGE_NEEDS_MIPMAPS; }
	};

	class VulkanImageTransfer
	{
		public:
			VulkanImageTransfer() {}
			~VulkanImageTransfer() {}

			bool IsFree() { return (m_iState >= AGK_VK_IMAGE_TRANSFER_COMPLETED) ? true : false; }

			VulkanImageData *dst = 0;
			VkBuffer src = 0;
			VmaAllocation srcMemory = 0;
			void *m_pMappedData = 0;
			uint32_t m_iWidth = 0;
			uint32_t m_iHeight = 0;
			uint32_t m_iOffsetX = 0;
			uint32_t m_iOffsetY = 0;
			uint16_t m_iUnusedCount = 0;
			uint8_t m_iMipLevel = 0;
			uint8_t m_iState = 0;
	};

	// used when a partial transfer is done on an image that has already been bound for a draw call
	// image data must be copied into a fresh image to make the partial transfer
	class VulkanImageCopy
	{
		public:
			VulkanImageCopy() {}
			~VulkanImageCopy() {}

			VulkanImageData *dst = 0;
			VulkanImageData *src = 0;
			uint32_t m_iState = 0;
	};

	class VulkanImage
	{
		public:
			// global
			static VulkanImage *g_pAllImages;
			VulkanImage *m_pNextImage = 0;
			VulkanImage *m_pPrevImage = 0;
			void RemoveImage();
			void AddImage();

			// local
			uint32_t m_iWidth = 0;
			uint32_t m_iHeight = 0;
			uint32_t m_iMipmapLevels = 1;
			uint32_t m_iFlags = 0;
			uint32_t m_iBytesPerPixel = 0;
			VkFormat m_vkFormat = VK_FORMAT_UNDEFINED;
			VkImageUsageFlags m_vkUsageFlags = 0;

			uint32_t m_iCurrentDataIndex = 0;
			AGKUnorderedArray<VulkanImageData*> m_pImageData;

			AGKOrderedArray<VulkanImageTransfer*> m_pPartialTransfers;
			AGKUnorderedArray<VulkanImageTransfer*> m_pFullTransfers;
			AGKOrderedArray<VulkanImageCopy*> m_pCopyTransfers;

			AGKUnorderedArray<VulkanImageTransfer*> m_pOldPartialTransfers;
			AGKUnorderedArray<VulkanImageTransfer*> m_pOldFullTransfers;
		
			VkSampler m_vkSampler = 0;

			// functions
			VulkanImage() { AddImage(); }
			~VulkanImage() { RemoveImage(); }

			void WillBind() { m_pImageData[ m_iCurrentDataIndex ]->DidBind(); } 
			void WillRender() { m_pImageData[ m_iCurrentDataIndex ]->DidRender(); }
			bool ShouldDelete() { return (m_iFlags & AGK_VK_IMAGE_DELETE) != 0; }
			bool WillGenerateMipmaps() { return (m_iFlags & AGK_IMAGE_GEN_MIPMAP) ? 1 : 0; }
			bool HasTransfers() { return (m_iFlags & AGK_VK_IMAGE_HAS_TRANSFERS) ? 1 : 0; }
			bool IsRenderable() { return (m_iFlags & AGK_IMAGE_RENDERABLE) ? 1 : 0; }
			bool IsDepth() { return (m_iFlags & AGK_VK_IMAGE_IS_DEPTH) ? 1 : 0; }
			bool IsUndefined() { return (m_pImageData[ m_iCurrentDataIndex ]->m_iCurrentMode == AGK_VK_IMAGE_MODE_UNDEFINED) ? 1 : 0; }
			bool IsVR() { return (m_iFlags & AGK_VK_IMAGE_IS_VR) != 0; }

			VulkanImageData* GetCurrImageData() { return m_pImageData[ m_iCurrentDataIndex ]; }
			int GetFreeImageData()
			{
				uint32_t currIndex = m_iCurrentDataIndex;
				while( currIndex < m_pImageData.NumItems() ) 
				{
					if ( m_pImageData[ currIndex ]->m_iFlags & (AGK_VK_IMAGE_IN_USE | AGK_VK_IMAGE_AWAITING_USE) ) currIndex++;
					else return currIndex;
				}

				currIndex = 0;
				while( currIndex < m_iCurrentDataIndex )
				{
					if ( m_pImageData[ currIndex ]->m_iFlags & (AGK_VK_IMAGE_IN_USE | AGK_VK_IMAGE_AWAITING_USE) ) currIndex++;
					else return currIndex;
				}

				return -1;
			}

			VkImageView GetCurrentView() { return m_pImageData[ m_iCurrentDataIndex ]->m_vkImageView; }
			VkImageView GetCurrentFrameBufferView() 
			{
				m_pImageData[ m_iCurrentDataIndex ]->m_iFlags |= AGK_VK_IMAGE_RENDERED;
				return m_pImageData[ m_iCurrentDataIndex ]->m_vkImageViewFrameBuffer; 
			}
	};
}

#endif