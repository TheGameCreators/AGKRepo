#if !defined(DISABLE_VULKAN) && (!defined(__APPLE__) || defined(__LP64__))

#include "AppErrors.h"
#include "AGKVulkan.h"

using namespace AGK;

VulkanFrameBuffer* VulkanFrameBuffer::g_pCurrFrameBuffer = 0;
VulkanFrameBuffer* VulkanFrameBuffer::g_pAllFrameBuffers = 0;

void VulkanFrameBuffer::RemoveFrameBuffer() 
{
	if ( !m_pNextFrameBuffer && !m_pPrevFrameBuffer && g_pAllFrameBuffers != this ) return;

	if ( m_pPrevFrameBuffer ) m_pPrevFrameBuffer->m_pNextFrameBuffer = m_pNextFrameBuffer;
	else g_pAllFrameBuffers = m_pNextFrameBuffer;
	if ( m_pNextFrameBuffer ) m_pNextFrameBuffer->m_pPrevFrameBuffer = m_pPrevFrameBuffer;
	m_pNextFrameBuffer = 0;
	m_pPrevFrameBuffer = 0;
}

void VulkanFrameBuffer::AddFrameBuffer()
{
	if ( m_pNextFrameBuffer || m_pPrevFrameBuffer || g_pAllFrameBuffers == this ) return;

	if ( g_pAllFrameBuffers ) g_pAllFrameBuffers->m_pPrevFrameBuffer = this;
	m_pPrevFrameBuffer = 0;
	m_pNextFrameBuffer = g_pAllFrameBuffers;
	g_pAllFrameBuffers = this;
}

int VulkanRenderer::CreateRenderPass( VkFormat colorFormat, VkFormat depthFormat, uint32_t flags, VkRenderPass &renderPass )
{
	VkAttachmentDescription attachments[ 2 ] = {0};
	uint32_t numAttachments = 0;
	uint32_t numDependencies = 0;

	VkAttachmentReference depthAttachment = {};
	depthAttachment.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference colorAttachment = {};
	colorAttachment.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subPass = {};
	subPass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	
	if ( depthFormat != VK_FORMAT_UNDEFINED )
	{
		depthAttachment.attachment = numAttachments;
		subPass.pDepthStencilAttachment = &depthAttachment;

		attachments[ numAttachments ].format = depthFormat;
		attachments[ numAttachments ].samples = VK_SAMPLE_COUNT_1_BIT;
		attachments[ numAttachments ].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachments[ numAttachments ].storeOp = (flags & AGK_VK_RENDER_PASS_FLAG_STORE_DEPTH) ? VK_ATTACHMENT_STORE_OP_STORE : VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachments[ numAttachments ].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachments[ numAttachments ].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachments[ numAttachments ].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attachments[ numAttachments ].finalLayout = (flags & AGK_VK_RENDER_PASS_FLAG_STORE_DEPTH) ? VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL : VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		numAttachments++;
	}

	if ( colorFormat != VK_FORMAT_UNDEFINED )
	{
		colorAttachment.attachment = numAttachments;
		subPass.colorAttachmentCount = 1;
		subPass.pColorAttachments = &colorAttachment;

		attachments[ numAttachments ].format = colorFormat;
		attachments[ numAttachments ].samples = VK_SAMPLE_COUNT_1_BIT;
		attachments[ numAttachments ].loadOp = (flags & AGK_VK_RENDER_PASS_FLAG_NO_CLEAR) ? VK_ATTACHMENT_LOAD_OP_DONT_CARE : VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachments[ numAttachments ].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachments[ numAttachments ].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachments[ numAttachments ].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachments[ numAttachments ].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attachments[ numAttachments ].finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		numAttachments++;
	}
	/*
	// make sure everyone is finished reading before we write to this image
	VkSubpassDependency dependencies[2];
	dependencies[ 0 ] = {};
	dependencies[ 0 ].srcSubpass = VK_SUBPASS_EXTERNAL;
	dependencies[ 0 ].dstSubpass = 0;
	dependencies[ 0 ].srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	dependencies[ 0 ].srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
	dependencies[ 0 ].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependencies[ 0 ].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	// make sure we are finished writing before anyone reads from this image
	dependencies[ 1 ] = {};
	dependencies[ 1 ].srcSubpass = 0;
	dependencies[ 1 ].dstSubpass = VK_SUBPASS_EXTERNAL;
	dependencies[ 1 ].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependencies[ 1 ].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	dependencies[ 1 ].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	dependencies[ 1 ].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		*/
	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = numAttachments;
	renderPassInfo.pAttachments = attachments;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subPass;
	renderPassInfo.dependencyCount = 0;
	renderPassInfo.pDependencies = 0;

	VkResult res = vkCreateRenderPass( m_vkDevice, &renderPassInfo, nullptr, &renderPass );
	if ( VK_SUCCESS != res ) ReportAndReturnErrorID( APP_ERROR_VK_CREATE_RENDER_PASS_FAILED, res );

	return APP_SUCCESS;
}

