#if !defined(DISABLE_VULKAN) && (!defined(__APPLE__) || defined(__LP64__))

#include "PlatformDefines.h"
#include "VulkanFunctions.h"
#include "AppErrors.h"

#if !defined(AGK_IOS) && !defined(AGK_MACOS)

PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = 0;
PFN_vkGetDeviceProcAddr vkGetDeviceProcAddr = 0;

// setup functions
PFN_vkCreateInstance vkCreateInstance = 0;
PFN_vkEnumerateInstanceExtensionProperties vkEnumerateInstanceExtensionProperties = 0;
PFN_vkEnumerateInstanceLayerProperties vkEnumerateInstanceLayerProperties = 0;

#ifdef VK_VERSION_1_1
	PFN_vkEnumerateInstanceVersion vkEnumerateInstanceVersion = 0;
#endif

// main functions
PFN_vkAllocateCommandBuffers vkAllocateCommandBuffers = 0;
PFN_vkAllocateDescriptorSets vkAllocateDescriptorSets = 0;
PFN_vkAllocateMemory vkAllocateMemory = 0;
PFN_vkBeginCommandBuffer vkBeginCommandBuffer = 0;
PFN_vkBindBufferMemory vkBindBufferMemory = 0;
PFN_vkBindImageMemory vkBindImageMemory = 0;
PFN_vkCmdBeginQuery vkCmdBeginQuery = 0;
PFN_vkCmdBeginRenderPass vkCmdBeginRenderPass = 0;
PFN_vkCmdBindDescriptorSets vkCmdBindDescriptorSets = 0;
PFN_vkCmdBindIndexBuffer vkCmdBindIndexBuffer = 0;
PFN_vkCmdBindPipeline vkCmdBindPipeline = 0;
PFN_vkCmdBindVertexBuffers vkCmdBindVertexBuffers = 0;
PFN_vkCmdBlitImage vkCmdBlitImage = 0;
PFN_vkCmdClearAttachments vkCmdClearAttachments = 0;
PFN_vkCmdClearColorImage vkCmdClearColorImage = 0;
PFN_vkCmdClearDepthStencilImage vkCmdClearDepthStencilImage = 0;
PFN_vkCmdCopyBuffer vkCmdCopyBuffer = 0;
PFN_vkCmdCopyBufferToImage vkCmdCopyBufferToImage = 0;
PFN_vkCmdCopyImage vkCmdCopyImage = 0;
PFN_vkCmdCopyImageToBuffer vkCmdCopyImageToBuffer = 0;
PFN_vkCmdCopyQueryPoolResults vkCmdCopyQueryPoolResults = 0;
PFN_vkCmdDispatch vkCmdDispatch = 0;
PFN_vkCmdDispatchIndirect vkCmdDispatchIndirect = 0;
PFN_vkCmdDraw vkCmdDraw = 0;
PFN_vkCmdDrawIndexed vkCmdDrawIndexed = 0;
PFN_vkCmdDrawIndexedIndirect vkCmdDrawIndexedIndirect = 0;
PFN_vkCmdDrawIndirect vkCmdDrawIndirect = 0;
PFN_vkCmdEndQuery vkCmdEndQuery = 0;
PFN_vkCmdEndRenderPass vkCmdEndRenderPass = 0;
PFN_vkCmdExecuteCommands vkCmdExecuteCommands = 0;
PFN_vkCmdFillBuffer vkCmdFillBuffer = 0;
PFN_vkCmdNextSubpass vkCmdNextSubpass = 0;
PFN_vkCmdPipelineBarrier vkCmdPipelineBarrier = 0;
PFN_vkCmdPushConstants vkCmdPushConstants = 0;
PFN_vkCmdResetEvent vkCmdResetEvent = 0;
PFN_vkCmdResetQueryPool vkCmdResetQueryPool = 0;
PFN_vkCmdResolveImage vkCmdResolveImage = 0;
PFN_vkCmdSetBlendConstants vkCmdSetBlendConstants = 0;
PFN_vkCmdSetDepthBias vkCmdSetDepthBias = 0;
PFN_vkCmdSetDepthBounds vkCmdSetDepthBounds = 0;
PFN_vkCmdSetEvent vkCmdSetEvent = 0;
PFN_vkCmdSetLineWidth vkCmdSetLineWidth = 0;
PFN_vkCmdSetScissor vkCmdSetScissor = 0;
PFN_vkCmdSetStencilCompareMask vkCmdSetStencilCompareMask = 0;
PFN_vkCmdSetStencilReference vkCmdSetStencilReference = 0;
PFN_vkCmdSetStencilWriteMask vkCmdSetStencilWriteMask = 0;
PFN_vkCmdSetViewport vkCmdSetViewport = 0;
PFN_vkCmdUpdateBuffer vkCmdUpdateBuffer = 0;
PFN_vkCmdWaitEvents vkCmdWaitEvents = 0;
PFN_vkCmdWriteTimestamp vkCmdWriteTimestamp = 0;
PFN_vkCreateBuffer vkCreateBuffer = 0;
PFN_vkCreateBufferView vkCreateBufferView = 0;
PFN_vkCreateCommandPool vkCreateCommandPool = 0;
PFN_vkCreateComputePipelines vkCreateComputePipelines = 0;
PFN_vkCreateDescriptorPool vkCreateDescriptorPool = 0;
PFN_vkCreateDescriptorSetLayout vkCreateDescriptorSetLayout = 0;
PFN_vkCreateDevice vkCreateDevice = 0;
PFN_vkCreateEvent vkCreateEvent = 0;
PFN_vkCreateFence vkCreateFence = 0;
PFN_vkCreateFramebuffer vkCreateFramebuffer = 0;
PFN_vkCreateGraphicsPipelines vkCreateGraphicsPipelines = 0;
PFN_vkCreateImage vkCreateImage = 0;
PFN_vkCreateImageView vkCreateImageView = 0;
PFN_vkCreatePipelineCache vkCreatePipelineCache = 0;
PFN_vkCreatePipelineLayout vkCreatePipelineLayout = 0;
PFN_vkCreateQueryPool vkCreateQueryPool = 0;
PFN_vkCreateRenderPass vkCreateRenderPass = 0;
PFN_vkCreateSampler vkCreateSampler = 0;
PFN_vkCreateSemaphore vkCreateSemaphore = 0;
PFN_vkCreateShaderModule vkCreateShaderModule = 0;
PFN_vkDestroyBuffer vkDestroyBuffer = 0;
PFN_vkDestroyBufferView vkDestroyBufferView = 0;
PFN_vkDestroyCommandPool vkDestroyCommandPool = 0;
PFN_vkDestroyDescriptorPool vkDestroyDescriptorPool = 0;
PFN_vkDestroyDescriptorSetLayout vkDestroyDescriptorSetLayout = 0;
PFN_vkDestroyDevice vkDestroyDevice = 0;
PFN_vkDestroyEvent vkDestroyEvent = 0;
PFN_vkDestroyFence vkDestroyFence = 0;
PFN_vkDestroyFramebuffer vkDestroyFramebuffer = 0;
PFN_vkDestroyImage vkDestroyImage = 0;
PFN_vkDestroyImageView vkDestroyImageView = 0;
PFN_vkDestroyInstance vkDestroyInstance = 0;
PFN_vkDestroyPipeline vkDestroyPipeline = 0;
PFN_vkDestroyPipelineCache vkDestroyPipelineCache = 0;
PFN_vkDestroyPipelineLayout vkDestroyPipelineLayout = 0;
PFN_vkDestroyQueryPool vkDestroyQueryPool = 0;
PFN_vkDestroyRenderPass vkDestroyRenderPass = 0;
PFN_vkDestroySampler vkDestroySampler = 0;
PFN_vkDestroySemaphore vkDestroySemaphore = 0;
PFN_vkDestroyShaderModule vkDestroyShaderModule = 0;
PFN_vkDeviceWaitIdle vkDeviceWaitIdle = 0;
PFN_vkEndCommandBuffer vkEndCommandBuffer = 0;
PFN_vkEnumerateDeviceExtensionProperties vkEnumerateDeviceExtensionProperties = 0;
PFN_vkEnumerateDeviceLayerProperties vkEnumerateDeviceLayerProperties = 0;
PFN_vkEnumeratePhysicalDevices vkEnumeratePhysicalDevices = 0;
PFN_vkFlushMappedMemoryRanges vkFlushMappedMemoryRanges = 0;
PFN_vkFreeCommandBuffers vkFreeCommandBuffers = 0;
PFN_vkFreeDescriptorSets vkFreeDescriptorSets = 0;
PFN_vkFreeMemory vkFreeMemory = 0;
PFN_vkGetBufferMemoryRequirements vkGetBufferMemoryRequirements = 0;
PFN_vkGetDeviceMemoryCommitment vkGetDeviceMemoryCommitment = 0;
PFN_vkGetDeviceQueue vkGetDeviceQueue = 0;
PFN_vkGetEventStatus vkGetEventStatus = 0;
PFN_vkGetFenceStatus vkGetFenceStatus = 0;
PFN_vkGetImageMemoryRequirements vkGetImageMemoryRequirements = 0;
PFN_vkGetImageSparseMemoryRequirements vkGetImageSparseMemoryRequirements = 0;
PFN_vkGetImageSubresourceLayout vkGetImageSubresourceLayout = 0;
PFN_vkGetPhysicalDeviceFeatures vkGetPhysicalDeviceFeatures = 0;
PFN_vkGetPhysicalDeviceFormatProperties vkGetPhysicalDeviceFormatProperties = 0;
PFN_vkGetPhysicalDeviceImageFormatProperties vkGetPhysicalDeviceImageFormatProperties = 0;
PFN_vkGetPhysicalDeviceMemoryProperties vkGetPhysicalDeviceMemoryProperties = 0;
PFN_vkGetPhysicalDeviceProperties vkGetPhysicalDeviceProperties = 0;
PFN_vkGetPhysicalDeviceQueueFamilyProperties vkGetPhysicalDeviceQueueFamilyProperties = 0;
PFN_vkGetPhysicalDeviceSparseImageFormatProperties vkGetPhysicalDeviceSparseImageFormatProperties = 0;
PFN_vkGetPipelineCacheData vkGetPipelineCacheData = 0;
PFN_vkGetQueryPoolResults vkGetQueryPoolResults = 0;
PFN_vkGetRenderAreaGranularity vkGetRenderAreaGranularity = 0;
PFN_vkInvalidateMappedMemoryRanges vkInvalidateMappedMemoryRanges = 0;
PFN_vkMapMemory vkMapMemory = 0;
PFN_vkMergePipelineCaches vkMergePipelineCaches = 0;
PFN_vkQueueBindSparse vkQueueBindSparse = 0;
PFN_vkQueueSubmit vkQueueSubmit = 0;
PFN_vkQueueWaitIdle vkQueueWaitIdle = 0;
PFN_vkResetCommandBuffer vkResetCommandBuffer = 0;
PFN_vkResetCommandPool vkResetCommandPool = 0;
PFN_vkResetDescriptorPool vkResetDescriptorPool = 0;
PFN_vkResetEvent vkResetEvent = 0;
PFN_vkResetFences vkResetFences = 0;
PFN_vkSetEvent vkSetEvent = 0;
PFN_vkUnmapMemory vkUnmapMemory = 0;
PFN_vkUpdateDescriptorSets vkUpdateDescriptorSets = 0;
PFN_vkWaitForFences vkWaitForFences = 0;

#ifdef VK_VERSION_1_1
	PFN_vkBindBufferMemory2 vkBindBufferMemory2 = 0;
	PFN_vkBindImageMemory2 vkBindImageMemory2 = 0;
	PFN_vkCmdDispatchBase vkCmdDispatchBase = 0;
	PFN_vkCmdSetDeviceMask vkCmdSetDeviceMask = 0;
	PFN_vkCreateDescriptorUpdateTemplate vkCreateDescriptorUpdateTemplate = 0;
	PFN_vkCreateSamplerYcbcrConversion vkCreateSamplerYcbcrConversion = 0;
	PFN_vkDestroyDescriptorUpdateTemplate vkDestroyDescriptorUpdateTemplate = 0;
	PFN_vkDestroySamplerYcbcrConversion vkDestroySamplerYcbcrConversion = 0;
	PFN_vkEnumeratePhysicalDeviceGroups vkEnumeratePhysicalDeviceGroups = 0;
	PFN_vkGetBufferMemoryRequirements2 vkGetBufferMemoryRequirements2 = 0;
	PFN_vkGetDescriptorSetLayoutSupport vkGetDescriptorSetLayoutSupport = 0;
	PFN_vkGetDeviceGroupPeerMemoryFeatures vkGetDeviceGroupPeerMemoryFeatures = 0;
	PFN_vkGetDeviceQueue2 vkGetDeviceQueue2 = 0;
	PFN_vkGetImageMemoryRequirements2 vkGetImageMemoryRequirements2 = 0;
	PFN_vkGetImageSparseMemoryRequirements2 vkGetImageSparseMemoryRequirements2 = 0;
	PFN_vkGetPhysicalDeviceExternalBufferProperties vkGetPhysicalDeviceExternalBufferProperties = 0;
	PFN_vkGetPhysicalDeviceExternalFenceProperties vkGetPhysicalDeviceExternalFenceProperties = 0;
	PFN_vkGetPhysicalDeviceExternalSemaphoreProperties vkGetPhysicalDeviceExternalSemaphoreProperties = 0;
	PFN_vkGetPhysicalDeviceFeatures2 vkGetPhysicalDeviceFeatures2 = 0;
	PFN_vkGetPhysicalDeviceFormatProperties2 vkGetPhysicalDeviceFormatProperties2 = 0;
	PFN_vkGetPhysicalDeviceImageFormatProperties2 vkGetPhysicalDeviceImageFormatProperties2 = 0;
	PFN_vkGetPhysicalDeviceMemoryProperties2 vkGetPhysicalDeviceMemoryProperties2 = 0;
	PFN_vkGetPhysicalDeviceProperties2 vkGetPhysicalDeviceProperties2 = 0;
	PFN_vkGetPhysicalDeviceQueueFamilyProperties2 vkGetPhysicalDeviceQueueFamilyProperties2 = 0;
	PFN_vkGetPhysicalDeviceSparseImageFormatProperties2 vkGetPhysicalDeviceSparseImageFormatProperties2 = 0;
	PFN_vkTrimCommandPool vkTrimCommandPool = 0;
	PFN_vkUpdateDescriptorSetWithTemplate vkUpdateDescriptorSetWithTemplate = 0;
#endif 

#if defined(VK_AMD_buffer_marker)
	PFN_vkCmdWriteBufferMarkerAMD vkCmdWriteBufferMarkerAMD = 0;
#endif 
#if defined(VK_AMD_draw_indirect_count)
	PFN_vkCmdDrawIndexedIndirectCountAMD vkCmdDrawIndexedIndirectCountAMD = 0;
	PFN_vkCmdDrawIndirectCountAMD vkCmdDrawIndirectCountAMD = 0;
#endif 
#if defined(VK_AMD_shader_info)
	PFN_vkGetShaderInfoAMD vkGetShaderInfoAMD = 0;
#endif 
#if defined(VK_ANDROID_external_memory_android_hardware_buffer)
	PFN_vkGetAndroidHardwareBufferPropertiesANDROID vkGetAndroidHardwareBufferPropertiesANDROID = 0;
	PFN_vkGetMemoryAndroidHardwareBufferANDROID vkGetMemoryAndroidHardwareBufferANDROID = 0;
#endif 
#if defined(VK_EXT_acquire_xlib_display)
	PFN_vkAcquireXlibDisplayEXT vkAcquireXlibDisplayEXT = 0;
	PFN_vkGetRandROutputDisplayEXT vkGetRandROutputDisplayEXT = 0;
#endif 
#if defined(VK_EXT_buffer_device_address)
	PFN_vkGetBufferDeviceAddressEXT vkGetBufferDeviceAddressEXT = 0;
