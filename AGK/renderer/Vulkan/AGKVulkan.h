#ifndef _H_AGK_VULKAN_
#define _H_AGK_VULKAN_

#include "PlatformDefines.h"
#include <vector>

// define AGK_VK_SINGLE_QUEUE to use a single queue for both transfers and graphics, MoltenVK needs this for MTLFence to work correctly
#if defined(AGK_MACOS) || defined(AGK_IOS)
	#define AGK_VK_SINGLE_QUEUE
#endif

#define AGK_BUFFER_VERTEX VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
#define AGK_BUFFER_INDEX VK_BUFFER_USAGE_INDEX_BUFFER_BIT
#define AGK_BUFFER_UNIFORM VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT
#define AGK_BUFFER_TRANSFER_SRC VK_BUFFER_USAGE_TRANSFER_SRC_BIT
#define AGK_BUFFER_TRANSFER_DST VK_BUFFER_USAGE_TRANSFER_DST_BIT

#define AGK_MEM_STAGING (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
#define AGK_MEM_DEVICE VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT

#include "VulkanFunctions.h"

#define VMA_RECORDING_ENABLED 0
#define VMA_STATIC_VULKAN_FUNCTIONS 0
#define VMA_STATS_STRING_ENABLED 0
#include "vk_mem_alloc.h"

#include "Renderer.h"

#include "VulkanImage.h"
#include "VulkanVertices.h"
#include "VulkanShaders.h"
#include "VulkanFrameBuffer.h"
#include "VulkanPipeline.h"

#define AGK_VULKAN_SURFACE_CLEARED				0x0001
#define AGK_VULKAN_RENDER_PASS_STARTED			0x0002
#define AGK_VULKAN_SURFACE_VALID				0x0004
#define AGK_VULKAN_WAIT_ON_ACQUIRE_SEMAPHORE	0x0008

#define AGK_VK_RENDER_PASS_SURFACE						0
#define AGK_VK_RENDER_PASS_SURFACE_NO_CLEAR				1
#define AGK_VK_RENDER_PASS_RGBA_DEPTH					2
#define AGK_VK_RENDER_PASS_RGBA_DEPTH_NO_CLEAR			3
#define AGK_VK_RENDER_PASS_RGBA_DEPTH_STORE				4
#define AGK_VK_RENDER_PASS_RGBA_DEPTH_STORE_NO_CLEAR	5
#define AGK_VK_RENDER_PASS_RGBA							6
#define AGK_VK_RENDER_PASS_RGBA_NO_CLEAR				7
#define AGK_VK_RENDER_PASS_DEPTH						8	// will always store depth, will always clear depth
#define AGK_VK_RENDER_PASS_NUM							9

#define AGK_VK_RENDER_PASS_FLAG_STORE_DEPTH		0x01
#define AGK_VK_RENDER_PASS_FLAG_NO_CLEAR		0x02

#define AGK_VK_NUM_FRAMES	2 // recommend keeping this at 2, images/descriptor sets may be reused too soon at higher values

namespace AGK
{
	class VulkanRenderer : public Renderer
	{
		protected:
			static VulkanRenderer* g_pInstance;

			// state
			uint32_t m_iFlags = AGK_VULKAN_SURFACE_CLEARED;

			uint16_t m_iGlobalScissorX = 0;
			uint16_t m_iGlobalScissorY = 0;
			uint16_t m_iGlobalScissorWidth = 0;
			uint16_t m_iGlobalScissorHeight = 0;

			uint16_t m_iGlobalViewportX = 0;
			uint16_t m_iGlobalViewportY = 0;
			uint16_t m_iGlobalViewportWidth = 0;
			uint16_t m_iGlobalViewportHeight = 0;

			float m_fClearRed = 0;
			float m_fClearGreen = 0;
			float m_fClearBlue = 0;
			float m_fClearAlpha = 0;

			int m_iPresentMode = 1; // 0=delayed, 1=immediate, 2=manual
			int m_iAntialiasMode = 0;