int VulkanRenderer::CreateFrameBuffer( void *pColorImage, void *pDepthImage, int iForceDepth, void **outResource )
{
	if ( !outResource ) return APP_ERROR_UNKNOWN;
	if ( *outResource ) DeleteFrameBuffer( *outResource );

	VulkanFrameBuffer *pFrameBuffer = new VulkanFrameBuffer();
	*outResource = pFrameBuffer;

	pFrameBuffer->m_pColor = (VulkanImage*) pColorImage;
	pFrameBuffer->m_pDepth = (VulkanImage*) pDepthImage;

	if ( !pDepthImage && iForceDepth )
	{
		AGKImgFormat depthFormat;
		if ( IsImageFormatSupported( AGK_DEPTH_FORMAT_32_FLOAT ) ) depthFormat = AGK_DEPTH_FORMAT_32_FLOAT;
		else if ( IsImageFormatSupported( AGK_DEPTH_FORMAT_24_INT ) ) depthFormat = AGK_DEPTH_FORMAT_24_INT;
		else depthFormat = AGK_DEPTH_FORMAT_16_INT;
		CreateDepthBuffer( depthFormat, pFrameBuffer->m_pColor->m_iWidth, pFrameBuffer->m_pColor->m_iHeight, AGK_VK_IMAGE_IS_DEFAULT, (void**)&pFrameBuffer->m_pDepth );
		pFrameBuffer->m_iFlags |= AGK_VK_FRAMEBUFFER_FORCE_DEPTH;
	}

	pFrameBuffer->m_iWidth = 0;
	pFrameBuffer->m_iHeight = 0;
	
	uint32_t m_iNumViews = 0;
	VkImageView attachmentViews[ 2 ];

	// depth and color order must match the order used in the render pass
	if ( pFrameBuffer->m_pDepth )
	{
		if ( !pFrameBuffer->m_pDepth->IsDepth() )
		{
			AppError( "Cannot render depth to an image unless it was created with a depth format in CreateRenderImage" );
			return APP_ERROR_GENERAL;
		}
		pFrameBuffer->m_iWidth = pFrameBuffer->m_pDepth->m_iWidth;
		pFrameBuffer->m_iHeight = pFrameBuffer->m_pDepth->m_iHeight;
		attachmentViews[ m_iNumViews ] = pFrameBuffer->m_pDepth->GetCurrentFrameBufferView();
		m_iNumViews++;
	}

	if ( pFrameBuffer->m_pColor )
	{
		if ( !pFrameBuffer->m_pColor->IsRenderable() )
		{
			AppError( "Vulkan does not support rendering to loaded images, only images create with CreateRenderImage can be used with SetRenderToImage" );
			return APP_ERROR_GENERAL;
		}
		pFrameBuffer->m_iWidth = pFrameBuffer->m_pColor->m_iWidth;
		pFrameBuffer->m_iHeight = pFrameBuffer->m_pColor->m_iHeight;
		attachmentViews[ m_iNumViews ] = pFrameBuffer->m_pColor->GetCurrentFrameBufferView();
		m_iNumViews++;
	}

	// Render pass doesn't have to be an exact match, as long as it is 'compatible' by Vulkan standards
	pFrameBuffer->m_iRenderPassIndex = AGK_VK_RENDER_PASS_RGBA_DEPTH;
	if ( !pFrameBuffer->m_pColor ) pFrameBuffer->m_iRenderPassIndex = AGK_VK_RENDER_PASS_DEPTH;
	else if ( !pFrameBuffer->m_pDepth ) pFrameBuffer->m_iRenderPassIndex = AGK_VK_RENDER_PASS_RGBA;
	else if ( !pFrameBuffer->IsLocalDepth() ) pFrameBuffer->m_iRenderPassIndex = AGK_VK_RENDER_PASS_RGBA_DEPTH_STORE;

	VkFramebufferCreateInfo frameBufferInfo = {};
	frameBufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	frameBufferInfo.renderPass = m_vkRenderPasses[ pFrameBuffer->m_iRenderPassIndex ];
	frameBufferInfo.width = pFrameBuffer->m_iWidth;
	frameBufferInfo.height = pFrameBuffer->m_iHeight;
	frameBufferInfo.layers = 1;
	frameBufferInfo.attachmentCount = m_iNumViews;
	frameBufferInfo.pAttachments = attachmentViews;

	VkResult res = vkCreateFramebuffer( m_vkDevice, &frameBufferInfo, nullptr, &pFrameBuffer->m_vkFrameBuffer );
	if ( VK_SUCCESS != res ) ReportAndReturnErrorID( APP_ERROR_VK_CREATE_FRAMEBUFFER_FAILED, res );

	return APP_SUCCESS;
}

