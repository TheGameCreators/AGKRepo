#if !defined(DISABLE_VULKAN) && (!defined(__APPLE__) || defined(__LP64__))

#include "AppErrors.h"
#include "AGKVulkan.h"

using namespace AGK;

VulkanVertices* VulkanVertices::g_pAllVertexObjects = 0;

void VulkanVertices::RemoveVertexObject() 
{
	if ( !m_pNextObject && !m_pPrevObject && g_pAllVertexObjects != this ) return;

	if ( m_pPrevObject ) m_pPrevObject->m_pNextObject = m_pNextObject;
	else g_pAllVertexObjects = m_pNextObject;
	if ( m_pNextObject ) m_pNextObject->m_pPrevObject = m_pPrevObject;
	m_pNextObject = 0;
	m_pPrevObject = 0;
}

void VulkanVertices::AddVertexObject()
{
	if ( m_pNextObject || m_pPrevObject || g_pAllVertexObjects == this ) return;

	if ( g_pAllVertexObjects ) g_pAllVertexObjects->m_pPrevObject = this;
	m_pPrevObject = 0;
	m_pNextObject = g_pAllVertexObjects;
	g_pAllVertexObjects = this;
}

VulkanVertices::VulkanVertices() 
{ 
	AddVertexObject(); 
}

int VulkanRenderer::InternalCreateVertexBuffer( VulkanVertices *pVertices, VulkanBufferData *pNewBuffer )
{
	pNewBuffer->m_iSize = pVertices->m_iNumVertices * pVertices->m_iVertexSize;

	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = pNewBuffer->m_iSize;
	bufferInfo.usage = AGK_BUFFER_VERTEX | AGK_BUFFER_TRANSFER_DST;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VmaAllocationCreateInfo allocInfo = {};
	allocInfo.requiredFlags = AGK_MEM_DEVICE;
	if ( bufferInfo.size > 8 * 1024 * 1024 ) allocInfo.pool = 0;
	else allocInfo.pool = m_vmaVertexBufferPool;
	
	VmaAllocationInfo allocationInfo;
	VkResult res = vmaCreateBuffer( m_vkAllocator, &bufferInfo, &allocInfo, &pNewBuffer->m_vkBuffer, &pNewBuffer->m_vkBufferMemory, &allocationInfo );
	if ( VK_SUCCESS != res ) ReportAndReturnErrorID( APP_ERROR_VK_CREATE_BUFFER_FAILED, res );

	// map it if possible
	if ( (m_vkGPUMemoryProps.memoryTypes[ allocationInfo.memoryType ].propertyFlags & AGK_MEM_STAGING) == AGK_MEM_STAGING )
	{
		pNewBuffer->m_iFlags |= AGK_VK_VERTEX_BUFFER_CAN_MAP;
		int res = vmaMapMemory( m_vkAllocator, pNewBuffer->m_vkBufferMemory, &pNewBuffer->m_pMappedData );
		if ( VK_SUCCESS != res ) ReportAndReturnErrorID( APP_ERROR_VK_MAP_MEM_FAILED, res );
	}
		
	return APP_SUCCESS;
}

int VulkanRenderer::InternalCreateIndexBuffer( VulkanVertices *pVertices, VulkanBufferData *pNewBuffer )
{
	uint32_t indexSize = sizeof(unsigned short);
	if ( pVertices->m_iFlags & AGK_VK_VERTICES_INDICES_INT ) indexSize = sizeof(unsigned int);

	pNewBuffer->m_iSize = pVertices->m_iNumIndices * indexSize;

	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = pNewBuffer->m_iSize;
	bufferInfo.usage = AGK_BUFFER_INDEX | AGK_BUFFER_TRANSFER_DST;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VmaAllocationCreateInfo allocInfo = {};
	allocInfo.requiredFlags = AGK_MEM_DEVICE;
	if ( bufferInfo.size > 8 * 1024 * 1024 ) allocInfo.pool = 0;
	else allocInfo.pool = m_vmaVertexBufferPool;

	VmaAllocationInfo allocationInfo;
	VkResult res = vmaCreateBuffer( m_vkAllocator, &bufferInfo, &allocInfo, &pNewBuffer->m_vkBuffer, &pNewBuffer->m_vkBufferMemory, &allocationInfo );
	if ( VK_SUCCESS != res ) ReportAndReturnErrorID( APP_ERROR_VK_CREATE_BUFFER_FAILED, res );

	// map it if possible
	if ( (m_vkGPUMemoryProps.memoryTypes[ allocationInfo.memoryType ].propertyFlags & AGK_MEM_STAGING) == AGK_MEM_STAGING )
	{
		pNewBuffer->m_iFlags |= AGK_VK_VERTEX_BUFFER_CAN_MAP;
		int res = vmaMapMemory( m_vkAllocator, pNewBuffer->m_vkBufferMemory, &pNewBuffer->m_pMappedData );
		if ( VK_SUCCESS != res ) ReportAndReturnErrorID( APP_ERROR_VK_MAP_MEM_FAILED, res );
	}
		
	return APP_SUCCESS;
}