			// transfer data
			int m_iImagesWaitForPrevFrame = 0;
			int m_iVerticesWaitForPrevFrame = 0;
			int m_iFrameHasGraphicsVertexTransfers = 0;
			int m_iFrameHasGraphicsImageTransfers = 0;
			AGKUnorderedArray<VulkanImage*> m_pImagesWithTransfers;
			AGKUnorderedArray<VulkanVertices*> m_pVerticesWithTransfers;
			AGKUnorderedArray<VulkanImage*> m_pVRImages;

			// internal resources
			VulkanPipelineCache m_globalPipelineCache;
			VulkanImage *m_pNullImage2D = 0;

			std::vector<const char*> m_szDeviceExtensions;
			std::vector<const char*> m_szInstanceExtensions;

			VkCommandPool m_vkGraphicsCommandPool = 0;
			VkCommandPool m_vkTransferCommandPool = 0;

			VulkanImage *m_pDefaultDepthImage = 0;

			VkRenderPass m_vkRenderPasses[ AGK_VK_RENDER_PASS_NUM ];
			
			uint32_t m_iCurrentFrame = 0;
			int m_iPrevFrame = -1;
			
			VkSwapchainKHR m_vkSwapChain = 0;
			uint32_t m_iNumSwapImages = 0;
			VkImage* m_vkSwapImages = 0;
			VkImageView* m_vkSwapImageViews = 0;
			VkFramebuffer* m_vkSwapFrameBuffers = 0;
			int m_iSurfaceBytesPerPixel = 0;
			int m_iCurrSwapImage = -1;
			int m_iPrevSwapImage = -1;

			VkFence m_vkFenceFrameFinished[ AGK_VK_NUM_FRAMES ];
			VkFence m_vkFenceGetImageFinished;
			VkSemaphore m_vkSemaphoreTransferredVertices[ AGK_VK_NUM_FRAMES ];
			VkSemaphore m_vkSemaphoreTransferredImages[ AGK_VK_NUM_FRAMES ];
			VkSemaphore m_vkSemaphoreReceivedVertices[ AGK_VK_NUM_FRAMES ];
			VkSemaphore m_vkSemaphoreReceivedImages[ AGK_VK_NUM_FRAMES ];
			VkSemaphore m_vkSemaphoreAcquired[ AGK_VK_NUM_FRAMES ];
			VkSemaphore m_vkSemaphoreSubmitted[ AGK_VK_NUM_FRAMES ];

			VkCommandBuffer m_vkTransferVerticesCommandBuffer[ AGK_VK_NUM_FRAMES ];
			VkCommandBuffer m_vkTransferImagesCommandBuffer[ AGK_VK_NUM_FRAMES ];
			VkCommandBuffer m_vkReceiveVerticesCommandBuffer[ AGK_VK_NUM_FRAMES ];
			VkCommandBuffer m_vkReceiveImagesCommandBuffer[ AGK_VK_NUM_FRAMES ];
			VkCommandBuffer m_vkCommandBuffer[ AGK_VK_NUM_FRAMES ];
			VkCommandBuffer m_vkGetImageCommandBuffer;

			uint32_t m_iCurrPrimitiveMode = AGK_PRIMITIVE_TRIANGLES;

			// device resources
			VmaAllocator m_vkAllocator = 0;
			bool bDedicatedAllocationSupported = false;
			VmaPool m_vmaBufferTransferPool;
			VmaPool m_vmaVertexBufferPool;

			uint32_t m_iInstanceVersion = VK_MAKE_VERSION( 1,0,0 );
			uint32_t m_iDeviceVersion = VK_MAKE_VERSION( 1,0,0 );
		
			uint32_t m_iInstanceExtensionCount = 0;
			VkExtensionProperties* m_pInstanceExtensions = 0;
			VkInstance m_vkInstance = 0;
			uint32_t m_iProperties2Extension = 0;
		
			uint32_t m_iDeviceExtensionCount = 0;
			VkExtensionProperties* m_pDeviceExtensions = 0;
			VkDevice m_vkDevice = 0;

			VkPhysicalDevice m_vkGPU = 0;
			VkPhysicalDeviceMemoryProperties m_vkGPUMemoryProps = {};
#ifdef VK_EXT_memory_budget
			VkPhysicalDeviceMemoryBudgetPropertiesEXT m_vkGPUMemoryBudget = {};
#endif

