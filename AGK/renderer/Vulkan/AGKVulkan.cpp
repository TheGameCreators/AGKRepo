#include "agk.h"

#ifndef DISABLE_VULKAN

#include "AGKVulkan.h"
//#include <shaderc\shaderc.h>

#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"

//#include <shaderc\shaderc.h>
#include "spirv_reflect.h"

#define AGK_BUFFER_VERTEX VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
#define AGK_BUFFER_INDEX VK_BUFFER_USAGE_INDEX_BUFFER_BIT
#define AGK_BUFFER_UNIFORM VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT
#define AGK_BUFFER_TRANSFER_SRC VK_BUFFER_USAGE_TRANSFER_SRC_BIT
#define AGK_BUFFER_TRANSFER_DST VK_BUFFER_USAGE_TRANSFER_DST_BIT

#define AGK_MEM_STAGING (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
#define AGK_MEM_DEVICE VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT

VulkanRenderer* VulkanRenderer::g_pInstance = 0;

const char* g_szValidationLayers[1] = { "VK_LAYER_LUNARG_standard_validation" };

//#define ARM_LAYER

VulkanRenderer::VulkanRenderer() 
{
	g_pInstance = this;

#ifdef _DEBUG
	m_bEnableValidationLayers = true;
#endif
}

VulkanRenderer::~VulkanRenderer()
{
	if ( m_vkSwapImages ) delete [] m_vkSwapImages;
	if ( m_vkSwapImageViews ) delete [] m_vkSwapImageViews;
	if ( m_vkSwapFrameBuffers ) delete [] m_vkSwapFrameBuffers;

	g_pInstance = 0;
}

VKAPI_ATTR VkBool32 VKAPI_CALL VulkanRenderer::debugCallback( VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType,    
														 uint64_t obj, size_t location, int32_t code, const char* layerPrefix, 
														 const char* msg, void* userData ) 
{
	char level[6] = "-----";
	if ( flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT ) level[0] = 'I';
	if ( flags & VK_DEBUG_REPORT_DEBUG_BIT_EXT ) level[1] = 'D';
	if ( flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT ) level[2] = 'P';
	if ( flags & VK_DEBUG_REPORT_WARNING_BIT_EXT ) level[3] = 'W';
	if ( flags & VK_DEBUG_REPORT_ERROR_BIT_EXT ) level[4] = 'E';
			
	if ( flags & VK_DEBUG_REPORT_ERROR_BIT_EXT ) 
	{
		AppError( "validation message: %s : %s : %s", level, layerPrefix, msg );
		throw 1;
	}
	else
	{
		AppInfo( "validation message: %s : %s : %s", level, layerPrefix, msg );
	}
	return VK_FALSE;
}

bool VulkanRenderer::CheckValidationLayerSupport() 
{
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties( &layerCount, nullptr );

	std::vector<VkLayerProperties> availableLayers( layerCount );
	vkEnumerateInstanceLayerProperties( &layerCount, availableLayers.data() );

	for( const char* layerName : g_szValidationLayers )
	{
		bool layerFound = false;

		for( const VkLayerProperties& layerProperties : availableLayers )
		{
			if ( strcmp(layerName, layerProperties.layerName) == 0 ) 
			{
				layerFound = true;
				break;
			}
		}

		if ( !layerFound ) return false;
	}

	return true;
}

int VulkanRenderer::InitMemoryAllocator()
{
	VmaVulkanFunctions sFunctions = {};
	sFunctions.vkAllocateMemory = vkAllocateMemory;
	sFunctions.vkGetPhysicalDeviceProperties = vkGetPhysicalDeviceProperties;
    sFunctions.vkGetPhysicalDeviceMemoryProperties = vkGetPhysicalDeviceMemoryProperties;
    sFunctions.vkAllocateMemory = vkAllocateMemory;
    sFunctions.vkFreeMemory = vkFreeMemory;
    sFunctions.vkMapMemory = vkMapMemory;
    sFunctions.vkUnmapMemory = vkUnmapMemory;
    sFunctions.vkFlushMappedMemoryRanges = vkFlushMappedMemoryRanges;
    sFunctions.vkInvalidateMappedMemoryRanges = vkInvalidateMappedMemoryRanges;
    sFunctions.vkBindBufferMemory = vkBindBufferMemory;
    sFunctions.vkBindImageMemory = vkBindImageMemory;
    sFunctions.vkGetBufferMemoryRequirements = vkGetBufferMemoryRequirements;
    sFunctions.vkGetImageMemoryRequirements = vkGetImageMemoryRequirements;
    sFunctions.vkCreateBuffer = vkCreateBuffer;
    sFunctions.vkDestroyBuffer = vkDestroyBuffer;
    sFunctions.vkCreateImage = vkCreateImage;
    sFunctions.vkDestroyImage = vkDestroyImage;
    sFunctions.vkCmdCopyBuffer = vkCmdCopyBuffer;
#ifdef VK_VERSION_1_1
  #if VMA_DEDICATED_ALLOCATION
    sFunctions.vkGetBufferMemoryRequirements2KHR = vkGetBufferMemoryRequirements2KHR;
    sFunctions.vkGetImageMemoryRequirements2KHR = vkGetImageMemoryRequirements2KHR;
  #endif
#endif

	// create memory allocator
	VmaAllocatorCreateInfo allocatorInfo = {};
	allocatorInfo.physicalDevice = m_vkGPU;
	allocatorInfo.device = m_vkDevice;
	allocatorInfo.frameInUseCount = 1;
	allocatorInfo.pVulkanFunctions = &sFunctions;
	allocatorInfo.flags = VMA_ALLOCATOR_CREATE_EXTERNALLY_SYNCHRONIZED_BIT; // only use if single threaded
	if ( bDedicatedAllocationSupported ) allocatorInfo.flags |= VMA_ALLOCATOR_CREATE_KHR_DEDICATED_ALLOCATION_BIT;

	VkResult result = vmaCreateAllocator( &allocatorInfo, &m_vkAllocator );
	if ( VK_SUCCESS != result ) ReportAndReturnErrorID( APP_ERROR_VK_CREATE_ALLOCATOR_FAILED, result );

	// create transfer pool
	VkBufferCreateInfo exampleBufCreateInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
	exampleBufCreateInfo.size = 1024; // any value
	exampleBufCreateInfo.usage = AGK_BUFFER_TRANSFER_SRC;
 
	VmaAllocationCreateInfo allocCreateInfo = {};
	allocCreateInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
 
	uint32_t memTypeIndex;
	vmaFindMemoryTypeIndexForBufferInfo( m_vkAllocator, &exampleBufCreateInfo, &allocCreateInfo, &memTypeIndex );
 
	VmaPoolCreateInfo poolCreateInfo = {};
	poolCreateInfo.flags = VMA_POOL_CREATE_IGNORE_BUFFER_IMAGE_GRANULARITY_BIT | VMA_POOL_CREATE_BUDDY_ALGORITHM_BIT;
	poolCreateInfo.memoryTypeIndex = memTypeIndex;
	poolCreateInfo.blockSize = 4 * 1024 * 1024; // 4MB
	//poolCreateInfo.minBlockCount = 1;
	vmaCreatePool( m_vkAllocator, &poolCreateInfo, &m_vmaBufferTransferPool );

	// create vertex buffer pool
	exampleBufCreateInfo.usage = AGK_BUFFER_INDEX | AGK_BUFFER_VERTEX | AGK_BUFFER_TRANSFER_DST;
	allocCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
 
	vmaFindMemoryTypeIndexForBufferInfo( m_vkAllocator, &exampleBufCreateInfo, &allocCreateInfo, &memTypeIndex );
 
	poolCreateInfo.flags = VMA_POOL_CREATE_IGNORE_BUFFER_IMAGE_GRANULARITY_BIT | VMA_POOL_CREATE_BUDDY_ALGORITHM_BIT;
	poolCreateInfo.memoryTypeIndex = memTypeIndex;
	poolCreateInfo.blockSize = 16 * 1024 * 1024; // 16MB
	//poolCreateInfo.minBlockCount = 1;
	vmaCreatePool( m_vkAllocator, &poolCreateInfo, &m_vmaVertexBufferPool );

	return APP_SUCCESS;
}

void VulkanRenderer::AddDeviceExtensions( AGKUnorderedArray<uString*>* pExtensions )
{
	for( uint32_t i = 0; i < pExtensions->NumItems(); i++ )
	{
		m_szDeviceExtensions.push_back( pExtensions->GetItem(i)->GetStr() );
	}
}

void VulkanRenderer::AddInstanceExtensions( AGKUnorderedArray<uString*>* pExtensions )
{
	for( uint32_t i = 0; i < pExtensions->NumItems(); i++ )
	{
		m_szInstanceExtensions.push_back( pExtensions->GetItem(i)->GetStr() );
	}
}

int VulkanRenderer::Init() 
{
	m_iFBOFlipped = 0;
	m_iTopLeftOrigin = 1;
	m_iSupportsDepthTexture = 1;
	m_iSupportsDepth24 = 1;
	m_iSupportsDepthNonLinear = 1;
	m_iSupportsIntIndices = 1; 
	m_iSupportsNPOT = 1; 
	m_iSupportsMipmapNPOT = 1;
	m_iClipSpaceZeroOne = 1; // Vulkan is 0 to 1
	m_iInvertedDepth = 1; // Vulkan supports floating point depth buffers, so always use inverted depth for best precision
	m_iSupportsPSHighP = 1;

	ReturnIfFail( VulkanLoadFunctionsInit() );
	ReturnIfFail( VulkanLoadInstanceFunctions() );

	ReturnIfFail( CreateInstance() );
	ReturnIfFail( GetDevice() );
	ReturnIfFail( InitMemoryAllocator() );

	return APP_SUCCESS;
}

int VulkanRenderer::SetupWindow( void* param1, void* param2, unsigned int width, unsigned int height )
{
	int result = CreateSurface( param1, param2 );
	if ( result != APP_SUCCESS )
	{
		m_vkSurface = 0;
		return result;
	}
    
	if ( !(m_iFlags & AGK_VULKAN_SURFACE_VALID) ) 
	{
		m_vkSurface = 0;
		return APP_SUCCESS;
	}
	
    ReturnIfFail( CreateSwapChain(VK_PRESENT_MODE_FIFO_KHR) );

	return APP_SUCCESS;
}

int VulkanRenderer::WindowResized( void* param1, void* param2, unsigned int width, unsigned int height )
{
	if ( !(m_iFlags & AGK_VULKAN_SURFACE_VALID) )
	{
		ReturnIfFail( CreateSurface( param1, param2 ) );
    
		if ( !(m_iFlags & AGK_VULKAN_SURFACE_VALID) ) return APP_SUCCESS;
	
		ReturnIfFail( ResizeSwapChain() );
	}
	else
	{
		m_iSurfaceResized = 1;
	}
	return APP_SUCCESS;
}

int VulkanRenderer::SetScreenRecordParams( void* param1, void* param2 )
{
	return APP_ERROR_GENERAL;
}

void VulkanRenderer::LosingSurface()
{
	Present();
	vkDeviceWaitIdle( m_vkDevice );

	m_globalPipelineCache.DeleteScreenPipelines();

	// delete old
	if ( m_vkSwapFrameBuffers )
	{
		for( uint32_t i = 0; i < m_iNumSwapImages; i++ ) vkDestroyFramebuffer( m_vkDevice, m_vkSwapFrameBuffers[i], nullptr );
		delete [] m_vkSwapFrameBuffers;
		m_vkSwapFrameBuffers = 0;
	}

	if ( m_vkRenderPasses[AGK_VK_RENDER_PASS_SURFACE] ) vkDestroyRenderPass( m_vkDevice, m_vkRenderPasses[AGK_VK_RENDER_PASS_SURFACE], nullptr );
	if ( m_vkRenderPasses[AGK_VK_RENDER_PASS_SURFACE_NO_CLEAR] ) vkDestroyRenderPass( m_vkDevice, m_vkRenderPasses[AGK_VK_RENDER_PASS_SURFACE_NO_CLEAR], nullptr );
	m_vkRenderPasses[AGK_VK_RENDER_PASS_SURFACE] = 0;
	m_vkRenderPasses[AGK_VK_RENDER_PASS_SURFACE_NO_CLEAR] = 0;

	if ( m_vkSwapImageViews )
	{
		for( uint32_t i = 0; i < m_iNumSwapImages; i++ ) vkDestroyImageView( m_vkDevice, m_vkSwapImageViews[i], nullptr ); // VkImages themselves should be deleted by the swap chain
		delete [] m_vkSwapImageViews;
		m_vkSwapImageViews = 0;
	}

	if ( m_pDefaultDepthImage ) ForceDeleteImage( m_pDefaultDepthImage );
	m_pDefaultDepthImage = 0;

	if ( m_vkSwapImages ) delete [] m_vkSwapImages;
	m_vkSwapImages = 0;

	if ( m_vkSwapChain ) vkDestroySwapchainKHR( m_vkDevice, m_vkSwapChain, nullptr );
	m_vkSwapChain = 0;

	if ( m_vkSurface ) vkDestroySurfaceKHR( m_vkInstance, m_vkSurface, nullptr );
	m_vkSurface = 0;

	m_iFlags &= ~AGK_VULKAN_SURFACE_VALID;

	AppInfo( "Destroyed surface" );
}

