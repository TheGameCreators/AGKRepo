#if !defined(DISABLE_VULKAN) && (!defined(__APPLE__) || defined(__LP64__))

#include "AppErrors.h"
#include "AGKVulkan.h"
#include <cmath>

using namespace AGK;

#ifdef AGK_IOS
    // for video to Metal texture
    __CVMetalTextureCache *MtlVideoCache = 0;
    CVMetalTextureRef MtlVideoTextureCV[2] = {0,0};
    unsigned int MtlVideoCurrTexture = 0;

    // for camera to Metal texture
    __CVMetalTextureCache *MtlCameraCache = 0;
    CVMetalTextureRef MtlCameraTextureCV[2] = {0,0};
    unsigned int MtlCameraCurrTexture = 0;

    // for AR to Metal texture
    __CVMetalTextureCache *MtlARCache = 0;
    CVMetalTextureRef MtlARTextureCV[2] = {0,0};
    CVMetalTextureRef MtlARTextureCV2[2] = {0,0};
    unsigned int MtlARCurrTexture = 0;

    int MetalCreateVideoImageData( VkPhysicalDevice vkDevice )
    {
        if ( !MtlVideoCache )
        {
            id<MTLDevice> metalDevice = 0;
            vkGetMTLDeviceMVK( vkDevice, &metalDevice );
            CVReturn err = CVMetalTextureCacheCreate(kCFAllocatorDefault, NULL, metalDevice, NULL, &MtlVideoCache);
            if (err != noErr) {
                return 0;
            }
        }
        
        return 1;
    }

    int MetalHasVideoImageData()
    {
        return MtlVideoCache ? 1 : 0;
    }

    void MetalCopyVideoImageToAGKImage( CVPixelBufferRef buffer, cImage* pImage )
    {
        MtlVideoCurrTexture = 1 - MtlVideoCurrTexture;
        if ( MtlVideoTextureCV[MtlVideoCurrTexture] )
        {
            CFRelease(MtlVideoTextureCV[MtlVideoCurrTexture]);
            MtlVideoTextureCV[MtlVideoCurrTexture] = 0;
        }
        
        CVMetalTextureCacheFlush(MtlVideoCache, 0);
        
        CVMetalTextureCacheCreateTextureFromImage(kCFAllocatorDefault,
                                                  MtlVideoCache,
                                                  buffer,
                                                  NULL,
                                                  MTLPixelFormatBGRA8Unorm,
                                                  (int)CVPixelBufferGetWidth(buffer),
                                                  (int)CVPixelBufferGetHeight(buffer),
                                                  0,
                                                  &(MtlVideoTextureCV[MtlVideoCurrTexture]));
    pImage->OverrideTexture(CVMetalTextureGetTexture(MtlVideoTextureCV[MtlVideoCurrTexture]), (int)CVPixelBufferGetWidth(buffer), (int)CVPixelBufferGetHeight(buffer), AGK_COLOR_FORMAT_RGBA_8);
    }

    void MetalReleaseVideoImageData()
    {
        if ( MtlVideoTextureCV[0] )
        {
            CFRelease(MtlVideoTextureCV[0]);
            MtlVideoTextureCV[0] = 0;
        }
        if ( MtlVideoTextureCV[1] )
        {
            CFRelease(MtlVideoTextureCV[1]);
            MtlVideoTextureCV[1] = 0;
        }
        
        if ( MtlVideoCache )
        {
            CVMetalTextureCacheFlush(MtlVideoCache, 0);
            CFRelease(MtlVideoCache);
            MtlVideoCache = 0;
        }
    }

    int MetalCreateCameraImageData( VkPhysicalDevice vkDevice )
    {
        if ( !MtlCameraCache )
        {
            id<MTLDevice> metalDevice = 0;
            vkGetMTLDeviceMVK( vkDevice, &metalDevice );
            CVReturn err = CVMetalTextureCacheCreate(kCFAllocatorDefault, NULL, metalDevice, NULL, &MtlCameraCache);
            if (err != noErr) {
                return 0;
            }
        }
         
        return 1;
    }

    int MetalHasCameraImageData()
    {
        return MtlCameraCache ? 1 : 0;
    }

    void MetalCopyCameraImageToAGKImage( CVImageBufferRef buffer, cImage* pImage )
    {
        MtlCameraCurrTexture = 1 - MtlCameraCurrTexture;
        if ( MtlCameraTextureCV[MtlCameraCurrTexture] )
        {
            CFRelease(MtlCameraTextureCV[MtlCameraCurrTexture]);
            MtlCameraTextureCV[MtlCameraCurrTexture] = 0;
        }
        
        CVMetalTextureCacheFlush(MtlCameraCache, 0);
            
        CVMetalTextureCacheCreateTextureFromImage(kCFAllocatorDefault,
                                                  MtlCameraCache,
                                                  buffer,
                                                  NULL,
                                                  MTLPixelFormatBGRA8Unorm,
                                                  (int)CVPixelBufferGetWidth(buffer),
                                                  (int)CVPixelBufferGetHeight(buffer),
                                                  0,
                                                  &(MtlCameraTextureCV[MtlCameraCurrTexture]));
    pImage->OverrideTexture(CVMetalTextureGetTexture(MtlCameraTextureCV[MtlCameraCurrTexture]), (int)CVPixelBufferGetWidth(buffer), (int)CVPixelBufferGetHeight(buffer), AGK_COLOR_FORMAT_RGBA_8);
    }

    void MetalReleaseCameraImageData()
    {
        if ( MtlCameraTextureCV[0] )
        {
            CFRelease(MtlCameraTextureCV[0]);
            MtlCameraTextureCV[0] = 0;
        }
        if ( MtlCameraTextureCV[1] )
        {
            CFRelease(MtlCameraTextureCV[1]);
            MtlCameraTextureCV[1] = 0;
        }
        
        if ( MtlCameraCache )
        {
            CVMetalTextureCacheFlush(MtlCameraCache, 0);
            CFRelease(MtlCameraCache);
            MtlCameraCache = 0;
        }
    }

    int MetalCreateARImageData( VkPhysicalDevice vkDevice )
    {
        if ( !MtlARCache )
        {
            id<MTLDevice> metalDevice = 0;
            vkGetMTLDeviceMVK( vkDevice, &metalDevice );
            CVReturn err = CVMetalTextureCacheCreate(kCFAllocatorDefault, NULL, metalDevice, NULL, &MtlARCache);
            if (err != noErr) {
                return 0;
            }
        }
        
        return 1;
    }

    int MetalHasARImageData()
    {
        return MtlARCache ? 1 : 0;
    }

    void MetalCopyARImageToAGKImage( CVPixelBufferRef buffer, cImage* pImageY, cImage* pImageUV )
    {
        int width = (int)CVPixelBufferGetWidthOfPlane(buffer,0);
        int height = (int)CVPixelBufferGetHeightOfPlane(buffer,0);
        
        int width2 = (int)CVPixelBufferGetWidthOfPlane(buffer,1);
        int height2 = (int)CVPixelBufferGetHeightOfPlane(buffer,1);
        
        if ( width == 0 || height == 0 ) return;
        if ( width2 == 0 || height2 == 0 ) return;
        
        MtlARCurrTexture = 1 - MtlARCurrTexture;
        if ( MtlARTextureCV[MtlARCurrTexture] )
        {
            CFRelease(MtlARTextureCV[MtlARCurrTexture]);
            MtlARTextureCV[MtlARCurrTexture] = 0;
        }
        
        if ( MtlARTextureCV2[MtlARCurrTexture] )
        {
            CFRelease(MtlARTextureCV2[MtlARCurrTexture]);
            MtlARTextureCV2[MtlARCurrTexture] = 0;
        }
        
        CVMetalTextureCacheFlush(MtlARCache, 0);
        
        // Lumninace texture
        CVMetalTextureCacheCreateTextureFromImage(kCFAllocatorDefault,
                                                  MtlARCache,
                                                  buffer,
                                                  NULL,
                                                  MTLPixelFormatR8Unorm,
                                                  width,
                                                  height,
                                                  0,
                                              &(MtlARTextureCV[MtlARCurrTexture]));
    pImageY->OverrideTexture(CVMetalTextureGetTexture(MtlARTextureCV[MtlARCurrTexture]), width, height, AGK_COLOR_FORMAT_R_8);
        
        CVMetalTextureCacheCreateTextureFromImage(kCFAllocatorDefault,
                                                  MtlARCache,
                                                  buffer,
                                                  NULL,
                                                  MTLPixelFormatRG8Unorm,
                                                  width2,
                                                  height2,
                                                  1,
                                                 &(MtlARTextureCV2[MtlARCurrTexture]));
           
    pImageUV->OverrideTexture(CVMetalTextureGetTexture(MtlARTextureCV2[MtlARCurrTexture]), width2, height2, AGK_COLOR_FORMAT_RG_8);
    }

    void MetalReleaseARImageData()
    {
        if ( MtlARTextureCV[0] ) CFRelease(MtlARTextureCV[0]);
        if ( MtlARTextureCV[1] ) CFRelease(MtlARTextureCV[1]);
        if ( MtlARTextureCV2[0] ) CFRelease(MtlARTextureCV2[0]);
        if ( MtlARTextureCV2[1] ) CFRelease(MtlARTextureCV2[1]);
        MtlARTextureCV[0] = 0;
        MtlARTextureCV[1] = 0;
        MtlARTextureCV2[0] = 0;
        MtlARTextureCV2[1] = 0;
        
        if ( MtlARCache )
        {
            CVMetalTextureCacheFlush(MtlARCache, 0);
            CFRelease(MtlARCache);
            MtlARCache = 0;
        }
    }
#endif

VulkanSamplerPool VulkanRenderer::g_pSamplers;

void VulkanSamplerPool::Clear()
{
	if ( m_pSamplers ) 
	{
		for( uint32_t i = 0; i < m_iNumItems; i++ ) VulkanRenderer::GetInstance()->DeleteSampler( m_pSamplers[ i ].pSampler );
	}
	m_iNumItems = 0;
}

VkSampler VulkanSamplerPool::Get( uint32_t flags )
{
	int low = 0;
	switch( m_iNumItems )
	{
		case 0: break;
		case 1: 
		{
			if ( m_pSamplers[0].flags == flags ) return m_pSamplers[0].pSampler; 
			break;
		}
		case 2: 
		{
			if ( m_pSamplers[0].flags == flags ) return m_pSamplers[0].pSampler;
			if ( m_pSamplers[1].flags == flags ) return m_pSamplers[1].pSampler; 
			break;
		}
		default:
		{
			// binary search
			int high = m_iNumItems-1;
			while( high >= low )
			{
				int mid = (high+low)/2;
				if( m_pSamplers[ mid ].flags > flags ) high = mid-1;
				else if ( m_pSamplers[ mid ].flags < flags ) low = mid+1;
				else 
				{
					return m_pSamplers[ mid ].pSampler;
				}
			}
		}
	}

	// not found, add new one
	VkSampler sampler;
	if ( VulkanRenderer::GetInstance()->CreateSampler( &sampler, flags ) != APP_SUCCESS ) return 0;

	if ( m_iArraySize < m_iNumItems+1 )
	{
		m_iArraySize = m_iArraySize + m_iArraySize/2;
		SamplerItem* newArray = new SamplerItem[ m_iArraySize ];
		memcpy( newArray, m_pSamplers, sizeof(SamplerItem)*m_iNumItems );
		delete [] m_pSamplers;
		m_pSamplers = newArray;
	}

	if ( low >= (int)m_iNumItems )
	{
		low = (int)m_iNumItems;
	}
	else
	{
		// make room
		for( int i = m_iNumItems-1; i >= low; i-- )
		{
			m_pSamplers[ i+1 ] = m_pSamplers[ i ];
		}
	}

	m_pSamplers[ low ].flags = flags;
	m_pSamplers[ low ].pSampler = sampler;

	m_iNumItems++;
	return sampler;
}

VulkanImage* VulkanImage::g_pAllImages = 0;
uint64_t VulkanImageData::g_iLastCreatedID = 1;