int VulkanRenderer::InternalCreateFullBufferTransfer( VulkanVertices *pVertices, VulkanBufferData *pDst, uint32_t size, void* data )
{
	if ( pDst->IsMapped() )
	{
		memcpy( pDst->m_pMappedData, data, size );
		return APP_SUCCESS;
	}

	VulkanBufferTransfer *pTransfer = 0;
	
	// look for an old transfer that we can repurpose
	for( uint32_t i = 0; i < pVertices->m_pOldFullTransfers.NumItems(); i++ )
	{
		VulkanBufferTransfer *pTransferCheck = pVertices->m_pOldFullTransfers[i];

		// exact size doesn't matter, as long as it is big enough and doesn't waste too much memory
		if ( pTransferCheck->m_iMaxSize >= size && pTransferCheck->m_iMaxSize < size*2 )
		{
			pVertices->m_pOldFullTransfers.RemoveIndex( i );
			pTransfer = pTransferCheck;
			pTransfer->m_iUnusedCount = 0;
			break;
		}
	}

	if ( !pTransfer ) 
	{
		pTransfer = new VulkanBufferTransfer();
		pTransfer->m_iMaxSize = size;
	}

	pVertices->m_pFullTransfers.AddItem( pTransfer ); // adds it to the end

		// remove all partial transfers for this buffer
	for( int i = 0; i < (int) pVertices->m_pPartialTransfers.NumItems(); i++ )
	{
		VulkanBufferTransfer *pTransferCheck = pVertices->m_pPartialTransfers[i];
		if ( pTransferCheck->m_iState == AGK_VK_BUFFER_TRANSFER_AWAITING && pTransferCheck->dst == pDst )
		{
			// don't delete it in case this scenario occurs again and we constantly create and delete it
			pTransferCheck->m_iState = AGK_VK_BUFFER_TRANSFER_COMPLETED;
			pVertices->m_pPartialTransfers.RemoveIndex( i );
			pVertices->m_pOldPartialTransfers.AddItem( pTransferCheck );
			i--;
		}
	}
	
	pTransfer->m_iOffset = 0;
	pTransfer->m_iSize = size;
	pTransfer->m_iState = AGK_VK_BUFFER_TRANSFER_AWAITING;
	pTransfer->dst = pDst;

	if ( !pTransfer->src )
	{
		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = pTransfer->m_iMaxSize;
		bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VmaAllocationCreateInfo allocCreateInfo = {};
		allocCreateInfo.requiredFlags = AGK_MEM_STAGING; // ignored if pool is set
		if ( bufferInfo.size > 2 * 1024 * 1024 ) allocCreateInfo.pool = 0;
		else allocCreateInfo.pool = m_vmaBufferTransferPool;

		VmaAllocationInfo allocInfo;
		VkResult res = vmaCreateBuffer( m_vkAllocator, &bufferInfo, &allocCreateInfo, &pTransfer->src, &pTransfer->srcMemory, &allocInfo );
		if ( VK_SUCCESS != res ) ReportAndReturnErrorID( APP_ERROR_VK_CREATE_BUFFER_FAILED, res );
	}
	
	if ( !pTransfer->m_pMappedData )
	{
		VkResult res = vmaMapMemory( m_vkAllocator, pTransfer->srcMemory, &pTransfer->m_pMappedData );
		if ( VK_SUCCESS != res ) ReportAndReturnErrorID( APP_ERROR_VK_MAP_MEM_FAILED, res );
	}
		
	memcpy( pTransfer->m_pMappedData, data, size );

	if ( !pVertices->HasTransfers() )
	{
		pVertices->m_iFlags |= AGK_VK_VERTICES_HAS_TRANSFERS;
		m_pVerticesWithTransfers.AddItem( pVertices ); // duplicates are ok, but this check should stop that
	}
		
	return APP_SUCCESS;
}

int VulkanRenderer::InternalCreatePartialBufferTransfer( VulkanVertices *pVertices, VulkanBufferData *pDst, uint32_t offset, uint32_t size, void* data )
{
	if ( pDst->IsMapped() )
	{
		memcpy( ((unsigned char*)pDst->m_pMappedData) + offset, data, size );
		return APP_SUCCESS;
	}

	VulkanBufferTransfer *pTransfer = 0;
	
	// look for an old transfer that we can repurpose
	for( uint32_t i = 0; i < pVertices->m_pOldPartialTransfers.NumItems(); i++ )
	{
		VulkanBufferTransfer *pTransferCheck = pVertices->m_pOldPartialTransfers[i];
		if ( pTransferCheck->IsFree() 
			/*|| (pTransferCheck->m_iState == AGK_VK_BUFFER_TRANSFER_AWAITING && pTransferCheck->dst == pDst && pTransferCheck->m_iOffset == offset)*/ ) // extra check will never apply to transfers in the old  partial list
		{
			// exact size doesn't matter, as long as it is big enough and doesn't waste too much memory
			if ( pTransferCheck->m_iMaxSize >= size && pTransferCheck->m_iMaxSize < size*2 )
			{
				pVertices->m_pOldPartialTransfers.RemoveIndex( i );
				pTransfer = pTransferCheck;
				pTransfer->m_iUnusedCount = 0;
				break;
			}
		}
	}

	if ( !pTransfer ) 
	{
		pTransfer = new VulkanBufferTransfer();
		pTransfer->m_iMaxSize = size;
	}

	pVertices->m_pPartialTransfers.AddItem( pTransfer );

	// must not delete any existing transfers in this function as it may be called from the loop that
	// copies all partial transfers to a new image. Deleting any will shift the remaining transfers
	// to different indices and potentially miss them in the copy loop.
	
	pTransfer->m_iOffset = offset;
	pTransfer->m_iSize = size;
	pTransfer->m_iState = AGK_VK_BUFFER_TRANSFER_AWAITING;
	pTransfer->dst = pDst;

	if ( !pTransfer->src )
	{
		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = pTransfer->m_iMaxSize;
		bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VmaAllocationCreateInfo allocCreateInfo = {};
		allocCreateInfo.requiredFlags = AGK_MEM_STAGING; // ignored if pool is set
		if ( bufferInfo.size > 2 * 1024 * 1024 ) allocCreateInfo.pool = 0;
		else allocCreateInfo.pool = m_vmaBufferTransferPool;

		VmaAllocationInfo allocInfo;
		VkResult res = vmaCreateBuffer( m_vkAllocator, &bufferInfo, &allocCreateInfo, &pTransfer->src, &pTransfer->srcMemory, &allocInfo );
		if ( VK_SUCCESS != res ) ReportAndReturnErrorID( APP_ERROR_VK_CREATE_BUFFER_FAILED, res );
	}
	
	if ( !pTransfer->m_pMappedData )
	{
		VkResult res = vmaMapMemory( m_vkAllocator, pTransfer->srcMemory, &pTransfer->m_pMappedData );
		if ( VK_SUCCESS != res ) ReportAndReturnErrorID( APP_ERROR_VK_MAP_MEM_FAILED, res );
	}
		
	memcpy( pTransfer->m_pMappedData, data, size );

	if ( !pVertices->HasTransfers() )
	{
		pVertices->m_iFlags |= AGK_VK_VERTICES_HAS_TRANSFERS;
		m_pVerticesWithTransfers.AddItem( pVertices ); // duplicates are ok, but this check should stop that
	}
		
	return APP_SUCCESS;
}

