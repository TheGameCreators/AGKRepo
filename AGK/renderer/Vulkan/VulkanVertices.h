#ifndef _H_AGK_VULKAN_VERTICES_
#define _H_AGK_VULKAN_VERTICES_

#include "VulkanFunctions.h"
#include "vk_mem_alloc.h"
#include "../Renderer.h"

// internal flags
#define AGK_VK_VERTICES_ANY_IN_USE		0x00000001
#define AGK_VK_VERTICES_DELETE			0x00000002
#define AGK_VK_VERTICES_HAS_TRANSFERS	0x00000004
#define AGK_VK_VERTICES_INDICES_INT		0x00000008

#define AGK_VK_VERTEX_BUFFER_AWAITING_USE	0x0001
#define AGK_VK_VERTEX_BUFFER_IN_USE			0x0002
#define AGK_VK_VERTEX_BUFFER_UNUSED			0x0004
#define AGK_VK_VERTEX_BUFFER_CAN_MAP		0x0008
#define AGK_VK_VERTEX_BUFFER_VERTEX_BUFFER	0x0010
#define AGK_VK_VERTEX_BUFFER_INDEX_BUFFER	0x0020

#define AGK_VK_BUFFER_TRANSFER_IN_PROGRESS	1
#define AGK_VK_BUFFER_TRANSFER_AWAITING		2
#define AGK_VK_BUFFER_TRANSFER_COMPLETED	3

namespace AGK
{
	class VulkanBufferData
	{
		public:
			VulkanBufferData() {}
			~VulkanBufferData() {}

			VkBuffer m_vkBuffer = 0;
			VmaAllocation m_vkBufferMemory = 0;
			void *m_pMappedData = 0; // may or may not be possible to map directly
			uint32_t m_iSize = 0;
			uint16_t m_iFlags = 0;
			uint16_t m_iUnusedCount = 0;

			// as soon as the buffer is used to draw then it can no longer be changed, future changes will need to create a new VulkanBufferData object
			void DidDraw() { m_iFlags |= AGK_VK_VERTEX_BUFFER_AWAITING_USE; } 
			bool IsAwaitingUse() { return (m_iFlags & AGK_VK_VERTEX_BUFFER_AWAITING_USE) != 0; }
			bool IsInUse() { return (m_iFlags & AGK_VK_VERTEX_BUFFER_IN_USE) != 0; }
			bool IsMapped() { return (m_iFlags & AGK_VK_VERTEX_BUFFER_CAN_MAP) != 0; }
			bool IsIndexBuffer() { return (m_iFlags & AGK_VK_VERTEX_BUFFER_INDEX_BUFFER) != 0; }
	};

	// only necessary to transfer on dedicated GPUs
	class VulkanBufferTransfer
	{
		public:
			VulkanBufferTransfer() {}
			~VulkanBufferTransfer() {}

			bool IsFree() { return m_iState == AGK_VK_BUFFER_TRANSFER_COMPLETED; }

			VulkanBufferData *dst = 0;
			VkBuffer src = 0;
			VmaAllocation srcMemory = 0;
			void *m_pMappedData = 0;
			uint32_t m_iOffset = 0;
			uint32_t m_iSize = 0;
			uint32_t m_iMaxSize = 0;
			uint16_t m_iState = 0;
			uint16_t m_iUnusedCount = 0;
	};

	// used when a partial transfer is done on a buffer that has already been bound for a draw call
	// buffer data must be copied into a fresh buffer to make the partial transfer
	class VulkanBufferCopy
	{
		public:
			VulkanBufferCopy() {}
			~VulkanBufferCopy() {}

			VulkanBufferData *dst = 0;
			VulkanBufferData *src = 0;
			uint32_t m_iSize = 0;
			uint16_t m_iState = 0;
			uint16_t m_iUnusedCount = 0;
	};

	class VulkanVertices
	{
		public:
			static VulkanVertices *g_pAllVertexObjects;
			VulkanVertices *m_pNextObject = 0;
			VulkanVertices *m_pPrevObject = 0;
			void RemoveVertexObject();
			void AddVertexObject();