void VulkanImage::RemoveImage() 
{
	if ( !m_pNextImage && !m_pPrevImage && g_pAllImages != this ) return;

	if ( m_pPrevImage ) m_pPrevImage->m_pNextImage = m_pNextImage;
	else g_pAllImages = m_pNextImage;
	if ( m_pNextImage ) m_pNextImage->m_pPrevImage = m_pPrevImage;
	m_pNextImage = 0;
	m_pPrevImage = 0;
}

void VulkanImage::AddImage()
{
	if ( m_pNextImage || m_pPrevImage || g_pAllImages == this ) return;

	if ( g_pAllImages ) g_pAllImages->m_pPrevImage = this;
	m_pPrevImage = 0;
	m_pNextImage = g_pAllImages;
	g_pAllImages = this;
}

int VulkanRenderer::InternalTransferImage( VulkanImage *pImage )
{
	// if ( pImage->ShouldDelete() ) return APP_SUCCESS; // even if it was deleted we must still transfer it

	// order must be full transfers, then copies, then partial transfers

	// only full transfers can go on the transfer queue, due to image granularity and incomplete images
	for( uint32_t i = 0; i < pImage->m_pFullTransfers.NumItems(); i++ )
	{
		int iGenerateMipmaps = 0;
		VulkanImageTransfer *pTransfer = pImage->m_pFullTransfers[i];
		if ( pTransfer->m_iState == AGK_VK_IMAGE_TRANSFER_AWAITING )
		{
			//**********************
			// Transfer queue side
			//**********************

			// transition mipmap level to DST_OPTIMAL
			VkImageMemoryBarrier memoryBarrier = {};
			memoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			memoryBarrier.srcAccessMask = 0;
			memoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			memoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			memoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			memoryBarrier.image = pTransfer->dst->m_vkImage;
			memoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			memoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			memoryBarrier.subresourceRange.aspectMask = pImage->IsDepth() ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
			memoryBarrier.subresourceRange.baseArrayLayer = 0;
			memoryBarrier.subresourceRange.layerCount = 1;
			memoryBarrier.subresourceRange.baseMipLevel = pTransfer->m_iMipLevel; 
			memoryBarrier.subresourceRange.levelCount = 1; // only transfer this level
						
			vkCmdPipelineBarrier( m_vkTransferImagesCommandBuffer[m_iCurrentFrame], VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, 0, 0, 0, 1, &memoryBarrier );

			// copy image data
			VkDeviceSize size = pTransfer->m_iWidth * pTransfer->m_iHeight * pImage->m_iBytesPerPixel;
			VkBufferImageCopy copyRegion = {};
			copyRegion.bufferImageHeight = 0;
			copyRegion.bufferRowLength = 0;
			copyRegion.bufferOffset = 0;
			copyRegion.imageSubresource.aspectMask = pImage->IsDepth() ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
			copyRegion.imageSubresource.mipLevel = pTransfer->m_iMipLevel;
			copyRegion.imageSubresource.baseArrayLayer = 0;
			copyRegion.imageSubresource.layerCount = 1;
			copyRegion.imageOffset.x = pTransfer->m_iOffsetX;
			copyRegion.imageOffset.y = pTransfer->m_iOffsetY;
			copyRegion.imageExtent.width = pTransfer->m_iWidth;
			copyRegion.imageExtent.height = pTransfer->m_iHeight;
			copyRegion.imageExtent.depth = 1;

			vkCmdCopyBufferToImage( m_vkTransferImagesCommandBuffer[m_iCurrentFrame], pTransfer->src, pTransfer->dst->m_vkImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion );
            
            if ( m_iTransferQueueFamilyIndex == m_iQueueFamilyIndex )
            {
                // transition mip level to SRC_OPTIMAL for mipmap generation or SHADER_READ_ONLY_OPTIMAL for reading
                memoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                memoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
                memoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                memoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                memoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                memoryBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

                // transfers on mipmapped images are guaranteed to be to level 0
                if ( pImage->m_iMipmapLevels > 1 && pImage->WillGenerateMipmaps() )
                {
                    memoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
                    iGenerateMipmaps = 1;
                }
                
                if ( !iGenerateMipmaps )
                {
                    vkCmdPipelineBarrier( m_vkReceiveImagesCommandBuffer[m_iCurrentFrame], VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, 0, 0, 0, 1, &memoryBarrier );
                }
                else
                {
                    // mipmap generation requires level 0 to be in SRC format
                    memoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
                    memoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
                    vkCmdPipelineBarrier( m_vkReceiveImagesCommandBuffer[m_iCurrentFrame], VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, 0, 0, 0, 1, &memoryBarrier );

                    pTransfer->dst->GenerateMipmaps(); // only flags it for generation in case further partial updates are made
                }
            }
            else
            {
                // transition mip level to SRC_OPTIMAL for mipmap generation or SHADER_READ_ONLY_OPTIMAL for reading
                memoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                memoryBarrier.dstAccessMask = 0;
                memoryBarrier.srcQueueFamilyIndex = m_iTransferQueueFamilyIndex;
                memoryBarrier.dstQueueFamilyIndex = m_iQueueFamilyIndex;
                memoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                memoryBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

                // transfers on mipmapped images are guaranteed to be to level 0
                if ( pImage->m_iMipmapLevels > 1 && pImage->WillGenerateMipmaps() )
                {
                    memoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
                    iGenerateMipmaps = 1;
                }
                
                vkCmdPipelineBarrier( m_vkTransferImagesCommandBuffer[m_iCurrentFrame], VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, 0, 0, 0, 1, &memoryBarrier );

                //**********************
                // Graphics queue side
                //**********************

                // other half of the transition barrier
                memoryBarrier = {};
                memoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
                memoryBarrier.srcAccessMask = 0;
                memoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
                memoryBarrier.srcQueueFamilyIndex = m_iTransferQueueFamilyIndex;
                memoryBarrier.dstQueueFamilyIndex = m_iQueueFamilyIndex;
                memoryBarrier.image = pTransfer->dst->m_vkImage;
                memoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                memoryBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                memoryBarrier.subresourceRange.aspectMask = pImage->IsDepth() ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
                memoryBarrier.subresourceRange.baseArrayLayer = 0;
                memoryBarrier.subresourceRange.layerCount = 1;
                memoryBarrier.subresourceRange.baseMipLevel = pTransfer->m_iMipLevel;
                memoryBarrier.subresourceRange.levelCount = 1;
            
                if ( !iGenerateMipmaps )
                {
                    vkCmdPipelineBarrier( m_vkReceiveImagesCommandBuffer[m_iCurrentFrame], VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, 0, 0, 0, 1, &memoryBarrier );
                }
                else
                {
                    // mipmap generation requires level 0 to be in SRC format
                    memoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
                    memoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
                    vkCmdPipelineBarrier( m_vkReceiveImagesCommandBuffer[m_iCurrentFrame], VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, 0, 0, 0, 1, &memoryBarrier );

                    pTransfer->dst->GenerateMipmaps(); // only flags it for generation in case further partial updates are made
                }
            }
		}
	}

	// copies must be on the graphics queue for simplicity
	for( uint32_t i = 0; i < pImage->m_pCopyTransfers.NumItems(); i++ )
	{
		VulkanImageCopy *pTransfer = pImage->m_pCopyTransfers[i];
		if ( pTransfer->m_iState == AGK_VK_IMAGE_TRANSFER_AWAITING )
		{
			if ( pTransfer->src->NeedsMipmaps() )
			{
				ReturnIfFail( InternalImageGenerateMipmaps( pImage, pTransfer->src ) );
			}

			VkImageMemoryBarrier memoryBarrier[ 2 ];

			// transition dst mipmap levels to DST_OPTIMAL
			memoryBarrier[ 0 ] = {};
			memoryBarrier[ 0 ].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			memoryBarrier[ 0 ].srcAccessMask = 0;
			memoryBarrier[ 0 ].dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			memoryBarrier[ 0 ].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			memoryBarrier[ 0 ].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			memoryBarrier[ 0 ].image = pTransfer->dst->m_vkImage;
			memoryBarrier[ 0 ].oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			memoryBarrier[ 0 ].newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			memoryBarrier[ 0 ].subresourceRange.aspectMask = pImage->IsDepth() ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
			memoryBarrier[ 0 ].subresourceRange.baseArrayLayer = 0;
			memoryBarrier[ 0 ].subresourceRange.layerCount = 1;
			memoryBarrier[ 0 ].subresourceRange.baseMipLevel = 0; 
			memoryBarrier[ 0 ].subresourceRange.levelCount = pImage->m_iMipmapLevels;

			// transition src mipmap levels to SRC_OPTIMAL
			memoryBarrier[ 1 ] = {};
			memoryBarrier[ 1 ].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			memoryBarrier[ 1 ].srcAccessMask = 0;//VK_ACCESS_SHADER_READ_BIT;
			memoryBarrier[ 1 ].dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			memoryBarrier[ 1 ].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			memoryBarrier[ 1 ].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			memoryBarrier[ 1 ].image = pTransfer->src->m_vkImage;
			memoryBarrier[ 1 ].oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			memoryBarrier[ 1 ].newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			memoryBarrier[ 1 ].subresourceRange.aspectMask = pImage->IsDepth() ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
			memoryBarrier[ 1 ].subresourceRange.baseArrayLayer = 0;
			memoryBarrier[ 1 ].subresourceRange.layerCount = 1;
			memoryBarrier[ 1 ].subresourceRange.baseMipLevel = 0; 
			memoryBarrier[ 1 ].subresourceRange.levelCount = pImage->m_iMipmapLevels;
						
			vkCmdPipelineBarrier( m_vkReceiveImagesCommandBuffer[m_iCurrentFrame], VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, 0, 0, 0, 2, memoryBarrier );

			// copy image data
			uint32_t mipWidth = pImage->m_iWidth;
			uint32_t mipHeight = pImage->m_iHeight;
			VkImageCopy *copyRegions = new VkImageCopy[ pImage->m_iMipmapLevels ];
			for( uint32_t i = 0; i < pImage->m_iMipmapLevels; i++ )
			{
				copyRegions[ i ].dstOffset = { 0, 0, 0 };
				copyRegions[ i ].extent = { mipWidth, mipHeight, 1 };
				copyRegions[ i ].srcOffset = { 0, 0, 0 };
				copyRegions[ i ].srcSubresource.aspectMask = pImage->IsDepth() ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
				copyRegions[ i ].srcSubresource.mipLevel = i;
				copyRegions[ i ].srcSubresource.baseArrayLayer = 0;
				copyRegions[ i ].srcSubresource.layerCount = 1;
				copyRegions[ i ].dstSubresource.aspectMask = pImage->IsDepth() ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
				copyRegions[ i ].dstSubresource.mipLevel = i;
				copyRegions[ i ].dstSubresource.baseArrayLayer = 0;
				copyRegions[ i ].dstSubresource.layerCount = 1;

				if (mipWidth > 1) mipWidth /= 2;
				if (mipHeight > 1) mipHeight /= 2;
			}

			vkCmdCopyImage( m_vkReceiveImagesCommandBuffer[m_iCurrentFrame], pTransfer->src->m_vkImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, pTransfer->dst->m_vkImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, pImage->m_iMipmapLevels, copyRegions );

			delete [] copyRegions;

			// transition dst to SHADER_READ_ONLY_OPTIMAL for reading
			memoryBarrier[ 0 ].srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			memoryBarrier[ 0 ].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			memoryBarrier[ 0 ].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			memoryBarrier[ 0 ].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			memoryBarrier[ 0 ].oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			memoryBarrier[ 0 ].newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

			// transition src to SHADER_READ_ONLY_OPTIMAL for reading
			memoryBarrier[ 1 ].srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			memoryBarrier[ 1 ].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			memoryBarrier[ 1 ].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			memoryBarrier[ 1 ].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			memoryBarrier[ 1 ].oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			memoryBarrier[ 1 ].newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			
			vkCmdPipelineBarrier( m_vkReceiveImagesCommandBuffer[m_iCurrentFrame], VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, 0, 0, 0, 2, memoryBarrier );

			// no mipmap generation for copies

			m_iFrameHasGraphicsImageTransfers = 1;
		}
	}

	// partial transfers must be on the graphics queue, due to image granularity and incomplete images
	for( uint32_t i = 0; i < pImage->m_pPartialTransfers.NumItems(); i++ )
	{
		int iGenerateMipmaps = 0;
		VulkanImageTransfer *pTransfer = pImage->m_pPartialTransfers[i];
		if ( pTransfer->m_iState == AGK_VK_IMAGE_TRANSFER_AWAITING )
		{
			// transition mipmap level to DST_OPTIMAL
			VkImageMemoryBarrier memoryBarrier = {};
			memoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			memoryBarrier.srcAccessMask = 0;//VK_ACCESS_SHADER_READ_BIT;
			memoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			memoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			memoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			memoryBarrier.image = pTransfer->dst->m_vkImage;
			memoryBarrier.oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			memoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			memoryBarrier.subresourceRange.aspectMask = pImage->IsDepth() ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
			memoryBarrier.subresourceRange.baseArrayLayer = 0;
			memoryBarrier.subresourceRange.layerCount = 1;
			memoryBarrier.subresourceRange.baseMipLevel = pTransfer->m_iMipLevel; 
			memoryBarrier.subresourceRange.levelCount = 1; // only transfer this level
						
			vkCmdPipelineBarrier( m_vkReceiveImagesCommandBuffer[m_iCurrentFrame], VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, 0, 0, 0, 1, &memoryBarrier );

			// copy image data
			VkBufferImageCopy copyRegion = {};
			copyRegion.bufferImageHeight = 0;
			copyRegion.bufferRowLength = 0;
			copyRegion.bufferOffset = 0;
			copyRegion.imageSubresource.aspectMask = pImage->IsDepth() ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
			copyRegion.imageSubresource.mipLevel = pTransfer->m_iMipLevel;
			copyRegion.imageSubresource.baseArrayLayer = 0;
			copyRegion.imageSubresource.layerCount = 1;
			copyRegion.imageOffset.x = pTransfer->m_iOffsetX;
			copyRegion.imageOffset.y = pTransfer->m_iOffsetY;
			copyRegion.imageExtent.width = pTransfer->m_iWidth;
			copyRegion.imageExtent.height = pTransfer->m_iHeight;
			copyRegion.imageExtent.depth = 1;

			vkCmdCopyBufferToImage( m_vkReceiveImagesCommandBuffer[m_iCurrentFrame], pTransfer->src, pTransfer->dst->m_vkImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion );

			// transition mip level to SRC_OPTIMAL for mipmap generation or SHADER_READ_ONLY_OPTIMAL for reading
			memoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			memoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			memoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			memoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			memoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			memoryBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

			VkPipelineStageFlags dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

			// transfers on mipmapped images are guaranteed to be to level 0
			if ( pImage->m_iMipmapLevels > 1 && pImage->WillGenerateMipmaps() ) 
			{
				pTransfer->dst->GenerateMipmaps(); // only flags the data for generation in case other transfers make partial changes
				memoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
				memoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
				dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			}

			vkCmdPipelineBarrier( m_vkReceiveImagesCommandBuffer[m_iCurrentFrame], VK_PIPELINE_STAGE_TRANSFER_BIT, dstStage, 0, 0, 0, 0, 0, 1, &memoryBarrier );

			m_iFrameHasGraphicsImageTransfers = 1;
		}
	}

	// mipmaps must be generated on the graphics queue
	for( uint32_t i = 0; i < pImage->m_pImageData.NumItems(); i++ )
	{
		VulkanImageData *pImageData = pImage->m_pImageData[ i ];
		if ( pImageData->NeedsMipmaps() )
		{
			ReturnIfFail( InternalImageGenerateMipmaps( pImage, pImageData ) );
		}
	}
	
	return APP_SUCCESS;
}