			VkPhysicalDeviceFeatures m_vkSupportedFeatures;
			int m_iQueueFamilyIndex = -1;
			int m_iTransferQueueFamilyIndex = -1;
			VkPhysicalDeviceProperties m_vkGPUProperties = {};
	
			VkQueue m_vkQueue = 0;
			VkQueue m_vkTransferQueue = 0;
	
			VkSurfaceKHR m_vkSurface = 0;
			VkSurfaceFormatKHR m_vkSurfaceFormat = {};
			VkPresentModeKHR m_iCurrentPresentMode = VK_PRESENT_MODE_FIFO_KHR;
			int m_iCurrVSyncMode = 1;
			int m_iNewVSyncMode = 1;
			int m_iSurfaceResized = 0;
			int m_iSurfaceOptimalWarning = 0;
	
			VkDebugReportCallbackEXT pDebugCallback = 0;
			bool m_bEnableValidationLayers = false;
	
		public:
			VulkanRenderer();
			~VulkanRenderer();
			static VulkanRenderer* GetInstance() { return g_pInstance; }

			AGKRenderer GetIdentifier() { return AGK_RENDERER_VULKAN; }
			const char* GetName() { return "Vulkan"; }

			int SupportsGLSL450() { return 1; }
			int SupportsSPIRV() { return 1; }

			void AddDeviceExtensions( AGKUnorderedArray<uString*>* pExtensions );
			void AddInstanceExtensions( AGKUnorderedArray<uString*>* pExtensions );

			int Init();
			int SetupWindow( void* param1, void* param2, unsigned int width, unsigned int height );
			int WindowResized( void* param1, void* param2, unsigned int width, unsigned int height );
			int SetScreenRecordParams( void* param1, void* param2 );
			int Setup();
			int Swap();
			void SetPresentMode( uint32_t mode );
			int ForcePresent();
			void UpdateSurfaceSize() { m_iSurfaceResized = 1; }
			void CleanUp();
			void ResetInternalState() {}
			void* GetDevicePtr() { return m_vkGPU; }
			void LosingSurface();
			void LosingFocus() {}
		
			// core
			void SetVSync( int mode );
			void SetGlobalScissor( uint16_t x, uint16_t y, uint16_t width, uint16_t height ) { m_iGlobalScissorX = x; m_iGlobalScissorY = y; m_iGlobalScissorWidth = width; m_iGlobalScissorHeight = height; }
			void SetViewport( uint16_t x, uint16_t y, uint16_t width, uint16_t height ) { m_iGlobalViewportX = x; m_iGlobalViewportY = y; m_iGlobalViewportWidth = width; m_iGlobalViewportHeight = height; }
			void ClearRenderTarget( float red, float green, float blue, float alpha );
			void ClearDepthBuffer();
			void SetAntialiasMode( int mode ) { m_iAntialiasMode = mode; }
			void GetDataFromScreen( uint32_t **pData, int x, int y, int width, int height );
			int GetRenderPassStarted() { return (m_iFlags & AGK_VULKAN_RENDER_PASS_STARTED) ? 1 : 0; }

			// functions defined in VulkanImage.cpp
			bool IsImageFormatSupported( AGKImgFormat format );
			int CreateImage2D( AGKImgFormat format, uint32_t width, uint32_t height, unsigned char *data, uint32_t flags, void **outResource ); 
			int UpdateSubImage2D( void *pResource, uint32_t mipmapLevel, uint32_t x, uint32_t y, uint32_t width, uint32_t height, unsigned char *data );
			int CreateDepthBuffer( AGKImgFormat format, uint32_t width, uint32_t height, uint32_t flags, void **outResource );
			int DeleteImage( void *inResource, int unbind ); // removes from global list and adds it to a queue to delete later
			int SetImageSampleSettings( void *pResource, uint32_t flags );
			void SetImageMagFilter( void *pResource, uint32_t mode );
			void SetImageMinFilter( void *pResource, uint32_t mode );
			void SetImageWrapU( void *pResource, uint32_t mode );
			void SetImageWrapV( void *pResource, uint32_t mode );
			int GetImageData( void **pResource, uint8_t **pData );
			void* GetVRImage( void *pResource );
			void SetVRImage( void *pResource, int mode );
			void ResetVRImages();
			uint32_t GetImageInternalID( void *pResource ) { return 0; }
			void ClearImageBindings() {}
			int OverrideTexture( uint32_t width, uint32_t height, AGKImgFormat format, void* texture, void **outResource );
		