int VulkanRenderer::DeleteFrameBuffer( void *pResource )
{
	if ( !pResource ) return APP_SUCCESS;
	VulkanFrameBuffer* pFrameBuffer = (VulkanFrameBuffer*) pResource;
	
	if ( pFrameBuffer->IsLocalDepth() ) DeleteImage( pFrameBuffer->m_pDepth, 0 );
	pFrameBuffer->m_iFlags |= AGK_VK_FRAMEBUFFER_DELETE;
	
	return APP_SUCCESS;
}

void VulkanRenderer::SetFrameBufferIsShadow( void *pResource ) 
{
	if ( !pResource ) return;
	VulkanFrameBuffer* pFrameBuffer = (VulkanFrameBuffer*) pResource;
	pFrameBuffer->m_iFlags |= AGK_VK_FRAMEBUFFER_IS_SHADOW;
}

void VulkanRenderer::StartShadowFrameBuffer( void *pResource ) 
{
	if ( !pResource ) return;
	VulkanFrameBuffer* pFrameBuffer = (VulkanFrameBuffer*) pResource;
	if ( !pFrameBuffer->IsShadow() ) return;

	InternalImageBarrier( m_vkCommandBuffer[m_iCurrentFrame], pFrameBuffer->m_pDepth, AGK_VK_IMAGE_MODE_DRAW );
}

void VulkanRenderer::StartShadowFrameBuffers( void *pResource1, void *pResource2, void *pResource3, void *pResource4 )
{
	VulkanImageData *pData[4];
	pData[0] = ((VulkanFrameBuffer*)pResource1)->m_pDepth->GetCurrImageData();
	pData[1] = ((VulkanFrameBuffer*)pResource2)->m_pDepth->GetCurrImageData();
	pData[2] = ((VulkanFrameBuffer*)pResource3)->m_pDepth->GetCurrImageData();
	pData[3] = ((VulkanFrameBuffer*)pResource4)->m_pDepth->GetCurrImageData();
	
	uint32_t srcStage;

	VkImageMemoryBarrier memoryBarrier[ 4 ] = {};
	for( int i = 0; i < 4; i++ )
	{
		memoryBarrier[i].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		memoryBarrier[i].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		memoryBarrier[i].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		memoryBarrier[i].image = pData[i]->m_vkImage;
		memoryBarrier[i].subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT ;
		memoryBarrier[i].subresourceRange.baseArrayLayer = 0;
		memoryBarrier[i].subresourceRange.layerCount = 1;
		memoryBarrier[i].subresourceRange.baseMipLevel = 0; 
		memoryBarrier[i].subresourceRange.levelCount = 1; // only transfer this level

		if( pData[i]->m_iCurrentMode == AGK_VK_IMAGE_MODE_UNDEFINED )
		{
			memoryBarrier[i].srcAccessMask = 0; 
			memoryBarrier[i].oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT; // don't know
		} else if ( pData[i]->m_iCurrentMode == AGK_VK_IMAGE_MODE_READ )
		{
			memoryBarrier[i].srcAccessMask = VK_ACCESS_SHADER_READ_BIT; 
			memoryBarrier[i].oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; 
			srcStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}

		memoryBarrier[i].dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT; 
		memoryBarrier[i].newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; // render pass will convert to the necessary layout

		pData[i]->m_iCurrentMode = AGK_VK_IMAGE_MODE_DRAW;
	}
		
	vkCmdPipelineBarrier( m_vkCommandBuffer[m_iCurrentFrame], srcStage, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT, 0, 0, 0, 0, 0, 4, memoryBarrier );
}