#endif 
#if defined(VK_EXT_calibrated_timestamps)
	PFN_vkGetCalibratedTimestampsEXT vkGetCalibratedTimestampsEXT = 0;
	PFN_vkGetPhysicalDeviceCalibrateableTimeDomainsEXT vkGetPhysicalDeviceCalibrateableTimeDomainsEXT = 0;
#endif 
#if defined(VK_EXT_conditional_rendering)
	PFN_vkCmdBeginConditionalRenderingEXT vkCmdBeginConditionalRenderingEXT = 0;
	PFN_vkCmdEndConditionalRenderingEXT vkCmdEndConditionalRenderingEXT = 0;
#endif 
#if defined(VK_EXT_debug_marker)
	PFN_vkCmdDebugMarkerBeginEXT vkCmdDebugMarkerBeginEXT = 0;
	PFN_vkCmdDebugMarkerEndEXT vkCmdDebugMarkerEndEXT = 0;
	PFN_vkCmdDebugMarkerInsertEXT vkCmdDebugMarkerInsertEXT = 0;
	PFN_vkDebugMarkerSetObjectNameEXT vkDebugMarkerSetObjectNameEXT = 0;
	PFN_vkDebugMarkerSetObjectTagEXT vkDebugMarkerSetObjectTagEXT = 0;
#endif 
#if defined(VK_EXT_debug_report)
	PFN_vkCreateDebugReportCallbackEXT vkCreateDebugReportCallbackEXT = 0;
	PFN_vkDebugReportMessageEXT vkDebugReportMessageEXT = 0;
	PFN_vkDestroyDebugReportCallbackEXT vkDestroyDebugReportCallbackEXT = 0;
#endif 
#if defined(VK_EXT_debug_utils)
	PFN_vkCmdBeginDebugUtilsLabelEXT vkCmdBeginDebugUtilsLabelEXT = 0;
	PFN_vkCmdEndDebugUtilsLabelEXT vkCmdEndDebugUtilsLabelEXT = 0;
	PFN_vkCmdInsertDebugUtilsLabelEXT vkCmdInsertDebugUtilsLabelEXT = 0;
	PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT = 0;
	PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT = 0;
	PFN_vkQueueBeginDebugUtilsLabelEXT vkQueueBeginDebugUtilsLabelEXT = 0;
	PFN_vkQueueEndDebugUtilsLabelEXT vkQueueEndDebugUtilsLabelEXT = 0;
	PFN_vkQueueInsertDebugUtilsLabelEXT vkQueueInsertDebugUtilsLabelEXT = 0;
	PFN_vkSetDebugUtilsObjectNameEXT vkSetDebugUtilsObjectNameEXT = 0;
	PFN_vkSetDebugUtilsObjectTagEXT vkSetDebugUtilsObjectTagEXT = 0;
	PFN_vkSubmitDebugUtilsMessageEXT vkSubmitDebugUtilsMessageEXT = 0;
#endif 
#if defined(VK_EXT_direct_mode_display)
	PFN_vkReleaseDisplayEXT vkReleaseDisplayEXT = 0;
#endif 
#if defined(VK_EXT_discard_rectangles)
	PFN_vkCmdSetDiscardRectangleEXT vkCmdSetDiscardRectangleEXT = 0;
#endif 
#if defined(VK_EXT_display_control)
	PFN_vkDisplayPowerControlEXT vkDisplayPowerControlEXT = 0;
	PFN_vkGetSwapchainCounterEXT vkGetSwapchainCounterEXT = 0;
	PFN_vkRegisterDeviceEventEXT vkRegisterDeviceEventEXT = 0;
	PFN_vkRegisterDisplayEventEXT vkRegisterDisplayEventEXT = 0;
#endif 
#if defined(VK_EXT_display_surface_counter)
	PFN_vkGetPhysicalDeviceSurfaceCapabilities2EXT vkGetPhysicalDeviceSurfaceCapabilities2EXT = 0;
#endif 
#if defined(VK_EXT_external_memory_host)
	PFN_vkGetMemoryHostPointerPropertiesEXT vkGetMemoryHostPointerPropertiesEXT = 0;
#endif 
#if defined(VK_EXT_hdr_metadata)
	PFN_vkSetHdrMetadataEXT vkSetHdrMetadataEXT = 0;
#endif 
#if defined(VK_EXT_image_drm_format_modifier)
	PFN_vkGetImageDrmFormatModifierPropertiesEXT vkGetImageDrmFormatModifierPropertiesEXT = 0;
#endif 
#if defined(VK_EXT_sample_locations)
	PFN_vkCmdSetSampleLocationsEXT vkCmdSetSampleLocationsEXT = 0;
	PFN_vkGetPhysicalDeviceMultisamplePropertiesEXT vkGetPhysicalDeviceMultisamplePropertiesEXT = 0;
#endif 
#if defined(VK_EXT_transform_feedback)
	PFN_vkCmdBeginQueryIndexedEXT vkCmdBeginQueryIndexedEXT = 0;
	PFN_vkCmdBeginTransformFeedbackEXT vkCmdBeginTransformFeedbackEXT = 0;
	PFN_vkCmdBindTransformFeedbackBuffersEXT vkCmdBindTransformFeedbackBuffersEXT = 0;
	PFN_vkCmdDrawIndirectByteCountEXT vkCmdDrawIndirectByteCountEXT = 0;
	PFN_vkCmdEndQueryIndexedEXT vkCmdEndQueryIndexedEXT = 0;
	PFN_vkCmdEndTransformFeedbackEXT vkCmdEndTransformFeedbackEXT = 0;
#endif 
#if defined(VK_EXT_validation_cache)
	PFN_vkCreateValidationCacheEXT vkCreateValidationCacheEXT = 0;
	PFN_vkDestroyValidationCacheEXT vkDestroyValidationCacheEXT = 0;
	PFN_vkGetValidationCacheDataEXT vkGetValidationCacheDataEXT = 0;
	PFN_vkMergeValidationCachesEXT vkMergeValidationCachesEXT = 0;
#endif 
#if defined(VK_FUCHSIA_imagepipe_surface)
	PFN_vkCreateImagePipeSurfaceFUCHSIA vkCreateImagePipeSurfaceFUCHSIA = 0;
#endif 
#if defined(VK_GOOGLE_display_timing)
	PFN_vkGetPastPresentationTimingGOOGLE vkGetPastPresentationTimingGOOGLE = 0;
	PFN_vkGetRefreshCycleDurationGOOGLE vkGetRefreshCycleDurationGOOGLE = 0;
#endif 
#if defined(VK_KHR_android_surface)
	PFN_vkCreateAndroidSurfaceKHR vkCreateAndroidSurfaceKHR = 0;
#endif 
#if defined(VK_KHR_bind_memory2)
	PFN_vkBindBufferMemory2KHR vkBindBufferMemory2KHR = 0;
	PFN_vkBindImageMemory2KHR vkBindImageMemory2KHR = 0;
#endif 
#if defined(VK_KHR_create_renderpass2)
	PFN_vkCmdBeginRenderPass2KHR vkCmdBeginRenderPass2KHR = 0;
	PFN_vkCmdEndRenderPass2KHR vkCmdEndRenderPass2KHR = 0;
	PFN_vkCmdNextSubpass2KHR vkCmdNextSubpass2KHR = 0;
	PFN_vkCreateRenderPass2KHR vkCreateRenderPass2KHR = 0;
#endif 
#if defined(VK_KHR_descriptor_update_template)
	PFN_vkCreateDescriptorUpdateTemplateKHR vkCreateDescriptorUpdateTemplateKHR = 0;
	PFN_vkDestroyDescriptorUpdateTemplateKHR vkDestroyDescriptorUpdateTemplateKHR = 0;
	PFN_vkUpdateDescriptorSetWithTemplateKHR vkUpdateDescriptorSetWithTemplateKHR = 0;
#endif 
#if defined(VK_KHR_device_group)
	PFN_vkCmdDispatchBaseKHR vkCmdDispatchBaseKHR = 0;
	PFN_vkCmdSetDeviceMaskKHR vkCmdSetDeviceMaskKHR = 0;
	PFN_vkGetDeviceGroupPeerMemoryFeaturesKHR vkGetDeviceGroupPeerMemoryFeaturesKHR = 0;
#endif 
#if defined(VK_KHR_device_group_creation)
	PFN_vkEnumeratePhysicalDeviceGroupsKHR vkEnumeratePhysicalDeviceGroupsKHR = 0;
#endif 
#if defined(VK_KHR_display)
	PFN_vkCreateDisplayModeKHR vkCreateDisplayModeKHR = 0;
	PFN_vkCreateDisplayPlaneSurfaceKHR vkCreateDisplayPlaneSurfaceKHR = 0;
	PFN_vkGetDisplayModePropertiesKHR vkGetDisplayModePropertiesKHR = 0;
	PFN_vkGetDisplayPlaneCapabilitiesKHR vkGetDisplayPlaneCapabilitiesKHR = 0;
	PFN_vkGetDisplayPlaneSupportedDisplaysKHR vkGetDisplayPlaneSupportedDisplaysKHR = 0;
	PFN_vkGetPhysicalDeviceDisplayPlanePropertiesKHR vkGetPhysicalDeviceDisplayPlanePropertiesKHR = 0;
	PFN_vkGetPhysicalDeviceDisplayPropertiesKHR vkGetPhysicalDeviceDisplayPropertiesKHR = 0;
#endif 
#if defined(VK_KHR_display_swapchain)
	PFN_vkCreateSharedSwapchainsKHR vkCreateSharedSwapchainsKHR = 0;
#endif 
#if defined(VK_KHR_draw_indirect_count)
	PFN_vkCmdDrawIndexedIndirectCountKHR vkCmdDrawIndexedIndirectCountKHR = 0;
	PFN_vkCmdDrawIndirectCountKHR vkCmdDrawIndirectCountKHR = 0;
#endif 
#if defined(VK_KHR_external_fence_capabilities)
	PFN_vkGetPhysicalDeviceExternalFencePropertiesKHR vkGetPhysicalDeviceExternalFencePropertiesKHR = 0;
#endif 
#if defined(VK_KHR_external_fence_fd)
	PFN_vkGetFenceFdKHR vkGetFenceFdKHR = 0;
	PFN_vkImportFenceFdKHR vkImportFenceFdKHR = 0;
#endif 
#if defined(VK_KHR_external_fence_win32)
	PFN_vkGetFenceWin32HandleKHR vkGetFenceWin32HandleKHR = 0;
	PFN_vkImportFenceWin32HandleKHR vkImportFenceWin32HandleKHR = 0;
#endif 
#if defined(VK_KHR_external_memory_capabilities)
	PFN_vkGetPhysicalDeviceExternalBufferPropertiesKHR vkGetPhysicalDeviceExternalBufferPropertiesKHR = 0;
#endif 
#if defined(VK_KHR_external_memory_fd)
	PFN_vkGetMemoryFdKHR vkGetMemoryFdKHR = 0;
	PFN_vkGetMemoryFdPropertiesKHR vkGetMemoryFdPropertiesKHR = 0;
#endif 
#if defined(VK_KHR_external_memory_win32)
	PFN_vkGetMemoryWin32HandleKHR vkGetMemoryWin32HandleKHR = 0;
	PFN_vkGetMemoryWin32HandlePropertiesKHR vkGetMemoryWin32HandlePropertiesKHR = 0;
#endif 
#if defined(VK_KHR_external_semaphore_capabilities)
	PFN_vkGetPhysicalDeviceExternalSemaphorePropertiesKHR vkGetPhysicalDeviceExternalSemaphorePropertiesKHR = 0;
#endif 
#if defined(VK_KHR_external_semaphore_fd)
	PFN_vkGetSemaphoreFdKHR vkGetSemaphoreFdKHR = 0;
	PFN_vkImportSemaphoreFdKHR vkImportSemaphoreFdKHR = 0;
#endif 
#if defined(VK_KHR_external_semaphore_win32)
	PFN_vkGetSemaphoreWin32HandleKHR vkGetSemaphoreWin32HandleKHR = 0;
	PFN_vkImportSemaphoreWin32HandleKHR vkImportSemaphoreWin32HandleKHR = 0;
#endif 
#if defined(VK_KHR_get_display_properties2)
	PFN_vkGetDisplayModeProperties2KHR vkGetDisplayModeProperties2KHR = 0;
	PFN_vkGetDisplayPlaneCapabilities2KHR vkGetDisplayPlaneCapabilities2KHR = 0;
	PFN_vkGetPhysicalDeviceDisplayPlaneProperties2KHR vkGetPhysicalDeviceDisplayPlaneProperties2KHR = 0;
	PFN_vkGetPhysicalDeviceDisplayProperties2KHR vkGetPhysicalDeviceDisplayProperties2KHR = 0;
#endif 
#if defined(VK_KHR_get_memory_requirements2)
	PFN_vkGetBufferMemoryRequirements2KHR vkGetBufferMemoryRequirements2KHR = 0;
	PFN_vkGetImageMemoryRequirements2KHR vkGetImageMemoryRequirements2KHR = 0;
	PFN_vkGetImageSparseMemoryRequirements2KHR vkGetImageSparseMemoryRequirements2KHR = 0;
#endif 
#if defined(VK_KHR_get_physical_device_properties2)
	PFN_vkGetPhysicalDeviceFeatures2KHR vkGetPhysicalDeviceFeatures2KHR = 0;
	PFN_vkGetPhysicalDeviceFormatProperties2KHR vkGetPhysicalDeviceFormatProperties2KHR = 0;
	PFN_vkGetPhysicalDeviceImageFormatProperties2KHR vkGetPhysicalDeviceImageFormatProperties2KHR = 0;
	PFN_vkGetPhysicalDeviceMemoryProperties2KHR vkGetPhysicalDeviceMemoryProperties2KHR = 0;
	PFN_vkGetPhysicalDeviceProperties2KHR vkGetPhysicalDeviceProperties2KHR = 0;
	PFN_vkGetPhysicalDeviceQueueFamilyProperties2KHR vkGetPhysicalDeviceQueueFamilyProperties2KHR = 0;
	PFN_vkGetPhysicalDeviceSparseImageFormatProperties2KHR vkGetPhysicalDeviceSparseImageFormatProperties2KHR = 0;
#endif 
#if defined(VK_KHR_get_surface_capabilities2)
	PFN_vkGetPhysicalDeviceSurfaceCapabilities2KHR vkGetPhysicalDeviceSurfaceCapabilities2KHR = 0;
	PFN_vkGetPhysicalDeviceSurfaceFormats2KHR vkGetPhysicalDeviceSurfaceFormats2KHR = 0;
#endif 
#if defined(VK_KHR_maintenance1)
	PFN_vkTrimCommandPoolKHR vkTrimCommandPoolKHR = 0;
#endif 
#if defined(VK_KHR_maintenance3)
	PFN_vkGetDescriptorSetLayoutSupportKHR vkGetDescriptorSetLayoutSupportKHR = 0;
#endif 
#if defined(VK_KHR_push_descriptor)
	PFN_vkCmdPushDescriptorSetKHR vkCmdPushDescriptorSetKHR = 0;
#endif 
#if defined(VK_KHR_sampler_ycbcr_conversion)
	PFN_vkCreateSamplerYcbcrConversionKHR vkCreateSamplerYcbcrConversionKHR = 0;
	PFN_vkDestroySamplerYcbcrConversionKHR vkDestroySamplerYcbcrConversionKHR = 0;
#endif 
#if defined(VK_KHR_shared_presentable_image)
	PFN_vkGetSwapchainStatusKHR vkGetSwapchainStatusKHR = 0;
#endif 
#if defined(VK_KHR_surface)
	PFN_vkDestroySurfaceKHR vkDestroySurfaceKHR = 0;
	PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR vkGetPhysicalDeviceSurfaceCapabilitiesKHR = 0;
	PFN_vkGetPhysicalDeviceSurfaceFormatsKHR vkGetPhysicalDeviceSurfaceFormatsKHR = 0;
	PFN_vkGetPhysicalDeviceSurfacePresentModesKHR vkGetPhysicalDeviceSurfacePresentModesKHR = 0;
	PFN_vkGetPhysicalDeviceSurfaceSupportKHR vkGetPhysicalDeviceSurfaceSupportKHR = 0;