// level 0 must have been transitioned to VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL
int VulkanRenderer::InternalImageGenerateMipmaps( VulkanImage *pImage, VulkanImageData *pImageData )
{
	//if ( pImage->ShouldDelete() ) return APP_SUCCESS;

	if ( pImage->m_iMipmapLevels <= 1 ) return APP_SUCCESS;
	m_iFrameHasGraphicsImageTransfers = 1;

	// transition all other levels to DST_OPTIMAL
	VkImageMemoryBarrier memoryBarrier = {};
	memoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	memoryBarrier.srcAccessMask = 0;
	memoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	memoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	memoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	memoryBarrier.image = pImageData->m_vkImage;
	memoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	memoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	memoryBarrier.subresourceRange.aspectMask = pImage->IsDepth() ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
	memoryBarrier.subresourceRange.baseArrayLayer = 0;
	memoryBarrier.subresourceRange.layerCount = 1;
	memoryBarrier.subresourceRange.baseMipLevel = 1;
	memoryBarrier.subresourceRange.levelCount = pImage->m_iMipmapLevels - 1;

	vkCmdPipelineBarrier( m_vkReceiveImagesCommandBuffer[m_iCurrentFrame], VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, 0, 0, 0, 1, &memoryBarrier );

	// generate mipmaps
	int32_t mipWidth = pImage->m_iWidth;
	int32_t mipHeight = pImage->m_iHeight;

	// common to all levels
	memoryBarrier = {};
	memoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	memoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	memoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	memoryBarrier.image = pImageData->m_vkImage;
	memoryBarrier.subresourceRange.aspectMask = pImage->IsDepth() ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
	memoryBarrier.subresourceRange.baseArrayLayer = 0;
	memoryBarrier.subresourceRange.layerCount = 1;
	memoryBarrier.subresourceRange.levelCount = 1;

	// copy each mipmap level into the next then transition it to SRC
	for( uint32_t i = 1; i < pImage->m_iMipmapLevels; i++ )
	{
		// copy to next level
		VkImageBlit blit = {};
		blit.srcOffsets[0] = { 0, 0, 0 };
		blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
		blit.srcSubresource.aspectMask = pImage->IsDepth() ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
		blit.srcSubresource.mipLevel = i - 1;
		blit.srcSubresource.baseArrayLayer = 0;
		blit.srcSubresource.layerCount = 1;
		blit.dstOffsets[0] = { 0, 0, 0 };
		blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
		blit.dstSubresource.aspectMask = pImage->IsDepth() ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
		blit.dstSubresource.mipLevel = i;
		blit.dstSubresource.baseArrayLayer = 0;
		blit.dstSubresource.layerCount = 1;

		vkCmdBlitImage( m_vkReceiveImagesCommandBuffer[m_iCurrentFrame], pImageData->m_vkImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, pImageData->m_vkImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, VK_FILTER_LINEAR );

		// transition src to shader format
		memoryBarrier.subresourceRange.baseMipLevel = i - 1;
		memoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		memoryBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		memoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		memoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		vkCmdPipelineBarrier( m_vkReceiveImagesCommandBuffer[m_iCurrentFrame], VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, 0, 0, 0, 1, &memoryBarrier );

		// transistion DST to SRC if still more levels to go
		if ( i < pImage->m_iMipmapLevels - 1 )
		{
			memoryBarrier.subresourceRange.baseMipLevel = i;
			memoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			memoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			memoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			memoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

			vkCmdPipelineBarrier( m_vkReceiveImagesCommandBuffer[m_iCurrentFrame], VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, 0, 0, 0, 1, &memoryBarrier );
		}

		if (mipWidth > 1) mipWidth /= 2;
		if (mipHeight > 1) mipHeight /= 2;
	}

	// transition last level to shader
	memoryBarrier.subresourceRange.baseMipLevel = pImage->m_iMipmapLevels - 1;
	memoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	memoryBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	memoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	memoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

	vkCmdPipelineBarrier( m_vkReceiveImagesCommandBuffer[m_iCurrentFrame], VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, 0, 0, 0, 1, &memoryBarrier );

	pImageData->MipmapsDone();

	return APP_SUCCESS;
}

// level 0 must have been transitioned to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
int VulkanRenderer::InternalImageGenerateFramebufferMipmaps( VulkanImage *pImage, VulkanImageData *pImageData )
{
	//if ( pImage->ShouldDelete() ) return APP_SUCCESS;
	if ( pImageData->m_iCurrentMode != AGK_VK_IMAGE_MODE_DRAW ) return APP_SUCCESS;

	if ( pImage->m_iMipmapLevels <= 1 ) 
	{
		InternalImageBarrier( m_vkCommandBuffer[m_iCurrentFrame], pImage, AGK_VK_IMAGE_MODE_READ );
		return APP_SUCCESS;
	}
	m_iFrameHasGraphicsImageTransfers = 1;

	// transition level 0 to SRC_OPTIMAL
	VkImageMemoryBarrier memoryBarrier = {};
	memoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	memoryBarrier.srcAccessMask = pImage->IsDepth() ? VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT : VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	memoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
	memoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	memoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	memoryBarrier.image = pImageData->m_vkImage;
	memoryBarrier.oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	memoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
	memoryBarrier.subresourceRange.aspectMask = pImage->IsDepth() ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
	memoryBarrier.subresourceRange.baseArrayLayer = 0;
	memoryBarrier.subresourceRange.layerCount = 1;
	memoryBarrier.subresourceRange.baseMipLevel = 0;
	memoryBarrier.subresourceRange.levelCount = 1;

	uint32_t stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	if ( pImage->IsDepth() ) stage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;

	vkCmdPipelineBarrier( m_vkCommandBuffer[m_iCurrentFrame], stage, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, 0, 0, 0, 1, &memoryBarrier );

	// transition all other levels to DST_OPTIMAL
	memoryBarrier = {};
	memoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	memoryBarrier.srcAccessMask = 0;
	memoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	memoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	memoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	memoryBarrier.image = pImageData->m_vkImage;
	memoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	memoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	memoryBarrier.subresourceRange.aspectMask = pImage->IsDepth() ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
	memoryBarrier.subresourceRange.baseArrayLayer = 0;
	memoryBarrier.subresourceRange.layerCount = 1;
	memoryBarrier.subresourceRange.baseMipLevel = 1;
	memoryBarrier.subresourceRange.levelCount = pImage->m_iMipmapLevels - 1;

	vkCmdPipelineBarrier( m_vkCommandBuffer[m_iCurrentFrame], VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, 0, 0, 0, 1, &memoryBarrier );

	// generate mipmaps
	int32_t mipWidth = pImage->m_iWidth;
	int32_t mipHeight = pImage->m_iHeight;

	// common to all levels
	memoryBarrier = {};
	memoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	memoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	memoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	memoryBarrier.image = pImageData->m_vkImage;
	memoryBarrier.subresourceRange.aspectMask = pImage->IsDepth() ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
	memoryBarrier.subresourceRange.baseArrayLayer = 0;
	memoryBarrier.subresourceRange.layerCount = 1;
	memoryBarrier.subresourceRange.levelCount = 1;

	// copy each mipmap level into the next then transition it to SRC
	for( uint32_t i = 1; i < pImage->m_iMipmapLevels; i++ )
	{
		// copy to next level
		VkImageBlit blit = {};
		blit.srcOffsets[0] = { 0, 0, 0 };
		blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
		blit.srcSubresource.aspectMask = pImage->IsDepth() ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
		blit.srcSubresource.mipLevel = i - 1;
		blit.srcSubresource.baseArrayLayer = 0;
		blit.srcSubresource.layerCount = 1;
		blit.dstOffsets[0] = { 0, 0, 0 };
		blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
		blit.dstSubresource.aspectMask = pImage->IsDepth() ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
		blit.dstSubresource.mipLevel = i;
		blit.dstSubresource.baseArrayLayer = 0;
		blit.dstSubresource.layerCount = 1;

		vkCmdBlitImage( m_vkCommandBuffer[m_iCurrentFrame], pImageData->m_vkImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, pImageData->m_vkImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, VK_FILTER_LINEAR );

		// transition src to shader format
		memoryBarrier.subresourceRange.baseMipLevel = i - 1;
		memoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		memoryBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		memoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		memoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		vkCmdPipelineBarrier( m_vkCommandBuffer[m_iCurrentFrame], VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, 0, 0, 0, 1, &memoryBarrier );

		// transistion DST to SRC if still more levels to go
		if ( i < pImage->m_iMipmapLevels - 1 )
		{
			memoryBarrier.subresourceRange.baseMipLevel = i;
			memoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			memoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			memoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			memoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

			vkCmdPipelineBarrier( m_vkCommandBuffer[m_iCurrentFrame], VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, 0, 0, 0, 1, &memoryBarrier );
		}

		if (mipWidth > 1) mipWidth /= 2;
		if (mipHeight > 1) mipHeight /= 2;
	}

	// transition last level to shader
	memoryBarrier.subresourceRange.baseMipLevel = pImage->m_iMipmapLevels - 1;
	memoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	memoryBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	memoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	memoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

	vkCmdPipelineBarrier( m_vkCommandBuffer[m_iCurrentFrame], VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, 0, 0, 0, 1, &memoryBarrier );

	pImageData->MipmapsDone();
	pImageData->m_iCurrentMode = AGK_VK_IMAGE_MODE_READ;

	return APP_SUCCESS;
}

