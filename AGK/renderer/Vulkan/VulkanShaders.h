#ifndef _H_AGK_VULKAN_SHADERS_
#define _H_AGK_VULKAN_SHADERS_

#include "VulkanFunctions.h"
#include "vk_mem_alloc.h"
#include "../Renderer.h"

// internal flags
#define AGK_VK_SHADER_DELETE			0x01
#define AGK_VK_SHADER_HAS_CONSTANTS		0x02
#define AGK_VK_SHADER_NO_CONSTANTS		0x04

#define AGK_VK_SHADER_CONSTANTS_DELETE	0x01

#define AGK_VK_SHADER_DATA_ANY_IN_USE		0x0001
#define AGK_VK_SHADER_DATA_AWAITING_USE		0x0002
#define AGK_VK_SHADER_DATA_IN_USE			0x0004
#define AGK_VK_SHADER_DATA_CAN_MAP			0x0008

#define AGK_VK_SHADER_TYPE_UNKNOWN			0
#define AGK_VK_SHADER_TYPE_NO_LAYOUTS		1
#define AGK_VK_SHADER_TYPE_USES_LAYOUTS		2
#define AGK_VK_SHADER_TYPE_SPIRV			3

#define AGK_VK_DESCRIPTOR_LOCATION_IS_SET			0x01
#define AGK_VK_DESCRIPTOR_LOCATION_USES_LOCAL_MEM	0x02
#define AGK_VK_DESCRIPTOR_LOCATION_RELOAD			0x04

#define AGK_VK_DESCRIPTOR_BINDING_VERTEX	0x0001
#define AGK_VK_DESCRIPTOR_BINDING_FRAGMENT	0x0002

#define AGK_SHADER_DESCRIPTORS_PER_POOL		16

namespace AGK
{
	class VulkanDescriptorSetLocation
	{
		public:
			AGKShaderConstantValue *m_pCurrValue = 0;
			uint32_t m_iChangeVersion = 0;
			uint8_t m_iFlags = 0;

			VulkanDescriptorSetLocation() {}
			~VulkanDescriptorSetLocation() { if ( m_iFlags & AGK_VK_DESCRIPTOR_LOCATION_USES_LOCAL_MEM ) delete m_pCurrValue; }

			void Set() { m_iFlags |= AGK_VK_DESCRIPTOR_LOCATION_IS_SET; }
			void UnSet() { m_iFlags &= ~AGK_VK_DESCRIPTOR_LOCATION_IS_SET; }
			bool IsSet() { return (m_iFlags & AGK_VK_DESCRIPTOR_LOCATION_IS_SET) != 0; }

			void Reload() { m_iFlags |= AGK_VK_DESCRIPTOR_LOCATION_RELOAD; }
			void Reloaded() { m_iFlags &= ~AGK_VK_DESCRIPTOR_LOCATION_RELOAD; }
			bool ShouldReload() { return (m_iFlags & AGK_VK_DESCRIPTOR_LOCATION_RELOAD) != 0; }

			void Reset() 
			{ 
				if ( m_iFlags & AGK_VK_DESCRIPTOR_LOCATION_USES_LOCAL_MEM ) delete m_pCurrValue;
				m_pCurrValue = 0; 
				m_iChangeVersion = 0; 
				m_iFlags = 0; 
			}
			void SetCurrValue( AGKShaderConstantValue *pNewValue )
			{
				if ( pNewValue == m_pCurrValue ) return;
				if ( m_iFlags & AGK_VK_DESCRIPTOR_LOCATION_USES_LOCAL_MEM ) delete m_pCurrValue;
				m_pCurrValue = pNewValue;
				Reload();
			}
			AGKShaderConstantValue* GetLocalValue()
			{
				if ( m_iFlags & AGK_VK_DESCRIPTOR_LOCATION_USES_LOCAL_MEM ) return m_pCurrValue;
				else 
				{
					m_pCurrValue = new AGKShaderConstantValue();
					m_iFlags |= AGK_VK_DESCRIPTOR_LOCATION_USES_LOCAL_MEM;
					return m_pCurrValue;
				}
			}
	};

