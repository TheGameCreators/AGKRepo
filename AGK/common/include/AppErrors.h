#ifndef _H_AGK_ERRORS_
#define _H_AGK_ERRORS_

typedef enum AppError
{
	APP_SUCCESS = 0,
	APP_ERROR_UNKNOWN,
	APP_ERROR_GENERAL,
	APP_ERROR_VULKAN_DLL_NOT_FOUND,
	APP_ERROR_VK_PROCADDR_NOT_DEF,
	APP_ERROR_VK_INSTANCE_NOT_DEF,
	APP_ERROR_VK_FUNC_NOT_FOUND,
	APP_ERROR_VK_ICD_NOT_FOUND,
	APP_ERROR_CREATE_WINDOW_FAILED,
	APP_ERROR_VK_CREATEINSTANCE_FAILED,
	APP_ERROR_VK_DEVICE_NOT_GRAPHICS,
	APP_ERROR_VK_CREATE_DEVICE_FAILED,
	APP_ERROR_VK_CREATE_ALLOCATOR_FAILED,
	APP_ERROR_VK_CREATE_WIN32_SURFACE_FAILED,
	APP_ERROR_VK_NO_SURFACE_FORMATS,
	APP_ERROR_VK_CREATE_SWAP_FAILED,
	APP_ERROR_VK_SURFACE_SUPPORT_FAILED,
	APP_ERROR_VK_SURFACE_NOT_SUPPORTED,
	APP_ERROR_VK_GET_SWAP_IMAGES_FAILED,
	APP_ERROR_VK_GET_IMAGE_VIEW_FAILED,
	APP_ERROR_VK_NO_DEPTH_FORMAT,
	APP_ERROR_VK_CREATE_IMAGE_FAILED,
	APP_ERROR_VK_MEM_TYPE_NOT_FOUND,
	APP_ERROR_VK_MEM_ALLOCATE_FAILED,
	APP_ERROR_VK_MEM_IMAGE_BIND_FAILED,
	APP_ERROR_VK_CREATE_RENDER_PASS_FAILED,
	APP_ERROR_VK_CREATE_FRAMEBUFFER_FAILED,
	APP_ERROR_VK_ACQUIRE_IMAGE_FAILED,
	APP_ERROR_VK_QUEUE_PRESENT_FAILED,
	APP_ERROR_VK_PRESENT_FAILED,
	APP_ERROR_VK_BEGIN_CMD_BUFFER_FAILED,
	APP_ERROR_VK_END_CMD_BUFFER_FAILED,
	APP_ERROR_VK_QUEUE_SUBMIT_FAILED,
	APP_ERROR_VK_IMAGE_FORMAT_NOT_SUPPORTED,
	APP_ERROR_VK_IMAGE_FORMAT_NOT_RENDERABLE,
	APP_ERROR_VK_FAILED_TO_LOAD_SHADER,
	APP_ERROR_VK_FAILED_TO_COMPILE_SHADER,
	APP_ERROR_VK_CREATE_SHADER_FAILED,
	APP_ERROR_VK_CREATE_PIPELINE_LAYOUT_FAILED,
	APP_ERROR_VK_CREATE_PIPELINE_FAILED,
	APP_ERROR_VK_CREATE_DESCRIPTOR_POOL_FAILED,
	APP_ERROR_VK_ALLOCATE_DESCRIPTOR_SET_FAILED,
	APP_ERROR_VK_ALLOCATE_COMMAND_BUFFER_FAILED,
	APP_ERROR_VK_CREATE_BUFFER_FAILED,
	APP_ERROR_VK_MEM_BUFFER_BIND_FAILED,
	APP_ERROR_VK_MAP_MEM_FAILED,
	APP_ERROR_VK_TOO_MANY_ALLOCATIONS,
	APP_ERROR_VK_DEVICE_NO_TRANSFER_QUEUE,
	APP_ERROR_VK_CREATE_DESCRIPTOR_LAYOUT_FAILED,
	APP_ERROR_VK_CREATE_SAMPLER_FAILED,
	APP_ERROR_VK_IMAGE_DATA_OUT_OF_BOUNDS,
	APP_ERROR_VK_INVALID_IMAGE_MIP_LEVEL,
	APP_ERROR_VK_INVALID_IMAGE_DATA,
	APP_ERROR_VK_VERTEX_UPDATE_OUT_OF_BOUNDS,
	APP_ERROR_VK_INDEX_UPDATE_OUT_OF_BOUNDS,
	APP_ERROR_UNRECOGNISED_ATTRIBUTE_FORMAT,
	APP_ERROR_GL2_CREATE_FRAMEBUFFER_FAILED,
	APP_ERROR_GL2_NO_SHADER_BOUND,
	APP_ERROR_GL2_NO_VERTEX_LAYOUT,

	APP_ERROR_IMAGE_FORMAT_NOT_RECOGNISED,

	APP_ERROR_NUM,
	APP_ERROR_MAX = 0x7FFFFFFF
} AppError;