int VulkanRenderer::InternalDeleteBufferTransfer( VulkanBufferTransfer *pTransfer )
{
	if ( pTransfer->src ) 
	{
		if ( pTransfer->m_pMappedData ) vmaUnmapMemory( m_vkAllocator, pTransfer->srcMemory );
		vmaDestroyBuffer( m_vkAllocator, pTransfer->src, pTransfer->srcMemory );
	}

	delete pTransfer;
	return APP_SUCCESS;
}

int VulkanRenderer::InternalTransferVertices( VulkanVertices *pVertices )
{
	//if ( pVertices->ShouldDelete() ) return APP_SUCCESS;

	// order must be full transfers, then copies, then partial transfers

	// full transfers
	for( uint32_t i = 0; i < pVertices->m_pFullTransfers.NumItems(); i++ )
	{
		VulkanBufferTransfer *pTransfer = pVertices->m_pFullTransfers[i];
		if ( pTransfer->m_iState == AGK_VK_BUFFER_TRANSFER_AWAITING )
		{
			// transfer queue
			VkBufferCopy copyRegion = {};
			copyRegion.srcOffset = 0;
			copyRegion.dstOffset = pTransfer->m_iOffset;
			copyRegion.size = pTransfer->m_iSize;
			vkCmdCopyBuffer( m_vkTransferVerticesCommandBuffer[m_iCurrentFrame], pTransfer->src, pTransfer->dst->m_vkBuffer, 1, &copyRegion );

			VkBufferMemoryBarrier memoryBarrier = {};
			memoryBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
			memoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			memoryBarrier.dstAccessMask = 0;
			memoryBarrier.srcQueueFamilyIndex = m_iTransferQueueFamilyIndex;
			memoryBarrier.dstQueueFamilyIndex = m_iQueueFamilyIndex;
			memoryBarrier.buffer = pTransfer->dst->m_vkBuffer;
			memoryBarrier.offset = pTransfer->m_iOffset;
			memoryBarrier.size = pTransfer->m_iSize;
            
            if ( m_iTransferQueueFamilyIndex == m_iQueueFamilyIndex )
            {
                memoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                memoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                memoryBarrier.dstAccessMask = pTransfer->dst->IsIndexBuffer() ? VK_ACCESS_INDEX_READ_BIT : VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
                
                vkCmdPipelineBarrier( m_vkTransferVerticesCommandBuffer[m_iCurrentFrame], VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, 0, 0, 0, 1, &memoryBarrier, 0, 0 );
            }
            else
            {
                vkCmdPipelineBarrier( m_vkTransferVerticesCommandBuffer[m_iCurrentFrame], VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, 0, 1, &memoryBarrier, 0, 0 );

                // graphics queue
                memoryBarrier = {};
                memoryBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
                memoryBarrier.srcAccessMask = 0;
                memoryBarrier.dstAccessMask = pTransfer->dst->IsIndexBuffer() ? VK_ACCESS_INDEX_READ_BIT : VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
                memoryBarrier.srcQueueFamilyIndex = m_iTransferQueueFamilyIndex;
                memoryBarrier.dstQueueFamilyIndex = m_iQueueFamilyIndex;
                memoryBarrier.buffer = pTransfer->dst->m_vkBuffer;
                memoryBarrier.offset = pTransfer->m_iOffset;
                memoryBarrier.size = pTransfer->m_iSize;

                vkCmdPipelineBarrier( m_vkReceiveVerticesCommandBuffer[m_iCurrentFrame], VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, 0, 0, 0, 1, &memoryBarrier, 0, 0 );
            }
		}
	}

	// copies
	for( uint32_t i = 0; i < pVertices->m_pCopyTransfers.NumItems(); i++ )
	{
		VulkanBufferCopy *pTransfer = pVertices->m_pCopyTransfers[i];
		if ( pTransfer->m_iState == AGK_VK_BUFFER_TRANSFER_AWAITING )
		{
			VkBufferCopy copyRegion = {};
			copyRegion.srcOffset = 0;
			copyRegion.dstOffset = 0;
			copyRegion.size = pTransfer->m_iSize;
			vkCmdCopyBuffer( m_vkReceiveVerticesCommandBuffer[m_iCurrentFrame], pTransfer->src->m_vkBuffer, pTransfer->dst->m_vkBuffer, 1, &copyRegion );

			VkBufferMemoryBarrier memoryBarrier = {};
			memoryBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
			memoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			memoryBarrier.dstAccessMask = pTransfer->dst->IsIndexBuffer() ? VK_ACCESS_INDEX_READ_BIT : VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
			memoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			memoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			memoryBarrier.buffer = pTransfer->dst->m_vkBuffer;
			memoryBarrier.offset = 0;
			memoryBarrier.size = pTransfer->m_iSize;

			vkCmdPipelineBarrier( m_vkReceiveVerticesCommandBuffer[m_iCurrentFrame], VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, 0, 0, 0, 1, &memoryBarrier, 0, 0 );

			m_iFrameHasGraphicsVertexTransfers = 1;
		}
	}

	// partial transfers
	for( uint32_t i = 0; i < pVertices->m_pPartialTransfers.NumItems(); i++ )
	{
		VulkanBufferTransfer *pTransfer = pVertices->m_pPartialTransfers[i];
		if ( pTransfer->m_iState == AGK_VK_BUFFER_TRANSFER_AWAITING )
		{
			VkBufferCopy copyRegion = {};
			copyRegion.srcOffset = 0;
			copyRegion.dstOffset = pTransfer->m_iOffset;
			copyRegion.size = pTransfer->m_iSize;
			vkCmdCopyBuffer( m_vkReceiveVerticesCommandBuffer[m_iCurrentFrame], pTransfer->src, pTransfer->dst->m_vkBuffer, 1, &copyRegion );

			VkBufferMemoryBarrier memoryBarrier = {};
			memoryBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
			memoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			memoryBarrier.dstAccessMask = pTransfer->dst->IsIndexBuffer() ? VK_ACCESS_INDEX_READ_BIT : VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
			memoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			memoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			memoryBarrier.buffer = pTransfer->dst->m_vkBuffer;
			memoryBarrier.offset = pTransfer->m_iOffset;
			memoryBarrier.size = pTransfer->m_iSize;

			vkCmdPipelineBarrier( m_vkReceiveVerticesCommandBuffer[m_iCurrentFrame], VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, 0, 0, 0, 1, &memoryBarrier, 0, 0 );

			m_iFrameHasGraphicsVertexTransfers = 1;
		}
	}

	return APP_SUCCESS;
}

