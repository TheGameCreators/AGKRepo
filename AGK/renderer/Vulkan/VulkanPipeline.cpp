#if !defined(DISABLE_VULKAN) && (!defined(__APPLE__) || defined(__LP64__))

#include "AppErrors.h"
#include "AGKVulkan.h"

using namespace AGK;

// pipeline cache

void VulkanPipelineCache::DeleteShader( AGKShader *pShader )
{
	uint32_t index1 = 0;
	uint32_t index2 = 0;
	while( index2 < m_iNumPipelines )
	{
		if ( m_pPipelines[ index2 ]->m_pShader == pShader )
		{
			if ( !m_pPipelines[ index2 ]->IsInUse() ) 
			{
				VulkanRenderer::GetInstance()->ForceDeletePipeline( m_pPipelines[ index2 ] );
				m_cAllPipelines.RemoveItem( m_pPipelines[ index2 ] );
				index2++;
			}
			else
			{
				m_pPipelines[ index2 ]->Delete(); // delete later
				if ( index1 != index2 ) m_pPipelines[ index1 ] = m_pPipelines[ index2 ];
				index1++;
				index2++;
			}
		}
		else
		{
			if ( index1 != index2 ) m_pPipelines[ index1 ] = m_pPipelines[ index2 ];
			index1++;
			index2++;
		}
	}

	m_iNumPipelines = index1;
}

void VulkanPipelineCache::CleanUpPipelines()
{
	uint32_t index1 = 0;
	uint32_t index2 = 0;
	while( index2 < m_iNumPipelines )
	{
		uint32_t newFlags = m_pPipelines[ index2 ]->m_iFlags & ~(AGK_VK_PIPELINE_AWAITING_USE | AGK_VK_PIPELINE_IN_USE);
		if ( m_pPipelines[ index2 ]->m_iFlags & AGK_VK_PIPELINE_AWAITING_USE )
		{
			m_pPipelines[ index2 ]->m_iFlags = newFlags | AGK_VK_PIPELINE_IN_USE;
		}
		else 
		{
			m_pPipelines[ index2 ]->m_iFlags = newFlags;
		}

		if ( m_pPipelines[ index2 ]->ShouldDelete() && !m_pPipelines[ index2 ]->IsInUse() )
		{
			VulkanRenderer::GetInstance()->ForceDeletePipeline( m_pPipelines[ index2 ] );
			m_cAllPipelines.RemoveItem( m_pPipelines[ index2 ] );
			index2++;
		}
		else
		{
			if ( index1 != index2 ) m_pPipelines[ index1 ] = m_pPipelines[ index2 ];
			index1++;
			index2++;
		}
	}

	m_iNumPipelines = index1;
}

void VulkanPipelineCache::DeleteAll()
{
	for( uint32_t i = 0; i < m_iNumPipelines; i++ )
	{
		VulkanRenderer::GetInstance()->ForceDeletePipeline( m_pPipelines[ i ] );
	}
	m_iNumPipelines = 0;
	m_cAllPipelines.ClearAll();
}

void VulkanPipelineCache::DeleteScreenPipelines()
{
	uint32_t index1 = 0;
	uint32_t index2 = 0;
	while( index2 < m_iNumPipelines )
	{
		if ( m_pPipelines[ index2 ]->m_iRenderPassIndex == AGK_VK_RENDER_PASS_SURFACE )
		{
			VulkanRenderer::GetInstance()->ForceDeletePipeline( m_pPipelines[ index2 ] );
			m_cAllPipelines.RemoveItem( m_pPipelines[ index2 ] );
			index2++;
		}
		else
		{
			if ( index1 != index2 ) m_pPipelines[ index1 ] = m_pPipelines[ index2 ];
			index1++;
			index2++;
		}
	}

	m_iNumPipelines = index1;
}

VulkanPipeline* VulkanPipelineCache::GetPipeline( VulkanPipeline *pPipelineTemplate ) const
{
	if ( m_iNumPipelines == 0 ) return 0;

	int high = m_iNumPipelines-1;
	int low = 0;
				
	// binary search
	int result = 0;
	while( high >= low )
	{
		int mid = (high+low)/2;
		int result = m_pPipelines[ mid ]->Compare( pPipelineTemplate );
		if( result > 0 ) high = mid-1;
		else if ( result < 0 ) low = mid+1;
		else 
		{
			return m_pPipelines[ mid ];
		}
	}

	return 0;
}