			// vulkan specific
			int CreateSampler( VkSampler *pSampler, uint32_t flags );
			void DeleteSampler( VkSampler sampler );
			static VulkanSamplerPool g_pSamplers;

			// functions defined in VulkanVertices.cpp
			int CreateVertices( uint32_t vertexSize, void **outResource );
			int UpdateVertices( void *pResource, float *pVertexData, uint32_t offset, uint32_t numVertices );
			int UpdateIndices( void *pResource, void *pIndexData, uint32_t offset, uint32_t numIndices ); // pIndexData must be unsigned short if original indices were unsigned short
			int SetVertices( void *pResource, float *pVertexData, uint32_t numVertices );
			int SetIndices( void *pResource, uint16_t *pIndexData, uint32_t numIndices );
			int SetIndicesInt( void *pResource, uint32_t *pIndexData, uint32_t numIndices );
			int DeleteVertices( void *pResource );
		
			int CreateFrameBuffer( void *pColorImage, void *pDepthImage, int iForceDepth, void **outResource );
			int DeleteFrameBuffer( void *pResource );
			void SetFrameBufferIsShadow( void *pResource );
			void StartShadowFrameBuffer( void *pResource );
			void StartShadowFrameBuffers( void *pResource1, void *pResource2, void *pResource3, void *pResource4 );
			void CompleteShadowFrameBuffer( void *pResource );
			void CompleteShadowFrameBuffers( void *pResource1, void *pResource2, void *pResource3, void *pResource4 );
			int BindFrameBuffer( void *pCommandBuffer, void *pResource );

			const char* GetShaderDefaultFullScreenVS();
			const char* GetShaderDefaultSpriteVS();

			AGKShader* LoadDefaultShaderColor();
			AGKShader* LoadDefaultShaderTexColor();
			AGKShader* LoadDefaultShaderFont();
			AGKShader* LoadDefaultShaderShadow();
			AGKShader* LoadDefaultShaderShadowAlpha();
			AGKShader* LoadDefaultShaderShadowBone( uint32_t numBones );
			AGKShader* LoadDefaultShaderShadowBoneAlpha( uint32_t numBones );
			AGKShader* LoadDefaultShader3DParticlesTex();
			AGKShader* LoadDefaultShader3DParticlesColor();
			AGKShader* LoadDefaultShaderVideo();
			AGKShader* LoadDefaultShaderDeviceCamera();
			AGKShader* LoadDefaultShaderARAndroid();
			AGKShader* LoadDefaultShaderARiOS();
			AGKShader* LoadDefaultShaderQuad();
			AGKShader* LoadDefaultShaderSkyBoxSun();
			AGKShader* LoadDefaultShaderSkyBoxNoSun();

			AGKShader* Make3DShader( cMesh *pMesh );
			AGKShader* MakeFinalShader( AGKShader *pBaseShader, int sunActive, int VSLights, int PSLights, int useShadows, int normalMap );
			AGKShader* LoadShader( const char *szVertexSource, const char *szPixelSource, const char *szVertexFile, const char *szPixelFile );
			AGKShader* LoadShaderSPIRV( uint32_t *pVertexSource, uint32_t sizeVS, uint32_t *pPixelSource, uint32_t sizePS );
			void ReloadShader( AGKShader *pShader );
			void DeleteShader( AGKShader *pShader );

			void SetShaderConstant( AGKShader *pShader, uint32_t index, AGKShaderConstantValue *pConstant );
			int BindShaderConstants( void *pCommandBuffer, AGKShader *pShader, AGKUnorderedArray<AGKShaderConstantValue*> *pConstants, AGKUnorderedArray<AGKShaderConstantValue*> *pAdditionalConstants, 
										AGKDynamicArray<cImage*> *pTextures, void **inoutConstants );
			int BindShaderConstantUpdates( void *pCommandBuffer, AGKShader *pShader, AGKUnorderedArray<AGKShaderConstantValue*> *pConstants, AGKDynamicArray<cImage*> *pTextures, void **inoutConstants );
			int DeleteShaderConstants( void *pConstants );