int VulkanRenderer::CreateVertices( uint32_t vertexSize, void **outResource )
{
	VulkanVertices *outVertices = (VulkanVertices*) *outResource;
	if ( outVertices )
	{
		if ( vertexSize == outVertices->m_iVertexSize ) return APP_SUCCESS;

		DeleteVertices( outVertices );
	}

	outVertices = new VulkanVertices();
	*outResource = outVertices;

	outVertices->m_iVertexSize = vertexSize;
	outVertices->m_iNumIndices = 0;
	outVertices->m_iNumVertices = 0;
			
	return APP_SUCCESS;
}

// can update a subset of vertices or the full buffer
int VulkanRenderer::UpdateVertices( void *pResource, float *pVertexData, uint32_t offset, uint32_t numVertices )
{
	if ( !pResource ) return APP_ERROR_UNKNOWN;
	if ( !pVertexData ) return APP_ERROR_UNKNOWN;
	
	VulkanVertices *pVertices = (VulkanVertices*) pResource;
	if ( pVertices->IsDeleting() ) return APP_ERROR_GENERAL;

	if ( offset + numVertices > pVertices->m_iNumVertices ) ReportAndReturnErrorID( APP_ERROR_VK_VERTEX_UPDATE_OUT_OF_BOUNDS, offset, numVertices, pVertices->m_iNumVertices );
	if ( pVertices->m_pVertexData.NumItems() == 0 ) return APP_ERROR_GENERAL;

	VulkanBufferData *pCurrData = pVertices->m_pVertexData[ pVertices->m_iCurrentVertexDataIndex ];
	uint32_t requiredSize = numVertices * pVertices->m_iVertexSize;

	if ( offset == 0 && numVertices == pVertices->m_iNumVertices )
	{
		// full transfer
		if ( pCurrData->IsInUse() || pCurrData->IsAwaitingUse() )
		{
			VulkanBufferData *pNewData;
			int index = pVertices->GetFreeVertexData( requiredSize );
			if ( index >= 0 ) 
			{
				pNewData = pVertices->m_pVertexData[ index ];
				pVertices->m_iCurrentVertexDataIndex = index;
			}
			else
			{
				pNewData = new VulkanBufferData();
				pNewData->m_iFlags |= AGK_VK_VERTEX_BUFFER_VERTEX_BUFFER;
				pVertices->m_pVertexData.AddItem( pNewData );
				pVertices->m_iCurrentVertexDataIndex = pVertices->m_pVertexData.NumItems()-1;
				ReturnIfFail( InternalCreateVertexBuffer(pVertices, pNewData) );
			}

			pCurrData = pNewData;
		}

		ReturnIfFail( InternalCreateFullBufferTransfer(pVertices, pCurrData, numVertices*pVertices->m_iVertexSize, pVertexData) );
	}
	else
	{
		// partial transfer needs existing data, can't use a blank buffer
		if ( pCurrData->IsAwaitingUse() || (pCurrData->IsMapped() && pCurrData->IsInUse()) )
		{
			// current buffer contents must be preserved as it is already used in a draw call in this frame
			VulkanBufferData *pNewData;
			int index = pVertices->GetFreeVertexData( requiredSize );
			if ( index >= 0 ) 
			{
				pNewData = pVertices->m_pVertexData[ index ];
				pVertices->m_iCurrentVertexDataIndex = index;
			}
			else
			{
				pNewData = new VulkanBufferData();
				pNewData->m_iFlags |= AGK_VK_VERTEX_BUFFER_VERTEX_BUFFER;
				pVertices->m_pVertexData.AddItem( pNewData );
				pVertices->m_iCurrentVertexDataIndex = pVertices->m_pVertexData.NumItems()-1;
				ReturnIfFail( InternalCreateVertexBuffer(pVertices, pNewData) );
			}

			if ( pNewData->IsMapped() && pCurrData->IsMapped() )
			{
				memcpy( pNewData->m_pMappedData, pCurrData->m_pMappedData, pVertices->m_iNumVertices*pVertices->m_iVertexSize );
			}
			else
			{
				// create copy transfer from current to new
				VulkanBufferCopy *pCopy = new VulkanBufferCopy();
				pVertices->m_pCopyTransfers.AddItem( pCopy ); // add it to the end
				pCopy->dst = pNewData;
				pCopy->src = pCurrData;
				pCopy->m_iSize = pVertices->m_iNumVertices * pVertices->m_iVertexSize;
				pCopy->m_iState = AGK_VK_BUFFER_TRANSFER_AWAITING;

				// must also dulplicate any partial transfers that are pending on the current data
				for( uint32_t i = 0; i < pVertices->m_pPartialTransfers.NumItems(); i++ )
				{
					VulkanBufferTransfer *pTransfer = pVertices->m_pPartialTransfers[i];
					if ( pTransfer->m_iState == AGK_VK_BUFFER_TRANSFER_AWAITING && pTransfer->dst == pCurrData )
					{
						ReturnIfFail( InternalCreatePartialBufferTransfer(pVertices, pNewData, pTransfer->m_iOffset, pTransfer->m_iSize, pTransfer->m_pMappedData) );
					}
				}
			}
			pCurrData = pNewData;
		}
		else
		{
			// if previous frame is using the buffer then that's fine as long as we wait for it to finish
			if ( pCurrData->IsInUse() ) m_iVerticesWaitForPrevFrame = 1;
		}

		ReturnIfFail( InternalCreatePartialBufferTransfer(pVertices, pCurrData, offset*pVertices->m_iVertexSize, numVertices*pVertices->m_iVertexSize, pVertexData) );
	}
	
	return APP_SUCCESS;
}