void VulkanRenderer::InternalImageBarrier( VkCommandBuffer cmdBuffer, VulkanImage *pImage, uint32_t newMode )
{
	VulkanImageData *pData = pImage->GetCurrImageData();
	if ( pData->m_iCurrentMode == newMode ) return;
	
	// if image is undefined then the only reasonable transition is to a transfer destination
	if ( pData->m_iCurrentMode == AGK_VK_IMAGE_MODE_UNDEFINED && newMode != AGK_VK_IMAGE_MODE_TRANSFER_DST ) return;

	VkImageMemoryBarrier memoryBarrier = {};
	memoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	memoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	memoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	memoryBarrier.image = pData->m_vkImage;
	memoryBarrier.subresourceRange.aspectMask = pImage->IsDepth() ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
	memoryBarrier.subresourceRange.baseArrayLayer = 0;
	memoryBarrier.subresourceRange.layerCount = 1;
	memoryBarrier.subresourceRange.baseMipLevel = 0; 
	memoryBarrier.subresourceRange.levelCount = 1; // only transfer this level

	uint32_t srcStage, dstStage;
	
	switch( pData->m_iCurrentMode )
	{
		case AGK_VK_IMAGE_MODE_UNDEFINED: 
		{
			memoryBarrier.srcAccessMask = 0; 
			memoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT; // don't know
			break;
		}
		case AGK_VK_IMAGE_MODE_READ: 
		{
			memoryBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT; 
			memoryBarrier.oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; 
			srcStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			break;
		}
		case AGK_VK_IMAGE_MODE_DRAW: 
		{
			if ( pImage->IsDepth() )
			{
				memoryBarrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT; 
				memoryBarrier.oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; // render pass converts to this layout as its final output
				srcStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
			}
			else
			{
				memoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT; 
				memoryBarrier.oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; // render pass converts to this layout as its final output
				srcStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			}
			break;
		}
		case AGK_VK_IMAGE_MODE_TRANSFER_DST: 
		{
			memoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT; 
			memoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			break;
		}
		case AGK_VK_IMAGE_MODE_TRANSFER_SRC: 
		{
			memoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT; 
			memoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL; 
			srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			break;
		}
	}

	switch( newMode )
	{
		case AGK_VK_IMAGE_MODE_UNDEFINED: 
		{
			memoryBarrier.dstAccessMask = 0; 
			memoryBarrier.newLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			dstStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT; // don't know
			break;
		}
		case AGK_VK_IMAGE_MODE_READ: 
		{
			memoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT; 
			memoryBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; 
			dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			break;
		}
		case AGK_VK_IMAGE_MODE_DRAW: 
		{
			if ( pImage->IsDepth() )
			{
				memoryBarrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT; 
				memoryBarrier.newLayout = memoryBarrier.oldLayout; // render pass will convert image to necessary layout
				dstStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
			}
			else
			{
				memoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT; 
				memoryBarrier.newLayout = memoryBarrier.oldLayout; // render pass will convert image to necessary layout
				dstStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			}
			break;
		}
		case AGK_VK_IMAGE_MODE_TRANSFER_DST: 
		{
			memoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT; 
			memoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL; 
			dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			break;
		}
		case AGK_VK_IMAGE_MODE_TRANSFER_SRC: 
		{
			memoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT; 
			memoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL; 
			dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			break;
		}
	}

	pData->m_iCurrentMode = newMode;
	
	vkCmdPipelineBarrier( cmdBuffer, srcStage, dstStage, 0, 0, 0, 0, 0, 1, &memoryBarrier );
}

bool VulkanRenderer::IsImageFormatSupported( AGKImgFormat format )
{
	VkFormat vformat = VK_FORMAT_UNDEFINED;
	int isDepth = 0;
	switch( format )
	{
		case AGK_COLOR_FORMAT_R_8:		vformat = VK_FORMAT_R8_UNORM;				break;
		case AGK_COLOR_FORMAT_RG_8:		vformat = VK_FORMAT_R8G8_UNORM;				break;
		case AGK_COLOR_FORMAT_RGBA_8:	vformat = VK_FORMAT_R8G8B8A8_UNORM;			break;
		case AGK_COLOR_FORMAT_R_F16:	vformat = VK_FORMAT_R16_SFLOAT;				break;
		case AGK_COLOR_FORMAT_RG_F16:	vformat = VK_FORMAT_R16G16_SFLOAT;			break;
		case AGK_COLOR_FORMAT_RGBA_F16: vformat = VK_FORMAT_R16G16B16A16_SFLOAT;	break;
		case AGK_COLOR_FORMAT_R_F32:	vformat = VK_FORMAT_R32_SFLOAT;				break;
		case AGK_COLOR_FORMAT_RG_F32:	vformat = VK_FORMAT_R32G32_SFLOAT;			break;
		case AGK_COLOR_FORMAT_RGBA_F32: vformat = VK_FORMAT_R32G32B32A32_SFLOAT;	break;

		case AGK_DEPTH_FORMAT_16_INT:	vformat = VK_FORMAT_D16_UNORM;				isDepth = 1; break;
		case AGK_DEPTH_FORMAT_24_INT:	vformat = VK_FORMAT_X8_D24_UNORM_PACK32;	isDepth = 1; break;
		case AGK_DEPTH_FORMAT_32_FLOAT:	vformat = VK_FORMAT_D32_SFLOAT;				isDepth = 1; break;
		default: 
		{
			AppError( g_AppErrorDescs[APP_ERROR_IMAGE_FORMAT_NOT_RECOGNISED].szDesc, format );
			return false;
		}
	}

	if ( isDepth )
	{
		VkFormatProperties formatProps;
		vkGetPhysicalDeviceFormatProperties( m_vkGPU, vformat, &formatProps );
		if ( formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT ) return true;
		else return false;
	}
	else
	{
		VkFormatProperties formatProps;
		vkGetPhysicalDeviceFormatProperties( m_vkGPU, vformat, &formatProps );
		if ( formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT ) return true;
		else return false;
	}
}

int VulkanRenderer::InternalCreateImage2D( VulkanImage *pImage )
{
	VulkanImageData *pImageData = pImage->m_pImageData[ pImage->m_iCurrentDataIndex ];
	if ( !pImageData ) ReportAndReturnErrorID( APP_ERROR_VK_INVALID_IMAGE_DATA );

	VkImageCreateInfo imageInfo = {};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = pImage->m_iWidth;
	imageInfo.extent.height = pImage->m_iHeight;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = pImage->m_iMipmapLevels;
	imageInfo.arrayLayers = 1;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.queueFamilyIndexCount = VK_QUEUE_FAMILY_IGNORED;
	imageInfo.pQueueFamilyIndices = nullptr;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.format = pImage->m_vkFormat;
	imageInfo.usage = pImage->m_vkUsageFlags;

	VmaAllocationCreateInfo allocCreateInfo = {};
	allocCreateInfo.requiredFlags = AGK_MEM_DEVICE; 
	VmaAllocationInfo allocInfo;
	VkResult res = vmaCreateImage( m_vkAllocator, &imageInfo, &allocCreateInfo, &pImageData->m_vkImage, &pImageData->m_vkImageMemory, &allocInfo );
	if ( VK_SUCCESS != res ) ReportAndReturnErrorID( APP_ERROR_VK_CREATE_IMAGE_FAILED, res );
	
	return APP_SUCCESS;
}

int VulkanRenderer::InternalCreateImageView( VulkanImage *pImage )
{
	VulkanImageData *pImageData = pImage->m_pImageData[ pImage->m_iCurrentDataIndex ];
	if ( !pImageData ) ReportAndReturnErrorID( APP_ERROR_VK_INVALID_IMAGE_DATA );

	VkImageViewCreateInfo imageViewInfo = {};
	imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	imageViewInfo.image = pImageData->m_vkImage;
	imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	imageViewInfo.format = pImage->m_vkFormat;
#if defined(AGK_IOS) || defined(AGK_MACOS)
    imageViewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
#else
    imageViewInfo.components.r = pImage->IsDepth() ? VK_COMPONENT_SWIZZLE_R : VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewInfo.components.g = pImage->IsDepth() ? VK_COMPONENT_SWIZZLE_R : VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewInfo.components.b = pImage->IsDepth() ? VK_COMPONENT_SWIZZLE_R : VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewInfo.components.a = pImage->IsDepth() ? VK_COMPONENT_SWIZZLE_R : VK_COMPONENT_SWIZZLE_IDENTITY;
#endif
	imageViewInfo.subresourceRange.aspectMask = pImage->IsDepth() ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
	imageViewInfo.subresourceRange.baseMipLevel = 0;
	imageViewInfo.subresourceRange.levelCount = pImage->m_iMipmapLevels;
	imageViewInfo.subresourceRange.baseArrayLayer = 0;
	imageViewInfo.subresourceRange.layerCount = 1;

	VkResult res = vkCreateImageView( m_vkDevice, &imageViewInfo, nullptr, &pImageData->m_vkImageView );
	if ( res != VK_SUCCESS ) ReportAndReturnErrorID( APP_ERROR_VK_GET_IMAGE_VIEW_FAILED, res );

	return APP_SUCCESS;
}

int VulkanRenderer::InternalCreateImageViewFrameBuffer( VulkanImage *pImage )
{
	VulkanImageData *pImageData = pImage->m_pImageData[ pImage->m_iCurrentDataIndex ];
	if ( !pImageData ) ReportAndReturnErrorID( APP_ERROR_VK_INVALID_IMAGE_DATA );

	VkImageViewCreateInfo imageViewInfo = {};
	imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	imageViewInfo.image = pImageData->m_vkImage;
	imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	imageViewInfo.format = pImage->m_vkFormat;
	imageViewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewInfo.subresourceRange.aspectMask = pImage->IsDepth() ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
	imageViewInfo.subresourceRange.baseMipLevel = 0;
	imageViewInfo.subresourceRange.levelCount = 1; // no mipmaps on framebuffer views
	imageViewInfo.subresourceRange.baseArrayLayer = 0;
	imageViewInfo.subresourceRange.layerCount = 1;

	VkResult res = vkCreateImageView( m_vkDevice, &imageViewInfo, nullptr, &pImageData->m_vkImageViewFrameBuffer );
	if ( res != VK_SUCCESS ) ReportAndReturnErrorID( APP_ERROR_VK_GET_IMAGE_VIEW_FAILED, res );

	return APP_SUCCESS;
}