int VulkanRenderer::Setup() 
{
	m_iMaxTextureSize = m_vkGPUProperties.limits.maxImageDimension2D;
	m_iSupportsDepthClamp = m_vkSupportedFeatures.depthClamp ? 1 : 0;
		
	m_iMaxVSUniforms = 16384;
	m_iMaxPSUniforms = 16384;
	m_iMaxVaryings = m_vkGPUProperties.limits.maxVertexOutputComponents;
	
	// fence for end of frame syncing with the CPU
	VkFenceCreateInfo fenceInfo = {};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
	for( int i = 0; i < AGK_VK_NUM_FRAMES; i++ )
	{
		vkCreateFence( m_vkDevice, &fenceInfo, nullptr, &m_vkFenceFrameFinished[i] );
	}

	// fence for image return
	vkCreateFence( m_vkDevice, &fenceInfo, nullptr, &m_vkFenceGetImageFinished );
	
	// semaphores for acquire->submit->present synchronization
	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	for( int i = 0; i < AGK_VK_NUM_FRAMES; i++ )
	{
		vkCreateSemaphore( m_vkDevice, &semaphoreInfo, nullptr, &m_vkSemaphoreTransferredVertices[i] );
		vkCreateSemaphore( m_vkDevice, &semaphoreInfo, nullptr, &m_vkSemaphoreTransferredImages[i] );
		vkCreateSemaphore( m_vkDevice, &semaphoreInfo, nullptr, &m_vkSemaphoreReceivedVertices[i] );
		vkCreateSemaphore( m_vkDevice, &semaphoreInfo, nullptr, &m_vkSemaphoreReceivedImages[i] );
		vkCreateSemaphore( m_vkDevice, &semaphoreInfo, nullptr, &m_vkSemaphoreAcquired[i] );
		vkCreateSemaphore( m_vkDevice, &semaphoreInfo, nullptr, &m_vkSemaphoreSubmitted[i] );
	}

	// command pool for graphics command buffers
	VkCommandPoolCreateInfo pool_info = {};
	pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	pool_info.queueFamilyIndex = m_iQueueFamilyIndex;
	vkCreateCommandPool( m_vkDevice, &pool_info, nullptr, &m_vkGraphicsCommandPool );

	// command pool for transfer command buffers
	pool_info.queueFamilyIndex = m_iTransferQueueFamilyIndex;
	vkCreateCommandPool( m_vkDevice, &pool_info, nullptr, &m_vkTransferCommandPool );

	// create command buffers
	VkCommandBufferAllocateInfo buffer_info = {};
	buffer_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	buffer_info.commandPool = m_vkGraphicsCommandPool;
	buffer_info.commandBufferCount = AGK_VK_NUM_FRAMES;
	buffer_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

	// main graphics command buffers
	VkResult res = vkAllocateCommandBuffers( m_vkDevice, &buffer_info, m_vkCommandBuffer );
	if ( VK_SUCCESS != res ) ReportAndReturnErrorID( APP_ERROR_VK_ALLOCATE_COMMAND_BUFFER_FAILED, res );

	buffer_info.commandBufferCount = 1;
	res = vkAllocateCommandBuffers( m_vkDevice, &buffer_info, &m_vkGetImageCommandBuffer );
	if ( VK_SUCCESS != res ) ReportAndReturnErrorID( APP_ERROR_VK_ALLOCATE_COMMAND_BUFFER_FAILED, res );

	buffer_info.commandBufferCount = AGK_VK_NUM_FRAMES;

	// receive vertices buffers
	res = vkAllocateCommandBuffers( m_vkDevice, &buffer_info, m_vkReceiveVerticesCommandBuffer );
	if ( VK_SUCCESS != res ) ReportAndReturnErrorID( APP_ERROR_VK_ALLOCATE_COMMAND_BUFFER_FAILED, res );

	// receive images buffers
	res = vkAllocateCommandBuffers( m_vkDevice, &buffer_info, m_vkReceiveImagesCommandBuffer );
	if ( VK_SUCCESS != res ) ReportAndReturnErrorID( APP_ERROR_VK_ALLOCATE_COMMAND_BUFFER_FAILED, res );

	// transfer vertices buffers
    if ( m_iTransferQueueFamilyIndex == m_iQueueFamilyIndex )
    {
        for( int i = 0; i < AGK_VK_NUM_FRAMES; i++ )
        {
            m_vkTransferVerticesCommandBuffer[i] = m_vkReceiveVerticesCommandBuffer[i];
            m_vkTransferImagesCommandBuffer[i] = m_vkReceiveImagesCommandBuffer[i];
        }
    }
    else
    {
        buffer_info.commandPool = m_vkTransferCommandPool;
        
        res = vkAllocateCommandBuffers( m_vkDevice, &buffer_info, m_vkTransferVerticesCommandBuffer );
        if ( VK_SUCCESS != res ) ReportAndReturnErrorID( APP_ERROR_VK_ALLOCATE_COMMAND_BUFFER_FAILED, res );

        // transfer images buffers
        res = vkAllocateCommandBuffers( m_vkDevice, &buffer_info, m_vkTransferImagesCommandBuffer );
        if ( VK_SUCCESS != res ) ReportAndReturnErrorID( APP_ERROR_VK_ALLOCATE_COMMAND_BUFFER_FAILED, res );
    }
    
	// all possible render passes used in AGK
	m_vkRenderPasses[AGK_VK_RENDER_PASS_SURFACE] = 0;
	m_vkRenderPasses[AGK_VK_RENDER_PASS_SURFACE_NO_CLEAR] = 0;
	
	ReturnIfFail( CreateRenderPass( VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_D32_SFLOAT, 0, m_vkRenderPasses[AGK_VK_RENDER_PASS_RGBA_DEPTH] ) );
	ReturnIfFail( CreateRenderPass( VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_D32_SFLOAT, AGK_VK_RENDER_PASS_FLAG_NO_CLEAR, m_vkRenderPasses[AGK_VK_RENDER_PASS_RGBA_DEPTH_NO_CLEAR] ) );

	ReturnIfFail( CreateRenderPass( VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_D32_SFLOAT, AGK_VK_RENDER_PASS_FLAG_STORE_DEPTH, m_vkRenderPasses[AGK_VK_RENDER_PASS_RGBA_DEPTH_STORE] ) );
	ReturnIfFail( CreateRenderPass( VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_D32_SFLOAT, AGK_VK_RENDER_PASS_FLAG_STORE_DEPTH | AGK_VK_RENDER_PASS_FLAG_NO_CLEAR, m_vkRenderPasses[AGK_VK_RENDER_PASS_RGBA_DEPTH_STORE_NO_CLEAR] ) );
	
	ReturnIfFail( CreateRenderPass( VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_UNDEFINED, 0, m_vkRenderPasses[AGK_VK_RENDER_PASS_RGBA] ) );
	ReturnIfFail( CreateRenderPass( VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_UNDEFINED, AGK_VK_RENDER_PASS_FLAG_NO_CLEAR, m_vkRenderPasses[AGK_VK_RENDER_PASS_RGBA_NO_CLEAR] ) );
	
	ReturnIfFail( CreateRenderPass( VK_FORMAT_UNDEFINED, VK_FORMAT_D32_SFLOAT, AGK_VK_RENDER_PASS_FLAG_STORE_DEPTH, m_vkRenderPasses[AGK_VK_RENDER_PASS_DEPTH] ) );

	uint32_t data = 0;
	ReturnIfFail( CreateImage2D( AGK_COLOR_FORMAT_RGBA_8, 1, 1, (uint8_t*)&data, 0, (void**)&m_pNullImage2D ) );

	// screen render pass
	if ( m_iFlags & AGK_VULKAN_SURFACE_VALID ) 
	{
		AGKImgFormat depthFormat;
		if ( IsImageFormatSupported( AGK_DEPTH_FORMAT_32_FLOAT ) ) depthFormat = AGK_DEPTH_FORMAT_32_FLOAT;
		else if ( IsImageFormatSupported( AGK_DEPTH_FORMAT_24_INT ) ) depthFormat = AGK_DEPTH_FORMAT_24_INT;
		else depthFormat = AGK_DEPTH_FORMAT_16_INT;
		ReturnIfFail( CreateDepthBuffer( depthFormat, m_iSurfaceWidth, m_iSurfaceHeight, AGK_VK_IMAGE_IS_DEFAULT, (void**)&m_pDefaultDepthImage ) )
		ReturnIfFail( CreateDefaultRenderPass() );
		ReturnIfFail( CreateDefaultFrameBuffer() );
	}
	
	ReturnIfFail( BeginFrame() );

	return APP_SUCCESS;
}

int VulkanRenderer::CreateInstance()
{
	if ( m_bEnableValidationLayers && !CheckValidationLayerSupport() ) 
	{
		AppWarning("Validation layers not supported, skipping");
		m_bEnableValidationLayers = false;
	}

	m_iInstanceVersion = VK_MAKE_VERSION( 1, 0, 0 );
#if !defined(AGK_IOS) && !defined(AGK_MACOS)
	if ( vkEnumerateInstanceVersion )
	{
		vkEnumerateInstanceVersion( &m_iInstanceVersion );
	}
#endif

	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "AppGameKit Vulkan";
	appInfo.applicationVersion = 1;
	appInfo.pEngineName = "AppGameKit";
	appInfo.engineVersion = 1;
	appInfo.apiVersion = m_iInstanceVersion & 0xFFFFF000;

	m_szInstanceExtensions.push_back( VK_KHR_SURFACE_EXTENSION_NAME );
#if defined(AGK_WINDOWS)
    m_szInstanceExtensions.push_back( VK_KHR_WIN32_SURFACE_EXTENSION_NAME );
#elif defined(AGK_IOS)
	//m_szInstanceExtensions.push_back( VK_MVK_IOS_SURFACE_EXTENSION_NAME );
    m_szInstanceExtensions.push_back( VK_EXT_METAL_SURFACE_EXTENSION_NAME );
#elif defined(AGK_MACOS)
    //m_szInstanceExtensions.push_back( VK_MVK_MAC_SURFACE_EXTENSION_NAME );
    m_szInstanceExtensions.push_back( VK_EXT_METAL_SURFACE_EXTENSION_NAME );
#elif defined(AGK_LINUX)
    m_szInstanceExtensions.push_back( VK_KHR_XLIB_SURFACE_EXTENSION_NAME );
#elif defined(AGK_ANDROID)
    m_szInstanceExtensions.push_back( VK_KHR_ANDROID_SURFACE_EXTENSION_NAME );
#else
    #error Unknown Vulkan platform
#endif

	//AppInfo( "Instance extensions" );
	
	vkEnumerateInstanceExtensionProperties( NULL, &m_iInstanceExtensionCount, 0 );
	if ( m_iInstanceExtensionCount > 0 )
	{
		m_pInstanceExtensions = new VkExtensionProperties[ m_iInstanceExtensionCount ];
		vkEnumerateInstanceExtensionProperties( NULL, &m_iInstanceExtensionCount, m_pInstanceExtensions );

		for( uint32_t i = 0; i < m_iInstanceExtensionCount; i++ )
		{
			if ( strcmp( m_pInstanceExtensions[ i ].extensionName, VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME ) == 0 )
			{
				m_iProperties2Extension = 1;
				m_szInstanceExtensions.push_back( VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME );
			}
			
			//AppInfo( m_pInstanceExtensions[i].extensionName );
		}
	}
		
	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pNext = NULL;
	createInfo.flags = 0;
	createInfo.pApplicationInfo = &appInfo;
	if ( m_bEnableValidationLayers )
	{
		m_szInstanceExtensions.push_back( VK_EXT_DEBUG_REPORT_EXTENSION_NAME );
		createInfo.enabledLayerCount = 1;
		createInfo.ppEnabledLayerNames = g_szValidationLayers;
		AppInfo( "Adding validation layers" );
	}
	else
	{
		createInfo.enabledLayerCount = 0;
		createInfo.ppEnabledLayerNames = NULL;
	}
	createInfo.enabledExtensionCount = (uint32_t)m_szInstanceExtensions.size();
	createInfo.ppEnabledExtensionNames = m_szInstanceExtensions.data();

	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties( &layerCount, nullptr );
	std::vector<VkLayerProperties> availableLayers( layerCount );
	vkEnumerateInstanceLayerProperties( &layerCount, availableLayers.data() );

	AppInfo( "Instance Layers" );
	for( const VkLayerProperties& layerProperties : availableLayers )
	{
		AppInfo( layerProperties.layerName );
	}

#if defined(AGK_ANDROID) && defined(ARM_LAYER)
	AppInfo( "Using Graphics Analyzer Layer" );
	const char* layer = "VK_LAYER_ARM_AGA";
	createInfo.enabledLayerCount = 1;
	createInfo.ppEnabledLayerNames = &layer;
#endif

	VkResult res = vkCreateInstance( &createInfo, NULL, &m_vkInstance );
	if (res == VK_ERROR_INCOMPATIBLE_DRIVER) {
		ReportAndReturnErrorID( APP_ERROR_VK_ICD_NOT_FOUND );
	} else if (res) {
		ReportAndReturnErrorID( APP_ERROR_VK_CREATEINSTANCE_FAILED, res );
	}

	ReturnIfFail( VulkanLoadDeviceFunctions( m_vkInstance ) );

	if ( m_bEnableValidationLayers )
	{
		VkDebugReportCallbackCreateInfoEXT debugCreateInfo = {};
		debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
		debugCreateInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT 
								| VK_DEBUG_REPORT_WARNING_BIT_EXT 
								| VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT 
							//| VK_DEBUG_REPORT_DEBUG_BIT_EXT 
							/*| VK_DEBUG_REPORT_INFORMATION_BIT_EXT*/;
		debugCreateInfo.pfnCallback = debugCallback;

		if (vkCreateDebugReportCallbackEXT != 0) {
			VkResult result = vkCreateDebugReportCallbackEXT( m_vkInstance, &debugCreateInfo, nullptr, &pDebugCallback );
			if ( result != VK_SUCCESS ) AppWarning( "Failed to create debug callback, validation won't report errors" );
		} else {
			AppWarning( "vkCreateDebugReportCallbackEXT not found, validation won't report errors" );
		}
	}
		
	return APP_SUCCESS;
}