// pIndexData must be unsigned short if original indices were unsigned short
int VulkanRenderer::UpdateIndices( void *pResource, void *pIndexData, uint32_t offset, uint32_t numIndices )
{
	if ( !pResource ) return APP_ERROR_UNKNOWN;
	if ( !pIndexData ) return APP_ERROR_UNKNOWN;

	VulkanVertices *pVertices = (VulkanVertices*) pResource;
	if ( pVertices->IsDeleting() ) return APP_ERROR_GENERAL;

	if ( offset + numIndices > pVertices->m_iNumIndices ) ReportAndReturnErrorID( APP_ERROR_VK_INDEX_UPDATE_OUT_OF_BOUNDS, offset, numIndices, pVertices->m_iNumIndices );
	if ( pVertices->m_pIndexData.NumItems() == 0 ) return APP_ERROR_GENERAL;

	VulkanBufferData *pCurrData = pVertices->m_pIndexData[ pVertices->m_iCurrentIndexDataIndex ];
	
	uint32_t indexSize = sizeof(unsigned short);
	if ( pVertices->m_iFlags & AGK_VK_VERTICES_INDICES_INT ) indexSize = sizeof(unsigned int);

	if ( offset == 0 && numIndices == pVertices->m_iNumIndices )
	{
		// full transfer
		if ( pCurrData->IsInUse() || pCurrData->IsAwaitingUse() )
		{
			VulkanBufferData *pNewData;
			int index = pVertices->GetFreeIndexData( numIndices * indexSize );
			if ( index >= 0 ) 
			{
				pNewData = pVertices->m_pIndexData[ index ];
				pVertices->m_iCurrentIndexDataIndex = index;
			}
			else
			{
				pNewData = new VulkanBufferData();
				pNewData->m_iFlags |= AGK_VK_VERTEX_BUFFER_INDEX_BUFFER;
				pVertices->m_pIndexData.AddItem( pNewData );
				pVertices->m_iCurrentIndexDataIndex = pVertices->m_pIndexData.NumItems()-1;
				ReturnIfFail( InternalCreateIndexBuffer(pVertices, pNewData) );
			}

			pCurrData = pNewData;
		}

		ReturnIfFail( InternalCreateFullBufferTransfer(pVertices, pCurrData, numIndices*indexSize, pIndexData) );
	}
	else
	{
		// partial transfer needs existing data, can't use a blank buffer
		if ( pCurrData->IsAwaitingUse() || (pCurrData->IsMapped() && pCurrData->IsInUse()) )
		{
			// current buffer contents must be preserved as it is already used in a draw call in this frame
			VulkanBufferData *pNewData;
			int index = pVertices->GetFreeIndexData( numIndices * indexSize );
			if ( index >= 0 ) 
			{
				pNewData = pVertices->m_pIndexData[ index ];
				pVertices->m_iCurrentIndexDataIndex = index;
			}
			else
			{
				pNewData = new VulkanBufferData();
				pNewData->m_iFlags |= AGK_VK_VERTEX_BUFFER_INDEX_BUFFER;
				pVertices->m_pIndexData.AddItem( pNewData );
				pVertices->m_iCurrentIndexDataIndex = pVertices->m_pIndexData.NumItems()-1; // must do this before the create commands
				ReturnIfFail( InternalCreateIndexBuffer(pVertices, pNewData) );
			}

			if ( pNewData->IsMapped() && pCurrData->IsMapped() )
			{
				memcpy( pNewData->m_pMappedData, pCurrData->m_pMappedData, pVertices->m_iNumIndices*indexSize );
			}
			else
			{
				// create copy transfer from current to new
				VulkanBufferCopy *pCopy = new VulkanBufferCopy();
				pVertices->m_pCopyTransfers.AddItem( pCopy ); // add it to the end
				pCopy->dst = pNewData;
				pCopy->src = pCurrData;
				pCopy->m_iSize = pVertices->m_iNumIndices * indexSize;
				pCopy->m_iState = AGK_VK_BUFFER_TRANSFER_AWAITING;

				// must also dulplicate any partial transfers that are pending on the current data
				for( uint32_t i = 0; i < pVertices->m_pPartialTransfers.NumItems(); i++ )
				{
					VulkanBufferTransfer *pTransfer = pVertices->m_pPartialTransfers[i];
					if ( pTransfer->m_iState == AGK_VK_BUFFER_TRANSFER_AWAITING && pTransfer->dst == pCurrData )
					{
						ReturnIfFail( InternalCreatePartialBufferTransfer(pVertices, pNewData, pTransfer->m_iOffset, pTransfer->m_iSize, pTransfer->m_pMappedData) );
					}
				}
			}
			pCurrData = pNewData;
		}
		else
		{
			// if previous frame is using the buffer then that's fine as long as we wait for it to finish
			if ( pCurrData->IsInUse() ) m_iVerticesWaitForPrevFrame = 1;
		}

		ReturnIfFail( InternalCreatePartialBufferTransfer(pVertices, pCurrData, offset*indexSize, numIndices*indexSize, pIndexData) );
	}
	
	return APP_SUCCESS;
}

// will resize the vertex buffer to match the number of vertices
int VulkanRenderer::SetVertices( void *pResource, float *pVertexData, uint32_t numVertices )
{
	if ( !pResource ) return APP_ERROR_UNKNOWN;
	if ( !pVertexData ) return APP_ERROR_UNKNOWN;
	
	VulkanVertices *pVertices = (VulkanVertices*) pResource;
	if ( pVertices->IsDeleting() ) return APP_ERROR_GENERAL;

	pVertices->m_iNumVertices = numVertices;

	VulkanBufferData *pCurrData = 0;
	if ( pVertices->m_pVertexData.NumItems() > 0 ) pCurrData = pVertices->m_pVertexData[ pVertices->m_iCurrentVertexDataIndex ];
	uint32_t requiredSize = numVertices * pVertices->m_iVertexSize;
	
	// full transfer
	if ( !pCurrData || pCurrData->IsInUse() || pCurrData->IsAwaitingUse() || pCurrData->m_iSize < requiredSize )
	{
		VulkanBufferData *pNewData;
		int index = pVertices->GetFreeVertexData( requiredSize );
		if ( index >= 0 ) 
		{
			pNewData = pVertices->m_pVertexData[ index ];
			pVertices->m_iCurrentVertexDataIndex = index;
		}
		else
		{
			pNewData = new VulkanBufferData();
			pNewData->m_iFlags |= AGK_VK_VERTEX_BUFFER_VERTEX_BUFFER;
			pVertices->m_pVertexData.AddItem( pNewData );
			pVertices->m_iCurrentVertexDataIndex = pVertices->m_pVertexData.NumItems()-1;
			ReturnIfFail( InternalCreateVertexBuffer(pVertices, pNewData) );
		}

		pCurrData = pNewData;
	}

	ReturnIfFail( InternalCreateFullBufferTransfer(pVertices, pCurrData, numVertices*pVertices->m_iVertexSize, pVertexData) );
	
	return APP_SUCCESS;
}