// when using the miplevel the width and height must be scaled appropriately
int VulkanRenderer::InternalCreateImageTransfer( VulkanImage *pImage, uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t mipLevel, void* data )
{
	VulkanImageData *pCurrData = pImage->m_pImageData[ pImage->m_iCurrentDataIndex ];
	VulkanImageTransfer *pTransfer = 0;

	if ( x == 0 && y == 0 && width == pImage->m_iWidth && height == pImage->m_iHeight )
	{
		// full transfer

		// look for an old transfer that we can repurpose
		if ( pImage->m_pOldFullTransfers.NumItems() > 0 )
		{
			pTransfer = pImage->m_pOldFullTransfers[ 0 ];
			pImage->m_pOldFullTransfers.RemoveIndex( 0 );
		}
		else 
		{
			pTransfer = new VulkanImageTransfer();
		}

		pImage->m_pFullTransfers.AddItem( pTransfer ); // add it to the end
		
		// remove all partial transfers for this mip level
		for( int i = 0; i < (int) pImage->m_pPartialTransfers.NumItems(); i++ )
		{
			VulkanImageTransfer *pTransferCheck = pImage->m_pPartialTransfers[i];
			if ( pTransferCheck->m_iState == AGK_VK_IMAGE_TRANSFER_AWAITING && pTransferCheck->m_iMipLevel == mipLevel && pTransferCheck->dst == pCurrData )
			{
				// don't delete it in case this scenario occurs again and we constantly create and delete it
				pTransferCheck->m_iState = AGK_VK_IMAGE_TRANSFER_COMPLETED;
				pImage->m_pPartialTransfers.RemoveIndex( i );
				pImage->m_pOldPartialTransfers.AddItem( pTransferCheck );
				i--;
			}
		}
	}
	else
	{
		// partial transfer

		// look for an old transfer that we can repurpose
		for( uint32_t i = 0; i < pImage->m_pOldPartialTransfers.NumItems(); i++ )
		{
			VulkanImageTransfer *pTransferCheck = pImage->m_pOldPartialTransfers[i];
			if ( pTransferCheck->m_iWidth == width && pTransferCheck->m_iHeight == height ) 
			{
				pImage->m_pOldPartialTransfers.RemoveIndex( i );
				pTransfer = pTransferCheck;
				break;
			}
		}

		if ( !pTransfer ) 
		{
			pTransfer = new VulkanImageTransfer();
		}

		pImage->m_pPartialTransfers.AddItem( pTransfer ); // add it to the end

		// must not delete any transfers from the m_pPartialTransfer list as this function may be called from the copy image routine
		// which traverses the partial transfer list to copy them to the image. Deleting any will change the indices of the 
		// remaining transfers which might be missed by the copy loop
	}

	pTransfer->m_iOffsetX = x;
	pTransfer->m_iOffsetY = y;
	pTransfer->m_iWidth = width;
	pTransfer->m_iHeight = height;
	pTransfer->m_iState = AGK_VK_IMAGE_TRANSFER_AWAITING;
	pTransfer->m_iMipLevel = mipLevel;
	pTransfer->dst = pCurrData;

	if ( !pTransfer->src )
	{
		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = width * height * pImage->m_iBytesPerPixel;
		bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VmaAllocationCreateInfo allocCreateInfo = {};
		allocCreateInfo.requiredFlags = AGK_MEM_STAGING;
		VmaAllocationInfo allocInfo;
		VkResult res = vmaCreateBuffer( m_vkAllocator, &bufferInfo, &allocCreateInfo, &pTransfer->src, &pTransfer->srcMemory, &allocInfo );
		if ( VK_SUCCESS != res ) ReportAndReturnErrorID( APP_ERROR_VK_CREATE_BUFFER_FAILED, res );
	}
	
	if ( !pTransfer->m_pMappedData )
	{
		VkResult res = vmaMapMemory( m_vkAllocator, pTransfer->srcMemory, &pTransfer->m_pMappedData );
		if ( VK_SUCCESS != res ) ReportAndReturnErrorID( APP_ERROR_VK_MAP_MEM_FAILED, res );
	}
		
	memcpy( pTransfer->m_pMappedData, data, width*height*pImage->m_iBytesPerPixel );

	if ( !pImage->HasTransfers() )
	{
		pImage->m_iFlags |= AGK_VK_IMAGE_HAS_TRANSFERS;
		m_pImagesWithTransfers.AddItem( pImage ); // duplicates are ok
	}

	pCurrData->m_iCurrentMode = AGK_VK_IMAGE_MODE_READ; // at the end of the transfer the image will be in read mode
		
	return APP_SUCCESS;
}

int VulkanRenderer::InternalDeleteImageTransfer( VulkanImageTransfer *pTransfer )
{
	if ( pTransfer->src ) 
	{
		if ( pTransfer->m_pMappedData ) vmaUnmapMemory( m_vkAllocator, pTransfer->srcMemory );
		vmaDestroyBuffer( m_vkAllocator, pTransfer->src, pTransfer->srcMemory );
	}

	delete pTransfer;
	return APP_SUCCESS;
}

int VulkanRenderer::CreateImage2D( AGKImgFormat format, uint32_t width, uint32_t height, unsigned char *data, uint32_t flags, void **outResource )
{
	VkFormat vformat = VK_FORMAT_UNDEFINED;
	unsigned int bytesPerPixel = 0;
	switch( format )
	{
		case AGK_COLOR_FORMAT_R_8:		vformat = VK_FORMAT_R8_UNORM;				bytesPerPixel = 1;  break;
		case AGK_COLOR_FORMAT_RG_8:		vformat = VK_FORMAT_R8G8_UNORM;				bytesPerPixel = 2;  break;
		case AGK_COLOR_FORMAT_RGBA_8:	vformat = VK_FORMAT_R8G8B8A8_UNORM;			bytesPerPixel = 4;  break;
		case AGK_COLOR_FORMAT_R_F16:	vformat = VK_FORMAT_R16_SFLOAT;				bytesPerPixel = 2;  break;
		case AGK_COLOR_FORMAT_RG_F16:	vformat = VK_FORMAT_R16G16_SFLOAT;			bytesPerPixel = 4;  break;
		case AGK_COLOR_FORMAT_RGBA_F16: vformat = VK_FORMAT_R16G16B16A16_SFLOAT;	bytesPerPixel = 8;  break;
		case AGK_COLOR_FORMAT_R_F32:	vformat = VK_FORMAT_R32_SFLOAT;				bytesPerPixel = 4;  break;
		case AGK_COLOR_FORMAT_RG_F32:	vformat = VK_FORMAT_R32G32_SFLOAT;			bytesPerPixel = 8;  break;
		case AGK_COLOR_FORMAT_RGBA_F32: vformat = VK_FORMAT_R32G32B32A32_SFLOAT;	bytesPerPixel = 16; break;
		default: ReportAndReturnErrorID( APP_ERROR_IMAGE_FORMAT_NOT_RECOGNISED, format );
	}

	if ( flags & AGK_IMAGE_GEN_MIPMAP ) flags |= AGK_IMAGE_USE_MIPMAP;

	VkFormatProperties formatProps;
	vkGetPhysicalDeviceFormatProperties( m_vkGPU, vformat, &formatProps );
	if ( !(formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT) )
	{
		ReportAndReturnErrorID( APP_ERROR_VK_IMAGE_FORMAT_NOT_SUPPORTED );
	}

	if ( (flags & AGK_IMAGE_RENDERABLE) && !(formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT) )
	{
		ReportAndReturnErrorID( APP_ERROR_VK_IMAGE_FORMAT_NOT_RENDERABLE );
	}

	VulkanImage *outImage = (VulkanImage*) *outResource;
	if ( !outImage )
	{
		outImage = new VulkanImage();
		*outResource = outImage;
	}
	else
	{
		VulkanImageData *pData = outImage->m_pImageData[ outImage->m_iCurrentDataIndex ];
		if ( pData && (pData->m_iFlags & AGK_VK_IMAGE_RENDERED) )
		{
			//AppError( "Cannot recreate image data on an image that has been used with SetRenderToImage, only modifications by the GPU are allowed" );
			//return APP_ERROR_GENERAL;
			DeleteImage( outImage, 1 );
			outImage = new VulkanImage();
			*outResource = outImage;
		}
		else
		{
			bool bMainFlagsChanged = false;
			if ( (outImage->m_iFlags & AGK_IMAGE_RENDERABLE) ^ (flags & AGK_IMAGE_RENDERABLE) ) bMainFlagsChanged = true;
			if ( (outImage->m_iFlags & AGK_IMAGE_USE_MIPMAP) ^ (flags & AGK_IMAGE_USE_MIPMAP) ) bMainFlagsChanged = true;
			if ( vformat != outImage->m_vkFormat || width != outImage->m_iWidth || height != outImage->m_iHeight || bMainFlagsChanged )
			{
				DeleteImage( outImage, 1 );
				outImage = new VulkanImage();
				*outResource = outImage;
			}
			else
			{
				if ( flags & AGK_IMAGE_GEN_MIPMAP ) outImage->m_iFlags |= AGK_IMAGE_GEN_MIPMAP;
				if ( !data ) return APP_SUCCESS; // no changes to image settings and no new data
				else return UpdateSubImage2D( outImage, 0, 0, 0, width, height, data );
			}
		}
	}

	outImage->m_iBytesPerPixel = bytesPerPixel;
	outImage->m_vkFormat = vformat;
	outImage->m_vkUsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT; // SRC for mipmaps and possible read back to CPU
	outImage->m_iWidth = width;
	outImage->m_iHeight = height;

	if ( flags & AGK_IMAGE_RENDERABLE ) 
	{
		outImage->m_vkUsageFlags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		outImage->m_iFlags |= AGK_IMAGE_RENDERABLE;
	}

	outImage->m_iMipmapLevels = 1;
	if ( flags & AGK_IMAGE_USE_MIPMAP )
	{
		uint32_t maxSize = outImage->m_iWidth;
		if ( outImage->m_iHeight > maxSize ) maxSize = outImage->m_iHeight;
		outImage->m_iMipmapLevels = (int) std::floor( std::log2( maxSize ) ) + 1;
		if ( outImage->m_iMipmapLevels < 1 ) outImage->m_iMipmapLevels = 1;
		outImage->m_iFlags |= AGK_IMAGE_USE_MIPMAP;
	}

	if ( (flags & AGK_IMAGE_GEN_MIPMAP) ) outImage->m_iFlags |= AGK_IMAGE_GEN_MIPMAP;

	VulkanImageData *pImageData = new VulkanImageData();
	outImage->m_pImageData.AddItem( pImageData );
	outImage->m_iCurrentDataIndex = 0;
		
	ReturnIfFail( InternalCreateImage2D(outImage) );
	ReturnIfFail( InternalCreateImageView(outImage) );
	ReturnIfFail( InternalCreateImageViewFrameBuffer(outImage) );
	if ( data ) 
	{
		ReturnIfFail( InternalCreateImageTransfer(outImage, 0, 0, width, height, 0, data) )
	}
	ReturnIfFail( SetImageSampleSettings( outImage, flags ) );

	return APP_SUCCESS;
}