void VulkanRenderer::CompleteShadowFrameBuffer( void *pResource ) 
{
	if ( !pResource ) return;
	VulkanFrameBuffer* pFrameBuffer = (VulkanFrameBuffer*) pResource;
	if ( !pFrameBuffer->IsShadow() ) return;

	InternalImageBarrier( m_vkCommandBuffer[m_iCurrentFrame], pFrameBuffer->m_pDepth, AGK_VK_IMAGE_MODE_READ );
}

void VulkanRenderer::CompleteShadowFrameBuffers( void *pResource1, void *pResource2, void *pResource3, void *pResource4 )
{
	VulkanImageData *pData[4];
	pData[0] = ((VulkanFrameBuffer*)pResource1)->m_pDepth->GetCurrImageData();
	pData[1] = ((VulkanFrameBuffer*)pResource2)->m_pDepth->GetCurrImageData();
	pData[2] = ((VulkanFrameBuffer*)pResource3)->m_pDepth->GetCurrImageData();
	pData[3] = ((VulkanFrameBuffer*)pResource4)->m_pDepth->GetCurrImageData();
	
	VkImageMemoryBarrier memoryBarrier[ 4 ] = {};
	for( int i = 0; i < 4; i++ )
	{
		memoryBarrier[i].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		memoryBarrier[i].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		memoryBarrier[i].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		memoryBarrier[i].image = pData[i]->m_vkImage;
		memoryBarrier[i].subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		memoryBarrier[i].subresourceRange.baseArrayLayer = 0;
		memoryBarrier[i].subresourceRange.layerCount = 1;
		memoryBarrier[i].subresourceRange.baseMipLevel = 0; 
		memoryBarrier[i].subresourceRange.levelCount = 1; // only transfer this level

		memoryBarrier[i].srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT; 
		memoryBarrier[i].oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; // render pass will have converted to this layout at the end
		
		memoryBarrier[i].dstAccessMask = VK_ACCESS_SHADER_READ_BIT; 
		memoryBarrier[i].newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; 

		pData[i]->m_iCurrentMode = AGK_VK_IMAGE_MODE_READ;
	}
		
	uint32_t srcStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
	vkCmdPipelineBarrier( m_vkCommandBuffer[m_iCurrentFrame], srcStage, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, 0, 0, 0, 4, memoryBarrier );
}

int VulkanRenderer::BindFrameBuffer( void *pCommandBuffer, void *pResource )
{
	if ( pResource == VulkanFrameBuffer::g_pCurrFrameBuffer ) return APP_SUCCESS;

	if( m_iFlags & AGK_VULKAN_RENDER_PASS_STARTED ) 
	{
		EndRenderPass();
	}

	VulkanFrameBuffer::g_pCurrFrameBuffer = (VulkanFrameBuffer*) pResource;
	// don't start render pass until first draw, in case we never actually draw anything
	// this could with happen with render to screen as default but the first draw is to a render target image

	return APP_SUCCESS;
}

void VulkanRenderer::CleanUpFrameBuffers()
{
	VulkanFrameBuffer *pFrameBuffer = VulkanFrameBuffer::g_pAllFrameBuffers;
	while( pFrameBuffer )
	{
		VulkanFrameBuffer *pNext = pFrameBuffer->m_pNextFrameBuffer;

		uint32_t newFlags = pFrameBuffer->m_iFlags & ~(AGK_VK_FRAMEBUFFER_AWAITING_USE | AGK_VK_FRAMEBUFFER_IN_USE);
		if ( pFrameBuffer->m_iFlags & AGK_VK_SHADER_DATA_AWAITING_USE )
		{
			pFrameBuffer->m_iFlags = newFlags | AGK_VK_SHADER_DATA_IN_USE;
		}
		else pFrameBuffer->m_iFlags = newFlags;

		// delete
		if ( pFrameBuffer->ShouldDelete() && !pFrameBuffer->IsInUse() )
		{
			ForceDeleteFrameBuffer( pFrameBuffer );
		}

		pFrameBuffer = pNext;
	}
}

void VulkanRenderer::ForceDeleteFrameBuffer( VulkanFrameBuffer *pFrameBuffer )
{
	vkDestroyFramebuffer( m_vkDevice, pFrameBuffer->m_vkFrameBuffer, nullptr );
	delete pFrameBuffer;
}

#endif