VulkanPipeline* VulkanPipelineCache::AddPipeline( const VulkanPipeline* pPipelineTemplate )
{
	VulkanPipeline *pPipeline = new VulkanPipeline();
	m_cAllPipelines.AddItem( pPipeline, pPipeline );

	pPipeline->Clone( pPipelineTemplate );

	if ( m_iPipelinesArraySize < m_iNumPipelines+1 )
	{
		m_iPipelinesArraySize = m_iPipelinesArraySize + m_iPipelinesArraySize/2;
		if ( m_iPipelinesArraySize < 2 ) m_iPipelinesArraySize = 2;
		VulkanPipeline** newArray = new VulkanPipeline*[ m_iPipelinesArraySize ];
		if ( m_pPipelines )
		{
			for( uint32_t i = 0; i < m_iNumPipelines; i++ ) newArray[ i ] = m_pPipelines[ i ];
			delete [] m_pPipelines;
		}
		m_pPipelines = newArray;
	}

	int high = m_iNumPipelines-1;
	int low = 0;
	int mid;

	// binary search
	while( high >= low )
	{
		mid = (high+low)/2;
		int result = m_pPipelines[ mid ]->Compare( pPipeline );
		if( result > 0 ) high = mid-1;
		else if ( result < 0 ) low = mid+1;
		else 
		{
			// not allowed dulplicates
			delete pPipeline;
			return m_pPipelines[ mid ]; 
		}
	}
				
	if ( low >= (int)m_iNumPipelines )
	{
		m_pPipelines[ m_iNumPipelines++ ] = pPipeline;
	}
	else
	{
		// insert new element at low index
		for( int i = m_iNumPipelines-1; i >= low; i-- )
		{
			m_pPipelines[ i+1 ] = m_pPipelines[ i ];
		}
		m_pPipelines[ low ] = pPipeline;
		m_iNumPipelines++;
	}

	return pPipeline;
}

// pipeline