int VulkanRenderer::UpdateSubImage2D( void *pResource, uint32_t mipmapLevel, uint32_t x, uint32_t y, uint32_t width, uint32_t height, unsigned char *data )
{
	if ( width == 0 || height == 0 ) 
	{
		AppError( "Tried to update an image with zero size" );
		return APP_ERROR_GENERAL;
	}
	if ( !pResource ) return APP_ERROR_UNKNOWN;
	VulkanImage *pImage = (VulkanImage*) pResource;

	VulkanImageData *pData = pImage->m_pImageData[ pImage->m_iCurrentDataIndex ];
	if ( pData && (pData->m_iFlags & AGK_VK_IMAGE_RENDERED) )
	{
		AppError( "Cannot update image data on an image that has been used with SetRenderToImage, only modifications by the GPU are allowed" );
		return APP_ERROR_GENERAL;
	}
	
	if ( pImage->IsDepth() )
	{
		AppError( "Cannot update image data on depth images, only modifications by the GPU are allowed" );
		return APP_ERROR_GENERAL;
	}

	if ( !data ) return APP_ERROR_UNKNOWN;

	// any updates to lower mipmap levels would be lost anyway
	if ( mipmapLevel != 0 && pImage->WillGenerateMipmaps() ) return APP_SUCCESS;

	uint32_t levelWidth = pImage->m_iWidth;
	uint32_t levelHeight = pImage->m_iHeight;
	uint32_t tempLevel = mipmapLevel;
	while ( tempLevel > 0 )
	{
		tempLevel--;
		levelWidth /= 2;
		levelHeight /= 2;
	}

	if ( levelWidth < 1 ) levelWidth = 1;
	if ( levelHeight < 1 ) levelHeight = 1;
	if ( mipmapLevel >= pImage->m_iMipmapLevels )
	{
		ReportAndReturnErrorID( APP_ERROR_VK_INVALID_IMAGE_MIP_LEVEL, mipmapLevel, pImage->m_iMipmapLevels );
	}

	if ( x+width > levelWidth || y+height > levelHeight ) 
	{
		AppError( "Mip level: %d, Image size: %dx%d, Data offset: %dx%d, Data size: %dx%d", mipmapLevel, levelWidth, levelHeight, x, y, width, height );
		ReportAndReturnErrorID( APP_ERROR_VK_IMAGE_DATA_OUT_OF_BOUNDS );
	}

	int fullTransfer = 0;
	if ( x == 0 && y == 0 && width == pImage->m_iWidth && height == pImage->m_iHeight ) fullTransfer = 1;

	if ( fullTransfer )
	{
		VulkanImageData *pNewData;
		int index = pImage->GetFreeImageData();
		if ( index >= 0 ) 
		{
			pNewData = pImage->m_pImageData[ index ];
			pImage->m_iCurrentDataIndex = index;
		}
		else
		{
			pNewData = new VulkanImageData();
			pImage->m_pImageData.AddItem( pNewData );
			pImage->m_iCurrentDataIndex = pImage->m_pImageData.NumItems()-1; // must do this before the create commands
			ReturnIfFail( InternalCreateImage2D(pImage) );
			ReturnIfFail( InternalCreateImageView(pImage) );
			ReturnIfFail( InternalCreateImageViewFrameBuffer(pImage) );
		}
	}
	else
	{
		// partial transfer needs existing data, can't use a blank image
		VulkanImageData *pCurrData = pImage->m_pImageData[ pImage->m_iCurrentDataIndex ];
		if ( pCurrData->IsAwaitingUse() )
		{
			// current image contents must be preserved as it is already used in a draw call in this frame
			VulkanImageData *pNewData;
			int index = pImage->GetFreeImageData();
			if ( index >= 0 ) 
			{
				pNewData = pImage->m_pImageData[ index ];
				pImage->m_iCurrentDataIndex = index;
			}
			else
			{
				pNewData = new VulkanImageData();
				pImage->m_pImageData.AddItem( pNewData );
				pImage->m_iCurrentDataIndex = pImage->m_pImageData.NumItems()-1; // must do this before the create commands
				ReturnIfFail( InternalCreateImage2D(pImage) );
				ReturnIfFail( InternalCreateImageView(pImage) );
				ReturnIfFail( InternalCreateImageViewFrameBuffer(pImage) );
			}

			// create copy transfer from current to new
			VulkanImageCopy *pCopy = new VulkanImageCopy();
			pImage->m_pCopyTransfers.AddItem( pCopy ); // add it to the end
			pCopy->dst = pNewData;
			pCopy->src = pCurrData;
			pCopy->m_iState = AGK_VK_IMAGE_TRANSFER_AWAITING;

			// must also dulplicate any partial transfers that are pending on the current data
			for( uint32_t i = 0; i < pImage->m_pPartialTransfers.NumItems(); i++ )
			{
				VulkanImageTransfer *pTransfer = pImage->m_pPartialTransfers[i];
				if ( pTransfer->m_iState == AGK_VK_IMAGE_TRANSFER_AWAITING && pTransfer->dst == pCurrData )
				{
					ReturnIfFail( InternalCreateImageTransfer(pImage, pTransfer->m_iOffsetX, pTransfer->m_iOffsetY, pTransfer->m_iWidth, pTransfer->m_iHeight, pTransfer->m_iMipLevel, pTransfer->m_pMappedData) );
				}
			}

			pCurrData = pNewData;
		}
		// if previous frame is using the image then that's fine as long as we wait for it to finish
		if ( pCurrData->IsInUse() ) m_iImagesWaitForPrevFrame = 1;
	}

	ReturnIfFail( InternalCreateImageTransfer(pImage, x, y, width, height, mipmapLevel, data) );

	return APP_SUCCESS;
}

int VulkanRenderer::CreateDepthBuffer( AGKImgFormat format, uint32_t width, uint32_t height, uint32_t flags, void **outResource )
{
	if ( !IsImageFormatSupported( format ) ) ReportAndReturnErrorID( APP_ERROR_VK_IMAGE_FORMAT_NOT_SUPPORTED );

	uint32_t bytesPerPixel;
	VkFormat vformat = VK_FORMAT_UNDEFINED;
	switch( format )
	{
		case AGK_DEPTH_FORMAT_16_INT:	vformat = VK_FORMAT_D16_UNORM;				bytesPerPixel = 2; break;
		case AGK_DEPTH_FORMAT_24_INT:	vformat = VK_FORMAT_X8_D24_UNORM_PACK32;	bytesPerPixel = 4; break;
		case AGK_DEPTH_FORMAT_32_FLOAT:	vformat = VK_FORMAT_D32_SFLOAT;				bytesPerPixel = 4; break;
		default: ReportAndReturnErrorID( APP_ERROR_IMAGE_FORMAT_NOT_RECOGNISED, format );
	}

	if ( flags & AGK_IMAGE_GEN_MIPMAP ) flags |= AGK_IMAGE_USE_MIPMAP;

	VulkanImage *outImage = new VulkanImage();
	*outResource = outImage;

	outImage->m_iBytesPerPixel = bytesPerPixel;
	outImage->m_vkFormat = vformat;
	outImage->m_iWidth = width;
	outImage->m_iHeight = height;

	outImage->m_iMipmapLevels = 1;
	if ( flags & AGK_IMAGE_USE_MIPMAP )
	{
		uint32_t maxSize = outImage->m_iWidth;
		if ( outImage->m_iHeight > maxSize ) maxSize = outImage->m_iHeight;
		outImage->m_iMipmapLevels = (int) std::floor( std::log2( maxSize ) ) + 1;
		if ( outImage->m_iMipmapLevels < 1 ) outImage->m_iMipmapLevels = 1;
		outImage->m_iFlags |= AGK_IMAGE_USE_MIPMAP;
	}

	if ( (flags & AGK_IMAGE_GEN_MIPMAP) ) outImage->m_iFlags |= AGK_IMAGE_GEN_MIPMAP;

	VulkanImageData *pImageData = new VulkanImageData();
	outImage->m_pImageData.AddItem( pImageData );
	outImage->m_iCurrentDataIndex = 0;

	outImage->m_vkFormat = vformat;
	outImage->m_iFlags |= AGK_VK_IMAGE_IS_DEPTH;
	outImage->m_vkUsageFlags = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	if ( flags & AGK_VK_IMAGE_IS_DEFAULT ) outImage->m_iFlags |= AGK_VK_IMAGE_IS_DEFAULT;
	else outImage->m_vkUsageFlags |= VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT; // SRC for mipmaps and possible read back to CPU
		
	ReturnIfFail( InternalCreateImage2D(outImage) );
	ReturnIfFail( InternalCreateImageView(outImage) );
	ReturnIfFail( InternalCreateImageViewFrameBuffer(outImage) );
	ReturnIfFail( SetImageSampleSettings( outImage, flags ) );

	return APP_SUCCESS;
}

int VulkanRenderer::DeleteImage( void *inResource, int unbind )
{
	if ( !inResource ) return APP_SUCCESS;
	VulkanImage* pImage = (VulkanImage*) inResource;
	pImage->m_iFlags |= AGK_VK_IMAGE_DELETE;
	return APP_SUCCESS;
}

int VulkanRenderer::OverrideTexture( uint32_t width, uint32_t height, AGKImgFormat format, void* texID, void **outResource )
{
    VkFormat vformat = VK_FORMAT_UNDEFINED;
    unsigned int bytesPerPixel = 0;
    switch( format )
    {
        case AGK_COLOR_FORMAT_R_8:		vformat = VK_FORMAT_R8_UNORM;            bytesPerPixel = 1;  break;
        case AGK_COLOR_FORMAT_RG_8:		vformat = VK_FORMAT_R8G8_UNORM;			 bytesPerPixel = 2;  break;
        case AGK_COLOR_FORMAT_RGBA_8:	vformat = VK_FORMAT_R8G8B8A8_UNORM;      bytesPerPixel = 4;  break;
        case AGK_COLOR_FORMAT_R_F16:	vformat = VK_FORMAT_R16_SFLOAT;          bytesPerPixel = 2;  break;
        case AGK_COLOR_FORMAT_RG_F16:	vformat = VK_FORMAT_R16G16_SFLOAT;       bytesPerPixel = 4;  break;
        case AGK_COLOR_FORMAT_RGBA_F16: vformat = VK_FORMAT_R16G16B16A16_SFLOAT; bytesPerPixel = 8;  break;
        case AGK_COLOR_FORMAT_R_F32:	vformat = VK_FORMAT_R32_SFLOAT;          bytesPerPixel = 4;  break;
        case AGK_COLOR_FORMAT_RG_F32:	vformat = VK_FORMAT_R32G32_SFLOAT;       bytesPerPixel = 8;  break;
        case AGK_COLOR_FORMAT_RGBA_F32: vformat = VK_FORMAT_R32G32B32A32_SFLOAT; bytesPerPixel = 16; break;
        default: ReportAndReturnErrorID( APP_ERROR_IMAGE_FORMAT_NOT_RECOGNISED, format );
    }
    
    VulkanImage *outImage = (VulkanImage*) *outResource;
    if ( !outImage )
    {
        outImage = new VulkanImage();
        *outResource = outImage;
        
        outImage->m_iBytesPerPixel = bytesPerPixel;
        outImage->m_vkFormat = vformat;
        outImage->m_vkUsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT; // SRC for mipmaps and possible read back to CPU
        outImage->m_iWidth = width;
        outImage->m_iHeight = height;

        outImage->m_iMipmapLevels = 1;
    }
       
    /*
    if ( flags & AGK_IMAGE_USE_MIPMAP )
    {
        uint32_t maxSize = outImage->m_iWidth;
        if ( outImage->m_iHeight > maxSize ) maxSize = outImage->m_iHeight;
        outImage->m_iMipmapLevels = (int) std::floor( std::log2( maxSize ) ) + 1;
        if ( outImage->m_iMipmapLevels < 1 ) outImage->m_iMipmapLevels = 1;
        outImage->m_iFlags |= AGK_IMAGE_USE_MIPMAP;
    }

    if ( (flags & AGK_IMAGE_GEN_MIPMAP) ) outImage->m_iFlags |= AGK_IMAGE_GEN_MIPMAP;
     */
    
    VulkanImageData *pNewData;
    int index = outImage->GetFreeImageData();
    if ( index >= 0 )
    {
        pNewData = outImage->m_pImageData[ index ];
        outImage->m_iCurrentDataIndex = index;
    }
    else
    {
        pNewData = new VulkanImageData();
        outImage->m_pImageData.AddItem( pNewData );
        outImage->m_iCurrentDataIndex = outImage->m_pImageData.NumItems()-1; // must do this before the create commands
        ReturnIfFail( InternalCreateImage2D(outImage) );
        ReturnIfFail( InternalCreateImageView(outImage) );
        ReturnIfFail( InternalCreateImageViewFrameBuffer(outImage) );
    }
    
#if defined(AGK_IOS) || defined(AGK_MACOS)
    vkSetMTLTextureMVK( pNewData->m_vkImage, (id<MTLTexture>) texID );
#endif
    
    pNewData->m_iCurrentMode = AGK_VK_IMAGE_MODE_READ;

    return APP_SUCCESS;
}