typedef struct AppErrorDesc
{
	AppError ID;
	const char* szDesc;
} AppErrorDesc;

const AppErrorDesc g_AppErrorDescs[] = 
{
	{ APP_SUCCESS, "SUCCESS" },
	{ APP_ERROR_UNKNOWN, "Unknown error" },
	{ APP_ERROR_GENERAL, "%s" },
	{ APP_ERROR_VULKAN_DLL_NOT_FOUND, "Vulkan DLL not found" },
	{ APP_ERROR_VK_PROCADDR_NOT_DEF, "Vulkan vkGetInstanceProcAddr undefined" },
	{ APP_ERROR_VK_INSTANCE_NOT_DEF, "Vulakn instance undefined" },
	{ APP_ERROR_VK_FUNC_NOT_FOUND, "Vulkan function %s not found" },
	{ APP_ERROR_VK_ICD_NOT_FOUND, "Cannot find a compatible Vulkan ICD" },
	{ APP_ERROR_CREATE_WINDOW_FAILED, "Create window failed" },
	{ APP_ERROR_VK_CREATEINSTANCE_FAILED, "vkCreateInstance error: %d" },
	{ APP_ERROR_VK_DEVICE_NOT_GRAPHICS, "Vulkan device does not support graphics" },
	{ APP_ERROR_VK_CREATE_DEVICE_FAILED, "vkCreateDevice error: %d" },
	{ APP_ERROR_VK_CREATE_ALLOCATOR_FAILED, "vmaCreateAllocator error: %d" },
	{ APP_ERROR_VK_CREATE_WIN32_SURFACE_FAILED, "vkCreateSurface error: %d" },
	{ APP_ERROR_VK_NO_SURFACE_FORMATS, "No surface formats found" },
	{ APP_ERROR_VK_CREATE_SWAP_FAILED, "vkCreateSwapchainKHR error: %d" },
	{ APP_ERROR_VK_SURFACE_SUPPORT_FAILED, "vkGetPhysicalDeviceSurfaceSupportKHR error: %d" },
	{ APP_ERROR_VK_SURFACE_NOT_SUPPORTED, "Window surface is not supported by the device"},
	{ APP_ERROR_VK_GET_SWAP_IMAGES_FAILED, "vkGetSwapchainImagesKHR error: %d" },
	{ APP_ERROR_VK_GET_IMAGE_VIEW_FAILED, "vkCreateImageView error: %d" },
	{ APP_ERROR_VK_NO_DEPTH_FORMAT, "Could not find a suitable depth buffer format" },
	{ APP_ERROR_VK_CREATE_IMAGE_FAILED, "vkCreateImage error: %d" },
	{ APP_ERROR_VK_MEM_TYPE_NOT_FOUND, "Could not find suitable memory location for requirements %d" },
	{ APP_ERROR_VK_MEM_ALLOCATE_FAILED, "vkAllocateMemory error: %d" },
	{ APP_ERROR_VK_MEM_IMAGE_BIND_FAILED, "vkBindImageMemory error: %d" },
	{ APP_ERROR_VK_CREATE_RENDER_PASS_FAILED, "vkCreateRenderPass error: %d" },
	{ APP_ERROR_VK_CREATE_FRAMEBUFFER_FAILED, "vkCreateFramebuffer error: %d" },
	{ APP_ERROR_VK_ACQUIRE_IMAGE_FAILED, "vkAcquireNextImageKHR error: %d" },
	{ APP_ERROR_VK_QUEUE_PRESENT_FAILED, "vkQueuePresentKHR error: %d" },
	{ APP_ERROR_VK_PRESENT_FAILED, "Queue present error: %d" },
	{ APP_ERROR_VK_BEGIN_CMD_BUFFER_FAILED, "vkBeginCommandBuffer error: %d" },
	{ APP_ERROR_VK_END_CMD_BUFFER_FAILED, "vkEndCommandBuffer error: %d" },
	{ APP_ERROR_VK_QUEUE_SUBMIT_FAILED, "vkQueueSubmit error: %d" },
	{ APP_ERROR_VK_IMAGE_FORMAT_NOT_SUPPORTED, "Image format is not supported" },
	{ APP_ERROR_VK_IMAGE_FORMAT_NOT_RENDERABLE, "Image format cannot be used as a render target" },
	{ APP_ERROR_VK_FAILED_TO_LOAD_SHADER, "Failed to load shader \"%s\": %s" },
	{ APP_ERROR_VK_FAILED_TO_COMPILE_SHADER, "Failed to compile shader \"%s\": %s" },
	{ APP_ERROR_VK_CREATE_SHADER_FAILED, "vkCreateShader error: %d" },
	{ APP_ERROR_VK_CREATE_PIPELINE_LAYOUT_FAILED, "vkCreatePipelineLayout error: %d" },
	{ APP_ERROR_VK_CREATE_PIPELINE_FAILED, "vkCreateGraphicsPipelines error: %d" },
	{ APP_ERROR_VK_CREATE_DESCRIPTOR_POOL_FAILED, "vkCreateDescriptorPool error: %d" },
	{ APP_ERROR_VK_ALLOCATE_DESCRIPTOR_SET_FAILED, "vkAllocateDescriptorSets error: %d" },
	{ APP_ERROR_VK_ALLOCATE_COMMAND_BUFFER_FAILED, "vkAllocateCommandBuffers error: %d" },
	{ APP_ERROR_VK_CREATE_BUFFER_FAILED, "vkCreateBuffer error: %d" },
	{ APP_ERROR_VK_MEM_BUFFER_BIND_FAILED, "vkBindBufferMemory error: %d" },
	{ APP_ERROR_VK_MAP_MEM_FAILED, "vkMapMemory error: %d" },
	{ APP_ERROR_VK_TOO_MANY_ALLOCATIONS, "Too memory memory allocations" },
	{ APP_ERROR_VK_DEVICE_NO_TRANSFER_QUEUE, "No transfer queue" },
	{ APP_ERROR_VK_CREATE_DESCRIPTOR_LAYOUT_FAILED, "vkCreateDescriptorSetLayout error: %d" },
	{ APP_ERROR_VK_CREATE_SAMPLER_FAILED, "vkCreateSampler error: %d" },
	{ APP_ERROR_VK_IMAGE_DATA_OUT_OF_BOUNDS, "Image data does not fit in image bounds" },
	{ APP_ERROR_VK_INVALID_IMAGE_MIP_LEVEL, "Image mipmap level %d does not exist, image has %d mip levels" },
	{ APP_ERROR_VK_INVALID_IMAGE_DATA, "Image data is null" },
	{ APP_ERROR_VK_VERTEX_UPDATE_OUT_OF_BOUNDS, "Vertex data update for offset %d with %d vertices exceeds total number of vertices %d" },
	{ APP_ERROR_VK_INDEX_UPDATE_OUT_OF_BOUNDS, "Index data update for offset %d with %d indices exceeds total number of indices %d" },
	{ APP_ERROR_UNRECOGNISED_ATTRIBUTE_FORMAT, "Vertex attribute format %d is not recognised" },
	{ APP_ERROR_GL2_CREATE_FRAMEBUFFER_FAILED, "CreateFrameBuffer error: %d" },
	{ APP_ERROR_GL2_NO_SHADER_BOUND, "You must call BindPipeline with a valid shader before DrawVertices" },
	{ APP_ERROR_GL2_NO_VERTEX_LAYOUT, "You must call BindPipeline with a valid vertex layout before DrawVertices" },
	

	{ APP_ERROR_IMAGE_FORMAT_NOT_RECOGNISED, "Image format %d is not recognised" },
};

void ReportAppMessage( const char* type, const char* file, int line, const char* msg, ... );

#define ReportAndReturnErrorID( ID, ... ) {\
	const char* szErr = "Unknown Error";\
	if ( (ID) > 0 && (ID) < APP_ERROR_NUM ) szErr = g_AppErrorDescs[(ID)].szDesc; \
	ReportAppMessage( "Error", __FILE__, __LINE__, szErr, ##__VA_ARGS__ ); \
	return (ID); \
}

#define AppError( error, ... ) ReportAppMessage( "Error", __FILE__, __LINE__, (error), ##__VA_ARGS__ )
#define AppWarning( warning, ... ) ReportAppMessage( "Warning", __FILE__, __LINE__, (warning), ##__VA_ARGS__ )
#define AppInfo( info, ... ) ReportAppMessage( "Info", __FILE__, __LINE__, (info), ##__VA_ARGS__ )

#define ReturnIfFail( expr ) { int returnCode = (expr); if ( returnCode != APP_SUCCESS ) return returnCode; }

int CheckAppErrors();

#endif // _H_AGK_ERRORS_