#endif 
#if defined(VK_KHR_swapchain)
	PFN_vkAcquireNextImageKHR vkAcquireNextImageKHR = 0;
	PFN_vkCreateSwapchainKHR vkCreateSwapchainKHR = 0;
	PFN_vkDestroySwapchainKHR vkDestroySwapchainKHR = 0;
	PFN_vkGetSwapchainImagesKHR vkGetSwapchainImagesKHR = 0;
	PFN_vkQueuePresentKHR vkQueuePresentKHR = 0;
#endif 
#if defined(VK_KHR_wayland_surface)
	PFN_vkCreateWaylandSurfaceKHR vkCreateWaylandSurfaceKHR = 0;
	PFN_vkGetPhysicalDeviceWaylandPresentationSupportKHR vkGetPhysicalDeviceWaylandPresentationSupportKHR = 0;
#endif 
#if defined(VK_KHR_win32_surface)
	PFN_vkCreateWin32SurfaceKHR vkCreateWin32SurfaceKHR = 0;
	PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR vkGetPhysicalDeviceWin32PresentationSupportKHR = 0;
#endif 
#if defined(VK_KHR_xcb_surface)
	PFN_vkCreateXcbSurfaceKHR vkCreateXcbSurfaceKHR = 0;
	PFN_vkGetPhysicalDeviceXcbPresentationSupportKHR vkGetPhysicalDeviceXcbPresentationSupportKHR = 0;
#endif 
#if defined(VK_KHR_xlib_surface)
	PFN_vkCreateXlibSurfaceKHR vkCreateXlibSurfaceKHR = 0;
	PFN_vkGetPhysicalDeviceXlibPresentationSupportKHR vkGetPhysicalDeviceXlibPresentationSupportKHR = 0;
#endif 
#if defined(VK_MVK_ios_surface)
	PFN_vkCreateIOSSurfaceMVK vkCreateIOSSurfaceMVK = 0;
#endif 
#if defined(VK_MVK_macos_surface)
	PFN_vkCreateMacOSSurfaceMVK vkCreateMacOSSurfaceMVK = 0;
#endif 
#if defined(VK_NN_vi_surface)
	PFN_vkCreateViSurfaceNN vkCreateViSurfaceNN = 0;
#endif 
#if defined(VK_NVX_device_generated_commands)
	PFN_vkCmdProcessCommandsNVX vkCmdProcessCommandsNVX = 0;
	PFN_vkCmdReserveSpaceForCommandsNVX vkCmdReserveSpaceForCommandsNVX = 0;
	PFN_vkCreateIndirectCommandsLayoutNVX vkCreateIndirectCommandsLayoutNVX = 0;
	PFN_vkCreateObjectTableNVX vkCreateObjectTableNVX = 0;
	PFN_vkDestroyIndirectCommandsLayoutNVX vkDestroyIndirectCommandsLayoutNVX = 0;
	PFN_vkDestroyObjectTableNVX vkDestroyObjectTableNVX = 0;
	PFN_vkGetPhysicalDeviceGeneratedCommandsPropertiesNVX vkGetPhysicalDeviceGeneratedCommandsPropertiesNVX = 0;
	PFN_vkRegisterObjectsNVX vkRegisterObjectsNVX = 0;
	PFN_vkUnregisterObjectsNVX vkUnregisterObjectsNVX = 0;
#endif 
#if defined(VK_NV_clip_space_w_scaling)
	PFN_vkCmdSetViewportWScalingNV vkCmdSetViewportWScalingNV = 0;
#endif 
#if defined(VK_NV_device_diagnostic_checkpoints)
	PFN_vkCmdSetCheckpointNV vkCmdSetCheckpointNV = 0;
	PFN_vkGetQueueCheckpointDataNV vkGetQueueCheckpointDataNV = 0;
#endif 
#if defined(VK_NV_external_memory_capabilities)
	PFN_vkGetPhysicalDeviceExternalImageFormatPropertiesNV vkGetPhysicalDeviceExternalImageFormatPropertiesNV = 0;
#endif 
#if defined(VK_NV_external_memory_win32)
	PFN_vkGetMemoryWin32HandleNV vkGetMemoryWin32HandleNV = 0;
#endif 
#if defined(VK_NV_mesh_shader)
	PFN_vkCmdDrawMeshTasksIndirectCountNV vkCmdDrawMeshTasksIndirectCountNV = 0;
	PFN_vkCmdDrawMeshTasksIndirectNV vkCmdDrawMeshTasksIndirectNV = 0;
	PFN_vkCmdDrawMeshTasksNV vkCmdDrawMeshTasksNV = 0;
#endif 
#if defined(VK_NV_ray_tracing)
	PFN_vkBindAccelerationStructureMemoryNV vkBindAccelerationStructureMemoryNV = 0;
	PFN_vkCmdBuildAccelerationStructureNV vkCmdBuildAccelerationStructureNV = 0;
	PFN_vkCmdCopyAccelerationStructureNV vkCmdCopyAccelerationStructureNV = 0;
	PFN_vkCmdTraceRaysNV vkCmdTraceRaysNV = 0;
	PFN_vkCmdWriteAccelerationStructuresPropertiesNV vkCmdWriteAccelerationStructuresPropertiesNV = 0;
	PFN_vkCompileDeferredNV vkCompileDeferredNV = 0;
	PFN_vkCreateAccelerationStructureNV vkCreateAccelerationStructureNV = 0;
	PFN_vkCreateRayTracingPipelinesNV vkCreateRayTracingPipelinesNV = 0;
	PFN_vkDestroyAccelerationStructureNV vkDestroyAccelerationStructureNV = 0;
	PFN_vkGetAccelerationStructureHandleNV vkGetAccelerationStructureHandleNV = 0;
	PFN_vkGetAccelerationStructureMemoryRequirementsNV vkGetAccelerationStructureMemoryRequirementsNV = 0;
	PFN_vkGetRayTracingShaderGroupHandlesNV vkGetRayTracingShaderGroupHandlesNV = 0;
#endif 
#if defined(VK_NV_scissor_exclusive)
	PFN_vkCmdSetExclusiveScissorNV vkCmdSetExclusiveScissorNV = 0;
#endif 
#if defined(VK_NV_shading_rate_image)
	PFN_vkCmdBindShadingRateImageNV vkCmdBindShadingRateImageNV = 0;
	PFN_vkCmdSetCoarseSampleOrderNV vkCmdSetCoarseSampleOrderNV = 0;
	PFN_vkCmdSetViewportShadingRatePaletteNV vkCmdSetViewportShadingRatePaletteNV = 0;
#endif 
#if (defined(VK_KHR_descriptor_update_template) && defined(VK_KHR_push_descriptor)) || (defined(VK_KHR_push_descriptor) && defined(VK_VERSION_1_1))
	PFN_vkCmdPushDescriptorSetWithTemplateKHR vkCmdPushDescriptorSetWithTemplateKHR = 0;
#endif 
#if (defined(VK_KHR_device_group) && defined(VK_KHR_surface)) || (defined(VK_KHR_swapchain) && defined(VK_VERSION_1_1))
	PFN_vkGetDeviceGroupPresentCapabilitiesKHR vkGetDeviceGroupPresentCapabilitiesKHR = 0;
	PFN_vkGetDeviceGroupSurfacePresentModesKHR vkGetDeviceGroupSurfacePresentModesKHR = 0;
	PFN_vkGetPhysicalDevicePresentRectanglesKHR vkGetPhysicalDevicePresentRectanglesKHR = 0;
#endif 
#if (defined(VK_KHR_device_group) && defined(VK_KHR_swapchain)) || (defined(VK_KHR_swapchain) && defined(VK_VERSION_1_1))
	PFN_vkAcquireNextImage2KHR vkAcquireNextImage2KHR = 0;
#endif

#if defined(VK_AMD_display_native_hdr)
	PFN_vkSetLocalDimmingAMD vkSetLocalDimmingAMD = 0;
#endif
#if defined(VK_EXT_full_screen_exclusive)
	PFN_vkAcquireFullScreenExclusiveModeEXT vkAcquireFullScreenExclusiveModeEXT = 0;
	PFN_vkGetPhysicalDeviceSurfacePresentModes2EXT vkGetPhysicalDeviceSurfacePresentModes2EXT = 0;
	PFN_vkReleaseFullScreenExclusiveModeEXT vkReleaseFullScreenExclusiveModeEXT = 0;
#endif
#if defined(VK_EXT_headless_surface)
	PFN_vkCreateHeadlessSurfaceEXT vkCreateHeadlessSurfaceEXT = 0;
#endif
#if defined(VK_EXT_host_query_reset)
	PFN_vkResetQueryPoolEXT vkResetQueryPoolEXT = 0;
#endif
#if defined(VK_EXT_metal_surface)
	PFN_vkCreateMetalSurfaceEXT vkCreateMetalSurfaceEXT = 0;
#endif
#if defined(VK_GGP_stream_descriptor_surface)
	PFN_vkCreateStreamDescriptorSurfaceGGP vkCreateStreamDescriptorSurfaceGGP = 0;
#endif
#if defined(VK_NVX_image_view_handle)
	PFN_vkGetImageViewHandleNVX vkGetImageViewHandleNVX = 0;
#endif
#if defined(VK_NV_cooperative_matrix)
	PFN_vkGetPhysicalDeviceCooperativeMatrixPropertiesNV vkGetPhysicalDeviceCooperativeMatrixPropertiesNV = 0;
#endif
#if (defined(VK_EXT_full_screen_exclusive) && defined(VK_VERSION_1_1)) || (defined(VK_KHR_device_group) && defined(VK_EXT_full_screen_exclusive))
	PFN_vkGetDeviceGroupSurfacePresentModes2EXT vkGetDeviceGroupSurfacePresentModes2EXT = 0;
#endif

#if defined(AGK_IOS) || defined(AGK_MACOS)
    PFN_vkGetMTLDeviceMVK vkGetMTLDeviceMVK = 0;
    PFN_vkSetMTLTextureMVK vkSetMTLTextureMVK = 0;
#endif

void *g_VulkanLibrary = 0;
	