			uint32_t m_iVertexSize = 0; // single vertex size in bytes
			uint32_t m_iNumVertices = 0;
			uint32_t m_iMaxNumIndices = 0;
			uint32_t m_iNumIndices = 0;
			uint32_t m_iFlags = 0;

			uint32_t m_iCurrentVertexDataIndex = 0;
			AGKUnorderedArray<VulkanBufferData*> m_pVertexData;

			uint32_t m_iCurrentIndexDataIndex = 0;
			AGKUnorderedArray<VulkanBufferData*> m_pIndexData;

			AGKOrderedArray<VulkanBufferTransfer*> m_pPartialTransfers;
			AGKUnorderedArray<VulkanBufferTransfer*> m_pFullTransfers;
			AGKOrderedArray<VulkanBufferCopy*> m_pCopyTransfers;

			AGKUnorderedArray<VulkanBufferTransfer*> m_pOldFullTransfers;
			AGKUnorderedArray<VulkanBufferTransfer*> m_pOldPartialTransfers;
			AGKUnorderedArray<VulkanBufferCopy*> m_pOldCopyTransfers;

			VulkanVertices();
			~VulkanVertices() { RemoveVertexObject(); }

			void DidDraw() 
			{ 
				if ( m_pVertexData.NumItems() > 0 ) m_pVertexData[ m_iCurrentVertexDataIndex ]->DidDraw(); 
				if ( m_pIndexData.NumItems() > 0 )  m_pIndexData[ m_iCurrentIndexDataIndex ]->DidDraw(); 
			} 
			bool ShouldDelete() { return (m_iFlags & AGK_VK_VERTICES_DELETE) != 0; }
			bool HasTransfers() { return (m_iFlags & AGK_VK_VERTICES_HAS_TRANSFERS) != 0; }
			bool IsDeleting() { return (m_iFlags & AGK_VK_VERTICES_DELETE) != 0; }

			int GetFreeVertexData( uint32_t requiredSize )
			{
				uint32_t currIndex = m_iCurrentVertexDataIndex;
				while( currIndex < m_pVertexData.NumItems() ) 
				{
					if ( (m_pVertexData[ currIndex ]->m_iFlags & (AGK_VK_VERTEX_BUFFER_IN_USE | AGK_VK_VERTEX_BUFFER_AWAITING_USE)) || m_pVertexData[ currIndex ]->m_iSize < requiredSize ) currIndex++;
					else return currIndex;
				}

				currIndex = 0;
				while( currIndex < m_iCurrentVertexDataIndex )
				{
					if ( (m_pVertexData[ currIndex ]->m_iFlags & (AGK_VK_VERTEX_BUFFER_IN_USE | AGK_VK_VERTEX_BUFFER_AWAITING_USE)) || m_pVertexData[ currIndex ]->m_iSize < requiredSize ) currIndex++;
					else return currIndex;
				}

				return -1;
			}
			int GetFreeIndexData( uint32_t requiredSize )
			{
				uint32_t currIndex = m_iCurrentIndexDataIndex;
				while( currIndex < m_pIndexData.NumItems() ) 
				{
					if ( (m_pIndexData[ currIndex ]->m_iFlags & (AGK_VK_VERTEX_BUFFER_IN_USE | AGK_VK_VERTEX_BUFFER_AWAITING_USE)) || m_pIndexData[ currIndex ]->m_iSize < requiredSize ) currIndex++;
					else return currIndex;
				}

				currIndex = 0;
				while( currIndex < m_iCurrentIndexDataIndex )
				{
					if ( (m_pIndexData[ currIndex ]->m_iFlags & (AGK_VK_VERTEX_BUFFER_IN_USE | AGK_VK_VERTEX_BUFFER_AWAITING_USE)) || m_pIndexData[ currIndex ]->m_iSize < requiredSize ) currIndex++;
					else return currIndex;
				}

				return -1;
			}
	};
}

#endif