// will resize the index buffer to match the number of indices, can also change from unsigned short to unsigned int or vice versa
int VulkanRenderer::SetIndices( void *pResource, uint16_t *pIndexData, uint32_t numIndices )
{
	if ( !pResource ) return APP_ERROR_UNKNOWN;
	if ( !pIndexData ) return APP_ERROR_UNKNOWN;

	VulkanVertices *pVertices = (VulkanVertices*) pResource;
	if ( pVertices->IsDeleting() ) return APP_ERROR_GENERAL;

	VulkanBufferData *pCurrData = 0;
	if ( pVertices->m_pIndexData.NumItems() > 0 ) pCurrData = pVertices->m_pIndexData[ pVertices->m_iCurrentIndexDataIndex ];
	
	pVertices->m_iFlags &= ~AGK_VK_VERTICES_INDICES_INT;
	
	uint32_t requiredSize = numIndices * sizeof(uint16_t);
	pVertices->m_iNumIndices = numIndices;

	// full transfer
	if ( !pCurrData || pCurrData->IsInUse() || pCurrData->IsAwaitingUse() || pCurrData->m_iSize < requiredSize )
	{
		VulkanBufferData *pNewData;
		int index = pVertices->GetFreeIndexData( requiredSize );
		if ( index >= 0 ) 
		{
			pNewData = pVertices->m_pIndexData[ index ];
			pVertices->m_iCurrentIndexDataIndex = index;
		}
		else
		{
			pNewData = new VulkanBufferData();
			pNewData->m_iFlags |= AGK_VK_VERTEX_BUFFER_INDEX_BUFFER;
			pVertices->m_pIndexData.AddItem( pNewData );
			pVertices->m_iCurrentIndexDataIndex = pVertices->m_pIndexData.NumItems()-1;
			ReturnIfFail( InternalCreateIndexBuffer(pVertices, pNewData) );
		}

		pCurrData = pNewData;
	}

	ReturnIfFail( InternalCreateFullBufferTransfer(pVertices, pCurrData, numIndices*sizeof(uint16_t), pIndexData) );
	return APP_SUCCESS;
}

// will resize the index buffer to match the number of indices, can also change from unsigned short to unsigned int or vice versa
int VulkanRenderer::SetIndicesInt( void *pResource, uint32_t *pIndexData, uint32_t numIndices )
{
	if ( !pResource ) return APP_ERROR_UNKNOWN;
	if ( !pIndexData ) return APP_ERROR_UNKNOWN;

	VulkanVertices *pVertices = (VulkanVertices*) pResource;
	if ( pVertices->IsDeleting() ) return APP_ERROR_GENERAL;

	VulkanBufferData *pCurrData = 0;
	if ( pVertices->m_pIndexData.NumItems() > 0 ) pCurrData = pVertices->m_pIndexData[ pVertices->m_iCurrentIndexDataIndex ];
	
	pVertices->m_iFlags |= AGK_VK_VERTICES_INDICES_INT;
	
	uint32_t requiredSize = numIndices * sizeof(uint32_t);
	pVertices->m_iNumIndices = numIndices;

	// full transfer
	if ( !pCurrData || pCurrData->IsInUse() || pCurrData->IsAwaitingUse() || pCurrData->m_iSize < requiredSize )
	{
		VulkanBufferData *pNewData;
		int index = pVertices->GetFreeIndexData( numIndices * sizeof(uint32_t) );
		if ( index >= 0 ) 
		{
			pNewData = pVertices->m_pIndexData[ index ];
			pVertices->m_iCurrentIndexDataIndex = index;
		}
		else
		{
			pNewData = new VulkanBufferData();
			pNewData->m_iFlags |= AGK_VK_VERTEX_BUFFER_INDEX_BUFFER;
			pVertices->m_pIndexData.AddItem( pNewData );
			pVertices->m_iCurrentIndexDataIndex = pVertices->m_pIndexData.NumItems()-1;
			ReturnIfFail( InternalCreateIndexBuffer(pVertices, pNewData) );
		}

		pCurrData = pNewData;
	}

	ReturnIfFail( InternalCreateFullBufferTransfer(pVertices, pCurrData, numIndices*sizeof(uint32_t), pIndexData) );
	return APP_SUCCESS;
}

int VulkanRenderer::DeleteVertices( void *pResource )
{
	if ( !pResource ) return APP_SUCCESS;
	VulkanVertices* pVertices = (VulkanVertices*) pResource;
	pVertices->m_iFlags |= AGK_VK_VERTICES_DELETE;
	return APP_SUCCESS;
}