int VulkanRenderer::BindPipeline( void *pCommandBuffer, AGKShader *pShader, const AGKRenderState *pRenderState, const AGKVertexLayout *pVertexLayout, void **inoutPipeline )
{
	CheckRenderPass(); // will begin render pass if necessary

	if ( !(m_iFlags & AGK_VULKAN_SURFACE_VALID) ) return APP_SUCCESS;

	VulkanPipeline pipelineTemplate;
	pipelineTemplate.m_iRenderPassIndex = AGK_VK_RENDER_PASS_SURFACE;
	if ( VulkanFrameBuffer::g_pCurrFrameBuffer ) pipelineTemplate.m_iRenderPassIndex = VulkanFrameBuffer::g_pCurrFrameBuffer->m_iRenderPassIndex;
	pipelineTemplate.m_iViewportX = m_iGlobalViewportX;
	pipelineTemplate.m_iViewportY = m_iGlobalViewportY;
	pipelineTemplate.m_iViewportWidth = m_iGlobalViewportWidth;
	pipelineTemplate.m_iViewportHeight = m_iGlobalViewportHeight;
	pipelineTemplate.m_pShader = pShader;
	pipelineTemplate.m_renderState = *pRenderState;
	pipelineTemplate.m_vertexLayout.Clone( pVertexLayout );

	//if ( VulkanFrameBuffer::g_pCurrFrameBuffer ) pipelineTemplate.m_renderState.InvertWindingOrder();

	if ( IsInvertedDepth() )
	{
		switch( pipelineTemplate.m_renderState.GetDepthFunc() )
		{
			case AGK_DEPTH_FUNC_LESS: pipelineTemplate.m_renderState.SetDepthFunc( AGK_DEPTH_FUNC_GREATER ); break;
			case AGK_DEPTH_FUNC_LEQUAL: pipelineTemplate.m_renderState.SetDepthFunc( AGK_DEPTH_FUNC_GEQUAL ); break;
			case AGK_DEPTH_FUNC_GREATER: pipelineTemplate.m_renderState.SetDepthFunc( AGK_DEPTH_FUNC_LESS ); break;
			case AGK_DEPTH_FUNC_GEQUAL: pipelineTemplate.m_renderState.SetDepthFunc( AGK_DEPTH_FUNC_LEQUAL ); break;
		}

		if ( pipelineTemplate.m_renderState.HasDepthRange() )
		{
			float newMin = 1.0f - pipelineTemplate.m_renderState.GetDepthRangeMax();
			float newMax = 1.0f - pipelineTemplate.m_renderState.GetDepthRangeMin();
			pipelineTemplate.m_renderState.SetDepthRange( newMin, newMax );
		}
	}

	if ( !pipelineTemplate.m_renderState.DoesOverrideScissor() ) 
	{
		pipelineTemplate.m_renderState.SetScissor( m_iGlobalScissorX, m_iGlobalScissorY, m_iGlobalScissorWidth, m_iGlobalScissorHeight );
		pipelineTemplate.m_renderState.OverrideScissor( 1 );
	}

	if ( pipelineTemplate.m_renderState.GetScissorHash() == 0 )
	{
		pipelineTemplate.m_renderState.SetScissor( 0, 0, 65535, 65535 );
	}

	m_iCurrPrimitiveMode = pVertexLayout->m_iPrimitiveType;

	// check if old pipeline is still valid
	VulkanPipeline *pPipeline = 0;
	if ( inoutPipeline && *inoutPipeline )
	{
		VulkanPipeline *pPrevPipeline = (VulkanPipeline*) *inoutPipeline;
		if ( m_globalPipelineCache.IsValid( pPrevPipeline ) )
		{
			if ( pPrevPipeline->Compare( &pipelineTemplate ) == 0 ) pPipeline = pPrevPipeline;
		}
		else
		{
			*inoutPipeline = 0;
		}
	}
	
	if ( !pPipeline )
	{
		// check for an old pipeline that matches our requirements
		pPipeline = m_globalPipelineCache.GetPipeline( &pipelineTemplate );
		if ( !pPipeline )
		{
			// construct a new pipeline (expensive)
			ReturnIfFail( ConstructPipeline( &pipelineTemplate ) );
			pPipeline = m_globalPipelineCache.AddPipeline( &pipelineTemplate );
		}

		if ( inoutPipeline )
		{
			if ( *inoutPipeline != pPipeline ) 
			{
				if ( *inoutPipeline ) ((VulkanPipeline*)*inoutPipeline)->m_iRefCount--;
				pPipeline->m_iRefCount++;
				*inoutPipeline = pPipeline;
			}
		}
	}

	pPipeline->DidBind();

	vkCmdBindPipeline( m_vkCommandBuffer[ m_iCurrentFrame ], VK_PIPELINE_BIND_POINT_GRAPHICS, pPipeline->m_vkPipeline );

	return APP_SUCCESS; 
}

int VulkanRenderer::DeletePipeline( void *pPipeline ) 
{ 
	if ( !pPipeline ) return APP_SUCCESS;
	// don't delete the pipeline, it will remain in the cache
	VulkanPipeline* pVulkanPipeline = (VulkanPipeline*) pPipeline;
	pVulkanPipeline->m_iRefCount--;
	return APP_SUCCESS; 
}

void VulkanRenderer::ForceDeletePipeline( VulkanPipeline *pPipeline )
{
	if ( pPipeline->m_vkPipeline ) vkDestroyPipeline( m_vkDevice, pPipeline->m_vkPipeline, nullptr );
	delete pPipeline;
}