void VulkanRenderer::CleanUpImages()
{
	// free up any resources in the images if possible
	VulkanImage *pImage = VulkanImage::g_pAllImages;
	while( pImage )
	{
		pImage->m_iFlags &= ~AGK_VK_IMAGE_ANY_IN_USE;
		for( int i = 0; i < (int) pImage->m_pImageData.NumItems(); i++ )
		{
			VulkanImageData *pData = pImage->m_pImageData[ i ];

			// modified flags
			if ( pData->m_iFlags & AGK_VK_IMAGE_GPU_MODIFIED ) 
			{
				pData->m_iFlags &= ~AGK_VK_IMAGE_GPU_MODIFIED;
				pData->m_iFlags |= AGK_VK_IMAGE_GPU_PREV_MODIFIED;
			}
			else if ( pData->m_iFlags & AGK_VK_IMAGE_GPU_PREV_MODIFIED ) 
			{
				pData->m_iFlags &= ~AGK_VK_IMAGE_GPU_PREV_MODIFIED;
			}

			// bound flags
			uint32_t newFlags = pData->m_iFlags & ~(AGK_VK_IMAGE_AWAITING_USE | AGK_VK_IMAGE_IN_USE | AGK_VK_IMAGE_UNUSED);
			if ( pData->m_iFlags & AGK_VK_IMAGE_AWAITING_USE ) 
			{
				pData->m_iFlags = newFlags | AGK_VK_IMAGE_IN_USE;
				pImage->m_iFlags |= AGK_VK_IMAGE_ANY_IN_USE;
			}
			else if ( pData->m_iFlags & AGK_VK_IMAGE_IN_USE ) 
			{
				pData->m_iFlags = newFlags | AGK_VK_IMAGE_UNUSED;
				pData->m_iUnusedCount = 0;
			}
			else if ( pData->m_iFlags & AGK_VK_IMAGE_UNUSED ) 
			{
				pData->m_iUnusedCount++;
				if ( pData->m_iUnusedCount > 3 )
				{
					if ( pImage->m_pImageData.NumItems() > 1 && pImage->m_iCurrentDataIndex != i ) 
					{
						if ( pData->m_vkImageView ) vkDestroyImageView( m_vkDevice, pData->m_vkImageView, nullptr );
						if ( pData->m_vkImageViewFrameBuffer ) vkDestroyImageView( m_vkDevice, pData->m_vkImageViewFrameBuffer, nullptr );
						if ( pData->m_vkImage && pData->m_vkImageMemory ) vmaDestroyImage( m_vkAllocator, pData->m_vkImage, pData->m_vkImageMemory );

						delete pData;

						if ( pImage->m_iCurrentDataIndex == pImage->m_pImageData.NumItems()-1 ) pImage->m_iCurrentDataIndex = i;
						pImage->m_pImageData.RemoveIndex( i );
						i--;
					}
				}
			}
		}

		for( int i = 0; i < (int) pImage->m_pOldPartialTransfers.NumItems(); i++ )
		{
			VulkanImageTransfer *pTransfer = pImage->m_pOldPartialTransfers[ i ];
			pTransfer->m_iUnusedCount++;
			if( pTransfer->m_iUnusedCount > 3 )
			{
				pImage->m_pOldPartialTransfers.RemoveIndex( i );
				InternalDeleteImageTransfer( pTransfer );
				i--;
			}
		}

		for( int i = 0; i < (int) pImage->m_pPartialTransfers.NumItems(); i++ )
		{
			VulkanImageTransfer *pTransfer = pImage->m_pPartialTransfers[ i ];
			switch( pTransfer->m_iState )
			{
				case AGK_VK_IMAGE_TRANSFER_AWAITING: pTransfer->m_iState = AGK_VK_IMAGE_TRANSFER_IN_PROGRESS; pImage->m_iFlags |= AGK_VK_IMAGE_ANY_IN_USE; break;
				case AGK_VK_IMAGE_TRANSFER_IN_PROGRESS: 
				{
					pTransfer->m_iState = AGK_VK_IMAGE_TRANSFER_COMPLETED; 
					pTransfer->m_iUnusedCount = 0;
					pImage->m_pPartialTransfers.RemoveIndex( i );
					pImage->m_pOldPartialTransfers.AddItem( pTransfer );
					i--;
					break;
				}
			}
		}

		for( int i = 0; i < (int) pImage->m_pCopyTransfers.NumItems(); i++ )
		{
			VulkanImageCopy *pCopy = pImage->m_pCopyTransfers[ i ];
			switch( pCopy->m_iState )
			{
				case AGK_VK_IMAGE_TRANSFER_AWAITING: pCopy->m_iState = AGK_VK_IMAGE_TRANSFER_IN_PROGRESS; pImage->m_iFlags |= AGK_VK_IMAGE_ANY_IN_USE; break;
				case AGK_VK_IMAGE_TRANSFER_IN_PROGRESS: pCopy->m_iState = AGK_VK_IMAGE_TRANSFER_COMPLETED; break;
				case AGK_VK_IMAGE_TRANSFER_COMPLETED:
				{
					// these are cheap to delete and create
					delete pCopy;
					pImage->m_pCopyTransfers.RemoveIndex( i );
					i--;
					break;
				}
			}
		}

		for( int i = 0; i < (int) pImage->m_pOldFullTransfers.NumItems(); i++ )
		{
			VulkanImageTransfer *pTransfer = pImage->m_pOldFullTransfers[ i ];
			pTransfer->m_iUnusedCount++;
			if ( pTransfer->m_iUnusedCount > 3 )
			{
				pImage->m_pOldFullTransfers.RemoveIndex( i );
				InternalDeleteImageTransfer( pTransfer );
				i--;
			}
		}

		for( int i = 0; i < (int) pImage->m_pFullTransfers.NumItems(); i++ )
		{
			VulkanImageTransfer *pTransfer = pImage->m_pFullTransfers[ i ];
			switch( pTransfer->m_iState )
			{
				case AGK_VK_IMAGE_TRANSFER_AWAITING: pTransfer->m_iState = AGK_VK_IMAGE_TRANSFER_IN_PROGRESS; pImage->m_iFlags |= AGK_VK_IMAGE_ANY_IN_USE; break;
				case AGK_VK_IMAGE_TRANSFER_IN_PROGRESS: 
				{
					pTransfer->m_iState = AGK_VK_IMAGE_TRANSFER_COMPLETED; 
					pImage->m_pFullTransfers.RemoveIndex( i );
					pImage->m_pOldFullTransfers.AddItem( pTransfer );
					i--;
					break;
				}
			}
		}

		if ( pImage->ShouldDelete() && !(pImage->m_iFlags & AGK_VK_IMAGE_ANY_IN_USE) )
		{
			VulkanImage *pNext = pImage->m_pNextImage;
			ForceDeleteImage( pImage );
			pImage = pNext;
			continue;
		}

		pImage = pImage->m_pNextImage;
	}
}

void VulkanRenderer::ForceDeleteImage( VulkanImage *pImage )
{
	for( uint32_t i = 0; i < pImage->m_pImageData.NumItems(); i++ )
	{
		VulkanImageData *pData = pImage->m_pImageData[ i ];
		if ( pData->m_vkImageView ) vkDestroyImageView( m_vkDevice, pData->m_vkImageView, nullptr );
		if ( pData->m_vkImageViewFrameBuffer ) vkDestroyImageView( m_vkDevice, pData->m_vkImageViewFrameBuffer, nullptr );
		if ( pData->m_vkImage && pData->m_vkImageMemory ) vmaDestroyImage( m_vkAllocator, pData->m_vkImage, pData->m_vkImageMemory );
		delete pData;
	}

	// don't delete the sampler, it belongs to a pool

	// partial transfers
	for( uint32_t i = 0; i < pImage->m_pOldPartialTransfers.NumItems(); i++ )
	{
		InternalDeleteImageTransfer( pImage->m_pOldPartialTransfers[i] );
	}

	for( uint32_t i = 0; i < pImage->m_pPartialTransfers.NumItems(); i++ )
	{
		InternalDeleteImageTransfer( pImage->m_pPartialTransfers[i] );
	}

	// full transfers
	for( uint32_t i = 0; i < pImage->m_pOldFullTransfers.NumItems(); i++ )
	{
		InternalDeleteImageTransfer( pImage->m_pOldFullTransfers[i] );
	}

	for( uint32_t i = 0; i < pImage->m_pFullTransfers.NumItems(); i++ )
	{
		InternalDeleteImageTransfer( pImage->m_pFullTransfers[i] );
	}

	// copies
	for( uint32_t i = 0; i < pImage->m_pCopyTransfers.NumItems(); i++ )
	{
		delete pImage->m_pCopyTransfers[i];
	}

	delete pImage;
}

int VulkanRenderer::SetImageSampleSettings( void *pResource, uint32_t flags )
{
	if ( !pResource ) return APP_ERROR_UNKNOWN;
	VulkanImage *pImage = (VulkanImage*) pResource;

	uint32_t mask = AGK_IMAGE_USE_MIPMAP | AGK_IMAGE_WRAP_U_REPEAT | AGK_IMAGE_WRAP_V_REPEAT | AGK_IMAGE_MAG_FILTER_NEAREST | AGK_IMAGE_MIN_FILTER_NEAREST | AGK_IMAGE_MIPMAP_FILTER_NEAREST;
	flags &= mask;
	if ( flags == (pImage->m_iFlags & mask) ) return APP_SUCCESS; // nothing changed
	
	pImage->m_iFlags &= ~mask;
	pImage->m_iFlags |= flags | AGK_VK_IMAGE_SAMPLER_CHANGED;
	
	return APP_SUCCESS;
}

void VulkanRenderer::SetImageMagFilter( void *pResource, uint32_t mode )
{
	if ( !pResource ) return;
	VulkanImage *pImage = (VulkanImage*) pResource;

	uint32_t mask = AGK_IMAGE_MAG_FILTER_NEAREST;
	if ( mode == 0 )
	{
		if ( (pImage->m_iFlags & mask) == mask ) return; // nothing changed
		pImage->m_iFlags |= mask | AGK_VK_IMAGE_SAMPLER_CHANGED;
	}
	else
	{
		if ( (pImage->m_iFlags & mask) == 0 ) return; // nothing changed
		pImage->m_iFlags &= ~mask;
		pImage->m_iFlags |= AGK_VK_IMAGE_SAMPLER_CHANGED;
	}	
}

void VulkanRenderer::SetImageMinFilter( void *pResource, uint32_t mode )
{
	if ( !pResource ) return;
	VulkanImage *pImage = (VulkanImage*) pResource;

	uint32_t mask = AGK_IMAGE_MIN_FILTER_NEAREST | AGK_IMAGE_MIPMAP_FILTER_NEAREST;
	if ( mode == 0 )
	{
		if ( (pImage->m_iFlags & mask) == mask ) return; // nothing changed
		pImage->m_iFlags |= mask | AGK_VK_IMAGE_SAMPLER_CHANGED;
	}
	else
	{
		if ( (pImage->m_iFlags & mask) == 0 ) return; // nothing changed
		pImage->m_iFlags &= ~mask;
		pImage->m_iFlags |= AGK_VK_IMAGE_SAMPLER_CHANGED;
	}	
}

void VulkanRenderer::SetImageWrapU( void *pResource, uint32_t mode )
{
	if ( !pResource ) return;
	VulkanImage *pImage = (VulkanImage*) pResource;

	uint32_t mask = AGK_IMAGE_WRAP_U_REPEAT;
	if ( mode )
	{
		if ( (pImage->m_iFlags & mask) == mask ) return; // nothing changed
		pImage->m_iFlags |= mask | AGK_VK_IMAGE_SAMPLER_CHANGED;
	}
	else
	{
		if ( (pImage->m_iFlags & mask) == 0 ) return; // nothing changed
		pImage->m_iFlags &= ~mask;
		pImage->m_iFlags |= AGK_VK_IMAGE_SAMPLER_CHANGED;
	}	
}

void VulkanRenderer::SetImageWrapV( void *pResource, uint32_t mode )
{
	if ( !pResource ) return;
	VulkanImage *pImage = (VulkanImage*) pResource;

	uint32_t mask = AGK_IMAGE_WRAP_V_REPEAT;
	if ( mode )
	{
		if ( (pImage->m_iFlags & mask) == mask ) return; // nothing changed
		pImage->m_iFlags |= mask | AGK_VK_IMAGE_SAMPLER_CHANGED;
	}
	else
	{
		if ( (pImage->m_iFlags & mask) == 0 ) return; // nothing changed
		pImage->m_iFlags &= ~mask;
		pImage->m_iFlags |= AGK_VK_IMAGE_SAMPLER_CHANGED;
	}	
}


VkSampler VulkanRenderer::InternalGetImageSampler( VulkanImage *pImage )
{
	if ( !pImage->m_vkSampler || (pImage->m_iFlags & AGK_VK_IMAGE_SAMPLER_CHANGED) )
	{
		uint32_t mask = AGK_IMAGE_USE_MIPMAP | AGK_IMAGE_WRAP_U_REPEAT | AGK_IMAGE_WRAP_V_REPEAT | AGK_IMAGE_MAG_FILTER_NEAREST | AGK_IMAGE_MIN_FILTER_NEAREST | AGK_IMAGE_MIPMAP_FILTER_NEAREST;
		pImage->m_vkSampler = g_pSamplers.Get( pImage->m_iFlags & mask );
		pImage->m_iFlags &= ~AGK_VK_IMAGE_SAMPLER_CHANGED;
	}

	return pImage->m_vkSampler;
}