void VulkanRenderer::CleanUpVertices()
{
	// free up any resources in the vertex objects if possible
	VulkanVertices *pVertices = VulkanVertices::g_pAllVertexObjects;
	while( pVertices )
	{
		pVertices->m_iFlags &= ~AGK_VK_VERTICES_ANY_IN_USE;
		for( int i = 0; i < (int) pVertices->m_pVertexData.NumItems(); i++ )
		{
			VulkanBufferData *pData = pVertices->m_pVertexData[ i ];
			uint32_t newFlags = pData->m_iFlags & ~(AGK_VK_VERTEX_BUFFER_AWAITING_USE | AGK_VK_VERTEX_BUFFER_IN_USE | AGK_VK_VERTEX_BUFFER_UNUSED);
			if ( pData->m_iFlags & AGK_VK_VERTEX_BUFFER_AWAITING_USE ) 
			{
				pData->m_iFlags = newFlags | AGK_VK_VERTEX_BUFFER_IN_USE;
				pVertices->m_iFlags |= AGK_VK_VERTICES_ANY_IN_USE;
			}
			else if ( pData->m_iFlags & AGK_VK_VERTEX_BUFFER_IN_USE ) 
			{
				pData->m_iFlags = newFlags | AGK_VK_VERTEX_BUFFER_UNUSED;
				pData->m_iUnusedCount = 0;
			}
			else if ( pData->m_iFlags & AGK_VK_VERTEX_BUFFER_UNUSED ) 
			{
				pData->m_iUnusedCount++;
				if ( pData->m_iUnusedCount > 10 )
				{
					if ( pVertices->m_pVertexData.NumItems() > 1 && pVertices->m_iCurrentVertexDataIndex != i ) 
					{
						if ( pData->m_pMappedData ) vmaUnmapMemory( m_vkAllocator, pData->m_vkBufferMemory );
						if ( pData->m_vkBuffer ) vmaDestroyBuffer( m_vkAllocator, pData->m_vkBuffer, pData->m_vkBufferMemory );

						delete pData;

						if ( pVertices->m_iCurrentVertexDataIndex == pVertices->m_pVertexData.NumItems()-1 ) pVertices->m_iCurrentVertexDataIndex = i;
						pVertices->m_pVertexData.RemoveIndex( i );
						i--;
					}
				}
			}
		}

		for( int i = 0; i < (int) pVertices->m_pIndexData.NumItems(); i++ )
		{
			VulkanBufferData *pData = pVertices->m_pIndexData[ i ];
			uint32_t newFlags = pData->m_iFlags & ~(AGK_VK_VERTEX_BUFFER_AWAITING_USE | AGK_VK_VERTEX_BUFFER_IN_USE | AGK_VK_VERTEX_BUFFER_UNUSED);
			if ( pData->m_iFlags & AGK_VK_VERTEX_BUFFER_AWAITING_USE ) 
			{
				pData->m_iFlags = newFlags | AGK_VK_VERTEX_BUFFER_IN_USE;
				pVertices->m_iFlags |= AGK_VK_VERTICES_ANY_IN_USE;
			}
			else if ( pData->m_iFlags & AGK_VK_VERTEX_BUFFER_IN_USE ) 
			{
				pData->m_iFlags = newFlags | AGK_VK_VERTEX_BUFFER_UNUSED;
				pData->m_iUnusedCount = 0;
			}
			else if ( pData->m_iFlags & AGK_VK_VERTEX_BUFFER_UNUSED ) 
			{
				pData->m_iUnusedCount++;
				if ( pData->m_iUnusedCount > 10 )
				{
					if ( pVertices->m_pIndexData.NumItems() > 1 && pVertices->m_iCurrentIndexDataIndex != i ) 
					{
						if ( pData->m_pMappedData ) vmaUnmapMemory( m_vkAllocator, pData->m_vkBufferMemory );
						if ( pData->m_vkBuffer ) vmaDestroyBuffer( m_vkAllocator, pData->m_vkBuffer, pData->m_vkBufferMemory );

						delete pData;

						if ( pVertices->m_iCurrentIndexDataIndex == pVertices->m_pIndexData.NumItems()-1 ) pVertices->m_iCurrentIndexDataIndex = i;
						pVertices->m_pIndexData.RemoveIndex( i );
						i--;
					}
				}
			}
		}

		for( int i = 0; i < (int) pVertices->m_pOldPartialTransfers.NumItems(); i++ )
		{
			VulkanBufferTransfer *pTransfer = pVertices->m_pOldPartialTransfers[ i ];
			pTransfer->m_iUnusedCount++;

			if ( pTransfer->m_iUnusedCount > 2 )
			{
				InternalDeleteBufferTransfer( pTransfer );
				pVertices->m_pOldPartialTransfers.RemoveIndex( i );
				i--;
			}
		}

		for( int i = 0; i < (int) pVertices->m_pPartialTransfers.NumItems(); i++ )
		{
			VulkanBufferTransfer *pTransfer = pVertices->m_pPartialTransfers[ i ];
			switch( pTransfer->m_iState )
			{
				case AGK_VK_BUFFER_TRANSFER_AWAITING: pTransfer->m_iState = AGK_VK_BUFFER_TRANSFER_IN_PROGRESS; pVertices->m_iFlags |= AGK_VK_VERTICES_ANY_IN_USE; break;
				case AGK_VK_BUFFER_TRANSFER_IN_PROGRESS: 
				{
					pTransfer->m_iUnusedCount = 0;
					pTransfer->m_iState = AGK_VK_BUFFER_TRANSFER_COMPLETED; 
					pVertices->m_pPartialTransfers.RemoveIndex( i );
					pVertices->m_pOldPartialTransfers.AddItem( pTransfer );
					i--;
				} break;
			}
		}

		for( int i = 0; i < (int) pVertices->m_pOldCopyTransfers.NumItems(); i++ )
		{
			VulkanBufferCopy *pCopy = pVertices->m_pOldCopyTransfers[ i ];
			pCopy->m_iUnusedCount++;

			if ( pCopy->m_iUnusedCount > 3 )
			{
				delete pCopy;
				pVertices->m_pOldCopyTransfers.RemoveIndex( i );
				i--;
			}
		}

		for( int i = 0; i < (int) pVertices->m_pCopyTransfers.NumItems(); i++ )
		{
			VulkanBufferCopy *pCopy = pVertices->m_pCopyTransfers[ i ];
			switch( pCopy->m_iState )
			{
				case AGK_VK_BUFFER_TRANSFER_AWAITING: 
				{
					pCopy->m_iState = AGK_VK_BUFFER_TRANSFER_IN_PROGRESS; 
					pVertices->m_iFlags |= AGK_VK_VERTICES_ANY_IN_USE; 
				} break;

				case AGK_VK_BUFFER_TRANSFER_IN_PROGRESS: 
				{
					pCopy->m_iUnusedCount = 0;
					pCopy->m_iState = AGK_VK_BUFFER_TRANSFER_COMPLETED; 
					pVertices->m_pCopyTransfers.RemoveIndex( i );
					pVertices->m_pOldCopyTransfers.AddItem( pCopy );
					i--;

				} break;
			}
		}

		for( int i = 0; i < (int) pVertices->m_pOldFullTransfers.NumItems(); i++ )
		{
			VulkanBufferTransfer *pTransfer = pVertices->m_pOldFullTransfers[ i ];
			pTransfer->m_iUnusedCount++;
			if ( pTransfer->m_iUnusedCount > 2 )
			{
				InternalDeleteBufferTransfer( pTransfer );
				pVertices->m_pOldFullTransfers.RemoveIndex( i );
				i--;
			}
		}

		for( int i = 0; i < (int) pVertices->m_pFullTransfers.NumItems(); i++ )
		{
			VulkanBufferTransfer *pTransfer = pVertices->m_pFullTransfers[ i ];
			switch( pTransfer->m_iState )
			{
				case AGK_VK_BUFFER_TRANSFER_AWAITING: 
				{
					pTransfer->m_iState = AGK_VK_BUFFER_TRANSFER_IN_PROGRESS; 
					pVertices->m_iFlags |= AGK_VK_VERTICES_ANY_IN_USE; 
				} break;

				case AGK_VK_BUFFER_TRANSFER_IN_PROGRESS: 
				{
					pTransfer->m_iUnusedCount = 0;
					pTransfer->m_iState = AGK_VK_BUFFER_TRANSFER_COMPLETED;
					pVertices->m_pFullTransfers.RemoveIndex( i );
					pVertices->m_pOldFullTransfers.AddItem( pTransfer );
					i--;
				} break;
			}
		}

		if ( pVertices->ShouldDelete() && !(pVertices->m_iFlags & AGK_VK_VERTICES_ANY_IN_USE) )
		{
			VulkanVertices *pNext = pVertices->m_pNextObject;
			ForceDeleteVertices( pVertices );
			pVertices = pNext;
			continue;
		}
		
		pVertices = pVertices->m_pNextObject;
	}
}