int VulkanRenderer::ConstructPipeline( VulkanPipeline *pPipeline )
{
	if ( !pPipeline ) return APP_ERROR_UNKNOWN;
	if ( !pPipeline->m_pShader ) ReportAndReturnErrorID( APP_ERROR_GENERAL, "Tried to create a pipeline without a shader" );
	if ( !pPipeline->m_pShader->m_pInternalResources || !pPipeline->m_pShader->IsValid() ) ReportAndReturnErrorID( APP_ERROR_GENERAL, "Tried to create a pipeline with an invalid shader" );
	VulkanShader *pVulkanShader = (VulkanShader*) pPipeline->m_pShader->m_pInternalResources;

	// shader stages
	VkPipelineShaderStageCreateInfo pipelineShaderInfo[2] = {};
	pipelineShaderInfo[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	pipelineShaderInfo[0].module = pVulkanShader->m_vkVertShader;
	pipelineShaderInfo[0].pName = "main";
	pipelineShaderInfo[0].stage = VK_SHADER_STAGE_VERTEX_BIT;

	pipelineShaderInfo[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	pipelineShaderInfo[1].module = pVulkanShader->m_vkFragShader;
	pipelineShaderInfo[1].pName = "main";
	pipelineShaderInfo[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;

	// vertex attribute bindings
	VkVertexInputBindingDescription bindingDescription = {};
	bindingDescription.binding = 0;
	bindingDescription.stride = pPipeline->m_vertexLayout.m_iVertexSize;
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	VkVertexInputAttributeDescription *attributeDescriptions = new VkVertexInputAttributeDescription[ pPipeline->m_vertexLayout.m_iNumOffsets ];
	for( uint32_t i = 0; i < pPipeline->m_vertexLayout.m_iNumOffsets; i++ )
	{
		attributeDescriptions[ i ].binding = 0;
		attributeDescriptions[ i ].location = pPipeline->m_pShader->m_pAttribs[ i ].m_iLocation;
		switch( pPipeline->m_pShader->m_pAttribs[ i ].m_iComponents )
		{
			case 1: attributeDescriptions[ i ].format = VK_FORMAT_R32_SFLOAT; break;
			case 2: attributeDescriptions[ i ].format = VK_FORMAT_R32G32_SFLOAT; break;
			case 3: attributeDescriptions[ i ].format = VK_FORMAT_R32G32B32_SFLOAT; break;
			case 4: attributeDescriptions[ i ].format = VK_FORMAT_R32G32B32A32_SFLOAT; break;
		}
		attributeDescriptions[ i ].offset = pPipeline->m_vertexLayout.m_pOffsets[ i ];
	}

	// shader input data
	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
	vertexInputInfo.vertexAttributeDescriptionCount = pPipeline->m_vertexLayout.m_iNumOffsets;
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions;

	// shader vertex format
	VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo  = {};
	inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	switch( pPipeline->m_vertexLayout.m_iPrimitiveType )
	{
		case AGK_PRIMITIVE_TRIANGLES: inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST; break;
		case AGK_PRIMITIVE_TRIANGLE_STRIP: inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP; break;
		case AGK_PRIMITIVE_TRIANGLE_FAN: inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN; break;
		case AGK_PRIMITIVE_LINE_LOOP: inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP; break;
		case AGK_PRIMITIVE_LINES: inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST; break;
		case AGK_PRIMITIVE_POINTS: inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST; break;
	}
	inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

	// viewport
	VkViewport viewport = {};
	viewport.x = pPipeline->m_iViewportX;
	viewport.y = pPipeline->m_iViewportY;
	viewport.width = (float) pPipeline->m_iViewportWidth;
	viewport.height = (float) pPipeline->m_iViewportHeight;
	viewport.minDepth = pPipeline->m_renderState.GetDepthRangeMin();
	viewport.maxDepth = pPipeline->m_renderState.GetDepthRangeMax();

	// scissor, pipeline will always store scissor values in the render state object
	VkRect2D scissor;
	scissor.offset.x = pPipeline->m_renderState.GetScissorX();
	scissor.offset.y = pPipeline->m_renderState.GetScissorY();
	scissor.extent.width = pPipeline->m_renderState.GetScissorWidth();
	scissor.extent.height = pPipeline->m_renderState.GetScissorHeight();
	
	// viewport and scissor pipeline object
	VkPipelineViewportStateCreateInfo viewportStateInfo = {};
	viewportStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportStateInfo.viewportCount = 1;
	viewportStateInfo.pViewports = &viewport;
	viewportStateInfo.scissorCount = 1;
	viewportStateInfo.pScissors = &scissor;

	// rendering options
	VkPipelineRasterizationStateCreateInfo rasterizerInfo = {};
	rasterizerInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizerInfo.depthClampEnable = pPipeline->m_renderState.GetDepthClampEnable();
	rasterizerInfo.rasterizerDiscardEnable = VK_FALSE; // discard entire primitive, not related to fragment discard
	rasterizerInfo.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizerInfo.lineWidth = 1.0f;
	switch( pPipeline->m_renderState.GetCullMode() )
	{
		case AGK_CULL_NONE: rasterizerInfo.cullMode = VK_CULL_MODE_NONE; break;
		case AGK_CULL_BACK: rasterizerInfo.cullMode = VK_CULL_MODE_BACK_BIT; break;
		case AGK_CULL_FRONT: rasterizerInfo.cullMode = VK_CULL_MODE_FRONT_BIT; break;
	}
	switch( pPipeline->m_renderState.GetWindingOrder() )
	{
		case AGK_WINDING_CLOCKWISE: rasterizerInfo.frontFace = VK_FRONT_FACE_CLOCKWISE; break;
		case AGK_WINDING_COUNTER_CLOCKWISE: rasterizerInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE; break;
	}
	rasterizerInfo.depthBiasEnable = pPipeline->m_renderState.HasDepthBias();
	
	// multisample options
	VkPipelineMultisampleStateCreateInfo multisamplingInfo = {};
	multisamplingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisamplingInfo.sampleShadingEnable = pPipeline->m_renderState.HasMultisampling();
	switch( pPipeline->m_renderState.GetMultiSampleCount() )
	{
		case 1: multisamplingInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT; break;
		case 2: multisamplingInfo.rasterizationSamples = VK_SAMPLE_COUNT_2_BIT; break;
		case 4: multisamplingInfo.rasterizationSamples = VK_SAMPLE_COUNT_4_BIT; break;
		case 8: multisamplingInfo.rasterizationSamples = VK_SAMPLE_COUNT_8_BIT; break;
		case 16: multisamplingInfo.rasterizationSamples = VK_SAMPLE_COUNT_16_BIT; break;
		default: multisamplingInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT; break;
	}
	multisamplingInfo.minSampleShading = 1.0f;
	multisamplingInfo.pSampleMask = nullptr;
	multisamplingInfo.alphaToCoverageEnable = VK_FALSE;
	multisamplingInfo.alphaToOneEnable = VK_FALSE;

	// depth test options
	VkPipelineDepthStencilStateCreateInfo depthTestInfo = {};
	depthTestInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthTestInfo.depthBoundsTestEnable = VK_FALSE;
	depthTestInfo.minDepthBounds = 0.0f; // only pass depth values that fall in this range
	depthTestInfo.maxDepthBounds = 1.0f;
	depthTestInfo.depthTestEnable = pPipeline->m_renderState.GetDepthEnable();
	switch( pPipeline->m_renderState.GetDepthFunc() )
	{
		case AGK_DEPTH_FUNC_NEVER: depthTestInfo.depthCompareOp = VK_COMPARE_OP_NEVER; break;
		case AGK_DEPTH_FUNC_LESS: depthTestInfo.depthCompareOp = VK_COMPARE_OP_LESS; break;
		case AGK_DEPTH_FUNC_EQUAL: depthTestInfo.depthCompareOp = VK_COMPARE_OP_EQUAL; break;
		case AGK_DEPTH_FUNC_LEQUAL: depthTestInfo.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL; break;
		case AGK_DEPTH_FUNC_GREATER: depthTestInfo.depthCompareOp = VK_COMPARE_OP_GREATER; break;
		case AGK_DEPTH_FUNC_NOTEQUAL: depthTestInfo.depthCompareOp = VK_COMPARE_OP_NOT_EQUAL; break;
		case AGK_DEPTH_FUNC_GEQUAL: depthTestInfo.depthCompareOp = VK_COMPARE_OP_GREATER_OR_EQUAL; break;
		case AGK_DEPTH_FUNC_ALWAYS: depthTestInfo.depthCompareOp = VK_COMPARE_OP_ALWAYS; break;
	}
	depthTestInfo.depthWriteEnable = pPipeline->m_renderState.GetDepthWriteEnable();
	depthTestInfo.stencilTestEnable = VK_FALSE;
	depthTestInfo.front = {}; // stencil
	depthTestInfo.back = {}; // stencil

	// blending options
	VkPipelineColorBlendAttachmentState colorBlendInfo = {};
	colorBlendInfo.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendInfo.blendEnable = pPipeline->m_renderState.GetBlendEnable();
	switch( pPipeline->m_renderState.GetBlendColorSrc() )
	{
		case AGK_BLEND_ZERO:				colorBlendInfo.srcColorBlendFactor = VK_BLEND_FACTOR_ZERO; break;
		case AGK_BLEND_ONE:					colorBlendInfo.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; break;
		case AGK_BLEND_SRC_ALPHA:			colorBlendInfo.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA; break;
		case AGK_BLEND_ONE_MINUS_SRC_ALPHA: colorBlendInfo.srcColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA; break;
		case AGK_BLEND_DST_ALPHA:			colorBlendInfo.srcColorBlendFactor = VK_BLEND_FACTOR_DST_ALPHA; break;
		case AGK_BLEND_ONE_MINUS_DST_ALPHA: colorBlendInfo.srcColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA; break;
		//case AGK_BLEND_SRC_COLOR:			  // illegal
		//case AGK_BLEND_ONE_MINUS_SRC_COLOR: // illegal
		case AGK_BLEND_DST_COLOR:			colorBlendInfo.srcColorBlendFactor = VK_BLEND_FACTOR_DST_COLOR; break;
		case AGK_BLEND_ONE_MINUS_DST_COLOR: colorBlendInfo.srcColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR; break;
		case AGK_BLEND_ALPHA_SATURATE:		colorBlendInfo.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA_SATURATE; break;
	}
	switch( pPipeline->m_renderState.GetBlendColorDst() )
	{
		case AGK_BLEND_ZERO:				colorBlendInfo.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; break;
		case AGK_BLEND_ONE:					colorBlendInfo.dstColorBlendFactor = VK_BLEND_FACTOR_ONE; break;
		case AGK_BLEND_SRC_ALPHA:			colorBlendInfo.dstColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA; break;
		case AGK_BLEND_ONE_MINUS_SRC_ALPHA: colorBlendInfo.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA; break;
		case AGK_BLEND_DST_ALPHA:			colorBlendInfo.dstColorBlendFactor = VK_BLEND_FACTOR_DST_ALPHA; break;
		case AGK_BLEND_ONE_MINUS_DST_ALPHA: colorBlendInfo.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA; break;
		case AGK_BLEND_SRC_COLOR:			colorBlendInfo.dstColorBlendFactor = VK_BLEND_FACTOR_SRC_COLOR; break;
		case AGK_BLEND_ONE_MINUS_SRC_COLOR:	colorBlendInfo.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR; break;
		//case AGK_BLEND_DST_COLOR:				// illegal
		//case AGK_BLEND_ONE_MINUS_DST_COLOR:	// illegal
		//case AGK_BLEND_ALPHA_SATURATE:		// illegal
	}
	colorBlendInfo.colorBlendOp = VK_BLEND_OP_ADD;
	colorBlendInfo.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
	colorBlendInfo.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendInfo.alphaBlendOp = VK_BLEND_OP_ADD;

	// blending options 2
	VkPipelineColorBlendStateCreateInfo colorBlend2Info = {};
	colorBlend2Info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlend2Info.logicOpEnable = VK_FALSE;
	colorBlend2Info.logicOp = VK_LOGIC_OP_COPY;
	colorBlend2Info.attachmentCount = 1;
	colorBlend2Info.pAttachments = &colorBlendInfo;

	// Things we want to define later
	/*
	VkDynamicState dynamicStates[] = { VK_DYNAMIC_STATE_VIEWPORT };
	VkPipelineDynamicStateCreateInfo dynamicState = {};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = 1;
	dynamicState.pDynamicStates = dynamicStates;
	*/
	
	// create the pipeline
	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = pipelineShaderInfo;
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssemblyInfo;
	pipelineInfo.pViewportState = &viewportStateInfo;
	pipelineInfo.pRasterizationState = &rasterizerInfo;
	pipelineInfo.pMultisampleState = &multisamplingInfo;
	pipelineInfo.pDepthStencilState = &depthTestInfo;
	pipelineInfo.pColorBlendState = &colorBlend2Info;
	pipelineInfo.pDynamicState = 0;//&dynamicState;
	pipelineInfo.layout = pVulkanShader->m_vkPipelineLayout;
	pipelineInfo.renderPass = m_vkRenderPasses[ pPipeline->m_iRenderPassIndex ]; // pipeline can be used with other render passes but they must be compatible
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = 0; // use not recommended by hardware vendors
	pipelineInfo.basePipelineIndex = -1;

	VkResult res = vkCreateGraphicsPipelines( m_vkDevice, 0, 1, &pipelineInfo, nullptr, &pPipeline->m_vkPipeline );

	delete [] attributeDescriptions;

	if ( res != VK_SUCCESS) ReportAndReturnErrorID( APP_ERROR_VK_CREATE_PIPELINE_FAILED, res );

	return APP_SUCCESS;
}

#endif // DISABLE_VULKAN