	class VulkanDescriptorSetLocationStatic // individual constant
	{
		public:
			VulkanDescriptorSetLocationStatic() {}
			~VulkanDescriptorSetLocationStatic() {}

			AGKShaderConstantValue *m_pDefaultValue = 0;
			uint16_t m_iOffset = 0;
			uint16_t m_iArrayStride = 0;
			uint16_t m_iArraySize = 0;
			uint8_t m_iStride = 0;
			uint8_t m_iVarType = AGK_SHADER_CONSTANT_TYPE_UNKNOWN;
	};

	class VulkanDescriptorSetBindingStatic // group of constants
	{
		public:
			VulkanDescriptorSetBindingStatic() {}
			~VulkanDescriptorSetBindingStatic() {}

			uint16_t m_iOffset = 0;
			uint16_t m_iSize = 0;
			uint16_t m_iBindingID = 0;
			uint16_t m_iFlags = 0;
	};

	class VulkanDescriptorSetData;

	class VulkanDescriptorSetStatic
	{
		public:
			VulkanDescriptorSetStatic() {}
			~VulkanDescriptorSetStatic() 
			{ 
				if ( m_pLocations ) delete [] m_pLocations; 
				if ( m_pUniformBindings ) delete [] m_pUniformBindings;
				if ( m_pTextureBindings ) delete [] m_pTextureBindings;
			}
			
			AGKUnorderedArray<VkDescriptorPool> m_pDescriptorPools; // pools used to create new dynamic data for this set

			AGKStackArray<VulkanDescriptorSetData*> m_pInactiveDescriptorData; // set data that has been discarded and can be resused
			VulkanDescriptorSetLocationStatic *m_pLocations = 0; // individual constants
			VulkanDescriptorSetBindingStatic *m_pUniformBindings = 0; // groups of constants
			uint16_t *m_pTextureBindings = 0;
			VkDescriptorSetLayout m_layout = 0;
			uint16_t m_iNumLocations = 0;
			uint16_t m_iNumUniformBindings = 0;
			uint16_t m_iBufferSize = 0;
			uint8_t m_iNumTextures = 0;
			uint8_t m_iCurrPoolNumSets = 0;
			uint8_t m_iSetID = 0;
	};

	class VulkanDescriptorSetData
	{
		public:
			VulkanDescriptorSetLocation* m_pLocations = 0;
			uint64_t *m_pCurrentTextures = 0;
			
			VkDescriptorSet m_vkDescriptorSet = 0;
			VkBuffer m_vkUniformBuffer = 0;
			VmaAllocation m_vkBufferMemory = 0;
			void *m_pMappedData = 0;
			uint16_t m_iFlags = 0;
			uint16_t m_iNumLocations = 0; // duplicate of the descriptor set static value
			uint8_t m_iNumTextures = 0; // duplicate of the descriptor set static value

			VulkanDescriptorSetData( VulkanDescriptorSetStatic *pParent ) 
			{ 
				m_iNumTextures = pParent->m_iNumTextures; 
				m_iNumLocations = pParent->m_iNumLocations; 
				m_pLocations = new VulkanDescriptorSetLocation[ m_iNumLocations ];
				m_pCurrentTextures = new uint64_t[ m_iNumTextures ];
				memset( m_pCurrentTextures, 0, sizeof(uint64_t) * m_iNumTextures );
			}
			~VulkanDescriptorSetData() 
			{ 
				if ( m_pLocations ) delete [] m_pLocations; 
				if ( m_pCurrentTextures ) delete [] m_pCurrentTextures;
			}

			void Reset() 
			{ 
				if ( m_pLocations ) { for( uint32_t i = 0; i < m_iNumLocations; i++ ) m_pLocations[i].Reset(); }
				m_iFlags = 0;
			}

			// as soon as the data is used to draw then it can no longer be changed, future changes will need to create a new VulkanDescriptorSetData object
			void DidBind() { m_iFlags |= AGK_VK_SHADER_DATA_AWAITING_USE; } 
			bool IsAwaitingUse() { return (m_iFlags & AGK_VK_SHADER_DATA_AWAITING_USE) != 0; }
			bool IsInUse() { return (m_iFlags & AGK_VK_SHADER_DATA_IN_USE) != 0; }
			bool IsMapped() { return (m_iFlags & AGK_VK_SHADER_DATA_CAN_MAP) != 0; }
	};