void VulkanRenderer::ForceDeleteVertices( VulkanVertices *pVertices )
{
	for( uint32_t i = 0; i < pVertices->m_pVertexData.NumItems(); i++ )
	{
		VulkanBufferData *pData = pVertices->m_pVertexData[ i ];
		if ( pData->m_pMappedData ) vmaUnmapMemory( m_vkAllocator, pData->m_vkBufferMemory );
		if ( pData->m_vkBuffer ) vmaDestroyBuffer( m_vkAllocator, pData->m_vkBuffer, pData->m_vkBufferMemory );
		delete pData;
	}

	for( uint32_t i = 0; i < pVertices->m_pIndexData.NumItems(); i++ )
	{
		VulkanBufferData *pData = pVertices->m_pIndexData[ i ];
		if ( pData->m_pMappedData ) vmaUnmapMemory( m_vkAllocator, pData->m_vkBufferMemory );
		if ( pData->m_vkBuffer ) vmaDestroyBuffer( m_vkAllocator, pData->m_vkBuffer, pData->m_vkBufferMemory );
		delete pData;
	}

	// partial transfers
	for( uint32_t i = 0; i < pVertices->m_pPartialTransfers.NumItems(); i++ )
	{
		InternalDeleteBufferTransfer( pVertices->m_pPartialTransfers[i] );
	}

	for( uint32_t i = 0; i < pVertices->m_pOldPartialTransfers.NumItems(); i++ )
	{
		InternalDeleteBufferTransfer( pVertices->m_pOldPartialTransfers[i] );
	}

	// full transfers
	for( uint32_t i = 0; i < pVertices->m_pFullTransfers.NumItems(); i++ )
	{
		InternalDeleteBufferTransfer( pVertices->m_pFullTransfers[i] );
	}

	for( uint32_t i = 0; i < pVertices->m_pOldFullTransfers.NumItems(); i++ )
	{
		InternalDeleteBufferTransfer( pVertices->m_pOldFullTransfers[i] );
	}

	// copies
	for( uint32_t i = 0; i < pVertices->m_pCopyTransfers.NumItems(); i++ )
	{
		delete pVertices->m_pCopyTransfers[i];
	}

	for( uint32_t i = 0; i < pVertices->m_pOldCopyTransfers.NumItems(); i++ )
	{
		delete pVertices->m_pOldCopyTransfers[i];
	}
	
	delete pVertices;
}

int VulkanRenderer::DrawVertices( void *pCommandBuffer, void *pResVertices, int count )
{
	if ( !pResVertices ) return APP_ERROR_UNKNOWN;
	
	VulkanVertices *pVertices = (VulkanVertices*) pResVertices;
	if ( pVertices->m_pVertexData.NumItems() == 0 ) return APP_ERROR_GENERAL;
	if ( pVertices->IsDeleting() ) return APP_ERROR_GENERAL;

	CheckRenderPass(); // will begin render pass if necessary

	if ( !(m_iFlags & AGK_VULKAN_SURFACE_VALID) ) return APP_SUCCESS;

	pVertices->DidDraw();

	VkDeviceSize nullOffsets = 0;
	VulkanBufferData *pVertexData = pVertices->m_pVertexData[ pVertices->m_iCurrentVertexDataIndex ];
	vkCmdBindVertexBuffers( m_vkCommandBuffer[m_iCurrentFrame], 0, 1, &pVertexData->m_vkBuffer, &nullOffsets );
	
	if ( pVertices->m_pIndexData.NumItems() > 0 )
	{
		VulkanBufferData *pIndexData = pVertices->m_pIndexData[ pVertices->m_iCurrentIndexDataIndex ];
		vkCmdBindIndexBuffer( m_vkCommandBuffer[m_iCurrentFrame], pIndexData->m_vkBuffer, 0, (pVertices->m_iFlags & AGK_VK_VERTICES_INDICES_INT) ? VK_INDEX_TYPE_UINT32 : VK_INDEX_TYPE_UINT16 );

		if ( count < 0 ) count = pVertices->m_iNumIndices;
		vkCmdDrawIndexed( m_vkCommandBuffer[m_iCurrentFrame], count, 1, 0, 0, 0 );

		m_iVerticesProcessed += pVertices->m_iNumVertices;
		if ( m_iCurrPrimitiveMode == AGK_PRIMITIVE_TRIANGLES ) m_iPolygonsDrawn += count / 3;
		else if ( m_iCurrPrimitiveMode == AGK_PRIMITIVE_TRIANGLE_STRIP ) m_iPolygonsDrawn += count - 2;
	}
	else
	{
		if ( count < 0 ) count = pVertices->m_iNumVertices;
		vkCmdDraw( m_vkCommandBuffer[m_iCurrentFrame], count, 1, 0, 0 );

		m_iVerticesProcessed += count;
		if ( m_iCurrPrimitiveMode == AGK_PRIMITIVE_TRIANGLES ) m_iPolygonsDrawn += count / 3;
		else if ( m_iCurrPrimitiveMode == AGK_PRIMITIVE_TRIANGLE_STRIP ) m_iPolygonsDrawn += count - 2;
	}	

	return APP_SUCCESS;
}

#endif // DISABLE_VULKAN
