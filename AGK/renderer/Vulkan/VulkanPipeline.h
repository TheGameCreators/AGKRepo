#ifndef _H_AGK_VULKAN_PIPELINE_
#define _H_AGK_VULKAN_PIPELINE_

#include "cMesh.h"

#define AGK_VK_PIPELINE_AWAITING_USE	0x01
#define AGK_VK_PIPELINE_IN_USE			0x02
#define AGK_VK_PIPELINE_DELETE			0x04

namespace AGK
{
	class VulkanPipeline;
	
	class VulkanPipelineCache
	{
		protected:
			VulkanPipeline **m_pPipelines = 0;
			uint32_t m_iNumPipelines = 0;
			uint32_t m_iPipelinesArraySize = 0;
			cHashedList<VulkanPipeline*> m_cAllPipelines;

		public:
			VulkanPipelineCache( uint32_t hashSize=1024 ) : m_cAllPipelines(hashSize) {}
			~VulkanPipelineCache() { if ( m_pPipelines ) delete [] m_pPipelines; } // don't delete the pipeline resources in case they are used elsewhere

			bool IsValid( const VulkanPipeline *pPipeline ) const { return m_cAllPipelines.GetItem( pPipeline ) != 0; }
			void DeleteAll(); // deletes all pipelines and their resources
			void DeleteScreenPipelines(); // deletes all pipelines and their resources

			void DeleteShader( AGKShader *pShader );
			VulkanPipeline* GetPipeline( VulkanPipeline* pPipelineTemplate ) const;
			VulkanPipeline* AddPipeline( const VulkanPipeline* pPipelineTemplate );

			void CleanUpPipelines();
	};


	class VulkanPipeline 
	{
		public:
			AGKShader *m_pShader = 0;
			union
			{
				struct
				{
					uint16_t m_iViewportX;
					uint16_t m_iViewportY;
					uint16_t m_iViewportWidth;
					uint16_t m_iViewportHeight;
				};
				uint64_t m_iViewportHash = 0;
			};
			AGKVertexLayout m_vertexLayout;
			AGKRenderState m_renderState;
			uint8_t m_iRenderPassIndex = 0;

			uint8_t m_iFlags = 0;
			uint32_t m_iRefCount = 0;

			VkPipeline m_vkPipeline = 0;

			VulkanPipeline() {}
			~VulkanPipeline() {}

			void Delete() { m_iFlags |= AGK_VK_PIPELINE_DELETE; }
			void DidBind() { m_iFlags |= AGK_VK_PIPELINE_AWAITING_USE; }
			bool ShouldDelete() { return (m_iFlags & AGK_VK_PIPELINE_DELETE) != 0; }
			bool IsInUse() { return (m_iFlags & (AGK_VK_PIPELINE_AWAITING_USE | AGK_VK_PIPELINE_IN_USE)) != 0; }

			void Clone( const VulkanPipeline *pOther )
			{
				m_pShader = pOther->m_pShader;
				m_iViewportHash = pOther->m_iViewportHash;
				m_vertexLayout.Clone( &pOther->m_vertexLayout );
				m_renderState = pOther->m_renderState;
				m_iRenderPassIndex = pOther->m_iRenderPassIndex;
				m_iRefCount = pOther->m_iRefCount;
				m_vkPipeline = pOther->m_vkPipeline;
			}

			int Compare( VulkanPipeline *pOther )
			{
				if ( m_pShader->GetCreated() > pOther->m_pShader->GetCreated() ) return 1;
				else if ( m_pShader->GetCreated() < pOther->m_pShader->GetCreated() ) return -1;

				int result = m_renderState.Compare( &pOther->m_renderState );
				if ( result != 0 ) return result;

				if ( m_iRenderPassIndex > pOther->m_iRenderPassIndex ) return 1;
				else if ( m_iRenderPassIndex < pOther->m_iRenderPassIndex ) return -1;
				
				result = m_vertexLayout.Compare( &pOther->m_vertexLayout );
				if ( result != 0 ) return result;

				if ( m_iViewportHash > pOther->m_iViewportHash ) return 1;
				else if ( m_iViewportHash < pOther->m_iViewportHash ) return -1;

				return 0;
			}
	};
}

#endif