			// OpenGL does not use the pCommandBuffer or inoutPipeline parameters
			int BindPipeline( void *pCommandBuffer, AGKShader *pShader, const AGKRenderState *pRenderState, const AGKVertexLayout *pVertexLayout, void **inoutPipeline );
			int DeletePipeline( void *pPipeline );
			void ForceDeletePipeline( VulkanPipeline *pPipeline );

			int DrawVertices( void *pCommandBuffer, void *pVertices, int count=-1 );

		protected:

			static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback( VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType,    
																 uint64_t obj, size_t location, int32_t code, const char* layerPrefix, 
																 const char* msg, void* userData );

			// internal image functions
			int InternalCreateImage2D( VulkanImage *pImage );
			int InternalCreateImageView( VulkanImage *pImage );
			int InternalCreateImageViewFrameBuffer( VulkanImage *pImage );
			int InternalCreateImageTransfer( VulkanImage *pImage, uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t mipLevel, void* data );
			int InternalDeleteImageTransfer( VulkanImageTransfer *pTransfer );
			int InternalTransferImage( VulkanImage *pImage );
			int InternalImageGenerateMipmaps( VulkanImage *pImage, VulkanImageData *pImageData );
			int InternalImageGenerateFramebufferMipmaps( VulkanImage *pImage, VulkanImageData *pImageData );
			VkSampler InternalGetImageSampler( VulkanImage *pImage );
			void InternalImageBarrier( VkCommandBuffer cmdBuffer, VulkanImage *pImage, uint32_t newMode );
			void CleanUpImages();
			void ForceDeleteImage( VulkanImage *pImage );
		
			// internal vertex functions
			int InternalCreateVertexBuffer( VulkanVertices *pVertices, VulkanBufferData *pNewBuffer );
			int InternalCreateIndexBuffer( VulkanVertices *pVertices, VulkanBufferData *pNewBuffer );
			int InternalCreateFullBufferTransfer( VulkanVertices *pVertices, VulkanBufferData *pDst, uint32_t size, void* data );
			int InternalCreatePartialBufferTransfer( VulkanVertices *pVertices, VulkanBufferData *pDst, uint32_t offset, uint32_t size, void* data );
			int InternalDeleteBufferTransfer( VulkanBufferTransfer *pTransfer );
			int InternalTransferVertices( VulkanVertices *pVertices );
			void CleanUpVertices();
			void ForceDeleteVertices( VulkanVertices *pVertices );

			// internal shader functions
			void InternalCompileShader( AGKShader *pAGKShader, int type=0 ); // 0=unknown, 1=GLSL110, 2=GLSL450, 3=SPIRV
			void InternalEnumerateShaderResources( AGKShader *pAGKShader );
			VulkanShaderConstants* CreateShaderConstants( AGKShader *pAGKShader );
			VulkanDescriptorSetData* CreateDescriptorData( VulkanShader *pShader, uint32_t setID );
			void CleanUpShaders();
			void ForceDeleteShader( VulkanShader *pShader );
			void ForceDeleteShaderConstants( VulkanShaderConstants *pConstants );

			// internal frame buffer functions
			int CreateRenderPass( VkFormat colorFormat, VkFormat depthFormat, uint32_t flags, VkRenderPass &renderPass );
			int CheckRenderPass();
			int EndRenderPass();
			void CleanUpFrameBuffers();
			void ForceDeleteFrameBuffer( VulkanFrameBuffer* pFrameBuffers );

			// internal pipeline functions
			int ConstructPipeline( VulkanPipeline *pPipeline );
			
			bool CheckValidationLayerSupport();

			// internal setup functions
			int CreateInstance();
			int GetDevice();
			int InitMemoryAllocator();
			int CreateSurface( void* param1, void* param2 );
			int CreateSwapChain( VkPresentModeKHR presentMode );
			int ResizeSwapChain();
			int ChangePresentMode( VkPresentModeKHR newMode );
			int CreateDefaultRenderPass();
			int CreateDefaultFrameBuffer();
			
			int BeginFrame();
			int EndFrameAndSubmit( uint32_t signalSemaphore=1 );
			int Present();
			void CleanUpResources();
	};
}

#endif // _H_AGK_VULKAN_