	class VulkanDescriptorSet
	{
		public:
			AGKUnorderedArray<VulkanDescriptorSetData*> m_pActiveDescriptorData; // set data objects currently or recently bound
			AGKStackArray<VulkanDescriptorSetData*> m_pInactiveDescriptorData; // set data not currently bound
			VulkanDescriptorSetData *pCurrData = 0;

			VulkanDescriptorSet() {}
			~VulkanDescriptorSet() {} // set data should be handed off to the shader before deletion
	};

	class VulkanShader;

	class VulkanShaderConstants
	{
		public:
			// global
			static VulkanShaderConstants *g_pAllShaderConstants;
			VulkanShaderConstants *m_pNextConstantObject = 0;
			VulkanShaderConstants *m_pPrevConstantObject = 0;
			void RemoveConstantObject();
			void AddConstantObject();

			// local
			VulkanShader *m_pShader = 0;
			uint64_t m_iShaderRef = 0;
			
			VulkanDescriptorSet *m_pDescriptorSets = 0;
			uint8_t m_iNumDescriptorSets = 0;
			uint8_t m_iFlags = 0;

			// functions
			VulkanShaderConstants() { AddConstantObject(); }
			~VulkanShaderConstants() { if ( m_pDescriptorSets ) delete [] m_pDescriptorSets; RemoveConstantObject(); }

			bool ShouldDelete() { return (m_iFlags & AGK_VK_SHADER_CONSTANTS_DELETE) != 0; }
	};

	class VulkanShader
	{
		public:
			// global
			static VulkanShader *g_pAllShaderObjects;
			VulkanShader *m_pNextShader = 0;
			VulkanShader *m_pPrevShader = 0;
			void RemoveShaderObject();
			void AddShaderObject();

			// local
			uint32_t* m_pVSSourceSPIRV = 0;
			uint32_t* m_pPSSourceSPIRV = 0;
			uint32_t m_iVSSourceSize = 0;
			uint32_t m_iPSSourceSize = 0;

			VkShaderModule m_vkVertShader = 0;
			VkShaderModule m_vkFragShader = 0;
			VkPipelineLayout m_vkPipelineLayout = 0;

			uint16_t m_indexByKnownName[ AGK_SHADER_CONSTANT_NAME_MAX_VALUE ]; // [15-12]=set, [11-0]=location index
			uint16_t m_indexByTexStage[ AGK_MAX_TEXTURE_STAGES ]; // [15-12]=set, [11-0]=index

			// shader only stores the static data that describes the descriptor sets
			// VulkanShaderConstants holds the dynamic data, each mesh can have its own version of the dynamic data
			VulkanDescriptorSetStatic *m_pDescriptorSetStatic = 0;

			uint8_t m_iNumDescriptorSets = 0;
			uint8_t m_iTotalTextures = 0; // number of textures across all sets, represents the maximum number of updates that can occur in BindShaderConstants
			uint8_t m_iFlags = 0;
			
			// functions
			VulkanShader() { AddShaderObject(); }
			~VulkanShader() 
			{ 
				RemoveShaderObject(); 
				if ( m_pVSSourceSPIRV ) delete [] m_pVSSourceSPIRV;
				if ( m_pPSSourceSPIRV ) delete [] m_pPSSourceSPIRV;
				if ( m_pDescriptorSetStatic ) delete [] m_pDescriptorSetStatic;
			}

			bool ShouldDelete() { return (m_iFlags & AGK_VK_SHADER_DELETE) != 0; }
			void SetHasConstants() { m_iFlags |= AGK_VK_SHADER_HAS_CONSTANTS; }
			void ClearHasConstants() { m_iFlags &= ~AGK_VK_SHADER_HAS_CONSTANTS; }
			bool HasConstants() { return (m_iFlags & AGK_VK_SHADER_HAS_CONSTANTS) != 0; }
			bool NoConstants() { return (m_iFlags & AGK_VK_SHADER_NO_CONSTANTS) != 0; }
	};

}


#endif