int VulkanLoadFunctionsInit()
{
#if defined(AGK_WINDOWS)
	g_VulkanLibrary = LoadLibrary( "vulkan-1.dll" );
	if ( !g_VulkanLibrary ) ReportAndReturnErrorID( APP_ERROR_VULKAN_DLL_NOT_FOUND );

	vkGetInstanceProcAddr = (PFN_vkGetInstanceProcAddr) GetProcAddress( (HMODULE)g_VulkanLibrary, "vkGetInstanceProcAddr" );
#elif defined(AGK_MACOS) || defined(AGK_IOS)
	g_VulkanLibrary = dlopen("libvulkan.dylib", RTLD_NOW | RTLD_LOCAL);
	if (!g_VulkanLibrary) g_VulkanLibrary = dlopen("libvulkan.dylib.1", RTLD_NOW | RTLD_LOCAL);
	if (!g_VulkanLibrary) g_VulkanLibrary = dlopen("libMoltenVK.dylib", RTLD_NOW | RTLD_LOCAL);
	if (!g_VulkanLibrary) ReportAndReturnErrorID( APP_ERROR_VULKAN_DLL_NOT_FOUND );

	vkGetInstanceProcAddr = (PFN_vkGetInstanceProcAddr) dlsym( g_VulkanLibrary, "vkGetInstanceProcAddr" );
    
    vkGetMTLDeviceMVK = (PFN_vkGetMTLDeviceMVK) dlsym( g_VulkanLibrary, "vkGetMTLDeviceMVK");
    vkSetMTLTextureMVK = (PFN_vkSetMTLTextureMVK) dlsym( g_VulkanLibrary, "vkSetMTLTextureMVK");
#else
	g_VulkanLibrary = dlopen("libvulkan.so", RTLD_NOW | RTLD_LOCAL);
	if (!g_VulkanLibrary) g_VulkanLibrary = dlopen("libvulkan.so.1", RTLD_NOW | RTLD_LOCAL);
	if (!g_VulkanLibrary) ReportAndReturnErrorID( APP_ERROR_VULKAN_DLL_NOT_FOUND );

	vkGetInstanceProcAddr = (PFN_vkGetInstanceProcAddr) dlsym( g_VulkanLibrary, "vkGetInstanceProcAddr" );
#endif

	if ( !vkGetInstanceProcAddr ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkGetInstanceProcAddr" );

	return APP_SUCCESS;
}

int VulkanCleanupFunctions()
{
#ifdef AGK_WINDOWS
	if ( g_VulkanLibrary ) FreeLibrary( (HMODULE)g_VulkanLibrary );
#elif defined(AGK_MACOS) || defined(AGK_IOS)
	if ( g_VulkanLibrary ) dlclose( g_VulkanLibrary );
#else
	if ( g_VulkanLibrary ) dlclose( g_VulkanLibrary );
#endif

	g_VulkanLibrary = 0;

	return APP_SUCCESS;
}

int VulkanLoadInstanceFunctions()
{
	if ( !vkGetInstanceProcAddr ) return APP_ERROR_VK_PROCADDR_NOT_DEF;

	vkCreateInstance = (PFN_vkCreateInstance) vkGetInstanceProcAddr( nullptr, "vkCreateInstance" );
	if ( !vkCreateInstance ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkCreateInstance" );

	vkEnumerateInstanceExtensionProperties = (PFN_vkEnumerateInstanceExtensionProperties) vkGetInstanceProcAddr( nullptr, "vkEnumerateInstanceExtensionProperties" );
	if ( !vkEnumerateInstanceExtensionProperties ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkEnumerateInstanceExtensionProperties" );

	vkEnumerateInstanceLayerProperties = (PFN_vkEnumerateInstanceLayerProperties) vkGetInstanceProcAddr( nullptr, "vkEnumerateInstanceLayerProperties" );
	if ( !vkEnumerateInstanceLayerProperties ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkEnumerateInstanceLayerProperties" );

#ifdef VK_VERSION_1_1
	vkEnumerateInstanceVersion = (PFN_vkEnumerateInstanceVersion) vkGetInstanceProcAddr( nullptr, "vkEnumerateInstanceVersion" );
#endif

	return APP_SUCCESS;
}

int VulkanLoadDeviceFunctions( VkInstance instance )
{
	if ( !vkGetInstanceProcAddr ) return APP_ERROR_VK_PROCADDR_NOT_DEF;
	if ( !instance ) return APP_ERROR_VK_INSTANCE_NOT_DEF;

	vkCreateDevice = (PFN_vkCreateDevice) vkGetInstanceProcAddr( instance, "vkCreateDevice" );
	if ( !vkCreateDevice ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkCreateDevice" );

	vkDestroyInstance = (PFN_vkDestroyInstance) vkGetInstanceProcAddr( instance, "vkDestroyInstance" );
	if ( !vkDestroyInstance ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkDestroyInstance" );

	vkEnumerateDeviceExtensionProperties = (PFN_vkEnumerateDeviceExtensionProperties) vkGetInstanceProcAddr( instance, "vkEnumerateDeviceExtensionProperties" );
	if ( !vkEnumerateDeviceExtensionProperties ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkEnumerateDeviceExtensionProperties" );

	vkEnumerateDeviceLayerProperties = (PFN_vkEnumerateDeviceLayerProperties) vkGetInstanceProcAddr( instance, "vkEnumerateDeviceLayerProperties" );
	if ( !vkEnumerateDeviceLayerProperties ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkEnumerateDeviceLayerProperties" );

	vkEnumeratePhysicalDevices = (PFN_vkEnumeratePhysicalDevices) vkGetInstanceProcAddr( instance, "vkEnumeratePhysicalDevices" );
	if ( !vkEnumeratePhysicalDevices ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkEnumeratePhysicalDevices" );
 
	vkGetDeviceProcAddr = (PFN_vkGetDeviceProcAddr) vkGetInstanceProcAddr( instance, "vkGetDeviceProcAddr" );
	if ( !vkGetDeviceProcAddr ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkGetDeviceProcAddr" );

	vkGetPhysicalDeviceFeatures = (PFN_vkGetPhysicalDeviceFeatures) vkGetInstanceProcAddr( instance, "vkGetPhysicalDeviceFeatures" );
	if ( !vkGetPhysicalDeviceFeatures ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkGetPhysicalDeviceFeatures" );
 
	vkGetPhysicalDeviceFormatProperties = (PFN_vkGetPhysicalDeviceFormatProperties) vkGetInstanceProcAddr( instance, "vkGetPhysicalDeviceFormatProperties" );
	if ( !vkGetPhysicalDeviceFormatProperties ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkGetPhysicalDeviceFormatProperties" );
 
	vkGetPhysicalDeviceImageFormatProperties = (PFN_vkGetPhysicalDeviceImageFormatProperties) vkGetInstanceProcAddr( instance, "vkGetPhysicalDeviceImageFormatProperties" );
	if ( !vkGetPhysicalDeviceImageFormatProperties ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkGetPhysicalDeviceImageFormatProperties" );
 
	vkGetPhysicalDeviceMemoryProperties = (PFN_vkGetPhysicalDeviceMemoryProperties) vkGetInstanceProcAddr( instance, "vkGetPhysicalDeviceMemoryProperties" );
	if ( !vkGetPhysicalDeviceMemoryProperties ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkGetPhysicalDeviceMemoryProperties" );

	vkGetPhysicalDeviceProperties = (PFN_vkGetPhysicalDeviceProperties) vkGetInstanceProcAddr( instance, "vkGetPhysicalDeviceProperties" );
	if ( !vkGetPhysicalDeviceProperties ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkGetPhysicalDeviceProperties" );
 
	vkGetPhysicalDeviceQueueFamilyProperties = (PFN_vkGetPhysicalDeviceQueueFamilyProperties) vkGetInstanceProcAddr( instance, "vkGetPhysicalDeviceQueueFamilyProperties" );
	if ( !vkGetPhysicalDeviceQueueFamilyProperties ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkGetPhysicalDeviceQueueFamilyProperties" );

	vkGetPhysicalDeviceSparseImageFormatProperties = (PFN_vkGetPhysicalDeviceSparseImageFormatProperties) vkGetInstanceProcAddr( instance, "vkGetPhysicalDeviceSparseImageFormatProperties" );
	if ( !vkGetPhysicalDeviceSparseImageFormatProperties ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkGetPhysicalDeviceSparseImageFormatProperties" );
 
#ifdef VK_VERSION_1_1
	vkEnumeratePhysicalDeviceGroups = (PFN_vkEnumeratePhysicalDeviceGroups) vkGetInstanceProcAddr( instance, "vkEnumeratePhysicalDeviceGroups" );
	vkGetPhysicalDeviceExternalBufferProperties = (PFN_vkGetPhysicalDeviceExternalBufferProperties) vkGetInstanceProcAddr( instance, "vkGetPhysicalDeviceExternalBufferProperties" );
	vkGetPhysicalDeviceExternalFenceProperties = (PFN_vkGetPhysicalDeviceExternalFenceProperties) vkGetInstanceProcAddr( instance, "vkGetPhysicalDeviceExternalFenceProperties" );
	vkGetPhysicalDeviceExternalSemaphoreProperties = (PFN_vkGetPhysicalDeviceExternalSemaphoreProperties) vkGetInstanceProcAddr( instance, "vkGetPhysicalDeviceExternalSemaphoreProperties" );
	vkGetPhysicalDeviceFeatures2 = (PFN_vkGetPhysicalDeviceFeatures2) vkGetInstanceProcAddr( instance, "vkGetPhysicalDeviceFeatures2" );
	vkGetPhysicalDeviceFormatProperties2 = (PFN_vkGetPhysicalDeviceFormatProperties2) vkGetInstanceProcAddr( instance, "vkGetPhysicalDeviceFormatProperties2" );
	vkGetPhysicalDeviceImageFormatProperties2 = (PFN_vkGetPhysicalDeviceImageFormatProperties2) vkGetInstanceProcAddr( instance, "vkGetPhysicalDeviceImageFormatProperties2" );
	vkGetPhysicalDeviceMemoryProperties2 = (PFN_vkGetPhysicalDeviceMemoryProperties2) vkGetInstanceProcAddr( instance, "vkGetPhysicalDeviceMemoryProperties2" );
	vkGetPhysicalDeviceProperties2 = (PFN_vkGetPhysicalDeviceProperties2) vkGetInstanceProcAddr( instance, "vkGetPhysicalDeviceProperties2" );
	vkGetPhysicalDeviceQueueFamilyProperties2 = (PFN_vkGetPhysicalDeviceQueueFamilyProperties2) vkGetInstanceProcAddr( instance, "vkGetPhysicalDeviceQueueFamilyProperties2" );
	vkGetPhysicalDeviceSparseImageFormatProperties2 = (PFN_vkGetPhysicalDeviceSparseImageFormatProperties2) vkGetInstanceProcAddr( instance, "vkGetPhysicalDeviceSparseImageFormatProperties2" );
#endif // VK_VERSION_1_1

#if defined(VK_EXT_acquire_xlib_display)
	vkAcquireXlibDisplayEXT = (PFN_vkAcquireXlibDisplayEXT) vkGetInstanceProcAddr( instance, "vkAcquireXlibDisplayEXT" );
	vkGetRandROutputDisplayEXT = (PFN_vkGetRandROutputDisplayEXT) vkGetInstanceProcAddr( instance, "vkGetRandROutputDisplayEXT" );
#endif 
#if defined(VK_EXT_calibrated_timestamps)
	vkGetPhysicalDeviceCalibrateableTimeDomainsEXT = (PFN_vkGetPhysicalDeviceCalibrateableTimeDomainsEXT) vkGetInstanceProcAddr( instance, "vkGetPhysicalDeviceCalibrateableTimeDomainsEXT" );
#endif 
#if defined(VK_EXT_debug_report)
	vkCreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT) vkGetInstanceProcAddr( instance, "vkCreateDebugReportCallbackEXT" );
	vkDebugReportMessageEXT = (PFN_vkDebugReportMessageEXT) vkGetInstanceProcAddr( instance, "vkDebugReportMessageEXT" );
	vkDestroyDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT) vkGetInstanceProcAddr( instance, "vkDestroyDebugReportCallbackEXT" );
#endif 
#if defined(VK_EXT_debug_utils)
	vkCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr( instance, "vkCreateDebugUtilsMessengerEXT" );
	vkDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr( instance, "vkDestroyDebugUtilsMessengerEXT" );
	vkSubmitDebugUtilsMessageEXT = (PFN_vkSubmitDebugUtilsMessageEXT) vkGetInstanceProcAddr( instance, "vkSubmitDebugUtilsMessageEXT" );
#endif 
#if defined(VK_EXT_direct_mode_display)
	vkReleaseDisplayEXT = (PFN_vkReleaseDisplayEXT) vkGetInstanceProcAddr( instance, "vkReleaseDisplayEXT" );
#endif 
#if defined(VK_EXT_display_surface_counter)
	vkGetPhysicalDeviceSurfaceCapabilities2EXT = (PFN_vkGetPhysicalDeviceSurfaceCapabilities2EXT) vkGetInstanceProcAddr( instance, "vkGetPhysicalDeviceSurfaceCapabilities2EXT" );
#endif 
#if defined(VK_EXT_sample_locations)
	vkGetPhysicalDeviceMultisamplePropertiesEXT = (PFN_vkGetPhysicalDeviceMultisamplePropertiesEXT) vkGetInstanceProcAddr( instance, "vkGetPhysicalDeviceMultisamplePropertiesEXT" );
#endif 
#if defined(VK_FUCHSIA_imagepipe_surface)
	vkCreateImagePipeSurfaceFUCHSIA = (PFN_vkCreateImagePipeSurfaceFUCHSIA) vkGetInstanceProcAddr( instance, "vkCreateImagePipeSurfaceFUCHSIA" );
#endif 
#if defined(VK_KHR_android_surface)
	vkCreateAndroidSurfaceKHR = (PFN_vkCreateAndroidSurfaceKHR) vkGetInstanceProcAddr( instance, "vkCreateAndroidSurfaceKHR" );
#endif 
#if defined(VK_KHR_device_group_creation)
	vkEnumeratePhysicalDeviceGroupsKHR = (PFN_vkEnumeratePhysicalDeviceGroupsKHR) vkGetInstanceProcAddr( instance, "vkEnumeratePhysicalDeviceGroupsKHR" );
#endif 
#if defined(VK_KHR_display)
	vkCreateDisplayModeKHR = (PFN_vkCreateDisplayModeKHR) vkGetInstanceProcAddr( instance, "vkCreateDisplayModeKHR" );
	vkCreateDisplayPlaneSurfaceKHR = (PFN_vkCreateDisplayPlaneSurfaceKHR) vkGetInstanceProcAddr( instance, "vkCreateDisplayPlaneSurfaceKHR" );
	vkGetDisplayModePropertiesKHR = (PFN_vkGetDisplayModePropertiesKHR) vkGetInstanceProcAddr( instance, "vkGetDisplayModePropertiesKHR" );
	vkGetDisplayPlaneCapabilitiesKHR = (PFN_vkGetDisplayPlaneCapabilitiesKHR) vkGetInstanceProcAddr( instance, "vkGetDisplayPlaneCapabilitiesKHR" );
	vkGetDisplayPlaneSupportedDisplaysKHR = (PFN_vkGetDisplayPlaneSupportedDisplaysKHR) vkGetInstanceProcAddr( instance, "vkGetDisplayPlaneSupportedDisplaysKHR" );
	vkGetPhysicalDeviceDisplayPlanePropertiesKHR = (PFN_vkGetPhysicalDeviceDisplayPlanePropertiesKHR) vkGetInstanceProcAddr( instance, "vkGetPhysicalDeviceDisplayPlanePropertiesKHR" );
	vkGetPhysicalDeviceDisplayPropertiesKHR = (PFN_vkGetPhysicalDeviceDisplayPropertiesKHR) vkGetInstanceProcAddr( instance, "vkGetPhysicalDeviceDisplayPropertiesKHR" );
#endif 
#if defined(VK_KHR_external_fence_capabilities)
	vkGetPhysicalDeviceExternalFencePropertiesKHR = (PFN_vkGetPhysicalDeviceExternalFencePropertiesKHR) vkGetInstanceProcAddr( instance, "vkGetPhysicalDeviceExternalFencePropertiesKHR" );
#endif 
#if defined(VK_KHR_external_memory_capabilities)
	vkGetPhysicalDeviceExternalBufferPropertiesKHR = (PFN_vkGetPhysicalDeviceExternalBufferPropertiesKHR) vkGetInstanceProcAddr( instance, "vkGetPhysicalDeviceExternalBufferPropertiesKHR" );
#endif 
#if defined(VK_KHR_external_semaphore_capabilities)
	vkGetPhysicalDeviceExternalSemaphorePropertiesKHR = (PFN_vkGetPhysicalDeviceExternalSemaphorePropertiesKHR) vkGetInstanceProcAddr( instance, "vkGetPhysicalDeviceExternalSemaphorePropertiesKHR" );
#endif 
#if defined(VK_KHR_get_display_properties2)
	vkGetDisplayModeProperties2KHR = (PFN_vkGetDisplayModeProperties2KHR) vkGetInstanceProcAddr( instance, "vkGetDisplayModeProperties2KHR" );
	vkGetDisplayPlaneCapabilities2KHR = (PFN_vkGetDisplayPlaneCapabilities2KHR) vkGetInstanceProcAddr( instance, "vkGetDisplayPlaneCapabilities2KHR" );
	vkGetPhysicalDeviceDisplayPlaneProperties2KHR = (PFN_vkGetPhysicalDeviceDisplayPlaneProperties2KHR) vkGetInstanceProcAddr( instance, "vkGetPhysicalDeviceDisplayPlaneProperties2KHR" );
	vkGetPhysicalDeviceDisplayProperties2KHR = (PFN_vkGetPhysicalDeviceDisplayProperties2KHR) vkGetInstanceProcAddr( instance, "vkGetPhysicalDeviceDisplayProperties2KHR" );
#endif 
#if defined(VK_KHR_get_physical_device_properties2)
	vkGetPhysicalDeviceFeatures2KHR = (PFN_vkGetPhysicalDeviceFeatures2KHR) vkGetInstanceProcAddr( instance, "vkGetPhysicalDeviceFeatures2KHR" );
	vkGetPhysicalDeviceFormatProperties2KHR = (PFN_vkGetPhysicalDeviceFormatProperties2KHR) vkGetInstanceProcAddr( instance, "vkGetPhysicalDeviceFormatProperties2KHR" );
	vkGetPhysicalDeviceImageFormatProperties2KHR = (PFN_vkGetPhysicalDeviceImageFormatProperties2KHR) vkGetInstanceProcAddr( instance, "vkGetPhysicalDeviceImageFormatProperties2KHR" );
	vkGetPhysicalDeviceMemoryProperties2KHR = (PFN_vkGetPhysicalDeviceMemoryProperties2KHR) vkGetInstanceProcAddr( instance, "vkGetPhysicalDeviceMemoryProperties2KHR" );
	vkGetPhysicalDeviceProperties2KHR = (PFN_vkGetPhysicalDeviceProperties2KHR) vkGetInstanceProcAddr( instance, "vkGetPhysicalDeviceProperties2KHR" );
	vkGetPhysicalDeviceQueueFamilyProperties2KHR = (PFN_vkGetPhysicalDeviceQueueFamilyProperties2KHR) vkGetInstanceProcAddr( instance, "vkGetPhysicalDeviceQueueFamilyProperties2KHR" );
	vkGetPhysicalDeviceSparseImageFormatProperties2KHR = (PFN_vkGetPhysicalDeviceSparseImageFormatProperties2KHR) vkGetInstanceProcAddr( instance, "vkGetPhysicalDeviceSparseImageFormatProperties2KHR" );
#endif 
#if defined(VK_KHR_get_surface_capabilities2)
	vkGetPhysicalDeviceSurfaceCapabilities2KHR = (PFN_vkGetPhysicalDeviceSurfaceCapabilities2KHR) vkGetInstanceProcAddr( instance, "vkGetPhysicalDeviceSurfaceCapabilities2KHR" );
	vkGetPhysicalDeviceSurfaceFormats2KHR = (PFN_vkGetPhysicalDeviceSurfaceFormats2KHR) vkGetInstanceProcAddr( instance, "vkGetPhysicalDeviceSurfaceFormats2KHR" );
#endif 
#if defined(VK_KHR_surface)
	vkDestroySurfaceKHR = (PFN_vkDestroySurfaceKHR) vkGetInstanceProcAddr( instance, "vkDestroySurfaceKHR" );
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR = (PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR) vkGetInstanceProcAddr( instance, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR" );
	vkGetPhysicalDeviceSurfaceFormatsKHR = (PFN_vkGetPhysicalDeviceSurfaceFormatsKHR) vkGetInstanceProcAddr( instance, "vkGetPhysicalDeviceSurfaceFormatsKHR" );
	vkGetPhysicalDeviceSurfacePresentModesKHR = (PFN_vkGetPhysicalDeviceSurfacePresentModesKHR) vkGetInstanceProcAddr( instance, "vkGetPhysicalDeviceSurfacePresentModesKHR" );
	vkGetPhysicalDeviceSurfaceSupportKHR = (PFN_vkGetPhysicalDeviceSurfaceSupportKHR) vkGetInstanceProcAddr( instance, "vkGetPhysicalDeviceSurfaceSupportKHR" );
#endif 
#if defined(VK_KHR_wayland_surface)
	vkCreateWaylandSurfaceKHR = (PFN_vkCreateWaylandSurfaceKHR) vkGetInstanceProcAddr( instance, "vkCreateWaylandSurfaceKHR" );
	vkGetPhysicalDeviceWaylandPresentationSupportKHR = (PFN_vkGetPhysicalDeviceWaylandPresentationSupportKHR) vkGetInstanceProcAddr( instance, "vkGetPhysicalDeviceWaylandPresentationSupportKHR" );
#endif 
#if defined(VK_KHR_win32_surface)
	vkCreateWin32SurfaceKHR = (PFN_vkCreateWin32SurfaceKHR) vkGetInstanceProcAddr( instance, "vkCreateWin32SurfaceKHR" );
	vkGetPhysicalDeviceWin32PresentationSupportKHR = (PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR) vkGetInstanceProcAddr( instance, "vkGetPhysicalDeviceWin32PresentationSupportKHR" );
#endif 
#if defined(VK_KHR_xcb_surface)
	vkCreateXcbSurfaceKHR = (PFN_vkCreateXcbSurfaceKHR) vkGetInstanceProcAddr( instance, "vkCreateXcbSurfaceKHR" );
	vkGetPhysicalDeviceXcbPresentationSupportKHR = (PFN_vkGetPhysicalDeviceXcbPresentationSupportKHR) vkGetInstanceProcAddr( instance, "vkGetPhysicalDeviceXcbPresentationSupportKHR" );
#endif 
#if defined(VK_KHR_xlib_surface)
	vkCreateXlibSurfaceKHR = (PFN_vkCreateXlibSurfaceKHR) vkGetInstanceProcAddr( instance, "vkCreateXlibSurfaceKHR" );
	vkGetPhysicalDeviceXlibPresentationSupportKHR = (PFN_vkGetPhysicalDeviceXlibPresentationSupportKHR) vkGetInstanceProcAddr( instance, "vkGetPhysicalDeviceXlibPresentationSupportKHR" );
#endif
#if defined(VK_MVK_ios_surface)
	vkCreateIOSSurfaceMVK = (PFN_vkCreateIOSSurfaceMVK) vkGetInstanceProcAddr( instance, "vkCreateIOSSurfaceMVK" );
#endif
#if defined(VK_MVK_macos_surface)
	vkCreateMacOSSurfaceMVK = (PFN_vkCreateMacOSSurfaceMVK) vkGetInstanceProcAddr( instance, "vkCreateMacOSSurfaceMVK" );
#endif
#if defined(VK_NN_vi_surface)
	vkCreateViSurfaceNN = (PFN_vkCreateViSurfaceNN) vkGetInstanceProcAddr( instance, "vkCreateViSurfaceNN" );
#endif
#if defined(VK_NVX_device_generated_commands)
	vkGetPhysicalDeviceGeneratedCommandsPropertiesNVX = (PFN_vkGetPhysicalDeviceGeneratedCommandsPropertiesNVX) vkGetInstanceProcAddr( instance, "vkGetPhysicalDeviceGeneratedCommandsPropertiesNVX" );
#endif
#if defined(VK_NV_external_memory_capabilities)
	vkGetPhysicalDeviceExternalImageFormatPropertiesNV = (PFN_vkGetPhysicalDeviceExternalImageFormatPropertiesNV) vkGetInstanceProcAddr( instance, "vkGetPhysicalDeviceExternalImageFormatPropertiesNV" );
#endif
#if (defined(VK_KHR_device_group) && defined(VK_KHR_surface)) || (defined(VK_KHR_swapchain) && defined(VK_VERSION_1_1))
	vkGetPhysicalDevicePresentRectanglesKHR = (PFN_vkGetPhysicalDevicePresentRectanglesKHR) vkGetInstanceProcAddr( instance, "vkGetPhysicalDevicePresentRectanglesKHR" );
#endif

	return APP_SUCCESS;
}

int VulkanLoadMainFunctions( VkDevice device )
{
	if ( !vkGetDeviceProcAddr ) return APP_ERROR_VK_PROCADDR_NOT_DEF;
	if ( !device ) return APP_ERROR_VK_INSTANCE_NOT_DEF;

	vkAllocateCommandBuffers = (PFN_vkAllocateCommandBuffers) vkGetDeviceProcAddr( device, "vkAllocateCommandBuffers" );
	if ( !vkAllocateCommandBuffers ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkAllocateCommandBuffers" );

	vkAllocateDescriptorSets = (PFN_vkAllocateDescriptorSets) vkGetDeviceProcAddr( device, "vkAllocateDescriptorSets" );
	if ( !vkAllocateDescriptorSets ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkAllocateDescriptorSets" );

	vkAllocateMemory = (PFN_vkAllocateMemory) vkGetDeviceProcAddr( device, "vkAllocateMemory" );
	if ( !vkAllocateMemory ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkAllocateMemory" );

	vkBeginCommandBuffer = (PFN_vkBeginCommandBuffer) vkGetDeviceProcAddr( device, "vkBeginCommandBuffer" );
	if ( !vkBeginCommandBuffer ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkBeginCommandBuffer" );

	vkBindBufferMemory = (PFN_vkBindBufferMemory) vkGetDeviceProcAddr( device, "vkBindBufferMemory" );
	if ( !vkBindBufferMemory ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkBindBufferMemory" );

	vkBindImageMemory = (PFN_vkBindImageMemory) vkGetDeviceProcAddr( device, "vkBindImageMemory" );
	if ( !vkBindImageMemory ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkBindImageMemory" );

	vkCmdBeginQuery = (PFN_vkCmdBeginQuery) vkGetDeviceProcAddr( device, "vkCmdBeginQuery" );
	if ( !vkCmdBeginQuery ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkCmdBeginQuery" );

	vkCmdBeginRenderPass = (PFN_vkCmdBeginRenderPass) vkGetDeviceProcAddr( device, "vkCmdBeginRenderPass" );
	if ( !vkCmdBeginRenderPass ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkCmdBeginRenderPass" );

	vkCmdBindDescriptorSets = (PFN_vkCmdBindDescriptorSets) vkGetDeviceProcAddr( device, "vkCmdBindDescriptorSets" );
	if ( !vkCmdBindDescriptorSets ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkCmdBindDescriptorSets" );

	vkCmdBindIndexBuffer = (PFN_vkCmdBindIndexBuffer) vkGetDeviceProcAddr( device, "vkCmdBindIndexBuffer" );
	if ( !vkCmdBindIndexBuffer ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkCmdBindIndexBuffer" );

	vkCmdBindPipeline = (PFN_vkCmdBindPipeline) vkGetDeviceProcAddr( device, "vkCmdBindPipeline" );
	if ( !vkCmdBindPipeline ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkCmdBindPipeline" );

	vkCmdBindVertexBuffers = (PFN_vkCmdBindVertexBuffers) vkGetDeviceProcAddr( device, "vkCmdBindVertexBuffers" );
	if ( !vkCmdBindVertexBuffers ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkCmdBindVertexBuffers" );

	vkCmdBlitImage = (PFN_vkCmdBlitImage) vkGetDeviceProcAddr( device, "vkCmdBlitImage" );
	if ( !vkCmdBlitImage ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkCmdBlitImage" );

	vkCmdClearAttachments = (PFN_vkCmdClearAttachments) vkGetDeviceProcAddr( device, "vkCmdClearAttachments" );
	if ( !vkCmdClearAttachments ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkCmdClearAttachments" );

	vkCmdClearColorImage = (PFN_vkCmdClearColorImage) vkGetDeviceProcAddr( device, "vkCmdClearColorImage" );
	if ( !vkCmdClearColorImage ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkCmdClearColorImage" );

	vkCmdClearDepthStencilImage = (PFN_vkCmdClearDepthStencilImage) vkGetDeviceProcAddr( device, "vkCmdClearDepthStencilImage" );
	if ( !vkCmdClearDepthStencilImage ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkCmdClearDepthStencilImage" );

	vkCmdCopyBuffer = (PFN_vkCmdCopyBuffer) vkGetDeviceProcAddr( device, "vkCmdCopyBuffer" );
	if ( !vkCmdCopyBuffer ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkCmdCopyBuffer" );

	vkCmdCopyBufferToImage = (PFN_vkCmdCopyBufferToImage) vkGetDeviceProcAddr( device, "vkCmdCopyBufferToImage" );
	if ( !vkCmdCopyBufferToImage ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkCmdCopyBufferToImage" );

	vkCmdCopyImage = (PFN_vkCmdCopyImage) vkGetDeviceProcAddr( device, "vkCmdCopyImage" );
	if ( !vkCmdCopyImage ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkCmdCopyImage" );

	vkCmdCopyImageToBuffer = (PFN_vkCmdCopyImageToBuffer) vkGetDeviceProcAddr( device, "vkCmdCopyImageToBuffer" );
	if ( !vkCmdCopyImageToBuffer ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkCmdCopyImageToBuffer" );

	vkCmdCopyQueryPoolResults = (PFN_vkCmdCopyQueryPoolResults) vkGetDeviceProcAddr( device, "vkCmdCopyQueryPoolResults" );
	if ( !vkCmdCopyQueryPoolResults ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkCmdCopyQueryPoolResults" );

	vkCmdDispatch = (PFN_vkCmdDispatch) vkGetDeviceProcAddr( device, "vkCmdDispatch" );
	if ( !vkCmdDispatch ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkCmdDispatch" );

	vkCmdDispatchIndirect = (PFN_vkCmdDispatchIndirect) vkGetDeviceProcAddr( device, "vkCmdDispatchIndirect" );
	if ( !vkCmdDispatchIndirect ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkCmdDispatchIndirect" );

	vkCmdDraw = (PFN_vkCmdDraw) vkGetDeviceProcAddr( device, "vkCmdDraw" );
	if ( !vkCmdDraw ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkCmdDraw" );

	vkCmdDrawIndexed = (PFN_vkCmdDrawIndexed) vkGetDeviceProcAddr( device, "vkCmdDrawIndexed" );
	if ( !vkCmdDrawIndexed ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkCmdDrawIndexed" );

	vkCmdDrawIndexedIndirect = (PFN_vkCmdDrawIndexedIndirect) vkGetDeviceProcAddr( device, "vkCmdDrawIndexedIndirect" );
	if ( !vkCmdDrawIndexedIndirect ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkCmdDrawIndexedIndirect" );

	vkCmdDrawIndirect = (PFN_vkCmdDrawIndirect) vkGetDeviceProcAddr( device, "vkCmdDrawIndirect" );
	if ( !vkCmdDrawIndirect ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkCmdDrawIndirect" );

	vkCmdEndQuery = (PFN_vkCmdEndQuery) vkGetDeviceProcAddr( device, "vkCmdEndQuery" );
	if ( !vkCmdEndQuery ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkCmdEndQuery" );

	vkCmdEndRenderPass = (PFN_vkCmdEndRenderPass) vkGetDeviceProcAddr( device, "vkCmdEndRenderPass" );
	if ( !vkCmdEndRenderPass ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkCmdEndRenderPass" );

	vkCmdExecuteCommands = (PFN_vkCmdExecuteCommands) vkGetDeviceProcAddr( device, "vkCmdExecuteCommands" );
	if ( !vkCmdExecuteCommands ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkCmdExecuteCommands" );

	vkCmdFillBuffer = (PFN_vkCmdFillBuffer) vkGetDeviceProcAddr( device, "vkCmdFillBuffer" );
	if ( !vkCmdFillBuffer ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkCmdFillBuffer" );

	vkCmdNextSubpass = (PFN_vkCmdNextSubpass) vkGetDeviceProcAddr( device, "vkCmdNextSubpass" );
	if ( !vkCmdNextSubpass ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkCmdNextSubpass" );

	vkCmdPipelineBarrier = (PFN_vkCmdPipelineBarrier) vkGetDeviceProcAddr( device, "vkCmdPipelineBarrier" );
	if ( !vkCmdPipelineBarrier ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkCmdPipelineBarrier" );

	vkCmdPushConstants = (PFN_vkCmdPushConstants) vkGetDeviceProcAddr( device, "vkCmdPushConstants" );
	if ( !vkCmdPushConstants ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkCmdPushConstants" );

	vkCmdResetEvent = (PFN_vkCmdResetEvent) vkGetDeviceProcAddr( device, "vkCmdResetEvent" );
	if ( !vkCmdResetEvent ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkCmdResetEvent" );

	vkCmdResetQueryPool = (PFN_vkCmdResetQueryPool) vkGetDeviceProcAddr( device, "vkCmdResetQueryPool" );
	if ( !vkCmdResetQueryPool ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkCmdResetQueryPool" );

	vkCmdResolveImage = (PFN_vkCmdResolveImage) vkGetDeviceProcAddr( device, "vkCmdResolveImage" );
	if ( !vkCmdResolveImage ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkCmdResolveImage" );

	vkCmdSetBlendConstants = (PFN_vkCmdSetBlendConstants) vkGetDeviceProcAddr( device, "vkCmdSetBlendConstants" );
	if ( !vkCmdSetBlendConstants ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkCmdSetBlendConstants" );

	vkCmdSetDepthBias = (PFN_vkCmdSetDepthBias) vkGetDeviceProcAddr( device, "vkCmdSetDepthBias" );
	if ( !vkCmdSetDepthBias ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkCmdSetDepthBias" );

	vkCmdSetDepthBounds = (PFN_vkCmdSetDepthBounds) vkGetDeviceProcAddr( device, "vkCmdSetDepthBounds" );
	if ( !vkCmdSetDepthBounds ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkCmdSetDepthBounds" );

	vkCmdSetEvent = (PFN_vkCmdSetEvent) vkGetDeviceProcAddr( device, "vkCmdSetEvent" );
	if ( !vkCmdSetEvent ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkCmdSetEvent" );

	vkCmdSetLineWidth = (PFN_vkCmdSetLineWidth) vkGetDeviceProcAddr( device, "vkCmdSetLineWidth" );
	if ( !vkCmdSetLineWidth ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkCmdSetLineWidth" );

	vkCmdSetScissor = (PFN_vkCmdSetScissor) vkGetDeviceProcAddr( device, "vkCmdSetScissor" );
	if ( !vkCmdSetScissor ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkCmdSetScissor" );

	vkCmdSetStencilCompareMask = (PFN_vkCmdSetStencilCompareMask) vkGetDeviceProcAddr( device, "vkCmdSetStencilCompareMask" );
	if ( !vkCmdSetStencilCompareMask ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkCmdSetStencilCompareMask" );

	vkCmdSetStencilReference = (PFN_vkCmdSetStencilReference) vkGetDeviceProcAddr( device, "vkCmdSetStencilReference" );
	if ( !vkCmdSetStencilReference ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkCmdSetStencilReference" );

	vkCmdSetStencilWriteMask = (PFN_vkCmdSetStencilWriteMask) vkGetDeviceProcAddr( device, "vkCmdSetStencilWriteMask" );
	if ( !vkCmdSetStencilWriteMask ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkCmdSetStencilWriteMask" );

	vkCmdSetViewport = (PFN_vkCmdSetViewport) vkGetDeviceProcAddr( device, "vkCmdSetViewport" );
	if ( !vkCmdSetViewport ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkCmdSetViewport" );

	vkCmdUpdateBuffer = (PFN_vkCmdUpdateBuffer) vkGetDeviceProcAddr( device, "vkCmdUpdateBuffer" );
	if ( !vkCmdUpdateBuffer ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkCmdUpdateBuffer" );

	vkCmdWaitEvents = (PFN_vkCmdWaitEvents) vkGetDeviceProcAddr( device, "vkCmdWaitEvents" );
	if ( !vkCmdWaitEvents ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkCmdWaitEvents" );

	vkCmdWriteTimestamp = (PFN_vkCmdWriteTimestamp) vkGetDeviceProcAddr( device, "vkCmdWriteTimestamp" );
	if ( !vkCmdWriteTimestamp ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkCmdWriteTimestamp" );

	vkCreateBuffer = (PFN_vkCreateBuffer) vkGetDeviceProcAddr( device, "vkCreateBuffer" );
	if ( !vkCreateBuffer ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkCreateBuffer" );

	vkCreateBufferView = (PFN_vkCreateBufferView) vkGetDeviceProcAddr( device, "vkCreateBufferView" );
	if ( !vkCreateBufferView ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkCreateBufferView" );

	vkCreateCommandPool = (PFN_vkCreateCommandPool) vkGetDeviceProcAddr( device, "vkCreateCommandPool" );
	if ( !vkCreateCommandPool ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkCreateCommandPool" );

	vkCreateComputePipelines = (PFN_vkCreateComputePipelines) vkGetDeviceProcAddr( device, "vkCreateComputePipelines" );
	if ( !vkCreateComputePipelines ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkCreateComputePipelines" );

	vkCreateDescriptorPool = (PFN_vkCreateDescriptorPool) vkGetDeviceProcAddr( device, "vkCreateDescriptorPool" );
	if ( !vkCreateDescriptorPool ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkCreateDescriptorPool" );

	vkCreateDescriptorSetLayout = (PFN_vkCreateDescriptorSetLayout) vkGetDeviceProcAddr( device, "vkCreateDescriptorSetLayout" );
	if ( !vkCreateDescriptorSetLayout ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkCreateDescriptorSetLayout" );

	vkCreateEvent = (PFN_vkCreateEvent) vkGetDeviceProcAddr( device, "vkCreateEvent" );
	if ( !vkCreateEvent ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkCreateEvent" );

	vkCreateFence = (PFN_vkCreateFence) vkGetDeviceProcAddr( device, "vkCreateFence" );
	if ( !vkCreateFence ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkCreateFence" );

	vkCreateFramebuffer = (PFN_vkCreateFramebuffer) vkGetDeviceProcAddr( device, "vkCreateFramebuffer" );
	if ( !vkCreateFramebuffer ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkCreateFramebuffer" );

	vkCreateGraphicsPipelines = (PFN_vkCreateGraphicsPipelines) vkGetDeviceProcAddr( device, "vkCreateGraphicsPipelines" );
	if ( !vkCreateGraphicsPipelines ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkCreateGraphicsPipelines" );

	vkCreateImage = (PFN_vkCreateImage) vkGetDeviceProcAddr( device, "vkCreateImage" );
	if ( !vkCreateImage ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkCreateImage" );

	vkCreateImageView = (PFN_vkCreateImageView) vkGetDeviceProcAddr( device, "vkCreateImageView" );
	if ( !vkCreateImageView ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkCreateImageView" );

	vkCreatePipelineCache = (PFN_vkCreatePipelineCache) vkGetDeviceProcAddr( device, "vkCreatePipelineCache" );
	if ( !vkCreatePipelineCache ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkCreatePipelineCache" );

	vkCreatePipelineLayout = (PFN_vkCreatePipelineLayout) vkGetDeviceProcAddr( device, "vkCreatePipelineLayout" );
	if ( !vkCreatePipelineLayout ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkCreatePipelineLayout" );

	vkCreateQueryPool = (PFN_vkCreateQueryPool) vkGetDeviceProcAddr( device, "vkCreateQueryPool" );
	if ( !vkCreateQueryPool ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkCreateQueryPool" );

	vkCreateRenderPass = (PFN_vkCreateRenderPass) vkGetDeviceProcAddr( device, "vkCreateRenderPass" );
	if ( !vkCreateRenderPass ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkCreateRenderPass" );

	vkCreateSampler = (PFN_vkCreateSampler) vkGetDeviceProcAddr( device, "vkCreateSampler" );
	if ( !vkCreateSampler ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkCreateSampler" );

	vkCreateSemaphore = (PFN_vkCreateSemaphore) vkGetDeviceProcAddr( device, "vkCreateSemaphore" );
	if ( !vkCreateSemaphore ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkCreateSemaphore" );

	vkCreateShaderModule = (PFN_vkCreateShaderModule) vkGetDeviceProcAddr( device, "vkCreateShaderModule" );
	if ( !vkCreateShaderModule ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkCreateShaderModule" );

	vkDestroyBuffer = (PFN_vkDestroyBuffer) vkGetDeviceProcAddr( device, "vkDestroyBuffer" );
	if ( !vkDestroyBuffer ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkDestroyBuffer" );

	vkDestroyBufferView = (PFN_vkDestroyBufferView) vkGetDeviceProcAddr( device, "vkDestroyBufferView" );
	if ( !vkDestroyBufferView ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkDestroyBufferView" );

	vkDestroyCommandPool = (PFN_vkDestroyCommandPool) vkGetDeviceProcAddr( device, "vkDestroyCommandPool" );
	if ( !vkDestroyCommandPool ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkDestroyCommandPool" );

	vkDestroyDescriptorPool = (PFN_vkDestroyDescriptorPool) vkGetDeviceProcAddr( device, "vkDestroyDescriptorPool" );
	if ( !vkDestroyDescriptorPool ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkDestroyDescriptorPool" );

	vkDestroyDescriptorSetLayout = (PFN_vkDestroyDescriptorSetLayout) vkGetDeviceProcAddr( device, "vkDestroyDescriptorSetLayout" );
	if ( !vkDestroyDescriptorSetLayout ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkDestroyDescriptorSetLayout" );

	vkDestroyDevice = (PFN_vkDestroyDevice) vkGetDeviceProcAddr( device, "vkDestroyDevice" );
	if ( !vkDestroyDevice ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkDestroyDevice" );

	vkDestroyEvent = (PFN_vkDestroyEvent) vkGetDeviceProcAddr( device, "vkDestroyEvent" );
	if ( !vkDestroyEvent ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkDestroyEvent" );

	vkDestroyFence = (PFN_vkDestroyFence) vkGetDeviceProcAddr( device, "vkDestroyFence" );
	if ( !vkDestroyFence ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkDestroyFence" );

	vkDestroyFramebuffer = (PFN_vkDestroyFramebuffer) vkGetDeviceProcAddr( device, "vkDestroyFramebuffer" );
	if ( !vkDestroyFramebuffer ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkDestroyFramebuffer" );

	vkDestroyImage = (PFN_vkDestroyImage) vkGetDeviceProcAddr( device, "vkDestroyImage" );
	if ( !vkDestroyImage ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkDestroyImage" );

	vkDestroyImageView = (PFN_vkDestroyImageView) vkGetDeviceProcAddr( device, "vkDestroyImageView" );
	if ( !vkDestroyImageView ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkDestroyImageView" );

	vkDestroyPipeline = (PFN_vkDestroyPipeline) vkGetDeviceProcAddr( device, "vkDestroyPipeline" );
	if ( !vkDestroyPipeline ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkDestroyPipeline" );

	vkDestroyPipelineCache = (PFN_vkDestroyPipelineCache) vkGetDeviceProcAddr( device, "vkDestroyPipelineCache" );
	if ( !vkDestroyPipelineCache ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkDestroyPipelineCache" );

	vkDestroyPipelineLayout = (PFN_vkDestroyPipelineLayout) vkGetDeviceProcAddr( device, "vkDestroyPipelineLayout" );
	if ( !vkDestroyPipelineLayout ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkDestroyPipelineLayout" );

	vkDestroyQueryPool = (PFN_vkDestroyQueryPool) vkGetDeviceProcAddr( device, "vkDestroyQueryPool" );
	if ( !vkDestroyQueryPool ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkDestroyQueryPool" );

	vkDestroyRenderPass = (PFN_vkDestroyRenderPass) vkGetDeviceProcAddr( device, "vkDestroyRenderPass" );
	if ( !vkDestroyRenderPass ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkDestroyRenderPass" );

	vkDestroySampler = (PFN_vkDestroySampler) vkGetDeviceProcAddr( device, "vkDestroySampler" );
	if ( !vkDestroySampler ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkDestroySampler" );

	vkDestroySemaphore = (PFN_vkDestroySemaphore) vkGetDeviceProcAddr( device, "vkDestroySemaphore" );
	if ( !vkDestroySemaphore ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkDestroySemaphore" );

	vkDestroyShaderModule = (PFN_vkDestroyShaderModule) vkGetDeviceProcAddr( device, "vkDestroyShaderModule" );
	if ( !vkDestroyShaderModule ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkDestroyShaderModule" );

	vkDeviceWaitIdle = (PFN_vkDeviceWaitIdle) vkGetDeviceProcAddr( device, "vkDeviceWaitIdle" );
	if ( !vkDeviceWaitIdle ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkDeviceWaitIdle" );

	vkEndCommandBuffer = (PFN_vkEndCommandBuffer) vkGetDeviceProcAddr( device, "vkEndCommandBuffer" );
	if ( !vkEndCommandBuffer ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkEndCommandBuffer" );

	vkFlushMappedMemoryRanges = (PFN_vkFlushMappedMemoryRanges) vkGetDeviceProcAddr( device, "vkFlushMappedMemoryRanges" );
	if ( !vkFlushMappedMemoryRanges ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkFlushMappedMemoryRanges" );

	vkFreeCommandBuffers = (PFN_vkFreeCommandBuffers) vkGetDeviceProcAddr( device, "vkFreeCommandBuffers" );
	if ( !vkFreeCommandBuffers ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkFreeCommandBuffers" );

	vkFreeDescriptorSets = (PFN_vkFreeDescriptorSets) vkGetDeviceProcAddr( device, "vkFreeDescriptorSets" );
	if ( !vkFreeDescriptorSets ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkFreeDescriptorSets" );

	vkFreeMemory = (PFN_vkFreeMemory) vkGetDeviceProcAddr( device, "vkFreeMemory" );
	if ( !vkFreeMemory ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkFreeMemory" );

	vkGetBufferMemoryRequirements = (PFN_vkGetBufferMemoryRequirements) vkGetDeviceProcAddr( device, "vkGetBufferMemoryRequirements" );
	if ( !vkGetBufferMemoryRequirements ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkGetBufferMemoryRequirements" );

	vkGetDeviceMemoryCommitment = (PFN_vkGetDeviceMemoryCommitment) vkGetDeviceProcAddr( device, "vkGetDeviceMemoryCommitment" );
	if ( !vkGetDeviceMemoryCommitment ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkGetDeviceMemoryCommitment" );

	vkGetDeviceQueue = (PFN_vkGetDeviceQueue) vkGetDeviceProcAddr( device, "vkGetDeviceQueue" );
	if ( !vkGetDeviceQueue ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkGetDeviceQueue" );

	vkGetEventStatus = (PFN_vkGetEventStatus) vkGetDeviceProcAddr( device, "vkGetEventStatus" );
	if ( !vkGetEventStatus ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkGetEventStatus" );

	vkGetFenceStatus = (PFN_vkGetFenceStatus) vkGetDeviceProcAddr( device, "vkGetFenceStatus" );
	if ( !vkGetFenceStatus ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkGetFenceStatus" );

	vkGetImageMemoryRequirements = (PFN_vkGetImageMemoryRequirements) vkGetDeviceProcAddr( device, "vkGetImageMemoryRequirements" );
	if ( !vkGetImageMemoryRequirements ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkGetImageMemoryRequirements" );

	vkGetImageSparseMemoryRequirements = (PFN_vkGetImageSparseMemoryRequirements) vkGetDeviceProcAddr( device, "vkGetImageSparseMemoryRequirements" );
	if ( !vkGetImageSparseMemoryRequirements ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkGetImageSparseMemoryRequirements" );

	vkGetImageSubresourceLayout = (PFN_vkGetImageSubresourceLayout) vkGetDeviceProcAddr( device, "vkGetImageSubresourceLayout" );
	if ( !vkGetImageSubresourceLayout ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkGetImageSubresourceLayout" );

	vkGetPipelineCacheData = (PFN_vkGetPipelineCacheData) vkGetDeviceProcAddr( device, "vkGetPipelineCacheData" );
	if ( !vkGetPipelineCacheData ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkGetPipelineCacheData" );

	vkGetQueryPoolResults = (PFN_vkGetQueryPoolResults) vkGetDeviceProcAddr( device, "vkGetQueryPoolResults" );
	if ( !vkGetQueryPoolResults ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkGetQueryPoolResults" );

	vkGetRenderAreaGranularity = (PFN_vkGetRenderAreaGranularity) vkGetDeviceProcAddr( device, "vkGetRenderAreaGranularity" );
	if ( !vkGetRenderAreaGranularity ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkGetRenderAreaGranularity" );

	vkInvalidateMappedMemoryRanges = (PFN_vkInvalidateMappedMemoryRanges) vkGetDeviceProcAddr( device, "vkInvalidateMappedMemoryRanges" );
	if ( !vkInvalidateMappedMemoryRanges ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkInvalidateMappedMemoryRanges" );

	vkMapMemory = (PFN_vkMapMemory) vkGetDeviceProcAddr( device, "vkMapMemory" );
	if ( !vkMapMemory ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkMapMemory" );

	vkMergePipelineCaches = (PFN_vkMergePipelineCaches) vkGetDeviceProcAddr( device, "vkMergePipelineCaches" );
	if ( !vkMergePipelineCaches ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkMergePipelineCaches" );

	vkQueueBindSparse = (PFN_vkQueueBindSparse) vkGetDeviceProcAddr( device, "vkQueueBindSparse" );
	if ( !vkQueueBindSparse ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkQueueBindSparse" );

	vkQueueSubmit = (PFN_vkQueueSubmit) vkGetDeviceProcAddr( device, "vkQueueSubmit" );
	if ( !vkQueueSubmit ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkQueueSubmit" );

	vkQueueWaitIdle = (PFN_vkQueueWaitIdle) vkGetDeviceProcAddr( device, "vkQueueWaitIdle" );
	if ( !vkQueueWaitIdle ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkQueueWaitIdle" );

	vkResetCommandBuffer = (PFN_vkResetCommandBuffer) vkGetDeviceProcAddr( device, "vkResetCommandBuffer" );
	if ( !vkResetCommandBuffer ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkResetCommandBuffer" );

	vkResetCommandPool = (PFN_vkResetCommandPool) vkGetDeviceProcAddr( device, "vkResetCommandPool" );
	if ( !vkResetCommandPool ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkResetCommandPool" );

	vkResetDescriptorPool = (PFN_vkResetDescriptorPool) vkGetDeviceProcAddr( device, "vkResetDescriptorPool" );
	if ( !vkResetDescriptorPool ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkResetDescriptorPool" );

	vkResetEvent = (PFN_vkResetEvent) vkGetDeviceProcAddr( device, "vkResetEvent" );
	if ( !vkResetEvent ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkResetEvent" );

	vkResetFences = (PFN_vkResetFences) vkGetDeviceProcAddr( device, "vkResetFences" );
	if ( !vkResetFences ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkResetFences" );

	vkSetEvent = (PFN_vkSetEvent) vkGetDeviceProcAddr( device, "vkSetEvent" );
	if ( !vkSetEvent ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkSetEvent" );

	vkUnmapMemory = (PFN_vkUnmapMemory) vkGetDeviceProcAddr( device, "vkUnmapMemory" );
	if ( !vkUnmapMemory ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkUnmapMemory" );

	vkUpdateDescriptorSets = (PFN_vkUpdateDescriptorSets) vkGetDeviceProcAddr( device, "vkUpdateDescriptorSets" );
	if ( !vkUpdateDescriptorSets ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkUpdateDescriptorSets" );

	vkWaitForFences = (PFN_vkWaitForFences) vkGetDeviceProcAddr( device, "vkWaitForFences" );
	if ( !vkWaitForFences ) ReportAndReturnErrorID( APP_ERROR_VK_FUNC_NOT_FOUND, "vkWaitForFences" );

#if defined(VK_VERSION_1_1)
	vkBindBufferMemory2 = (PFN_vkBindBufferMemory2) vkGetDeviceProcAddr( device, "vkBindBufferMemory2");
	vkBindImageMemory2 = (PFN_vkBindImageMemory2) vkGetDeviceProcAddr( device, "vkBindImageMemory2");
	vkCmdDispatchBase = (PFN_vkCmdDispatchBase) vkGetDeviceProcAddr( device, "vkCmdDispatchBase");
	vkCmdSetDeviceMask = (PFN_vkCmdSetDeviceMask) vkGetDeviceProcAddr( device, "vkCmdSetDeviceMask");
	vkCreateDescriptorUpdateTemplate = (PFN_vkCreateDescriptorUpdateTemplate) vkGetDeviceProcAddr( device, "vkCreateDescriptorUpdateTemplate");
	vkCreateSamplerYcbcrConversion = (PFN_vkCreateSamplerYcbcrConversion) vkGetDeviceProcAddr( device, "vkCreateSamplerYcbcrConversion");
	vkDestroyDescriptorUpdateTemplate = (PFN_vkDestroyDescriptorUpdateTemplate) vkGetDeviceProcAddr( device, "vkDestroyDescriptorUpdateTemplate");
	vkDestroySamplerYcbcrConversion = (PFN_vkDestroySamplerYcbcrConversion) vkGetDeviceProcAddr( device, "vkDestroySamplerYcbcrConversion");
	vkGetBufferMemoryRequirements2 = (PFN_vkGetBufferMemoryRequirements2) vkGetDeviceProcAddr( device, "vkGetBufferMemoryRequirements2");
	vkGetDescriptorSetLayoutSupport = (PFN_vkGetDescriptorSetLayoutSupport) vkGetDeviceProcAddr( device, "vkGetDescriptorSetLayoutSupport");
	vkGetDeviceGroupPeerMemoryFeatures = (PFN_vkGetDeviceGroupPeerMemoryFeatures) vkGetDeviceProcAddr( device, "vkGetDeviceGroupPeerMemoryFeatures");
	vkGetDeviceQueue2 = (PFN_vkGetDeviceQueue2) vkGetDeviceProcAddr( device, "vkGetDeviceQueue2");
	vkGetImageMemoryRequirements2 = (PFN_vkGetImageMemoryRequirements2) vkGetDeviceProcAddr( device, "vkGetImageMemoryRequirements2");
	vkGetImageSparseMemoryRequirements2 = (PFN_vkGetImageSparseMemoryRequirements2) vkGetDeviceProcAddr( device, "vkGetImageSparseMemoryRequirements2");
	vkTrimCommandPool = (PFN_vkTrimCommandPool) vkGetDeviceProcAddr( device, "vkTrimCommandPool");
	vkUpdateDescriptorSetWithTemplate = (PFN_vkUpdateDescriptorSetWithTemplate) vkGetDeviceProcAddr( device, "vkUpdateDescriptorSetWithTemplate");
#endif 

#if defined(VK_AMD_buffer_marker)
	vkCmdWriteBufferMarkerAMD = (PFN_vkCmdWriteBufferMarkerAMD) vkGetDeviceProcAddr( device, "vkCmdWriteBufferMarkerAMD");
#endif 
#if defined(VK_AMD_draw_indirect_count)
	vkCmdDrawIndexedIndirectCountAMD = (PFN_vkCmdDrawIndexedIndirectCountAMD) vkGetDeviceProcAddr( device, "vkCmdDrawIndexedIndirectCountAMD");
	vkCmdDrawIndirectCountAMD = (PFN_vkCmdDrawIndirectCountAMD) vkGetDeviceProcAddr( device, "vkCmdDrawIndirectCountAMD");
#endif 
#if defined(VK_AMD_shader_info)
	vkGetShaderInfoAMD = (PFN_vkGetShaderInfoAMD) vkGetDeviceProcAddr( device, "vkGetShaderInfoAMD");
#endif 
#if defined(VK_ANDROID_external_memory_android_hardware_buffer)
	vkGetAndroidHardwareBufferPropertiesANDROID = (PFN_vkGetAndroidHardwareBufferPropertiesANDROID) vkGetDeviceProcAddr( device, "vkGetAndroidHardwareBufferPropertiesANDROID");
	vkGetMemoryAndroidHardwareBufferANDROID = (PFN_vkGetMemoryAndroidHardwareBufferANDROID) vkGetDeviceProcAddr( device, "vkGetMemoryAndroidHardwareBufferANDROID");
#endif 
#if defined(VK_EXT_buffer_device_address)
	vkGetBufferDeviceAddressEXT = (PFN_vkGetBufferDeviceAddressEXT) vkGetDeviceProcAddr( device, "vkGetBufferDeviceAddressEXT");
#endif 
#if defined(VK_EXT_calibrated_timestamps)
	vkGetCalibratedTimestampsEXT = (PFN_vkGetCalibratedTimestampsEXT) vkGetDeviceProcAddr( device, "vkGetCalibratedTimestampsEXT");
#endif 
#if defined(VK_EXT_conditional_rendering)
	vkCmdBeginConditionalRenderingEXT = (PFN_vkCmdBeginConditionalRenderingEXT) vkGetDeviceProcAddr( device, "vkCmdBeginConditionalRenderingEXT");
	vkCmdEndConditionalRenderingEXT = (PFN_vkCmdEndConditionalRenderingEXT) vkGetDeviceProcAddr( device, "vkCmdEndConditionalRenderingEXT");
#endif 
#if defined(VK_EXT_debug_marker)
	vkCmdDebugMarkerBeginEXT = (PFN_vkCmdDebugMarkerBeginEXT) vkGetDeviceProcAddr( device, "vkCmdDebugMarkerBeginEXT");
	vkCmdDebugMarkerEndEXT = (PFN_vkCmdDebugMarkerEndEXT) vkGetDeviceProcAddr( device, "vkCmdDebugMarkerEndEXT");
	vkCmdDebugMarkerInsertEXT = (PFN_vkCmdDebugMarkerInsertEXT) vkGetDeviceProcAddr( device, "vkCmdDebugMarkerInsertEXT");
	vkDebugMarkerSetObjectNameEXT = (PFN_vkDebugMarkerSetObjectNameEXT) vkGetDeviceProcAddr( device, "vkDebugMarkerSetObjectNameEXT");
	vkDebugMarkerSetObjectTagEXT = (PFN_vkDebugMarkerSetObjectTagEXT) vkGetDeviceProcAddr( device, "vkDebugMarkerSetObjectTagEXT");
#endif 
#if defined(VK_EXT_debug_utils)
	vkCmdBeginDebugUtilsLabelEXT = (PFN_vkCmdBeginDebugUtilsLabelEXT) vkGetDeviceProcAddr( device, "vkCmdBeginDebugUtilsLabelEXT");
	vkCmdEndDebugUtilsLabelEXT = (PFN_vkCmdEndDebugUtilsLabelEXT) vkGetDeviceProcAddr( device, "vkCmdEndDebugUtilsLabelEXT");
	vkCmdInsertDebugUtilsLabelEXT = (PFN_vkCmdInsertDebugUtilsLabelEXT) vkGetDeviceProcAddr( device, "vkCmdInsertDebugUtilsLabelEXT");
	vkQueueBeginDebugUtilsLabelEXT = (PFN_vkQueueBeginDebugUtilsLabelEXT) vkGetDeviceProcAddr( device, "vkQueueBeginDebugUtilsLabelEXT");
	vkQueueEndDebugUtilsLabelEXT = (PFN_vkQueueEndDebugUtilsLabelEXT) vkGetDeviceProcAddr( device, "vkQueueEndDebugUtilsLabelEXT");
	vkQueueInsertDebugUtilsLabelEXT = (PFN_vkQueueInsertDebugUtilsLabelEXT) vkGetDeviceProcAddr( device, "vkQueueInsertDebugUtilsLabelEXT");
	vkSetDebugUtilsObjectNameEXT = (PFN_vkSetDebugUtilsObjectNameEXT) vkGetDeviceProcAddr( device, "vkSetDebugUtilsObjectNameEXT");
	vkSetDebugUtilsObjectTagEXT = (PFN_vkSetDebugUtilsObjectTagEXT) vkGetDeviceProcAddr( device, "vkSetDebugUtilsObjectTagEXT");
#endif 
#if defined(VK_EXT_discard_rectangles)
	vkCmdSetDiscardRectangleEXT = (PFN_vkCmdSetDiscardRectangleEXT) vkGetDeviceProcAddr( device, "vkCmdSetDiscardRectangleEXT");
#endif 
#if defined(VK_EXT_display_control)
	vkDisplayPowerControlEXT = (PFN_vkDisplayPowerControlEXT) vkGetDeviceProcAddr( device, "vkDisplayPowerControlEXT");
	vkGetSwapchainCounterEXT = (PFN_vkGetSwapchainCounterEXT) vkGetDeviceProcAddr( device, "vkGetSwapchainCounterEXT");
	vkRegisterDeviceEventEXT = (PFN_vkRegisterDeviceEventEXT) vkGetDeviceProcAddr( device, "vkRegisterDeviceEventEXT");
	vkRegisterDisplayEventEXT = (PFN_vkRegisterDisplayEventEXT) vkGetDeviceProcAddr( device, "vkRegisterDisplayEventEXT");
#endif 
#if defined(VK_EXT_external_memory_host)
	vkGetMemoryHostPointerPropertiesEXT = (PFN_vkGetMemoryHostPointerPropertiesEXT) vkGetDeviceProcAddr( device, "vkGetMemoryHostPointerPropertiesEXT");
#endif 
#if defined(VK_EXT_hdr_metadata)
	vkSetHdrMetadataEXT = (PFN_vkSetHdrMetadataEXT) vkGetDeviceProcAddr( device, "vkSetHdrMetadataEXT");
#endif 
#if defined(VK_EXT_image_drm_format_modifier)
	vkGetImageDrmFormatModifierPropertiesEXT = (PFN_vkGetImageDrmFormatModifierPropertiesEXT) vkGetDeviceProcAddr( device, "vkGetImageDrmFormatModifierPropertiesEXT");
#endif 
#if defined(VK_EXT_sample_locations)
	vkCmdSetSampleLocationsEXT = (PFN_vkCmdSetSampleLocationsEXT) vkGetDeviceProcAddr( device, "vkCmdSetSampleLocationsEXT");
#endif 
#if defined(VK_EXT_transform_feedback)
	vkCmdBeginQueryIndexedEXT = (PFN_vkCmdBeginQueryIndexedEXT) vkGetDeviceProcAddr( device, "vkCmdBeginQueryIndexedEXT");
	vkCmdBeginTransformFeedbackEXT = (PFN_vkCmdBeginTransformFeedbackEXT) vkGetDeviceProcAddr( device, "vkCmdBeginTransformFeedbackEXT");
	vkCmdBindTransformFeedbackBuffersEXT = (PFN_vkCmdBindTransformFeedbackBuffersEXT) vkGetDeviceProcAddr( device, "vkCmdBindTransformFeedbackBuffersEXT");
	vkCmdDrawIndirectByteCountEXT = (PFN_vkCmdDrawIndirectByteCountEXT) vkGetDeviceProcAddr( device, "vkCmdDrawIndirectByteCountEXT");
	vkCmdEndQueryIndexedEXT = (PFN_vkCmdEndQueryIndexedEXT) vkGetDeviceProcAddr( device, "vkCmdEndQueryIndexedEXT");
	vkCmdEndTransformFeedbackEXT = (PFN_vkCmdEndTransformFeedbackEXT) vkGetDeviceProcAddr( device, "vkCmdEndTransformFeedbackEXT");
#endif 
#if defined(VK_EXT_validation_cache)
	vkCreateValidationCacheEXT = (PFN_vkCreateValidationCacheEXT) vkGetDeviceProcAddr( device, "vkCreateValidationCacheEXT");
	vkDestroyValidationCacheEXT = (PFN_vkDestroyValidationCacheEXT) vkGetDeviceProcAddr( device, "vkDestroyValidationCacheEXT");
	vkGetValidationCacheDataEXT = (PFN_vkGetValidationCacheDataEXT) vkGetDeviceProcAddr( device, "vkGetValidationCacheDataEXT");
	vkMergeValidationCachesEXT = (PFN_vkMergeValidationCachesEXT) vkGetDeviceProcAddr( device, "vkMergeValidationCachesEXT");
#endif 
#if defined(VK_GOOGLE_display_timing)
	vkGetPastPresentationTimingGOOGLE = (PFN_vkGetPastPresentationTimingGOOGLE) vkGetDeviceProcAddr( device, "vkGetPastPresentationTimingGOOGLE");
	vkGetRefreshCycleDurationGOOGLE = (PFN_vkGetRefreshCycleDurationGOOGLE) vkGetDeviceProcAddr( device, "vkGetRefreshCycleDurationGOOGLE");
#endif 
#if defined(VK_KHR_bind_memory2)
	vkBindBufferMemory2KHR = (PFN_vkBindBufferMemory2KHR) vkGetDeviceProcAddr( device, "vkBindBufferMemory2KHR");
	vkBindImageMemory2KHR = (PFN_vkBindImageMemory2KHR) vkGetDeviceProcAddr( device, "vkBindImageMemory2KHR");
#endif 
#if defined(VK_KHR_create_renderpass2)
	vkCmdBeginRenderPass2KHR = (PFN_vkCmdBeginRenderPass2KHR) vkGetDeviceProcAddr( device, "vkCmdBeginRenderPass2KHR");
	vkCmdEndRenderPass2KHR = (PFN_vkCmdEndRenderPass2KHR) vkGetDeviceProcAddr( device, "vkCmdEndRenderPass2KHR");
	vkCmdNextSubpass2KHR = (PFN_vkCmdNextSubpass2KHR) vkGetDeviceProcAddr( device, "vkCmdNextSubpass2KHR");
	vkCreateRenderPass2KHR = (PFN_vkCreateRenderPass2KHR) vkGetDeviceProcAddr( device, "vkCreateRenderPass2KHR");
#endif 
#if defined(VK_KHR_descriptor_update_template)
	vkCreateDescriptorUpdateTemplateKHR = (PFN_vkCreateDescriptorUpdateTemplateKHR) vkGetDeviceProcAddr( device, "vkCreateDescriptorUpdateTemplateKHR");
	vkDestroyDescriptorUpdateTemplateKHR = (PFN_vkDestroyDescriptorUpdateTemplateKHR) vkGetDeviceProcAddr( device, "vkDestroyDescriptorUpdateTemplateKHR");
	vkUpdateDescriptorSetWithTemplateKHR = (PFN_vkUpdateDescriptorSetWithTemplateKHR) vkGetDeviceProcAddr( device, "vkUpdateDescriptorSetWithTemplateKHR");
#endif 
#if defined(VK_KHR_device_group)
	vkCmdDispatchBaseKHR = (PFN_vkCmdDispatchBaseKHR) vkGetDeviceProcAddr( device, "vkCmdDispatchBaseKHR");
	vkCmdSetDeviceMaskKHR = (PFN_vkCmdSetDeviceMaskKHR) vkGetDeviceProcAddr( device, "vkCmdSetDeviceMaskKHR");
	vkGetDeviceGroupPeerMemoryFeaturesKHR = (PFN_vkGetDeviceGroupPeerMemoryFeaturesKHR) vkGetDeviceProcAddr( device, "vkGetDeviceGroupPeerMemoryFeaturesKHR");
#endif 
#if defined(VK_KHR_display_swapchain)
	vkCreateSharedSwapchainsKHR = (PFN_vkCreateSharedSwapchainsKHR) vkGetDeviceProcAddr( device, "vkCreateSharedSwapchainsKHR");
#endif 
#if defined(VK_KHR_draw_indirect_count)
	vkCmdDrawIndexedIndirectCountKHR = (PFN_vkCmdDrawIndexedIndirectCountKHR) vkGetDeviceProcAddr( device, "vkCmdDrawIndexedIndirectCountKHR");
	vkCmdDrawIndirectCountKHR = (PFN_vkCmdDrawIndirectCountKHR) vkGetDeviceProcAddr( device, "vkCmdDrawIndirectCountKHR");
#endif 
#if defined(VK_KHR_external_fence_fd)
	vkGetFenceFdKHR = (PFN_vkGetFenceFdKHR) vkGetDeviceProcAddr( device, "vkGetFenceFdKHR");
	vkImportFenceFdKHR = (PFN_vkImportFenceFdKHR) vkGetDeviceProcAddr( device, "vkImportFenceFdKHR");
#endif 
#if defined(VK_KHR_external_fence_win32)
	vkGetFenceWin32HandleKHR = (PFN_vkGetFenceWin32HandleKHR) vkGetDeviceProcAddr( device, "vkGetFenceWin32HandleKHR");
	vkImportFenceWin32HandleKHR = (PFN_vkImportFenceWin32HandleKHR) vkGetDeviceProcAddr( device, "vkImportFenceWin32HandleKHR");
#endif 
#if defined(VK_KHR_external_memory_fd)
	vkGetMemoryFdKHR = (PFN_vkGetMemoryFdKHR) vkGetDeviceProcAddr( device, "vkGetMemoryFdKHR");
	vkGetMemoryFdPropertiesKHR = (PFN_vkGetMemoryFdPropertiesKHR) vkGetDeviceProcAddr( device, "vkGetMemoryFdPropertiesKHR");
#endif 
#if defined(VK_KHR_external_memory_win32)
	vkGetMemoryWin32HandleKHR = (PFN_vkGetMemoryWin32HandleKHR) vkGetDeviceProcAddr( device, "vkGetMemoryWin32HandleKHR");
	vkGetMemoryWin32HandlePropertiesKHR = (PFN_vkGetMemoryWin32HandlePropertiesKHR) vkGetDeviceProcAddr( device, "vkGetMemoryWin32HandlePropertiesKHR");
#endif 
#if defined(VK_KHR_external_semaphore_fd)
	vkGetSemaphoreFdKHR = (PFN_vkGetSemaphoreFdKHR) vkGetDeviceProcAddr( device, "vkGetSemaphoreFdKHR");
	vkImportSemaphoreFdKHR = (PFN_vkImportSemaphoreFdKHR) vkGetDeviceProcAddr( device, "vkImportSemaphoreFdKHR");
#endif 
#if defined(VK_KHR_external_semaphore_win32)
	vkGetSemaphoreWin32HandleKHR = (PFN_vkGetSemaphoreWin32HandleKHR) vkGetDeviceProcAddr( device, "vkGetSemaphoreWin32HandleKHR");
	vkImportSemaphoreWin32HandleKHR = (PFN_vkImportSemaphoreWin32HandleKHR) vkGetDeviceProcAddr( device, "vkImportSemaphoreWin32HandleKHR");
#endif 
#if defined(VK_KHR_get_memory_requirements2)
	vkGetBufferMemoryRequirements2KHR = (PFN_vkGetBufferMemoryRequirements2KHR) vkGetDeviceProcAddr( device, "vkGetBufferMemoryRequirements2KHR");
	vkGetImageMemoryRequirements2KHR = (PFN_vkGetImageMemoryRequirements2KHR) vkGetDeviceProcAddr( device, "vkGetImageMemoryRequirements2KHR");
	vkGetImageSparseMemoryRequirements2KHR = (PFN_vkGetImageSparseMemoryRequirements2KHR) vkGetDeviceProcAddr( device, "vkGetImageSparseMemoryRequirements2KHR");
#endif 
#if defined(VK_KHR_maintenance1)
	vkTrimCommandPoolKHR = (PFN_vkTrimCommandPoolKHR) vkGetDeviceProcAddr( device, "vkTrimCommandPoolKHR");
#endif 
#if defined(VK_KHR_maintenance3)
	vkGetDescriptorSetLayoutSupportKHR = (PFN_vkGetDescriptorSetLayoutSupportKHR) vkGetDeviceProcAddr( device, "vkGetDescriptorSetLayoutSupportKHR");
#endif 
#if defined(VK_KHR_push_descriptor)
	vkCmdPushDescriptorSetKHR = (PFN_vkCmdPushDescriptorSetKHR) vkGetDeviceProcAddr( device, "vkCmdPushDescriptorSetKHR");
#endif 
#if defined(VK_KHR_sampler_ycbcr_conversion)
	vkCreateSamplerYcbcrConversionKHR = (PFN_vkCreateSamplerYcbcrConversionKHR) vkGetDeviceProcAddr( device, "vkCreateSamplerYcbcrConversionKHR");
	vkDestroySamplerYcbcrConversionKHR = (PFN_vkDestroySamplerYcbcrConversionKHR) vkGetDeviceProcAddr( device, "vkDestroySamplerYcbcrConversionKHR");
#endif 
#if defined(VK_KHR_shared_presentable_image)
	vkGetSwapchainStatusKHR = (PFN_vkGetSwapchainStatusKHR) vkGetDeviceProcAddr( device, "vkGetSwapchainStatusKHR");
#endif 
#if defined(VK_KHR_swapchain)
	vkAcquireNextImageKHR = (PFN_vkAcquireNextImageKHR) vkGetDeviceProcAddr( device, "vkAcquireNextImageKHR");
	vkCreateSwapchainKHR = (PFN_vkCreateSwapchainKHR) vkGetDeviceProcAddr( device, "vkCreateSwapchainKHR");
	vkDestroySwapchainKHR = (PFN_vkDestroySwapchainKHR) vkGetDeviceProcAddr( device, "vkDestroySwapchainKHR");
	vkGetSwapchainImagesKHR = (PFN_vkGetSwapchainImagesKHR) vkGetDeviceProcAddr( device, "vkGetSwapchainImagesKHR");
	vkQueuePresentKHR = (PFN_vkQueuePresentKHR) vkGetDeviceProcAddr( device, "vkQueuePresentKHR");
#endif 
#if defined(VK_NVX_device_generated_commands)
	vkCmdProcessCommandsNVX = (PFN_vkCmdProcessCommandsNVX) vkGetDeviceProcAddr( device, "vkCmdProcessCommandsNVX");
	vkCmdReserveSpaceForCommandsNVX = (PFN_vkCmdReserveSpaceForCommandsNVX) vkGetDeviceProcAddr( device, "vkCmdReserveSpaceForCommandsNVX");
	vkCreateIndirectCommandsLayoutNVX = (PFN_vkCreateIndirectCommandsLayoutNVX) vkGetDeviceProcAddr( device, "vkCreateIndirectCommandsLayoutNVX");
	vkCreateObjectTableNVX = (PFN_vkCreateObjectTableNVX) vkGetDeviceProcAddr( device, "vkCreateObjectTableNVX");
	vkDestroyIndirectCommandsLayoutNVX = (PFN_vkDestroyIndirectCommandsLayoutNVX) vkGetDeviceProcAddr( device, "vkDestroyIndirectCommandsLayoutNVX");
	vkDestroyObjectTableNVX = (PFN_vkDestroyObjectTableNVX) vkGetDeviceProcAddr( device, "vkDestroyObjectTableNVX");
	vkRegisterObjectsNVX = (PFN_vkRegisterObjectsNVX) vkGetDeviceProcAddr( device, "vkRegisterObjectsNVX");
	vkUnregisterObjectsNVX = (PFN_vkUnregisterObjectsNVX) vkGetDeviceProcAddr( device, "vkUnregisterObjectsNVX");
#endif 
#if defined(VK_NV_clip_space_w_scaling)
	vkCmdSetViewportWScalingNV = (PFN_vkCmdSetViewportWScalingNV) vkGetDeviceProcAddr( device, "vkCmdSetViewportWScalingNV");
#endif 
#if defined(VK_NV_device_diagnostic_checkpoints)
	vkCmdSetCheckpointNV = (PFN_vkCmdSetCheckpointNV) vkGetDeviceProcAddr( device, "vkCmdSetCheckpointNV");
	vkGetQueueCheckpointDataNV = (PFN_vkGetQueueCheckpointDataNV) vkGetDeviceProcAddr( device, "vkGetQueueCheckpointDataNV");
#endif 
#if defined(VK_NV_external_memory_win32)
	vkGetMemoryWin32HandleNV = (PFN_vkGetMemoryWin32HandleNV) vkGetDeviceProcAddr( device, "vkGetMemoryWin32HandleNV");
#endif 
#if defined(VK_NV_mesh_shader)
	vkCmdDrawMeshTasksIndirectCountNV = (PFN_vkCmdDrawMeshTasksIndirectCountNV) vkGetDeviceProcAddr( device, "vkCmdDrawMeshTasksIndirectCountNV");
	vkCmdDrawMeshTasksIndirectNV = (PFN_vkCmdDrawMeshTasksIndirectNV) vkGetDeviceProcAddr( device, "vkCmdDrawMeshTasksIndirectNV");
	vkCmdDrawMeshTasksNV = (PFN_vkCmdDrawMeshTasksNV) vkGetDeviceProcAddr( device, "vkCmdDrawMeshTasksNV");
#endif 
#if defined(VK_NV_ray_tracing)
	vkBindAccelerationStructureMemoryNV = (PFN_vkBindAccelerationStructureMemoryNV) vkGetDeviceProcAddr( device, "vkBindAccelerationStructureMemoryNV");
	vkCmdBuildAccelerationStructureNV = (PFN_vkCmdBuildAccelerationStructureNV) vkGetDeviceProcAddr( device, "vkCmdBuildAccelerationStructureNV");
	vkCmdCopyAccelerationStructureNV = (PFN_vkCmdCopyAccelerationStructureNV) vkGetDeviceProcAddr( device, "vkCmdCopyAccelerationStructureNV");
	vkCmdTraceRaysNV = (PFN_vkCmdTraceRaysNV) vkGetDeviceProcAddr( device, "vkCmdTraceRaysNV");
	vkCmdWriteAccelerationStructuresPropertiesNV = (PFN_vkCmdWriteAccelerationStructuresPropertiesNV) vkGetDeviceProcAddr( device, "vkCmdWriteAccelerationStructuresPropertiesNV");
	vkCompileDeferredNV = (PFN_vkCompileDeferredNV) vkGetDeviceProcAddr( device, "vkCompileDeferredNV");
	vkCreateAccelerationStructureNV = (PFN_vkCreateAccelerationStructureNV) vkGetDeviceProcAddr( device, "vkCreateAccelerationStructureNV");
	vkCreateRayTracingPipelinesNV = (PFN_vkCreateRayTracingPipelinesNV) vkGetDeviceProcAddr( device, "vkCreateRayTracingPipelinesNV");
	vkDestroyAccelerationStructureNV = (PFN_vkDestroyAccelerationStructureNV) vkGetDeviceProcAddr( device, "vkDestroyAccelerationStructureNV");
	vkGetAccelerationStructureHandleNV = (PFN_vkGetAccelerationStructureHandleNV) vkGetDeviceProcAddr( device, "vkGetAccelerationStructureHandleNV");
	vkGetAccelerationStructureMemoryRequirementsNV = (PFN_vkGetAccelerationStructureMemoryRequirementsNV) vkGetDeviceProcAddr( device, "vkGetAccelerationStructureMemoryRequirementsNV");
	vkGetRayTracingShaderGroupHandlesNV = (PFN_vkGetRayTracingShaderGroupHandlesNV) vkGetDeviceProcAddr( device, "vkGetRayTracingShaderGroupHandlesNV");
#endif 
#if defined(VK_NV_scissor_exclusive)
	vkCmdSetExclusiveScissorNV = (PFN_vkCmdSetExclusiveScissorNV) vkGetDeviceProcAddr( device, "vkCmdSetExclusiveScissorNV");
#endif 
#if defined(VK_NV_shading_rate_image)
	vkCmdBindShadingRateImageNV = (PFN_vkCmdBindShadingRateImageNV) vkGetDeviceProcAddr( device, "vkCmdBindShadingRateImageNV");
	vkCmdSetCoarseSampleOrderNV = (PFN_vkCmdSetCoarseSampleOrderNV) vkGetDeviceProcAddr( device, "vkCmdSetCoarseSampleOrderNV");
	vkCmdSetViewportShadingRatePaletteNV = (PFN_vkCmdSetViewportShadingRatePaletteNV) vkGetDeviceProcAddr( device, "vkCmdSetViewportShadingRatePaletteNV");
#endif 
#if (defined(VK_KHR_descriptor_update_template) && defined(VK_KHR_push_descriptor)) || (defined(VK_KHR_push_descriptor) && defined(VK_VERSION_1_1))
	vkCmdPushDescriptorSetWithTemplateKHR = (PFN_vkCmdPushDescriptorSetWithTemplateKHR) vkGetDeviceProcAddr( device, "vkCmdPushDescriptorSetWithTemplateKHR");
#endif 
#if (defined(VK_KHR_device_group) && defined(VK_KHR_surface)) || (defined(VK_KHR_swapchain) && defined(VK_VERSION_1_1))
	vkGetDeviceGroupPresentCapabilitiesKHR = (PFN_vkGetDeviceGroupPresentCapabilitiesKHR) vkGetDeviceProcAddr( device, "vkGetDeviceGroupPresentCapabilitiesKHR");
	vkGetDeviceGroupSurfacePresentModesKHR = (PFN_vkGetDeviceGroupSurfacePresentModesKHR) vkGetDeviceProcAddr( device, "vkGetDeviceGroupSurfacePresentModesKHR");
#endif 
#if (defined(VK_KHR_device_group) && defined(VK_KHR_swapchain)) || (defined(VK_KHR_swapchain) && defined(VK_VERSION_1_1))
	vkAcquireNextImage2KHR = (PFN_vkAcquireNextImage2KHR) vkGetDeviceProcAddr( device, "vkAcquireNextImage2KHR");
#endif

#if defined(VK_AMD_display_native_hdr)
	vkSetLocalDimmingAMD = (PFN_vkSetLocalDimmingAMD) vkGetDeviceProcAddr( device, "vkSetLocalDimmingAMD");
#endif
#if defined(VK_EXT_full_screen_exclusive)
	vkAcquireFullScreenExclusiveModeEXT = (PFN_vkAcquireFullScreenExclusiveModeEXT) vkGetDeviceProcAddr( device, "vkAcquireFullScreenExclusiveModeEXT");
	vkGetPhysicalDeviceSurfacePresentModes2EXT = (PFN_vkGetPhysicalDeviceSurfacePresentModes2EXT) vkGetDeviceProcAddr( device, "vkGetPhysicalDeviceSurfacePresentModes2EXT");
	vkReleaseFullScreenExclusiveModeEXT = (PFN_vkReleaseFullScreenExclusiveModeEXT) vkGetDeviceProcAddr( device, "vkReleaseFullScreenExclusiveModeEXT");
#endif
#if defined(VK_EXT_headless_surface)
	vkCreateHeadlessSurfaceEXT = (PFN_vkCreateHeadlessSurfaceEXT) vkGetDeviceProcAddr( device, "vkCreateHeadlessSurfaceEXT");
#endif
#if defined(VK_EXT_host_query_reset)
	vkResetQueryPoolEXT = (PFN_vkResetQueryPoolEXT) vkGetDeviceProcAddr( device, "vkResetQueryPoolEXT");
#endif
#if defined(VK_EXT_metal_surface)
	vkCreateMetalSurfaceEXT = (PFN_vkCreateMetalSurfaceEXT) vkGetDeviceProcAddr( device, "vkCreateMetalSurfaceEXT");
#endif
#if defined(VK_GGP_stream_descriptor_surface)
	vkCreateStreamDescriptorSurfaceGGP = (PFN_vkCreateStreamDescriptorSurfaceGGP) vkGetDeviceProcAddr( device, "vkCreateStreamDescriptorSurfaceGGP");
#endif
#if defined(VK_NVX_image_view_handle)
	vkGetImageViewHandleNVX = (PFN_vkGetImageViewHandleNVX) vkGetDeviceProcAddr( device, "vkGetImageViewHandleNVX");
#endif
#if defined(VK_NV_cooperative_matrix)
	vkGetPhysicalDeviceCooperativeMatrixPropertiesNV = (PFN_vkGetPhysicalDeviceCooperativeMatrixPropertiesNV) vkGetDeviceProcAddr( device, "vkGetPhysicalDeviceCooperativeMatrixPropertiesNV");
#endif
#if (defined(VK_EXT_full_screen_exclusive) && defined(VK_VERSION_1_1)) || (defined(VK_KHR_device_group) && defined(VK_EXT_full_screen_exclusive))
	vkGetDeviceGroupSurfacePresentModes2EXT = (PFN_vkGetDeviceGroupSurfacePresentModes2EXT) vkGetDeviceProcAddr( device, "vkGetDeviceGroupSurfacePresentModes2EXT");
#endif

	return APP_SUCCESS;
}

#else

int VulkanLoadFunctionsInit() { return APP_SUCCESS; }
int VulkanCleanupFunctions() { return APP_SUCCESS; }
int VulkanLoadInstanceFunctions() { return APP_SUCCESS; }
int VulkanLoadDeviceFunctions( VkInstance instance ) { return APP_SUCCESS; }
int VulkanLoadMainFunctions( VkDevice device ) { return APP_SUCCESS; }

#endif // !AGK_IOS && !AGK_MACOS

#endif // DISABLE_VULKAN