int VulkanRenderer::GetDevice()
{
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices( m_vkInstance, &deviceCount, nullptr );
	std::vector<VkPhysicalDevice> devices( deviceCount );
	vkEnumeratePhysicalDevices( m_vkInstance, &deviceCount, devices.data() );
	AppInfo( "Found %d GPUs", deviceCount );
	if ( deviceCount == 0 ) return APP_ERROR_GENERAL;
	int index = -1;
	for( uint32_t i = 0; i < deviceCount; i++ )
	{
		VkPhysicalDeviceProperties prop;
		vkGetPhysicalDeviceProperties( devices[i], &prop );
		AppInfo( "GPU: %d, name: %s, type: %u", i, prop.deviceName, prop.deviceType );
		if ( index < 0 && prop.deviceType != VK_PHYSICAL_DEVICE_TYPE_CPU ) index = i;
	}
	if ( index < 0 ) return APP_ERROR_GENERAL;
	m_vkGPU = devices[ index ];

#if !defined(AGK_IOS) && !defined(AGK_MACOS)
	// device properties
	VkPhysicalDeviceProperties2 prop2 = {};
	prop2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
	//prop2.pNext = (void*)&m_vkConservativeRasterizerProp;
	//m_vkConservativeRasterizerProp.pNext = (void*)&m_vkPushDescriptorProp;
	
	if ( m_iInstanceVersion >= VK_MAKE_VERSION(1,1,0) && m_iProperties2Extension && vkGetPhysicalDeviceProperties2 )
	{
		vkGetPhysicalDeviceProperties2( m_vkGPU, &prop2 );
		m_vkGPUProperties = prop2.properties;
	}
	else if ( m_iProperties2Extension && vkGetPhysicalDeviceProperties2KHR )
	{
		vkGetPhysicalDeviceProperties2KHR( m_vkGPU, &prop2 );
		m_vkGPUProperties = prop2.properties;
	}
	else
#endif
	{		
		vkGetPhysicalDeviceProperties( m_vkGPU, &m_vkGPUProperties );
	}

#ifdef AGK_MACOS
	// Intel Iris 645 seems to have a driver bug with Metal not aligning memory correctly? Reports as a black screen
	if ( strstr(m_vkGPUProperties.deviceName, "Intel") != 0 && strstr(m_vkGPUProperties.deviceName, " Iris") != 0 && strstr(m_vkGPUProperties.deviceName, "645") != 0 )
	{
		return APP_ERROR_GENERAL;
	}
#endif

	// device extensions
	vkEnumerateDeviceExtensionProperties( m_vkGPU, NULL, &m_iDeviceExtensionCount, 0 );
	if ( m_iDeviceExtensionCount > 0 )
	{
		m_pDeviceExtensions = new VkExtensionProperties[ m_iDeviceExtensionCount ];
		vkEnumerateDeviceExtensionProperties( m_vkGPU, NULL, &m_iDeviceExtensionCount, m_pDeviceExtensions );
	}

	// memory properties
#if defined(VK_EXT_memory_budget) && !defined(AGK_IOS) && !defined(AGK_MACOS)
	if ( m_iInstanceVersion >= VK_MAKE_VERSION(1,1,0) && vkGetPhysicalDeviceMemoryProperties2 )
	{
		m_vkGPUMemoryBudget.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_BUDGET_PROPERTIES_EXT;
		VkPhysicalDeviceMemoryProperties2 memprops2;
		memprops2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2;
		memprops2.pNext = (void*) &m_vkGPUMemoryBudget;
		vkGetPhysicalDeviceMemoryProperties2( m_vkGPU, &memprops2 );

		m_vkGPUMemoryProps = memprops2.memoryProperties;
	}
	else
#endif
	{
		vkGetPhysicalDeviceMemoryProperties( m_vkGPU, &m_vkGPUMemoryProps );
	}

	m_iDeviceVersion = m_vkGPUProperties.apiVersion;
	
	m_iQueueFamilyIndex = -1;
	m_iTransferQueueFamilyIndex = -1;
	uint32_t familyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties( m_vkGPU, &familyCount, nullptr );
	std::vector<VkQueueFamilyProperties> families( familyCount );
	vkGetPhysicalDeviceQueueFamilyProperties( m_vkGPU, &familyCount, families.data() );
	for( uint32_t i = 0; i < familyCount; i++ )
	{
		if ( families[ i ].queueFlags & VK_QUEUE_GRAPHICS_BIT )
		{
			m_iQueueFamilyIndex = i;
		}
		if ( (families[ i ].queueFlags & (VK_QUEUE_TRANSFER_BIT | VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT)) == VK_QUEUE_TRANSFER_BIT )
		{
			m_iTransferQueueFamilyIndex = i;
		}
	}

	// if no dedicated transfer queue then use any non-graphics transfer queue
	if ( m_iTransferQueueFamilyIndex < 0 )
	{
		for( uint32_t i = 0; i < familyCount; i++ )
		{
			if ( (families[ i ].queueFlags & VK_QUEUE_TRANSFER_BIT) && m_iQueueFamilyIndex != i ) m_iTransferQueueFamilyIndex = i;
		}
	}

	// if still no transfer queue then use any transfer queue
	if ( m_iTransferQueueFamilyIndex < 0 )
	{
		for( uint32_t i = 0; i < familyCount; i++ )
		{
			if ( families[ i ].queueFlags & VK_QUEUE_TRANSFER_BIT ) m_iTransferQueueFamilyIndex = i;
		}
	}

	if ( m_iQueueFamilyIndex < 0 ) ReportAndReturnErrorID( APP_ERROR_VK_DEVICE_NOT_GRAPHICS );
	if ( m_iTransferQueueFamilyIndex < 0 ) m_iTransferQueueFamilyIndex = m_iQueueFamilyIndex;
    
#ifdef AGK_VK_SINGLE_QUEUE
    // Workaround for MTLFence not supporting multiple queues
    m_iTransferQueueFamilyIndex = m_iQueueFamilyIndex;
#endif
    
	float queuePriorities[] { 1.0f };

	VkDeviceQueueCreateInfo device_queue_info[2];
	device_queue_info[0] = {};
	device_queue_info[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	device_queue_info[0].queueCount = 1;
	device_queue_info[0].pQueuePriorities = queuePriorities;
	device_queue_info[0].queueFamilyIndex = m_iQueueFamilyIndex;

	device_queue_info[1] = {};
	device_queue_info[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	device_queue_info[1].queueCount = 1;
	device_queue_info[1].pQueuePriorities = queuePriorities;
	device_queue_info[1].queueFamilyIndex = m_iTransferQueueFamilyIndex;

	m_szDeviceExtensions.push_back( VK_KHR_SWAPCHAIN_EXTENSION_NAME );

	//AppInfo( "Device extensions" );

	// Check if dedicated allocation is supported
	bool extMemoryRequirements2 = false;
	bool extDedicatedAlloc = false;
	for( uint32_t i = 0; i < m_iDeviceExtensionCount; i++ )
	{
		if( strcmp(m_pDeviceExtensions[i].extensionName, VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME) == 0 ) extMemoryRequirements2 = true;
		else if( strcmp(m_pDeviceExtensions[i].extensionName, VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME) == 0 ) extDedicatedAlloc = true;
		
		//AppInfo( m_pDeviceExtensions[i].extensionName );
	}

	if ( extMemoryRequirements2 && extDedicatedAlloc )
	{
		m_szDeviceExtensions.push_back( VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME );
		m_szDeviceExtensions.push_back( VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME );
		bDedicatedAllocationSupported = true;
	}
	
	vkGetPhysicalDeviceFeatures( m_vkGPU, &m_vkSupportedFeatures );

	// request features
	VkPhysicalDeviceFeatures deviceFeatures = {};
	//if ( m_vkSupportedFeatures.robustBufferAccess ) deviceFeatures.robustBufferAccess = VK_TRUE;
    //if ( m_vkSupportedFeatures.fullDrawIndexUint32 ) deviceFeatures.fullDrawIndexUint32 = VK_TRUE;
    //if ( m_vkSupportedFeatures.imageCubeArray ) deviceFeatures.imageCubeArray = VK_TRUE;
    //if ( m_vkSupportedFeatures.independentBlend ) deviceFeatures.independentBlend = VK_TRUE; // each color attachment can have diffferent blend modes
    //if ( m_vkSupportedFeatures.geometryShader ) deviceFeatures.geometryShader = VK_TRUE;
    //if ( m_vkSupportedFeatures.tessellationShader ) deviceFeatures.tessellationShader = VK_TRUE;
    //if ( m_vkSupportedFeatures.sampleRateShading ) deviceFeatures.sampleRateShading = VK_TRUE;
    //if ( m_vkSupportedFeatures.dualSrcBlend ) deviceFeatures.dualSrcBlend = VK_TRUE;
    //if ( m_vkSupportedFeatures.logicOp ) deviceFeatures.logicOp = VK_TRUE; // alternative to blending
    //if ( m_vkSupportedFeatures.multiDrawIndirect ) deviceFeatures.multiDrawIndirect = VK_TRUE;
    //if ( m_vkSupportedFeatures.drawIndirectFirstInstance ) deviceFeatures.drawIndirectFirstInstance = VK_TRUE;
    if ( m_vkSupportedFeatures.depthClamp ) deviceFeatures.depthClamp = VK_TRUE;
    //if ( m_vkSupportedFeatures.depthBiasClamp ) deviceFeatures.depthBiasClamp = VK_TRUE;
    if ( m_vkSupportedFeatures.fillModeNonSolid ) deviceFeatures.fillModeNonSolid = VK_TRUE;
    //if ( m_vkSupportedFeatures.depthBounds ) deviceFeatures.depthBounds = VK_TRUE;
    //if ( m_vkSupportedFeatures.wideLines ) deviceFeatures.wideLines = VK_TRUE;
    //if ( m_vkSupportedFeatures.largePoints ) deviceFeatures.largePoints = VK_TRUE;
    //if ( m_vkSupportedFeatures.alphaToOne ) deviceFeatures.alphaToOne = VK_TRUE;
    //if ( m_vkSupportedFeatures.multiViewport ) deviceFeatures.multiViewport = VK_TRUE;
    if ( m_vkSupportedFeatures.samplerAnisotropy ) deviceFeatures.samplerAnisotropy = VK_TRUE;
    if ( m_vkSupportedFeatures.textureCompressionETC2 ) deviceFeatures.textureCompressionETC2 = VK_TRUE;
    if ( m_vkSupportedFeatures.textureCompressionASTC_LDR ) deviceFeatures.textureCompressionASTC_LDR = VK_TRUE;
    if ( m_vkSupportedFeatures.textureCompressionBC ) deviceFeatures.textureCompressionBC = VK_TRUE;
    //if ( m_vkSupportedFeatures.occlusionQueryPrecise ) deviceFeatures.occlusionQueryPrecise = VK_TRUE;
    //if ( m_vkSupportedFeatures.pipelineStatisticsQuery ) deviceFeatures.pipelineStatisticsQuery = VK_TRUE;
    //if ( m_vkSupportedFeatures.vertexPipelineStoresAndAtomics ) deviceFeatures.vertexPipelineStoresAndAtomics = VK_TRUE;
    //if ( m_vkSupportedFeatures.fragmentStoresAndAtomics ) deviceFeatures.fragmentStoresAndAtomics = VK_TRUE;
    //if ( m_vkSupportedFeatures.shaderTessellationAndGeometryPointSize ) deviceFeatures.shaderTessellationAndGeometryPointSize = VK_TRUE;
    //if ( m_vkSupportedFeatures.shaderImageGatherExtended ) deviceFeatures.shaderImageGatherExtended = VK_TRUE;
    //if ( m_vkSupportedFeatures.shaderStorageImageExtendedFormats ) deviceFeatures.shaderStorageImageExtendedFormats = VK_TRUE;
    //if ( m_vkSupportedFeatures.shaderStorageImageMultisample ) deviceFeatures.shaderStorageImageMultisample = VK_TRUE;
    //if ( m_vkSupportedFeatures.shaderStorageImageReadWithoutFormat ) deviceFeatures.shaderStorageImageReadWithoutFormat = VK_TRUE;
    //if ( m_vkSupportedFeatures.shaderStorageImageWriteWithoutFormat ) deviceFeatures.shaderStorageImageWriteWithoutFormat = VK_TRUE;
    if ( m_vkSupportedFeatures.shaderUniformBufferArrayDynamicIndexing ) deviceFeatures.shaderUniformBufferArrayDynamicIndexing = VK_TRUE;
    if ( m_vkSupportedFeatures.shaderSampledImageArrayDynamicIndexing ) deviceFeatures.shaderSampledImageArrayDynamicIndexing = VK_TRUE;
    if ( m_vkSupportedFeatures.shaderStorageBufferArrayDynamicIndexing ) deviceFeatures.shaderStorageBufferArrayDynamicIndexing = VK_TRUE;
    if ( m_vkSupportedFeatures.shaderStorageImageArrayDynamicIndexing ) deviceFeatures.shaderStorageImageArrayDynamicIndexing = VK_TRUE;
    if ( m_vkSupportedFeatures.shaderClipDistance ) deviceFeatures.shaderClipDistance = VK_TRUE;
    if ( m_vkSupportedFeatures.shaderCullDistance ) deviceFeatures.shaderCullDistance = VK_TRUE;
    //if ( m_vkSupportedFeatures.shaderFloat64 ) deviceFeatures.shaderFloat64 = VK_TRUE;
    //if ( m_vkSupportedFeatures.shaderInt64 ) deviceFeatures.shaderInt64 = VK_TRUE;
    if ( m_vkSupportedFeatures.shaderInt16 ) deviceFeatures.shaderInt16 = VK_TRUE;
    //if ( m_vkSupportedFeatures.shaderResourceResidency ) deviceFeatures.shaderResourceResidency = VK_TRUE;
    if ( m_vkSupportedFeatures.shaderResourceMinLod ) deviceFeatures.shaderResourceMinLod = VK_TRUE;
    //if ( m_vkSupportedFeatures.sparseBinding ) deviceFeatures.sparseBinding = VK_TRUE;
    //if ( m_vkSupportedFeatures.sparseResidencyBuffer ) deviceFeatures.sparseResidencyBuffer = VK_TRUE;
    //if ( m_vkSupportedFeatures.sparseResidencyImage2D ) deviceFeatures.sparseResidencyImage2D = VK_TRUE;
    //if ( m_vkSupportedFeatures.sparseResidencyImage3D ) deviceFeatures.sparseResidencyImage3D = VK_TRUE;
    //if ( m_vkSupportedFeatures.sparseResidency2Samples ) deviceFeatures.sparseResidency2Samples = VK_TRUE;
    //if ( m_vkSupportedFeatures.sparseResidency4Samples ) deviceFeatures.sparseResidency4Samples = VK_TRUE;
    //if ( m_vkSupportedFeatures.sparseResidency8Samples ) deviceFeatures.sparseResidency8Samples = VK_TRUE;
    //if ( m_vkSupportedFeatures.sparseResidency16Samples ) deviceFeatures.sparseResidency16Samples = VK_TRUE;
    //if ( m_vkSupportedFeatures.sparseResidencyAliased ) deviceFeatures.sparseResidencyAliased = VK_TRUE;
    //if ( m_vkSupportedFeatures.variableMultisampleRate ) deviceFeatures.variableMultisampleRate = VK_TRUE;
    //if ( m_vkSupportedFeatures.inheritedQueries ) deviceFeatures.inheritedQueries = VK_TRUE;

	// change it to match what we actually requested
	m_vkSupportedFeatures = deviceFeatures;

	VkDeviceCreateInfo device_info = {};
	device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	device_info.queueCreateInfoCount = (m_iTransferQueueFamilyIndex == m_iQueueFamilyIndex) ? 1 : 2;
	device_info.pQueueCreateInfos = device_queue_info;
	device_info.enabledExtensionCount = (uint32_t) m_szDeviceExtensions.size();
	device_info.ppEnabledExtensionNames = m_szDeviceExtensions.data();
	device_info.pEnabledFeatures = &deviceFeatures;

	uint32_t layerCount;
	vkEnumerateDeviceLayerProperties( m_vkGPU, &layerCount, nullptr );
	std::vector<VkLayerProperties> availableLayers( layerCount );
	vkEnumerateDeviceLayerProperties( m_vkGPU, &layerCount, availableLayers.data() );

	AppInfo( "Device Layers" );
	for( const VkLayerProperties& layerProperties : availableLayers )
	{
		AppInfo( layerProperties.layerName );
	}

#if defined(AGK_ANDROID) && defined(ARM_LAYER)
	AppInfo( "Using Graphics Analyzer Layer" );
	const char* layer = "VK_LAYER_ARM_AGA";
	device_info.enabledLayerCount = 1;
	device_info.ppEnabledLayerNames = &layer;
#endif

	VkResult result = vkCreateDevice( m_vkGPU, &device_info, nullptr, &m_vkDevice );
	if ( VK_SUCCESS != result ) ReportAndReturnErrorID( APP_ERROR_VK_CREATE_DEVICE_FAILED, result );

	ReturnIfFail( VulkanLoadMainFunctions( m_vkDevice ) );

	vkGetDeviceQueue( m_vkDevice, m_iQueueFamilyIndex, 0, &m_vkQueue );
	if ( m_iTransferQueueFamilyIndex == m_iQueueFamilyIndex ) m_vkTransferQueue = m_vkQueue;
	else vkGetDeviceQueue( m_vkDevice, m_iTransferQueueFamilyIndex, 0, &m_vkTransferQueue );

	return APP_SUCCESS;
}

int VulkanRenderer::CreateSurface( void* param1, void* param2 )
{
	m_iFlags &= ~AGK_VULKAN_SURFACE_VALID;
	m_iSurfaceOptimalWarning = 0;

#if defined(AGK_WINDOWS)
    
	VkWin32SurfaceCreateInfoKHR win32Info = {};
	win32Info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	win32Info.hinstance = (HINSTANCE)param1;
	win32Info.hwnd = (HWND)param2;
	VkResult res = vkCreateWin32SurfaceKHR( m_vkInstance, &win32Info, nullptr, &m_vkSurface );
    
#elif defined(AGK_IOS)
    
    UIView* view = (UIView*) param1;
    
    VkMetalSurfaceCreateInfoEXT surface;
    surface.sType = VK_STRUCTURE_TYPE_METAL_SURFACE_CREATE_INFO_EXT;
    surface.pNext = NULL;
    surface.flags = 0;
    surface.pLayer = (CAMetalLayer*) view.layer;

    VkResult res = vkCreateMetalSurfaceEXT( m_vkInstance, &surface, NULL, &m_vkSurface );
    
#elif defined(AGK_MACOS)
    
    NSWindow* window = (NSWindow*) param1;
    CALayer *metalLayer = [[CAMetalLayer class] layer];
    NSView *contentView = window.contentView;
    CGSize viewScale = [contentView convertSizeToBacking: CGSizeMake(1.0, 1.0)];
    metalLayer.contentsScale = MIN(viewScale.width, viewScale.height);
    contentView.wantsLayer = true;
    contentView.layer = metalLayer;
            
    VkMetalSurfaceCreateInfoEXT surface;
    surface.sType = VK_STRUCTURE_TYPE_METAL_SURFACE_CREATE_INFO_EXT;
    surface.pNext = NULL;
    surface.flags = 0;
    surface.pLayer = (CAMetalLayer*) metalLayer;

    VkResult res = vkCreateMetalSurfaceEXT( m_vkInstance, &surface, NULL, &m_vkSurface );

#elif defined(AGK_LINUX)
    
    VkXlibSurfaceCreateInfoKHR surface;
    surface.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
    surface.pNext = NULL;
    surface.flags = 0;
    surface.dpy = (Display*) param1;
    surface.window = (Window) param2;

    VkResult res = vkCreateXlibSurfaceKHR( m_vkInstance, &surface, NULL, &m_vkSurface );

#elif defined(AGK_ANDROID)
    
    VkAndroidSurfaceCreateInfoKHR surface;
    surface.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
    surface.pNext = NULL;
    surface.flags = 0;
    surface.window = (ANativeWindow*) param1;

    VkResult res = vkCreateAndroidSurfaceKHR( m_vkInstance, &surface, NULL, &m_vkSurface );
    
#else
    #error Unknown Vulkan platform, if not supported then define DISABLE_VULKAN
#endif

	if ( VK_SUCCESS != res ) ReportAndReturnErrorID( APP_ERROR_VK_CREATE_WIN32_SURFACE_FAILED, res );

	VkBool32 supported;
	res = vkGetPhysicalDeviceSurfaceSupportKHR( m_vkGPU, m_iQueueFamilyIndex, m_vkSurface, &supported );
	if ( VK_SUCCESS != res ) ReportAndReturnErrorID( APP_ERROR_VK_SURFACE_SUPPORT_FAILED, res );
	if ( !supported ) ReportAndReturnErrorID( APP_ERROR_VK_SURFACE_NOT_SUPPORTED );

	uint32_t formatCount = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR( m_vkGPU, m_vkSurface, &formatCount, nullptr );
	if ( formatCount == 0 ) ReportAndReturnErrorID( APP_ERROR_VK_NO_SURFACE_FORMATS ); 
	std::vector<VkSurfaceFormatKHR> formats( formatCount );
	vkGetPhysicalDeviceSurfaceFormatsKHR( m_vkGPU, m_vkSurface, &formatCount, formats.data() );
	//for( uint32_t i = 0; i < formatCount; i++ ) AppInfo( "Surface: %d", formats[i].format );
	for( uint32_t i = 0; i < formatCount; i++ )
	{
		// Android seems to use VK_FORMAT_R8G8B8A8_UNORM whilst everything else uses VK_FORMAT_B8G8R8A8_UNORM
		if ( formats[i].format == VK_FORMAT_B8G8R8A8_UNORM || formats[i].format == VK_FORMAT_R8G8B8A8_UNORM ) 
		{
			m_vkSurfaceFormat = formats[ i ];
			break;
		}
	}
	
	if ( m_vkSurfaceFormat.format == VK_FORMAT_UNDEFINED )
	{
		AppError( "Failed to find RGBA surface format" );
		return APP_ERROR_GENERAL;
	}

	VkSurfaceCapabilitiesKHR surfaceCaps;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR( m_vkGPU, m_vkSurface, &surfaceCaps );
	m_iSurfaceWidth = surfaceCaps.currentExtent.width;
	m_iSurfaceHeight = surfaceCaps.currentExtent.height;
	switch( m_vkSurfaceFormat.format )
	{
		case VK_FORMAT_B8G8R8A8_UNORM: 
		case VK_FORMAT_R8G8B8A8_UNORM: m_iSurfaceBytesPerPixel = 4; break;
		default: AppError( "Unrecognised surface format" );
	}

	if ( m_iSurfaceWidth > 0 && m_iSurfaceHeight > 0 ) m_iFlags |= AGK_VULKAN_SURFACE_VALID;

	AppInfo( "Surface Size: %dx%d", m_iSurfaceWidth, m_iSurfaceHeight );

	return APP_SUCCESS;
}

int VulkanRenderer::CreateSwapChain( VkPresentModeKHR desiredMode )
{
	// VK_PRESENT_MODE_IMMEDIATE_KHR is unlimited FPS with tearing
	// VK_PRESENT_MODE_MAILBOX_KHR is unlimited FPS but no tearing
	// VK_PRESENT_MODE_FIFO_KHR is FPS limited to monitor refresh rate, vsync on
	// VK_PRESENT_MODE_FIFO_RELAXED_KHR is FPS limited to monitor refresh rate but not strictly enforced, can rsult in tearing

	if ( desiredMode == VK_PRESENT_MODE_IMMEDIATE_KHR ) desiredMode = VK_PRESENT_MODE_MAILBOX_KHR;
	
	VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
	uint32_t modeCount = 0;
	
	vkGetPhysicalDeviceSurfacePresentModesKHR( m_vkGPU, m_vkSurface, &modeCount, nullptr );
	std::vector<VkPresentModeKHR> modes( modeCount );
	vkGetPhysicalDeviceSurfacePresentModesKHR( m_vkGPU, m_vkSurface, &modeCount, modes.data() );
	for( VkPresentModeKHR mode : modes )
	{
		if ( mode == desiredMode ) 
		{
			presentMode = mode;
			break;
		}
	}	

	if ( desiredMode == VK_PRESENT_MODE_MAILBOX_KHR && presentMode != VK_PRESENT_MODE_MAILBOX_KHR ) 
	{
		desiredMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
		for( VkPresentModeKHR mode : modes )
		{
			if ( mode == desiredMode ) 
			{
				presentMode = mode;
				break;
			}
		}
	}

	m_iCurrentPresentMode = presentMode;

	VkSwapchainCreateInfoKHR swapInfo = {};
	swapInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapInfo.surface = m_vkSurface;
	swapInfo.minImageCount = 3; // this must be at least 3 for best performance
	swapInfo.imageFormat = m_vkSurfaceFormat.format;
	swapInfo.imageColorSpace = m_vkSurfaceFormat.colorSpace;
	swapInfo.imageExtent.width = m_iSurfaceWidth;
	swapInfo.imageExtent.height = m_iSurfaceHeight;
	swapInfo.imageArrayLayers = 1;
	swapInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	swapInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	swapInfo.queueFamilyIndexCount = 0;
	swapInfo.pQueueFamilyIndices = nullptr;
	swapInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	swapInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapInfo.presentMode = presentMode;
	swapInfo.clipped = VK_TRUE;
	swapInfo.oldSwapchain = 0;
	VkResult res = vkCreateSwapchainKHR( m_vkDevice, &swapInfo, nullptr, &m_vkSwapChain );
	if ( VK_SUCCESS != res ) ReportAndReturnErrorID( APP_ERROR_VK_CREATE_SWAP_FAILED, res );

	vkGetSwapchainImagesKHR( m_vkDevice, m_vkSwapChain, &m_iNumSwapImages, nullptr );
	m_vkSwapImages = new VkImage[ m_iNumSwapImages ];
	m_vkSwapImageViews = new VkImageView[ m_iNumSwapImages ];

	res = vkGetSwapchainImagesKHR( m_vkDevice, m_vkSwapChain, &m_iNumSwapImages, m_vkSwapImages );
	if ( VK_SUCCESS != res ) ReportAndReturnErrorID( APP_ERROR_VK_GET_SWAP_IMAGES_FAILED, res );

	for( uint32_t i = 0; i < m_iNumSwapImages; i++ )
	{
		VkImageViewCreateInfo imageViewInfo = {};
		imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewInfo.image = m_vkSwapImages[ i ];
		imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		imageViewInfo.format = m_vkSurfaceFormat.format;
		imageViewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageViewInfo.subresourceRange.baseMipLevel = 0;
		imageViewInfo.subresourceRange.levelCount = 1;
		imageViewInfo.subresourceRange.baseArrayLayer = 0;
		imageViewInfo.subresourceRange.layerCount = 1;

		res = vkCreateImageView( m_vkDevice, &imageViewInfo, nullptr, &m_vkSwapImageViews[i] );
		if ( VK_SUCCESS != res ) ReportAndReturnErrorID( APP_ERROR_VK_GET_SWAP_IMAGES_FAILED, res );
	}

	m_iCurrSwapImage = -1;
	m_iPrevSwapImage = -1;
		
	return APP_SUCCESS;
}

int VulkanRenderer::ResizeSwapChain()
{
	vkDeviceWaitIdle( m_vkDevice );

	m_globalPipelineCache.DeleteScreenPipelines();

	// delete old
	if ( m_vkSwapFrameBuffers )
	{
		for( uint32_t i = 0; i < m_iNumSwapImages; i++ ) vkDestroyFramebuffer( m_vkDevice, m_vkSwapFrameBuffers[i], nullptr );
		delete [] m_vkSwapFrameBuffers;
		m_vkSwapFrameBuffers = 0;
	}

	if ( m_vkRenderPasses[AGK_VK_RENDER_PASS_SURFACE] ) vkDestroyRenderPass( m_vkDevice, m_vkRenderPasses[AGK_VK_RENDER_PASS_SURFACE], nullptr );
	if ( m_vkRenderPasses[AGK_VK_RENDER_PASS_SURFACE_NO_CLEAR] ) vkDestroyRenderPass( m_vkDevice, m_vkRenderPasses[AGK_VK_RENDER_PASS_SURFACE_NO_CLEAR], nullptr );
	m_vkRenderPasses[AGK_VK_RENDER_PASS_SURFACE] = 0;
	m_vkRenderPasses[AGK_VK_RENDER_PASS_SURFACE_NO_CLEAR] = 0;

	if ( m_vkSwapImageViews )
	{
		for( uint32_t i = 0; i < m_iNumSwapImages; i++ ) vkDestroyImageView( m_vkDevice, m_vkSwapImageViews[i], nullptr ); // VkImages themselves should be deleted by the swap chain
		delete [] m_vkSwapImageViews;
		m_vkSwapImageViews = 0;
	}

	if ( m_pDefaultDepthImage ) ForceDeleteImage( m_pDefaultDepthImage );
	m_pDefaultDepthImage = 0;

	if ( m_vkSwapImages ) delete [] m_vkSwapImages;
	m_vkSwapImages = 0;

	if ( m_vkSwapChain ) vkDestroySwapchainKHR( m_vkDevice, m_vkSwapChain, nullptr );
	m_vkSwapChain = 0;
	
	// get new surface properties
	uint32_t formatCount = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR( m_vkGPU, m_vkSurface, &formatCount, nullptr );
	if ( formatCount == 0 ) ReportAndReturnErrorID( APP_ERROR_VK_NO_SURFACE_FORMATS ); 
	std::vector<VkSurfaceFormatKHR> formats( formatCount );
	vkGetPhysicalDeviceSurfaceFormatsKHR( m_vkGPU, m_vkSurface, &formatCount, formats.data() );
	m_vkSurfaceFormat.format = VK_FORMAT_UNDEFINED;
	for( uint32_t i = 0; i < formatCount; i++ )
	{
		if ( formats[i].format == VK_FORMAT_B8G8R8A8_UNORM || formats[i].format == VK_FORMAT_R8G8B8A8_UNORM )
		{
			m_vkSurfaceFormat = formats[ i ];
			break;
		}
	}
	
	if ( m_vkSurfaceFormat.format == VK_FORMAT_UNDEFINED )
	{
		AppError( "Failed to find RGBA surface format" );
		return APP_ERROR_GENERAL;
	}

	VkSurfaceCapabilitiesKHR surfaceCaps;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR( m_vkGPU, m_vkSurface, &surfaceCaps );

	m_iSurfaceWidth = surfaceCaps.currentExtent.width;
	m_iSurfaceHeight = surfaceCaps.currentExtent.height;

	switch( m_vkSurfaceFormat.format )
	{
		case VK_FORMAT_B8G8R8A8_UNORM: 
		case VK_FORMAT_R8G8B8A8_UNORM: m_iSurfaceBytesPerPixel = 4; break;
		default: AppError( "Unrecognised surface format" );
	}
	
	if ( m_iSurfaceWidth < 1 || m_iSurfaceHeight < 1 ) 
	{
		m_iFlags &= ~AGK_VULKAN_SURFACE_VALID;
		return APP_SUCCESS;
	}
	else m_iFlags |= AGK_VULKAN_SURFACE_VALID;

	// recreate
	ReturnIfFail( CreateSwapChain(m_iCurrentPresentMode) )
	AGKImgFormat depthFormat;
	if ( IsImageFormatSupported( AGK_DEPTH_FORMAT_32_FLOAT ) ) depthFormat = AGK_DEPTH_FORMAT_32_FLOAT;
	else if ( IsImageFormatSupported( AGK_DEPTH_FORMAT_24_INT ) ) depthFormat = AGK_DEPTH_FORMAT_24_INT;
	else depthFormat = AGK_DEPTH_FORMAT_16_INT;
	ReturnIfFail( CreateDepthBuffer( depthFormat, m_iSurfaceWidth, m_iSurfaceHeight, AGK_VK_IMAGE_IS_DEFAULT, (void**)&m_pDefaultDepthImage ) )
	ReturnIfFail( CreateDefaultRenderPass() )
	ReturnIfFail( CreateDefaultFrameBuffer() )
    
    m_iSurfaceOptimalWarning = 0;
    agk::UpdateDeviceSize( 0 , 0 );
	
	return APP_SUCCESS;
}

int VulkanRenderer::ChangePresentMode( VkPresentModeKHR newMode )
{
	if ( !(m_iFlags & AGK_VULKAN_SURFACE_VALID) ) return APP_SUCCESS;

	vkDeviceWaitIdle( m_vkDevice );

	// delete old
	if ( m_vkSwapFrameBuffers )
	{
		for( uint32_t i = 0; i < m_iNumSwapImages; i++ ) vkDestroyFramebuffer( m_vkDevice, m_vkSwapFrameBuffers[i], nullptr );
		delete [] m_vkSwapFrameBuffers;
		m_vkSwapFrameBuffers = 0;
	}

	if ( m_vkSwapImageViews )
	{
		for( uint32_t i = 0; i < m_iNumSwapImages; i++ ) vkDestroyImageView( m_vkDevice, m_vkSwapImageViews[i], nullptr ); // VkImages themselves should be deleted by the swap chain
		delete [] m_vkSwapImageViews;
		m_vkSwapImageViews = 0;
	}
	
	if ( m_vkSwapImages )
	{
		delete [] m_vkSwapImages;
		m_vkSwapImages = 0;
	}

	if ( m_vkSwapChain ) vkDestroySwapchainKHR( m_vkDevice, m_vkSwapChain, nullptr );
	m_vkSwapChain = 0;

	// recreate
	ReturnIfFail( CreateSwapChain( newMode ) );
	ReturnIfFail( CreateDefaultFrameBuffer() );

	return APP_SUCCESS;
}

int VulkanRenderer::CreateDefaultRenderPass()
{
	VkAttachmentReference depthAttachment = {};
	depthAttachment.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	depthAttachment.attachment = 0;

	VkAttachmentReference colorAttachment = {};
	colorAttachment.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	colorAttachment.attachment = 1;
	
	VkSubpassDescription subPass = {};
	subPass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subPass.pDepthStencilAttachment = &depthAttachment;
	subPass.colorAttachmentCount = 1;
	subPass.pColorAttachments = &colorAttachment;

	VkAttachmentDescription attachments[ 2 ] = {0};
	attachments[ 0 ].format = m_pDefaultDepthImage->m_vkFormat;
	attachments[ 0 ].samples = VK_SAMPLE_COUNT_1_BIT;
	attachments[ 0 ].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachments[ 0 ].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachments[ 0 ].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachments[ 0 ].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachments[ 0 ].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachments[ 0 ].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	
		
	attachments[ 1 ].format = m_vkSurfaceFormat.format;
	attachments[ 1 ].samples = VK_SAMPLE_COUNT_1_BIT;
	attachments[ 1 ].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachments[ 1 ].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachments[ 1 ].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachments[ 1 ].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachments[ 1 ].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachments[ 1 ].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	
	VkSubpassDependency dependencies[ 1 ];
	dependencies[ 0 ] = {};
	dependencies[ 0 ].srcSubpass = VK_SUBPASS_EXTERNAL;
	dependencies[ 0 ].dstSubpass = 0;
	dependencies[ 0 ].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependencies[ 0 ].srcAccessMask = 0;
	dependencies[ 0 ].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependencies[ 0 ].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		
	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 2;
	renderPassInfo.pAttachments = attachments;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subPass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = dependencies;

	VkResult res = vkCreateRenderPass( m_vkDevice, &renderPassInfo, nullptr, &m_vkRenderPasses[ AGK_VK_RENDER_PASS_SURFACE ] );
	if ( VK_SUCCESS != res ) ReportAndReturnErrorID( APP_ERROR_VK_CREATE_RENDER_PASS_FAILED, res );

	attachments[ 1 ].loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;

	res = vkCreateRenderPass( m_vkDevice, &renderPassInfo, nullptr, &m_vkRenderPasses[ AGK_VK_RENDER_PASS_SURFACE_NO_CLEAR ] );
	if ( VK_SUCCESS != res ) ReportAndReturnErrorID( APP_ERROR_VK_CREATE_RENDER_PASS_FAILED, res );

	return APP_SUCCESS;
}

int VulkanRenderer::CreateDefaultFrameBuffer()
{
	m_vkSwapFrameBuffers = new VkFramebuffer[ m_iNumSwapImages ];

	VkImageView attachmentViews[ 2 ] = {};
	attachmentViews[ 0 ] = m_pDefaultDepthImage->GetCurrentFrameBufferView();
		
	VkFramebufferCreateInfo frameBufferInfo = {};
	frameBufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	frameBufferInfo.renderPass = m_vkRenderPasses[ AGK_VK_RENDER_PASS_SURFACE ]; // this frame buffer is compatible with both clearing and non-clearing render passes
	frameBufferInfo.width = m_iSurfaceWidth;
	frameBufferInfo.height = m_iSurfaceHeight;
	frameBufferInfo.layers = 1;
	frameBufferInfo.attachmentCount = 2;
	frameBufferInfo.pAttachments = attachmentViews;

	for( uint32_t i = 0; i < m_iNumSwapImages; i++ )
	{
		attachmentViews[ 1 ] = m_vkSwapImageViews[ i ];
			
		VkResult res = vkCreateFramebuffer( m_vkDevice, &frameBufferInfo, nullptr, &m_vkSwapFrameBuffers[i] );
		if ( VK_SUCCESS != res ) ReportAndReturnErrorID( APP_ERROR_VK_CREATE_FRAMEBUFFER_FAILED, res );
	}

	return APP_SUCCESS;
}

void VulkanRenderer::GetDataFromScreen( uint32_t **pData, int x, int y, int width, int height )
{
	uint32_t dataSize = width * height * m_iSurfaceBytesPerPixel;
	if ( m_iCurrSwapImage < 0 )
	{
		// nothing has been drawn yet, return a blank image
		*pData = (uint32_t*) new uint8_t[ dataSize ];
		memset( *pData, 0, dataSize );
		return;
	}

	// reading back buffer image, must submit everything then wait
	EndFrameAndSubmit( 0 ); // don't signal semaphore as we'll submit again at the end of the frame
	
	// wait for previous frame
	if ( m_iPrevFrame >= 0 ) vkWaitForFences( m_vkDevice, 1, &m_vkFenceFrameFinished[ m_iPrevFrame ], VK_TRUE, UINT64_MAX );

	CleanUpResources();

	// wait for this frame
	vkWaitForFences( m_vkDevice, 1, &m_vkFenceFrameFinished[ m_iCurrentFrame ], VK_TRUE, UINT64_MAX );

	BeginFrame();

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
	if ( VK_SUCCESS != res ) 
	{
		AppError( "Failed to create buffer: %d", res );
		return;
	}

	res = vmaMapMemory( m_vkAllocator, bufferMem, &pMappedMem );
	if ( VK_SUCCESS != res ) 
	{
		AppError( "Failed to map buffer memory: %d", res );
		return;
	}

	VkCommandBufferBeginInfo begin_info = {};
	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	vkBeginCommandBuffer( m_vkGetImageCommandBuffer, &begin_info );

	// transition image to transfer src
	VkImageMemoryBarrier memoryBarrier = {};
	memoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	memoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	memoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	memoryBarrier.image = m_vkSwapImages[ m_iCurrSwapImage ];
	memoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	memoryBarrier.subresourceRange.baseArrayLayer = 0;
	memoryBarrier.subresourceRange.layerCount = 1;
	memoryBarrier.subresourceRange.baseMipLevel = 0; 
	memoryBarrier.subresourceRange.levelCount = 1; // only transfer this level
	memoryBarrier.srcAccessMask = 0; 
	memoryBarrier.oldLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; 
	memoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT; 
	memoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL; 
		
	vkCmdPipelineBarrier( m_vkGetImageCommandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, 0, 0, 0, 1, &memoryBarrier );

	// copy image to buffer
	VkBufferImageCopy region = {};
	region.bufferImageHeight = 0;
	region.bufferRowLength = 0;
	region.bufferOffset = 0;
	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;
	region.imageOffset.x = x;
	region.imageOffset.y = y;
	region.imageExtent.width = width;
	region.imageExtent.height = height;
	region.imageExtent.depth = 1;

	vkCmdCopyImageToBuffer( m_vkGetImageCommandBuffer, m_vkSwapImages[m_iCurrSwapImage], VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, buffer, 1, &region );

	// transition image back to read
	memoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT; 
	memoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL; 
	memoryBarrier.dstAccessMask = 0;
	memoryBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; 
		
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
	*pData = (uint32_t*) new uint8_t[ dataSize ];
	memcpy( *pData, pMappedMem, dataSize );

	// clean up
	vmaUnmapMemory( m_vkAllocator, bufferMem );
	vmaDestroyBuffer( m_vkAllocator, buffer, bufferMem );

	// convert to ABGR
	for ( int y = 0; y < height; y++ )
	{
		uint32_t index = y*width;

		for ( int x = 0; x < width; x++ )
		{
			unsigned char red = ((*pData)[ index ] >> 16) & 0xff;
			unsigned char green = ((*pData)[ index ] >> 8) & 0xff;
			unsigned char blue = (*pData)[ index ] & 0xff;
			unsigned char alpha = ((*pData)[ index ] >> 24) & 0xff;

			(*pData)[ index ] = (alpha << 24) | (blue << 16) | (green << 8) | (red);

			index++;
		}
	}
}

void VulkanRenderer::ClearRenderTarget( float red, float green, float blue, float alpha ) 
{ 
	if ( VulkanFrameBuffer::g_pCurrFrameBuffer ) 
	{
		VulkanFrameBuffer::g_pCurrFrameBuffer->Clear( red, green, blue, alpha );
		CheckRenderPass();
	}
	else 
	{
		m_iFlags |= AGK_VULKAN_SURFACE_CLEARED; 
		m_fClearRed = red; 
		m_fClearGreen = green; 
		m_fClearBlue = blue; 
		m_fClearAlpha = alpha; 
	}
}

void VulkanRenderer::ClearDepthBuffer()
{
	// if the render pass has not yet started then no need to do anything, depth will be cleared automatically
	if ( (m_iFlags & AGK_VULKAN_RENDER_PASS_STARTED) == 0 ) return;

	uint32_t width = m_iSurfaceWidth;
	uint32_t height = m_iSurfaceHeight;

	if ( VulkanFrameBuffer::g_pCurrFrameBuffer )
	{
		if ( !VulkanFrameBuffer::g_pCurrFrameBuffer->m_pDepth ) return;
		width = VulkanFrameBuffer::g_pCurrFrameBuffer->m_iWidth;
		height = VulkanFrameBuffer::g_pCurrFrameBuffer->m_iHeight;
	}
	
	VkClearValue clear = {};
	clear.depthStencil.depth = IsInvertedDepth() ? 0.0f : 1.0f;

	VkClearAttachment attachment = {};
	attachment.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
	attachment.clearValue = clear;

	VkClearRect clearRect = {};
	clearRect.baseArrayLayer = 0;
	clearRect.layerCount = 1;
	clearRect.rect.offset = { 0, 0 };
	clearRect.rect.extent = { width, height };

	vkCmdClearAttachments( m_vkCommandBuffer[m_iCurrentFrame], 1, &attachment, 1, &clearRect );
}

int VulkanRenderer::BeginFrame()
{
	// transfer command buffer for transfers only
	VkCommandBufferBeginInfo begin_info = {};
	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	VkResult res = vkBeginCommandBuffer( m_vkTransferVerticesCommandBuffer[m_iCurrentFrame], &begin_info );
	if ( VK_SUCCESS != res ) ReportAndReturnErrorID( APP_ERROR_VK_BEGIN_CMD_BUFFER_FAILED, res );

	res = vkBeginCommandBuffer( m_vkTransferImagesCommandBuffer[m_iCurrentFrame], &begin_info );
	if ( VK_SUCCESS != res ) ReportAndReturnErrorID( APP_ERROR_VK_BEGIN_CMD_BUFFER_FAILED, res );

	// receive command buffer for transfers
    if ( m_vkReceiveVerticesCommandBuffer[m_iCurrentFrame] != m_vkTransferVerticesCommandBuffer[m_iCurrentFrame] )
    {
        res = vkBeginCommandBuffer( m_vkReceiveVerticesCommandBuffer[m_iCurrentFrame], &begin_info );
        if ( VK_SUCCESS != res ) ReportAndReturnErrorID( APP_ERROR_VK_BEGIN_CMD_BUFFER_FAILED, res );
    }

    if ( m_vkReceiveImagesCommandBuffer[m_iCurrentFrame] != m_vkTransferImagesCommandBuffer[m_iCurrentFrame] )
    {
        res = vkBeginCommandBuffer( m_vkReceiveImagesCommandBuffer[m_iCurrentFrame], &begin_info );
        if ( VK_SUCCESS != res ) ReportAndReturnErrorID( APP_ERROR_VK_BEGIN_CMD_BUFFER_FAILED, res );
    }
        
	// main command buffer for drawing
	res = vkBeginCommandBuffer( m_vkCommandBuffer[m_iCurrentFrame], &begin_info );
	if ( VK_SUCCESS != res ) ReportAndReturnErrorID( APP_ERROR_VK_BEGIN_CMD_BUFFER_FAILED, res );
	
	return APP_SUCCESS;
}

int VulkanRenderer::CheckRenderPass()
{
	if ( m_iFlags & AGK_VULKAN_RENDER_PASS_STARTED ) return APP_SUCCESS;
	
	// acquire swap image if not already done
	if ( m_iCurrSwapImage < 0 && VulkanFrameBuffer::g_pCurrFrameBuffer == 0 )
	{
		if ( !(m_iFlags & AGK_VULKAN_SURFACE_VALID) ) return APP_SUCCESS;

		// acquire new swap image
		VkResult res = vkAcquireNextImageKHR( m_vkDevice, m_vkSwapChain, UINT64_MAX, m_vkSemaphoreAcquired[m_iCurrentFrame], 0, (uint32_t*)&m_iCurrSwapImage );
		if ( VK_SUCCESS != res ) 
		{
			if ( res == VK_ERROR_OUT_OF_DATE_KHR )  
			{
				AppWarning( "Acquire: Swap chain image is out of date" );
				m_iSurfaceResized = 1;
			}
			else if ( res == VK_SUBOPTIMAL_KHR )
			{
				// may happen a lot on Android
				if ( m_iSurfaceOptimalWarning == 0 ) AppWarning( "Acquire: Swap chain image is not optimal" );
				m_iSurfaceOptimalWarning = 1;
			}
			else ReportAndReturnErrorID( APP_ERROR_VK_ACQUIRE_IMAGE_FAILED, res );
		}

		m_iFlags |= AGK_VULKAN_WAIT_ON_ACQUIRE_SEMAPHORE;
	}

	uint32_t width = m_iSurfaceWidth;
	uint32_t height = m_iSurfaceHeight;
	uint32_t hasColor = 0;
	uint32_t hasDepth = 0;
	uint32_t renderPassIndex = AGK_VK_RENDER_PASS_SURFACE;
	VkFramebuffer frameBuffer = 0;
	if ( !(m_iFlags & AGK_VULKAN_SURFACE_CLEARED) ) renderPassIndex = AGK_VK_RENDER_PASS_SURFACE_NO_CLEAR;

	if ( VulkanFrameBuffer::g_pCurrFrameBuffer ) 
	{
		width = VulkanFrameBuffer::g_pCurrFrameBuffer->m_iWidth;
		height = VulkanFrameBuffer::g_pCurrFrameBuffer->m_iHeight;
		renderPassIndex = VulkanFrameBuffer::g_pCurrFrameBuffer->m_iRenderPassIndex;
		frameBuffer = VulkanFrameBuffer::g_pCurrFrameBuffer->m_vkFrameBuffer;
		VulkanFrameBuffer::g_pCurrFrameBuffer->m_iFlags |= AGK_VK_FRAMEBUFFER_AWAITING_USE;

		if ( VulkanFrameBuffer::g_pCurrFrameBuffer->m_pColor )
		{
			InternalImageBarrier( m_vkCommandBuffer[m_iCurrentFrame], VulkanFrameBuffer::g_pCurrFrameBuffer->m_pColor, AGK_VK_IMAGE_MODE_DRAW );
			VulkanFrameBuffer::g_pCurrFrameBuffer->m_pColor->WillRender();
			hasColor = 1;
		}

		if ( VulkanFrameBuffer::g_pCurrFrameBuffer->m_pDepth )
		{
			// shadow framebuffers will transition their images before binding
			if ( !VulkanFrameBuffer::g_pCurrFrameBuffer->IsShadow() ) InternalImageBarrier( m_vkCommandBuffer[m_iCurrentFrame], VulkanFrameBuffer::g_pCurrFrameBuffer->m_pDepth, AGK_VK_IMAGE_MODE_DRAW );
			VulkanFrameBuffer::g_pCurrFrameBuffer->m_pDepth->WillRender();
			hasDepth = 1;
		}

		if ( !VulkanFrameBuffer::g_pCurrFrameBuffer->IsCleared() )
		{
			switch( renderPassIndex )
			{
				case AGK_VK_RENDER_PASS_RGBA_DEPTH: renderPassIndex = AGK_VK_RENDER_PASS_RGBA_DEPTH_NO_CLEAR; break;
				case AGK_VK_RENDER_PASS_RGBA_DEPTH_STORE: renderPassIndex = AGK_VK_RENDER_PASS_RGBA_DEPTH_STORE_NO_CLEAR; break;
				case AGK_VK_RENDER_PASS_RGBA: renderPassIndex = AGK_VK_RENDER_PASS_RGBA_NO_CLEAR; break;
			}
		}

		VulkanFrameBuffer::g_pCurrFrameBuffer->ResetClear();
	}
	else 
	{
		frameBuffer = m_vkSwapFrameBuffers[ m_iCurrSwapImage ];
		m_iFlags &= ~AGK_VULKAN_SURFACE_CLEARED;
		hasDepth = 1;
		hasColor = 1;
	}

	// begin render pass
	VkRect2D renderArea = {};
	renderArea.offset = {0,0};
	renderArea.extent.width = width;
	renderArea.extent.height = height;

	// color and depth clear values must be in the same order as those in the render pass
	VkClearValue clearValues[ 2 ] = {};
	uint32_t numClearValues = 0;

	if ( hasDepth )
	{
		clearValues[ numClearValues ].depthStencil.depth = IsInvertedDepth() ? 0.0f : 1.0f;
		numClearValues++;
	}

	if ( hasColor )
	{
		if ( VulkanFrameBuffer::g_pCurrFrameBuffer ) 
		{
			clearValues[ numClearValues ].color.float32[ 0 ] = VulkanFrameBuffer::g_pCurrFrameBuffer->m_fClearRed;
			clearValues[ numClearValues ].color.float32[ 1 ] = VulkanFrameBuffer::g_pCurrFrameBuffer->m_fClearGreen;
			clearValues[ numClearValues ].color.float32[ 2 ] = VulkanFrameBuffer::g_pCurrFrameBuffer->m_fClearBlue;
			clearValues[ numClearValues ].color.float32[ 3 ] = VulkanFrameBuffer::g_pCurrFrameBuffer->m_fClearAlpha;
		}
		else
		{
			clearValues[ numClearValues ].color.float32[ 0 ] = m_fClearRed;
			clearValues[ numClearValues ].color.float32[ 1 ] = m_fClearGreen;
			clearValues[ numClearValues ].color.float32[ 2 ] = m_fClearBlue;
			clearValues[ numClearValues ].color.float32[ 3 ] = m_fClearAlpha;
		}
		numClearValues++;
	}

	VkRenderPassBeginInfo renderBeginInfo = {};
	renderBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderBeginInfo.renderPass = m_vkRenderPasses[ renderPassIndex ];
	renderBeginInfo.framebuffer = frameBuffer;
	renderBeginInfo.renderArea = renderArea;
	renderBeginInfo.pClearValues = clearValues;
	renderBeginInfo.clearValueCount = numClearValues;
	vkCmdBeginRenderPass( m_vkCommandBuffer[m_iCurrentFrame], &renderBeginInfo, VK_SUBPASS_CONTENTS_INLINE );

	m_iFlags |= AGK_VULKAN_RENDER_PASS_STARTED;
	
	return APP_SUCCESS;
}

int VulkanRenderer::EndRenderPass()
{
	if ( !(m_iFlags & AGK_VULKAN_RENDER_PASS_STARTED) ) return APP_SUCCESS;
	
	vkCmdEndRenderPass( m_vkCommandBuffer[m_iCurrentFrame] );
	m_iFlags &= ~AGK_VULKAN_RENDER_PASS_STARTED;

	if ( VulkanFrameBuffer::g_pCurrFrameBuffer )
	{
		VulkanImage *pColor = VulkanFrameBuffer::g_pCurrFrameBuffer->m_pColor;
		VulkanImage *pDepth = VulkanFrameBuffer::g_pCurrFrameBuffer->m_pDepth;

		if ( pColor )
		{
			if ( pColor->WillGenerateMipmaps() ) InternalImageGenerateFramebufferMipmaps( pColor, pColor->GetCurrImageData() );
			else InternalImageBarrier( m_vkCommandBuffer[m_iCurrentFrame], pColor, AGK_VK_IMAGE_MODE_READ );
		}

		if ( pDepth && !VulkanFrameBuffer::g_pCurrFrameBuffer->IsLocalDepth() )
		{
			if ( pDepth->WillGenerateMipmaps() ) InternalImageGenerateFramebufferMipmaps( pDepth, pDepth->GetCurrImageData() );
			else if ( !VulkanFrameBuffer::g_pCurrFrameBuffer->IsShadow() ) InternalImageBarrier( m_vkCommandBuffer[m_iCurrentFrame], pDepth, AGK_VK_IMAGE_MODE_READ );
			// shadow framebuffers will defer their barriers until all shadow maps are complete
		}
	}
		
	return APP_SUCCESS;
}

int VulkanRenderer::Present()
{
	if ( m_iPrevSwapImage >= 0 )
	{
		uint32_t numSemaphores = 1;
		//VkSemaphore waitSemaphores[ 2 ] = { m_vkSemaphoreAcquired[m_iPrevFrame], m_vkSemaphoreSubmitted[m_iPrevFrame] };
		VkSemaphore waitSemaphores[ 1 ] = { m_vkSemaphoreSubmitted[m_iPrevFrame] };

		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &m_vkSwapChain;
		presentInfo.pImageIndices = (uint32_t*)&m_iPrevSwapImage;
		presentInfo.pResults = nullptr;
		presentInfo.waitSemaphoreCount = numSemaphores;
		presentInfo.pWaitSemaphores = waitSemaphores;

		VkResult res = vkQueuePresentKHR( m_vkQueue, &presentInfo );
		if ( VK_SUCCESS != res ) 
		{
			if ( res == VK_ERROR_OUT_OF_DATE_KHR ) 
			{
				AppWarning( "Present: Swap chain is out of date" );
				m_iSurfaceResized = 1;
			}
			else if ( res == VK_SUBOPTIMAL_KHR ) 
			{
				// this seems to happen a lot on Android
				if ( m_iSurfaceOptimalWarning == 0 ) AppWarning( "Present: Swap chain is not optimal" );
				m_iSurfaceOptimalWarning = 1;
                
                VkSurfaceCapabilitiesKHR surfaceCaps;
                vkGetPhysicalDeviceSurfaceCapabilitiesKHR( m_vkGPU, m_vkSurface, &surfaceCaps );
                uint32_t width = surfaceCaps.currentExtent.width;
                uint32_t height = surfaceCaps.currentExtent.height;
                if ( m_iSurfaceWidth != width || m_iSurfaceHeight != height ) m_iSurfaceResized = 1;
			}
			else ReportAndReturnErrorID( APP_ERROR_VK_QUEUE_PRESENT_FAILED, res );
		}
	}

	m_iPrevSwapImage = -1;
    
	return APP_SUCCESS;
}

void VulkanRenderer::CleanUpResources()
{
	CleanUpImages();
	CleanUpVertices();
	CleanUpShaders();
	CleanUpFrameBuffers();
	m_globalPipelineCache.CleanUpPipelines();
}

int VulkanRenderer::Swap() 
{
	if ( m_iFlags & AGK_VULKAN_SURFACE_VALID ) 
	{
		// if previous present was delayed then present it now
		if ( m_iPresentMode == AGK_PRESENT_MODE_DELAYED ) Present();

		// check if back buffer is valid
		if ( m_iCurrSwapImage == -1 )
		{
			AGKFrameBuffer *pCurrFB = AGKFrameBuffer::g_pCurrFrameBuffer;
			if ( pCurrFB != 0 ) AGKFrameBuffer::BindFrameBuffer( 0 );

			// clear the back buffer
			CheckRenderPass();

			if ( pCurrFB ) AGKFrameBuffer::BindFrameBuffer( pCurrFB );
		}
			
		// end current frame
		EndFrameAndSubmit();

		m_iPrevSwapImage = m_iCurrSwapImage;
		m_iCurrSwapImage = -1;
		m_iPrevFrame = m_iCurrentFrame;
		m_iCurrentFrame = (m_iCurrentFrame + 1) % AGK_VK_NUM_FRAMES;

		if ( m_iPresentMode == AGK_PRESENT_MODE_IMMEDIATE || m_iSurfaceResized || m_iCurrVSyncMode != m_iNewVSyncMode ) Present();

		// wait for start of next frame, if we have already waited above then it should return immediately
		vkWaitForFences( m_vkDevice, 1, &m_vkFenceFrameFinished[m_iCurrentFrame], VK_TRUE, UINT64_MAX );
	
		// now safe to change currentFrame resources
		CleanUpResources();

		// check if swapchain needs recreating
		if ( m_iSurfaceResized )
		{
			if ( m_iCurrVSyncMode != m_iNewVSyncMode ) m_iCurrentPresentMode = (m_iNewVSyncMode == 0) ? VK_PRESENT_MODE_IMMEDIATE_KHR : VK_PRESENT_MODE_FIFO_KHR;
			ResizeSwapChain();
		}
		else if ( m_iCurrVSyncMode != m_iNewVSyncMode )
		{
			if ( m_iNewVSyncMode == 0 ) ChangePresentMode( VK_PRESENT_MODE_IMMEDIATE_KHR );
			else ChangePresentMode( VK_PRESENT_MODE_FIFO_KHR );
		}
			
		m_iSurfaceResized = 0;
		m_iCurrVSyncMode = m_iNewVSyncMode;

		// begin next frame
		BeginFrame();
	}
	else
	{
		// if no surface then discard everthing we just tried to draw

		VkResult res = vkResetCommandBuffer( m_vkTransferVerticesCommandBuffer[m_iCurrentFrame], 0 );
		if ( VK_SUCCESS != res ) ReportAndReturnErrorID( APP_ERROR_GENERAL, "Failed to reset command buffer" );

		res = vkResetCommandBuffer( m_vkTransferImagesCommandBuffer[m_iCurrentFrame], 0 );
		if ( VK_SUCCESS != res ) ReportAndReturnErrorID( APP_ERROR_GENERAL, "Failed to reset command buffer" );

		// receive command buffer for transfers
        if ( m_vkReceiveVerticesCommandBuffer[m_iCurrentFrame] != m_vkTransferVerticesCommandBuffer[m_iCurrentFrame] )
        {
            res = vkResetCommandBuffer( m_vkReceiveVerticesCommandBuffer[m_iCurrentFrame], 0 );
            if ( VK_SUCCESS != res ) ReportAndReturnErrorID( APP_ERROR_GENERAL, "Failed to reset command buffer" );
        }

        if ( m_vkReceiveImagesCommandBuffer[m_iCurrentFrame] != m_vkTransferImagesCommandBuffer[m_iCurrentFrame] )
        {
            res = vkResetCommandBuffer( m_vkReceiveImagesCommandBuffer[m_iCurrentFrame], 0 );
            if ( VK_SUCCESS != res ) ReportAndReturnErrorID( APP_ERROR_GENERAL, "Failed to reset command buffer" );
        }

		// main command buffer for drawing
		res = vkResetCommandBuffer( m_vkCommandBuffer[m_iCurrentFrame], 0 );
		if ( VK_SUCCESS != res ) ReportAndReturnErrorID( APP_ERROR_GENERAL, "Failed to reset command buffer" );

		// wait for start of next frame, if we have already waited above then it should return immediately
		vkWaitForFences( m_vkDevice, 1, &m_vkFenceFrameFinished[m_iCurrentFrame], VK_TRUE, UINT64_MAX );
	
		// now safe to change currentFrame resources
		CleanUpResources();

		BeginFrame();
	}
		
	return APP_SUCCESS;
}

void VulkanRenderer::SetPresentMode( uint32_t mode ) 
{ 
	if ( mode > 1 ) 
	{
		agk::Error( "Invalid present mode, ust be 0 or 1" );
		return;
	}

	if ( m_iPresentMode != mode )
	{
		ForcePresent();
	}

	m_iPresentMode = mode; 
}

int VulkanRenderer::ForcePresent() 
{ 
	int result = Present();
	if ( result != VK_SUCCESS ) return result;

	// check if swapchain needs recreating
	if ( m_iSurfaceResized )
	{
		if ( m_iCurrVSyncMode != m_iNewVSyncMode ) m_iCurrentPresentMode = (m_iNewVSyncMode == 0) ? VK_PRESENT_MODE_IMMEDIATE_KHR : VK_PRESENT_MODE_FIFO_KHR;
		ResizeSwapChain();
	}
	else if ( m_iCurrVSyncMode != m_iNewVSyncMode )
	{
		if ( m_iNewVSyncMode == 0 ) ChangePresentMode( VK_PRESENT_MODE_IMMEDIATE_KHR );
		else ChangePresentMode( VK_PRESENT_MODE_FIFO_KHR );
	}
		
	m_iSurfaceResized = 0;
	m_iCurrVSyncMode = m_iNewVSyncMode;

	return VK_SUCCESS;
}

int VulkanRenderer::EndFrameAndSubmit( uint32_t signalSemaphore )
{
	EndRenderPass();

	VkResult res;

	// transfer any necessary vertices
	int hasVertTransfers = 0;
	for( uint32_t i = 0; i < m_pVerticesWithTransfers.NumItems(); i++ )
	{
		VulkanVertices *pVertices = m_pVerticesWithTransfers[ i ];
		if ( pVertices->HasTransfers() ) 
		{
			InternalTransferVertices( pVertices );
			pVertices->m_iFlags &= ~AGK_VK_VERTICES_HAS_TRANSFERS;
			hasVertTransfers = 1;
		}
	}
	m_pVerticesWithTransfers.Clear();

	vkEndCommandBuffer( m_vkTransferVerticesCommandBuffer[m_iCurrentFrame] );
	
	if ( hasVertTransfers && m_iTransferQueueFamilyIndex != m_iQueueFamilyIndex )
	{
		VkSubmitInfo transfer_submit_info = {};
		transfer_submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		transfer_submit_info.commandBufferCount = 1;
		transfer_submit_info.pCommandBuffers = &m_vkTransferVerticesCommandBuffer[m_iCurrentFrame];
		transfer_submit_info.signalSemaphoreCount = 1;
		transfer_submit_info.pSignalSemaphores = &m_vkSemaphoreTransferredVertices[m_iCurrentFrame];
		vkQueueSubmit( m_vkTransferQueue, 1, &transfer_submit_info, 0 );
	}

	// transfer any necessary images
	int hasImageTransfers = 0;
	for( uint32_t i = 0; i < m_pImagesWithTransfers.NumItems(); i++ )
	{
		VulkanImage *pImage = m_pImagesWithTransfers[ i ];
		if ( pImage->HasTransfers() ) 
		{
			InternalTransferImage( pImage );
			pImage->m_iFlags &= ~AGK_VK_IMAGE_HAS_TRANSFERS;
			hasImageTransfers = 1;
		}
	}
	m_pImagesWithTransfers.Clear();

	vkEndCommandBuffer( m_vkTransferImagesCommandBuffer[m_iCurrentFrame] );

    if ( hasImageTransfers && m_iTransferQueueFamilyIndex != m_iQueueFamilyIndex )
    {
		VkSubmitInfo transfer_submit_info = {};
		transfer_submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		transfer_submit_info.commandBufferCount = 1;
		transfer_submit_info.pCommandBuffers = &m_vkTransferImagesCommandBuffer[m_iCurrentFrame];
		transfer_submit_info.signalSemaphoreCount = 1;
		transfer_submit_info.pSignalSemaphores = &m_vkSemaphoreTransferredImages[m_iCurrentFrame];
		vkQueueSubmit( m_vkTransferQueue, 1, &transfer_submit_info, 0 );
	}
	
	// end command buffer, stops recording
	res = vkEndCommandBuffer( m_vkCommandBuffer[m_iCurrentFrame] );
	if ( VK_SUCCESS != res ) ReportAndReturnErrorID( APP_ERROR_VK_END_CMD_BUFFER_FAILED, res );

    if ( m_iTransferQueueFamilyIndex != m_iQueueFamilyIndex )
    {
        res = vkEndCommandBuffer( m_vkReceiveVerticesCommandBuffer[m_iCurrentFrame] );
        if ( VK_SUCCESS != res ) ReportAndReturnErrorID( APP_ERROR_VK_END_CMD_BUFFER_FAILED, res );

        res = vkEndCommandBuffer( m_vkReceiveImagesCommandBuffer[m_iCurrentFrame] );
        if ( VK_SUCCESS != res ) ReportAndReturnErrorID( APP_ERROR_VK_END_CMD_BUFFER_FAILED, res );
    }
        
	// work out which semaphores to wait for
	VkPipelineStageFlags waitStages[ 3 ];
	VkSemaphore waitSemaphores[ 3 ];
	uint32_t numSemaphores = 0;
		
	// buffers must be in this order
	VkCommandBuffer buffers[3];
	int numBuffers = 0;
	if ( hasVertTransfers ) 
	{
		buffers[numBuffers] = m_vkReceiveVerticesCommandBuffer[m_iCurrentFrame];
		numBuffers++;

        if ( m_iTransferQueueFamilyIndex != m_iQueueFamilyIndex )
        {
            waitStages[ numSemaphores ] = VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
            if ( m_iFrameHasGraphicsVertexTransfers ) waitStages[ numSemaphores ] |= VK_PIPELINE_STAGE_TRANSFER_BIT;
            waitSemaphores[ numSemaphores ] = m_vkSemaphoreTransferredVertices[ m_iCurrentFrame ];
            numSemaphores++;
        }
	}

	if ( hasImageTransfers )
	{
		buffers[numBuffers] = m_vkReceiveImagesCommandBuffer[m_iCurrentFrame];
		numBuffers++;

        if ( m_iTransferQueueFamilyIndex != m_iQueueFamilyIndex )
        {
            waitStages[ numSemaphores ] = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            if ( m_iFrameHasGraphicsImageTransfers ) waitStages[ numSemaphores ] |= VK_PIPELINE_STAGE_TRANSFER_BIT;
            waitSemaphores[ numSemaphores ] = m_vkSemaphoreTransferredImages[ m_iCurrentFrame ];
            numSemaphores++;
        }
	}

	buffers[numBuffers] = m_vkCommandBuffer[m_iCurrentFrame];
	numBuffers++;

	if ( m_iFlags & AGK_VULKAN_WAIT_ON_ACQUIRE_SEMAPHORE )
	{
		waitStages[ numSemaphores ] = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		waitSemaphores[ numSemaphores ] = m_vkSemaphoreAcquired[ m_iCurrentFrame ];
		numSemaphores++;
		m_iFlags &= ~AGK_VULKAN_WAIT_ON_ACQUIRE_SEMAPHORE;
	}

	if ( m_iImagesWaitForPrevFrame || m_iVerticesWaitForPrevFrame ) 
	{
		// wait for previous frame
		if ( m_iPrevFrame >= 0 ) vkWaitForFences( m_vkDevice, 1, &m_vkFenceFrameFinished[m_iPrevFrame], VK_TRUE, UINT64_MAX );
		m_iImagesWaitForPrevFrame = 0;
		m_iVerticesWaitForPrevFrame = 0;
	}

	VkSubmitInfo submit_info = {};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.commandBufferCount = numBuffers;
	submit_info.pCommandBuffers = buffers;
	submit_info.signalSemaphoreCount = signalSemaphore ? 1 : 0;
	submit_info.pSignalSemaphores = signalSemaphore ? &m_vkSemaphoreSubmitted[m_iCurrentFrame] : nullptr;
	submit_info.waitSemaphoreCount = numSemaphores;
	submit_info.pWaitSemaphores = waitSemaphores;
	submit_info.pWaitDstStageMask = waitStages;

	// don't move this higher in case we return from this function early
	vkResetFences( m_vkDevice, 1, &m_vkFenceFrameFinished[m_iCurrentFrame] );

	res = vkQueueSubmit( m_vkQueue, 1, &submit_info, m_vkFenceFrameFinished[ m_iCurrentFrame ] );
	if ( VK_SUCCESS != res ) ReportAndReturnErrorID( APP_ERROR_VK_QUEUE_SUBMIT_FAILED, res );
						
	return APP_SUCCESS;
}

void VulkanRenderer::CleanUp() 
{
	Present();
	vkDeviceWaitIdle( m_vkDevice );

	if ( m_bEnableValidationLayers )
	{
		if ( vkDestroyDebugReportCallbackEXT != nullptr ) vkDestroyDebugReportCallbackEXT( m_vkInstance, pDebugCallback, nullptr );
	}

	for( int i = 0; i < AGK_VK_NUM_FRAMES; i++ )
	{
		vkDestroySemaphore( m_vkDevice, m_vkSemaphoreAcquired[i], nullptr );
		vkDestroySemaphore( m_vkDevice, m_vkSemaphoreSubmitted[i], nullptr );
		vkDestroySemaphore( m_vkDevice, m_vkSemaphoreReceivedImages[i], nullptr );
		vkDestroySemaphore( m_vkDevice, m_vkSemaphoreReceivedVertices[i], nullptr );
		vkDestroySemaphore( m_vkDevice, m_vkSemaphoreTransferredImages[i], nullptr );
		vkDestroySemaphore( m_vkDevice, m_vkSemaphoreTransferredVertices[i], nullptr );

		vkDestroyFence( m_vkDevice, m_vkFenceFrameFinished[i], nullptr );
	}

	vkDestroyFence( m_vkDevice, m_vkFenceGetImageFinished, nullptr );

	vkDestroyCommandPool( m_vkDevice, m_vkGraphicsCommandPool, nullptr );
	vkDestroyCommandPool( m_vkDevice, m_vkTransferCommandPool, nullptr );

	if ( m_vkSwapFrameBuffers )
	{
		for( uint32_t i = 0; i < m_iNumSwapImages; i++ ) vkDestroyFramebuffer( m_vkDevice, m_vkSwapFrameBuffers[i], nullptr );
		delete [] m_vkSwapFrameBuffers;
		m_vkSwapFrameBuffers = 0;
	}

	for( uint32_t i = 0; i < AGK_VK_RENDER_PASS_NUM; i++ ) if ( m_vkRenderPasses[i] ) vkDestroyRenderPass( m_vkDevice, m_vkRenderPasses[i], nullptr );

	if ( m_vkSwapImageViews )
	{
		for( uint32_t i = 0; i < m_iNumSwapImages; i++ ) vkDestroyImageView( m_vkDevice, m_vkSwapImageViews[i], nullptr );
		delete [] m_vkSwapImageViews;
		m_vkSwapImageViews = 0;
	}

	if ( m_pDefaultDepthImage ) ForceDeleteImage( m_pDefaultDepthImage );

	if ( m_vkSwapImages )
	{
		delete [] m_vkSwapImages;
		m_vkSwapImages = 0;
	}

	while( VulkanImage::g_pAllImages )
	{
		ForceDeleteImage( VulkanImage::g_pAllImages ); // destructor will modify g_pAllImages
	}

	while( VulkanVertices::g_pAllVertexObjects )
	{
		ForceDeleteVertices( VulkanVertices::g_pAllVertexObjects ); // destructor will modify g_pAllVertexObjects
	}

	while( VulkanShaderConstants::g_pAllShaderConstants )
	{
		ForceDeleteShaderConstants( VulkanShaderConstants::g_pAllShaderConstants ); // destructor will modify g_pAllShaderConstants
	}

	while( VulkanShader::g_pAllShaderObjects )
	{
		ForceDeleteShader( VulkanShader::g_pAllShaderObjects ); // destructor will modify g_pAllShaderObjects
	}

	while( VulkanFrameBuffer::g_pAllFrameBuffers )
	{
		ForceDeleteFrameBuffer( VulkanFrameBuffer::g_pAllFrameBuffers ); // destructor will modify g_pAllFrameBuffers
	}

	g_pSamplers.Clear();
	m_globalPipelineCache.DeleteAll();

	if ( m_vkSwapChain ) vkDestroySwapchainKHR( m_vkDevice, m_vkSwapChain, nullptr );
	if ( m_vkSurface ) vkDestroySurfaceKHR( m_vkInstance, m_vkSurface, nullptr );

	vmaDestroyPool( m_vkAllocator, m_vmaBufferTransferPool );
	vmaDestroyPool( m_vkAllocator, m_vmaVertexBufferPool );

	vmaDestroyAllocator( m_vkAllocator );
	vkDestroyDevice( m_vkDevice, nullptr );
	vkDestroyInstance( m_vkInstance, nullptr );

	if ( m_pDeviceExtensions ) delete [] m_pDeviceExtensions;
	if ( m_pInstanceExtensions ) delete [] m_pInstanceExtensions;
}

void VulkanRenderer::SetVSync( int mode )
{
	m_iNewVSyncMode = mode; // will be acted upon at the end of the current frame
}

#endif // ALLOW_VULKAN