int VulkanRenderer::CreateSampler( VkSampler *pSampler, uint32_t flags )
{
	VkSamplerCreateInfo samplerInfo = {};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = (flags & AGK_IMAGE_MAG_FILTER_NEAREST) ? VK_FILTER_NEAREST : VK_FILTER_LINEAR;
	samplerInfo.minFilter = (flags & AGK_IMAGE_MIN_FILTER_NEAREST) ? VK_FILTER_NEAREST : VK_FILTER_LINEAR;;
	samplerInfo.addressModeU = (flags & AGK_IMAGE_WRAP_U_REPEAT) ? VK_SAMPLER_ADDRESS_MODE_REPEAT : VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	samplerInfo.addressModeV = (flags & AGK_IMAGE_WRAP_V_REPEAT) ? VK_SAMPLER_ADDRESS_MODE_REPEAT : VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	if ( m_vkSupportedFeatures.samplerAnisotropy )
	{
		samplerInfo.anisotropyEnable = VK_TRUE;
		samplerInfo.maxAnisotropy = 8;
	}
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerInfo.mipmapMode = (flags & AGK_IMAGE_MIPMAP_FILTER_NEAREST) ? VK_SAMPLER_MIPMAP_MODE_NEAREST : VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.minLod = 0;
	samplerInfo.maxLod = 16;

	VkResult res = vkCreateSampler( m_vkDevice, &samplerInfo, nullptr, pSampler );
	if ( VK_SUCCESS != res ) ReportAndReturnErrorID( APP_ERROR_VK_CREATE_SAMPLER_FAILED, res );

	return APP_SUCCESS;
}

void VulkanRenderer::DeleteSampler( VkSampler sampler )
{
	vkDestroySampler( m_vkDevice, sampler, nullptr );
}

int VulkanRenderer::GetImageData( void **pResource, uint8_t **pData )
{
	if ( !pData ) return 0;
	if ( !pResource ) return 0;
	VulkanImage *pVulkanImage = (VulkanImage*) *pResource;
	if ( !pVulkanImage ) return 0;
	VulkanImageData *pImageData = pVulkanImage->GetCurrImageData();
	uint32_t dataSize = pVulkanImage->m_iWidth * pVulkanImage->m_iHeight * pVulkanImage->m_iBytesPerPixel;
	if ( dataSize == 0 ) return 0;

	if ( pImageData->m_iCurrentMode == AGK_VK_IMAGE_MODE_UNDEFINED )
	{
		*pData = new uint8_t[ dataSize ];
		memset( *pData, 0, dataSize );
		return dataSize;
	}

	uint32_t modifiedThisFrame = 0;
	if ( (pImageData->m_iFlags & AGK_VK_IMAGE_GPU_MODIFIED) ) modifiedThisFrame = 1;
	else
	{
		// if the image has a recent transfer then we can just steal the data from there
		if ( pVulkanImage->HasTransfers() )
		{
			if ( pVulkanImage->m_pPartialTransfers.NumItems() > 0 || pVulkanImage->m_pCopyTransfers.NumItems() > 0 ) modifiedThisFrame = 1;
			else
			{
				if ( pVulkanImage->m_pFullTransfers.NumItems() == 0 ) 
				{
					AppError( "Image says it has transfers but none found" );
					return 0;
				}
				else
				{
					for( int i = (int)pVulkanImage->m_pFullTransfers.NumItems() - 1; i >= 0; i-- )
					{
						VulkanImageTransfer *pTransfer = pVulkanImage->m_pFullTransfers[ i ];
						if ( pTransfer->m_iState == AGK_VK_IMAGE_TRANSFER_AWAITING )
						{
							*pData = new uint8_t[ dataSize ];
							memcpy( *pData, pTransfer->m_pMappedData, dataSize );
							return dataSize;
						}
					}
				}
			}
		}
	}

	if ( modifiedThisFrame )
	{
		// image has been changed in this frame, must submit everything then wait
		EndFrameAndSubmit( 0 ); // don't signal semaphore as we'll submit again at the end of the frame
		
		// wait for previous frame
		vkWaitForFences( m_vkDevice, 1, &m_vkFenceFrameFinished[ 1-m_iCurrentFrame ], VK_TRUE, UINT64_MAX );

		CleanUpResources();

		// wait for this frame
		vkWaitForFences( m_vkDevice, 1, &m_vkFenceFrameFinished[ m_iCurrentFrame ], VK_TRUE, UINT64_MAX );

		BeginFrame();
	}
	else
	{
		// wait for the previous frame, may not need to do this in all cases?
		vkWaitForFences( m_vkDevice, 1, &m_vkFenceFrameFinished[ 1-m_iCurrentFrame ], VK_TRUE, UINT64_MAX );
	}

	// create receive buffer
	VkBuffer buffer;
	VmaAllocation bufferMem;
	void *pMappedMem;

	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = dataSize;
	bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VmaAllocationCreateInfo allocCreateInfo = {};
	allocCreateInfo.requiredFlags = AGK_MEM_STAGING;
	VmaAllocationInfo allocInfo;
	VkResult res = vmaCreateBuffer( m_vkAllocator, &bufferInfo, &allocCreateInfo, &buffer, &bufferMem, &allocInfo );
	if ( VK_SUCCESS != res ) ReportAndReturnErrorID( APP_ERROR_VK_CREATE_BUFFER_FAILED, res );

	res = vmaMapMemory( m_vkAllocator, bufferMem, &pMappedMem );
	if ( VK_SUCCESS != res ) ReportAndReturnErrorID( APP_ERROR_VK_MAP_MEM_FAILED, res );

	VkCommandBufferBeginInfo begin_info = {};
	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	vkBeginCommandBuffer( m_vkGetImageCommandBuffer, &begin_info );

	// transition image to transfer src
	VkImageMemoryBarrier memoryBarrier = {};
	memoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	memoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	memoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	memoryBarrier.image = pImageData->m_vkImage;
	memoryBarrier.subresourceRange.aspectMask = pVulkanImage->IsDepth() ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
	memoryBarrier.subresourceRange.baseArrayLayer = 0;
	memoryBarrier.subresourceRange.layerCount = 1;
	memoryBarrier.subresourceRange.baseMipLevel = 0; 
	memoryBarrier.subresourceRange.levelCount = 1; // only transfer this level
	memoryBarrier.srcAccessMask = 0; 
	memoryBarrier.oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; 
	memoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT; 
	memoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL; 
		
	vkCmdPipelineBarrier( m_vkGetImageCommandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, 0, 0, 0, 1, &memoryBarrier );

	// copy image to buffer
	VkBufferImageCopy region = {};
	region.bufferImageHeight = 0;
	region.bufferRowLength = 0;
	region.bufferOffset = 0;
	region.imageSubresource.aspectMask = pVulkanImage->IsDepth() ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;
	region.imageOffset.x = 0;
	region.imageOffset.y = 0;
	region.imageExtent.width = pVulkanImage->m_iWidth;
	region.imageExtent.height = pVulkanImage->m_iHeight;
	region.imageExtent.depth = 1;

	vkCmdCopyImageToBuffer( m_vkGetImageCommandBuffer, pImageData->m_vkImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, buffer, 1, &region );

	// transition image back to read
	memoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT; 
	memoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL; 
	memoryBarrier.dstAccessMask = 0;
	memoryBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; 
		
	vkCmdPipelineBarrier( m_vkGetImageCommandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, 0, 0, 0, 1, &memoryBarrier );

	// end and submit command buffer
	vkEndCommandBuffer( m_vkGetImageCommandBuffer );

	vkResetFences( m_vkDevice, 1, &m_vkFenceGetImageFinished );

	VkSubmitInfo submit_info = {};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &m_vkGetImageCommandBuffer;
	submit_info.signalSemaphoreCount = 0;
	submit_info.pSignalSemaphores = nullptr;
	submit_info.waitSemaphoreCount = 0;
	submit_info.pWaitSemaphores = nullptr;
	vkQueueSubmit( m_vkQueue, 1, &submit_info, m_vkFenceGetImageFinished );

	// wait for it to complete
	vkWaitForFences( m_vkDevice, 1, &m_vkFenceGetImageFinished, VK_TRUE, UINT64_MAX );

	// buffer is now valid
	*pData = new uint8_t[ dataSize ];
	memcpy( *pData, pMappedMem, dataSize );

	// clean up
	vmaUnmapMemory( m_vkAllocator, bufferMem );
	vmaDestroyBuffer( m_vkAllocator, buffer, bufferMem );

	return dataSize;
}

struct VRVulkanTextureData_t
{
	VkImage m_nImage;
	VkDevice_T *m_pDevice;
	VkPhysicalDevice_T *m_pPhysicalDevice;
	VkInstance_T *m_pInstance;
	VkQueue_T *m_pQueue;
	uint32_t m_nQueueFamilyIndex;
	uint32_t m_nWidth, m_nHeight, m_nFormat, m_nSampleCount;
};

void* VulkanRenderer::GetVRImage( void *pResource )
{
	if ( !pResource ) return 0;
	VulkanImage *pVulkanImage = (VulkanImage*) pResource;
	if ( !pVulkanImage->IsVR() )
	{
		AppError( "Failed to get VR image data, it has not been set as a VR image" );
		return 0;
	}

	if ( m_iFlags & AGK_VULKAN_RENDER_PASS_STARTED ) 
	{
		AppError( "Cannot get VR image data in the middle of a render, please use SetRenderToImage to end the current render pass and then get the image data before doing any new rendering" );
		return 0;
	}
	
	InternalImageBarrier( m_vkCommandBuffer[m_iCurrentFrame], pVulkanImage, AGK_VK_IMAGE_MODE_TRANSFER_SRC );

	VulkanImageData *pImageData = pVulkanImage->GetCurrImageData();

	VRVulkanTextureData_t *pVRData = new VRVulkanTextureData_t();
	pVRData->m_nImage = pImageData->m_vkImage;
	pVRData->m_pDevice = m_vkDevice;
	pVRData->m_pPhysicalDevice = m_vkGPU;
	pVRData->m_pInstance = m_vkInstance;
	pVRData->m_pQueue = m_vkQueue;
	pVRData->m_nQueueFamilyIndex = m_iQueueFamilyIndex;
	pVRData->m_nWidth = pVulkanImage->m_iWidth;
	pVRData->m_nHeight = pVulkanImage->m_iHeight;
	pVRData->m_nFormat = pVulkanImage->m_vkFormat;
	pVRData->m_nSampleCount = VK_SAMPLE_COUNT_1_BIT;

	return pVRData;
}

void VulkanRenderer::SetVRImage( void *pResource, int mode )
{
	if ( !pResource ) return;
	VulkanImage *pVulkanImage = (VulkanImage*) pResource;
	if ( mode ) 
	{
		if ( pVulkanImage->m_iFlags & AGK_IMAGE_USE_MIPMAP )
		{
			AppError( "Cannot enable VR mode on this image as it contains mipmaps" );
			return;
		}
		pVulkanImage->m_iFlags |= AGK_VK_IMAGE_IS_VR;
		m_pVRImages.AddItem( pVulkanImage );
	}
	else 
	{
		pVulkanImage->m_iFlags &= ~AGK_VK_IMAGE_IS_VR;
		for( int i = 0; i < (int) m_pVRImages.NumItems(); i++ )
		{
			if ( m_pVRImages[ i ] == pVulkanImage ) 
			{
				m_pVRImages.RemoveIndex( i );
				i--;
			}
		}
	}
}

void VulkanRenderer::ResetVRImages()
{
	if ( m_iFlags & AGK_VULKAN_RENDER_PASS_STARTED ) 
	{
		AppError( "Cannot reset VR images in the middle of a render, please use SetRenderToImage to end the current render pass and then reset the image data before doing any new rendering" );
		return;
	}

	// VR images are still in transfer source mode, so transition them back
	for( uint32_t i = 0; i < m_pVRImages.NumItems(); i++ )
	{
		// we don't know in advance if the image will be read or drawn, so read is default until it is used
		InternalImageBarrier( m_vkCommandBuffer[m_iCurrentFrame], m_pVRImages[ i ], AGK_VK_IMAGE_MODE_READ );
	}
}

#endif // DISABLE_VULKAN
