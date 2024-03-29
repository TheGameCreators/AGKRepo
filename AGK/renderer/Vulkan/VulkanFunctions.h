#ifndef _H_VULKAN_FUNCTIONS
#define _H_VULKAN_FUNCTIONS

#if !defined(DISABLE_VULKAN) && (!defined(__APPLE__) || defined(__LP64__))

#include "PlatformDefines.h"

// make sure functions aren't defined, we will load them ourselves
// except on iOS and Mac where we link statically to MoltenVK
#if !defined(AGK_IOS) && !defined(AGK_MACOS)
    #define VK_NO_PROTOTYPES
#endif
#include <vulkan/vulkan.h>

int VulkanLoadFunctionsInit();
int VulkanCleanupFunctions();

int VulkanLoadInstanceFunctions(); // functions for creating an instance, does not require an instance
int VulkanLoadDeviceFunctions( VkInstance instance ); // functions for enumerating and creating a device, does not require a device
int VulkanLoadMainFunctions( VkDevice device ); // load all other functions, can only use a single device but faster than using instance

#ifdef AGK_WINDOWS
	#include <Common.h>
	#include <vulkan/vulkan_win32.h>
#endif

#ifdef AGK_MACOS
	#include <dlfcn.h>
    #include <MoltenVK/mvk_vulkan.h>
    #include <vulkan/vulkan_metal.h>
    #include <MoltenVK/vk_mvk_moltenvk.h>
#endif

#ifdef AGK_LINUX
	#include <dlfcn.h>
	#include "X11/Xlib.h"
	#include <vulkan/vulkan_xlib.h>
#endif

#if defined(AGK_IOS) || defined(AGK_MACOS)
	#include <dlfcn.h>
    #include <MoltenVK/mvk_vulkan.h>
    #include <vulkan/vulkan_metal.h>
    #include <MoltenVK/vk_mvk_moltenvk.h>
    #import <CoreVideo/CVPixelBuffer.h>

    namespace AGK
    {
        class cImage;
    }

    int MetalCreateVideoImageData( VkPhysicalDevice vkDevice );
    int MetalHasVideoImageData();
void MetalCopyVideoImageToAGKImage( CVPixelBufferRef buffer, AGK::cImage* pImage );
    void MetalReleaseVideoImageData();

    int MetalCreateCameraImageData( VkPhysicalDevice vkDevice );
    int MetalHasCameraImageData();
void MetalCopyCameraImageToAGKImage( CVImageBufferRef buffer, AGK::cImage* pImage );
    void MetalReleaseCameraImageData();

    int MetalCreateARImageData( VkPhysicalDevice vkDevice );
    int MetalHasARImageData();
void MetalCopyARImageToAGKImage( CVPixelBufferRef buffer, AGK::cImage* pImageY, AGK::cImage* pImageUV );
    void MetalReleaseARImageData();
#endif

#ifdef AGK_ANDROID
	#include <dlfcn.h>
	#include <vulkan/vulkan_android.h>
#endif

#if !defined(AGK_IOS) && !defined(AGK_MACOS)
extern PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr;
extern PFN_vkGetDeviceProcAddr vkGetDeviceProcAddr;

// setup functions
extern PFN_vkCreateInstance vkCreateInstance;
extern PFN_vkEnumerateInstanceExtensionProperties vkEnumerateInstanceExtensionProperties;
extern PFN_vkEnumerateInstanceLayerProperties vkEnumerateInstanceLayerProperties;

#ifdef VK_VERSION_1_1
	extern PFN_vkEnumerateInstanceVersion vkEnumerateInstanceVersion;
#endif

// main functions
extern PFN_vkAllocateCommandBuffers vkAllocateCommandBuffers;
extern PFN_vkAllocateDescriptorSets vkAllocateDescriptorSets;
extern PFN_vkAllocateMemory vkAllocateMemory;
extern PFN_vkBeginCommandBuffer vkBeginCommandBuffer;
extern PFN_vkBindBufferMemory vkBindBufferMemory;
extern PFN_vkBindImageMemory vkBindImageMemory;
extern PFN_vkCmdBeginQuery vkCmdBeginQuery;
extern PFN_vkCmdBeginRenderPass vkCmdBeginRenderPass;
extern PFN_vkCmdBindDescriptorSets vkCmdBindDescriptorSets;
extern PFN_vkCmdBindIndexBuffer vkCmdBindIndexBuffer;
extern PFN_vkCmdBindPipeline vkCmdBindPipeline;
extern PFN_vkCmdBindVertexBuffers vkCmdBindVertexBuffers;
extern PFN_vkCmdBlitImage vkCmdBlitImage;
extern PFN_vkCmdClearAttachments vkCmdClearAttachments;
extern PFN_vkCmdClearColorImage vkCmdClearColorImage;
extern PFN_vkCmdClearDepthStencilImage vkCmdClearDepthStencilImage;
extern PFN_vkCmdCopyBuffer vkCmdCopyBuffer;
extern PFN_vkCmdCopyBufferToImage vkCmdCopyBufferToImage;
extern PFN_vkCmdCopyImage vkCmdCopyImage;
extern PFN_vkCmdCopyImageToBuffer vkCmdCopyImageToBuffer;
extern PFN_vkCmdCopyQueryPoolResults vkCmdCopyQueryPoolResults;
extern PFN_vkCmdDispatch vkCmdDispatch;
extern PFN_vkCmdDispatchIndirect vkCmdDispatchIndirect;
extern PFN_vkCmdDraw vkCmdDraw;
extern PFN_vkCmdDrawIndexed vkCmdDrawIndexed;
extern PFN_vkCmdDrawIndexedIndirect vkCmdDrawIndexedIndirect;
extern PFN_vkCmdDrawIndirect vkCmdDrawIndirect;
extern PFN_vkCmdEndQuery vkCmdEndQuery;
extern PFN_vkCmdEndRenderPass vkCmdEndRenderPass;
extern PFN_vkCmdExecuteCommands vkCmdExecuteCommands;
extern PFN_vkCmdFillBuffer vkCmdFillBuffer;
extern PFN_vkCmdNextSubpass vkCmdNextSubpass;
extern PFN_vkCmdPipelineBarrier vkCmdPipelineBarrier;
extern PFN_vkCmdPushConstants vkCmdPushConstants;
extern PFN_vkCmdResetEvent vkCmdResetEvent;
extern PFN_vkCmdResetQueryPool vkCmdResetQueryPool;
extern PFN_vkCmdResolveImage vkCmdResolveImage;
extern PFN_vkCmdSetBlendConstants vkCmdSetBlendConstants;
extern PFN_vkCmdSetDepthBias vkCmdSetDepthBias;
extern PFN_vkCmdSetDepthBounds vkCmdSetDepthBounds;
extern PFN_vkCmdSetEvent vkCmdSetEvent;
extern PFN_vkCmdSetLineWidth vkCmdSetLineWidth;
extern PFN_vkCmdSetScissor vkCmdSetScissor;
extern PFN_vkCmdSetStencilCompareMask vkCmdSetStencilCompareMask;
extern PFN_vkCmdSetStencilReference vkCmdSetStencilReference;
extern PFN_vkCmdSetStencilWriteMask vkCmdSetStencilWriteMask;
extern PFN_vkCmdSetViewport vkCmdSetViewport;
extern PFN_vkCmdUpdateBuffer vkCmdUpdateBuffer;
extern PFN_vkCmdWaitEvents vkCmdWaitEvents;
extern PFN_vkCmdWriteTimestamp vkCmdWriteTimestamp;
extern PFN_vkCreateBuffer vkCreateBuffer;
extern PFN_vkCreateBufferView vkCreateBufferView;
extern PFN_vkCreateCommandPool vkCreateCommandPool;
extern PFN_vkCreateComputePipelines vkCreateComputePipelines;
extern PFN_vkCreateDescriptorPool vkCreateDescriptorPool;
extern PFN_vkCreateDescriptorSetLayout vkCreateDescriptorSetLayout;
extern PFN_vkCreateDevice vkCreateDevice;
extern PFN_vkCreateEvent vkCreateEvent;
extern PFN_vkCreateFence vkCreateFence;
extern PFN_vkCreateFramebuffer vkCreateFramebuffer;
extern PFN_vkCreateGraphicsPipelines vkCreateGraphicsPipelines;
extern PFN_vkCreateImage vkCreateImage;
extern PFN_vkCreateImageView vkCreateImageView;
extern PFN_vkCreatePipelineCache vkCreatePipelineCache;
extern PFN_vkCreatePipelineLayout vkCreatePipelineLayout;
extern PFN_vkCreateQueryPool vkCreateQueryPool;
extern PFN_vkCreateRenderPass vkCreateRenderPass;
extern PFN_vkCreateSampler vkCreateSampler;
extern PFN_vkCreateSemaphore vkCreateSemaphore;
extern PFN_vkCreateShaderModule vkCreateShaderModule;
extern PFN_vkDestroyBuffer vkDestroyBuffer;
extern PFN_vkDestroyBufferView vkDestroyBufferView;
extern PFN_vkDestroyCommandPool vkDestroyCommandPool;
extern PFN_vkDestroyDescriptorPool vkDestroyDescriptorPool;
extern PFN_vkDestroyDescriptorSetLayout vkDestroyDescriptorSetLayout;
extern PFN_vkDestroyDevice vkDestroyDevice;
extern PFN_vkDestroyEvent vkDestroyEvent;
extern PFN_vkDestroyFence vkDestroyFence;
extern PFN_vkDestroyFramebuffer vkDestroyFramebuffer;
extern PFN_vkDestroyImage vkDestroyImage;
extern PFN_vkDestroyImageView vkDestroyImageView;
extern PFN_vkDestroyInstance vkDestroyInstance;
extern PFN_vkDestroyPipeline vkDestroyPipeline;
extern PFN_vkDestroyPipelineCache vkDestroyPipelineCache;
extern PFN_vkDestroyPipelineLayout vkDestroyPipelineLayout;
extern PFN_vkDestroyQueryPool vkDestroyQueryPool;
extern PFN_vkDestroyRenderPass vkDestroyRenderPass;
extern PFN_vkDestroySampler vkDestroySampler;
extern PFN_vkDestroySemaphore vkDestroySemaphore;
extern PFN_vkDestroyShaderModule vkDestroyShaderModule;
extern PFN_vkDeviceWaitIdle vkDeviceWaitIdle;
extern PFN_vkEndCommandBuffer vkEndCommandBuffer;
extern PFN_vkEnumerateDeviceExtensionProperties vkEnumerateDeviceExtensionProperties;
extern PFN_vkEnumerateDeviceLayerProperties vkEnumerateDeviceLayerProperties;
extern PFN_vkEnumeratePhysicalDevices vkEnumeratePhysicalDevices;
extern PFN_vkFlushMappedMemoryRanges vkFlushMappedMemoryRanges;
extern PFN_vkFreeCommandBuffers vkFreeCommandBuffers;
extern PFN_vkFreeDescriptorSets vkFreeDescriptorSets;
extern PFN_vkFreeMemory vkFreeMemory;
extern PFN_vkGetBufferMemoryRequirements vkGetBufferMemoryRequirements;
extern PFN_vkGetDeviceMemoryCommitment vkGetDeviceMemoryCommitment;
extern PFN_vkGetDeviceQueue vkGetDeviceQueue;
extern PFN_vkGetEventStatus vkGetEventStatus;
extern PFN_vkGetFenceStatus vkGetFenceStatus;
extern PFN_vkGetImageMemoryRequirements vkGetImageMemoryRequirements;
extern PFN_vkGetImageSparseMemoryRequirements vkGetImageSparseMemoryRequirements;
extern PFN_vkGetImageSubresourceLayout vkGetImageSubresourceLayout;
extern PFN_vkGetPhysicalDeviceFeatures vkGetPhysicalDeviceFeatures;
extern PFN_vkGetPhysicalDeviceFormatProperties vkGetPhysicalDeviceFormatProperties;
extern PFN_vkGetPhysicalDeviceImageFormatProperties vkGetPhysicalDeviceImageFormatProperties;
extern PFN_vkGetPhysicalDeviceMemoryProperties vkGetPhysicalDeviceMemoryProperties;
extern PFN_vkGetPhysicalDeviceProperties vkGetPhysicalDeviceProperties;
extern PFN_vkGetPhysicalDeviceQueueFamilyProperties vkGetPhysicalDeviceQueueFamilyProperties;
extern PFN_vkGetPhysicalDeviceSparseImageFormatProperties vkGetPhysicalDeviceSparseImageFormatProperties;
extern PFN_vkGetPipelineCacheData vkGetPipelineCacheData;
extern PFN_vkGetQueryPoolResults vkGetQueryPoolResults;
extern PFN_vkGetRenderAreaGranularity vkGetRenderAreaGranularity;
extern PFN_vkInvalidateMappedMemoryRanges vkInvalidateMappedMemoryRanges;
extern PFN_vkMapMemory vkMapMemory;
extern PFN_vkMergePipelineCaches vkMergePipelineCaches;
extern PFN_vkQueueBindSparse vkQueueBindSparse;
extern PFN_vkQueueSubmit vkQueueSubmit;
extern PFN_vkQueueWaitIdle vkQueueWaitIdle;
extern PFN_vkResetCommandBuffer vkResetCommandBuffer;
extern PFN_vkResetCommandPool vkResetCommandPool;
extern PFN_vkResetDescriptorPool vkResetDescriptorPool;
extern PFN_vkResetEvent vkResetEvent;
extern PFN_vkResetFences vkResetFences;
extern PFN_vkSetEvent vkSetEvent;
extern PFN_vkUnmapMemory vkUnmapMemory;
extern PFN_vkUpdateDescriptorSets vkUpdateDescriptorSets;
extern PFN_vkWaitForFences vkWaitForFences;

#ifdef VK_VERSION_1_1
	extern PFN_vkBindBufferMemory2 vkBindBufferMemory2;
	extern PFN_vkBindImageMemory2 vkBindImageMemory2;
	extern PFN_vkCmdDispatchBase vkCmdDispatchBase;
	extern PFN_vkCmdSetDeviceMask vkCmdSetDeviceMask;
	extern PFN_vkCreateDescriptorUpdateTemplate vkCreateDescriptorUpdateTemplate;
	extern PFN_vkCreateSamplerYcbcrConversion vkCreateSamplerYcbcrConversion;
	extern PFN_vkDestroyDescriptorUpdateTemplate vkDestroyDescriptorUpdateTemplate;
	extern PFN_vkDestroySamplerYcbcrConversion vkDestroySamplerYcbcrConversion;
	extern PFN_vkEnumeratePhysicalDeviceGroups vkEnumeratePhysicalDeviceGroups;
	extern PFN_vkGetBufferMemoryRequirements2 vkGetBufferMemoryRequirements2;
	extern PFN_vkGetDescriptorSetLayoutSupport vkGetDescriptorSetLayoutSupport;
	extern PFN_vkGetDeviceGroupPeerMemoryFeatures vkGetDeviceGroupPeerMemoryFeatures;
	extern PFN_vkGetDeviceQueue2 vkGetDeviceQueue2;
	extern PFN_vkGetImageMemoryRequirements2 vkGetImageMemoryRequirements2;
	extern PFN_vkGetImageSparseMemoryRequirements2 vkGetImageSparseMemoryRequirements2;
	extern PFN_vkGetPhysicalDeviceExternalBufferProperties vkGetPhysicalDeviceExternalBufferProperties;
	extern PFN_vkGetPhysicalDeviceExternalFenceProperties vkGetPhysicalDeviceExternalFenceProperties;
	extern PFN_vkGetPhysicalDeviceExternalSemaphoreProperties vkGetPhysicalDeviceExternalSemaphoreProperties;
	extern PFN_vkGetPhysicalDeviceFeatures2 vkGetPhysicalDeviceFeatures2;
	extern PFN_vkGetPhysicalDeviceFormatProperties2 vkGetPhysicalDeviceFormatProperties2;
	extern PFN_vkGetPhysicalDeviceImageFormatProperties2 vkGetPhysicalDeviceImageFormatProperties2;
	extern PFN_vkGetPhysicalDeviceMemoryProperties2 vkGetPhysicalDeviceMemoryProperties2;
	extern PFN_vkGetPhysicalDeviceProperties2 vkGetPhysicalDeviceProperties2;
	extern PFN_vkGetPhysicalDeviceQueueFamilyProperties2 vkGetPhysicalDeviceQueueFamilyProperties2;
	extern PFN_vkGetPhysicalDeviceSparseImageFormatProperties2 vkGetPhysicalDeviceSparseImageFormatProperties2;
	extern PFN_vkTrimCommandPool vkTrimCommandPool;
	extern PFN_vkUpdateDescriptorSetWithTemplate vkUpdateDescriptorSetWithTemplate;
#endif 

#if defined(VK_AMD_buffer_marker)
	extern PFN_vkCmdWriteBufferMarkerAMD vkCmdWriteBufferMarkerAMD;
#endif 
#if defined(VK_AMD_draw_indirect_count)
	extern PFN_vkCmdDrawIndexedIndirectCountAMD vkCmdDrawIndexedIndirectCountAMD;
	extern PFN_vkCmdDrawIndirectCountAMD vkCmdDrawIndirectCountAMD;
#endif 
#if defined(VK_AMD_shader_info)
	extern PFN_vkGetShaderInfoAMD vkGetShaderInfoAMD;
#endif 
#if defined(VK_ANDROID_external_memory_android_hardware_buffer)
	extern PFN_vkGetAndroidHardwareBufferPropertiesANDROID vkGetAndroidHardwareBufferPropertiesANDROID;
	extern PFN_vkGetMemoryAndroidHardwareBufferANDROID vkGetMemoryAndroidHardwareBufferANDROID;
#endif 
#if defined(VK_EXT_acquire_xlib_display)
	extern PFN_vkAcquireXlibDisplayEXT vkAcquireXlibDisplayEXT;
	extern PFN_vkGetRandROutputDisplayEXT vkGetRandROutputDisplayEXT;
#endif 
#if defined(VK_EXT_buffer_device_address)
	extern PFN_vkGetBufferDeviceAddressEXT vkGetBufferDeviceAddressEXT;
#endif 
#if defined(VK_EXT_calibrated_timestamps)
	extern PFN_vkGetCalibratedTimestampsEXT vkGetCalibratedTimestampsEXT;
	extern PFN_vkGetPhysicalDeviceCalibrateableTimeDomainsEXT vkGetPhysicalDeviceCalibrateableTimeDomainsEXT;
#endif 
#if defined(VK_EXT_conditional_rendering)
	extern PFN_vkCmdBeginConditionalRenderingEXT vkCmdBeginConditionalRenderingEXT;
	extern PFN_vkCmdEndConditionalRenderingEXT vkCmdEndConditionalRenderingEXT;
#endif 
#if defined(VK_EXT_debug_marker)
	extern PFN_vkCmdDebugMarkerBeginEXT vkCmdDebugMarkerBeginEXT;
	extern PFN_vkCmdDebugMarkerEndEXT vkCmdDebugMarkerEndEXT;
	extern PFN_vkCmdDebugMarkerInsertEXT vkCmdDebugMarkerInsertEXT;
	extern PFN_vkDebugMarkerSetObjectNameEXT vkDebugMarkerSetObjectNameEXT;
	extern PFN_vkDebugMarkerSetObjectTagEXT vkDebugMarkerSetObjectTagEXT;
#endif 
#if defined(VK_EXT_debug_report)
	extern PFN_vkCreateDebugReportCallbackEXT vkCreateDebugReportCallbackEXT;
	extern PFN_vkDebugReportMessageEXT vkDebugReportMessageEXT;
	extern PFN_vkDestroyDebugReportCallbackEXT vkDestroyDebugReportCallbackEXT;
#endif 
#if defined(VK_EXT_debug_utils)
	extern PFN_vkCmdBeginDebugUtilsLabelEXT vkCmdBeginDebugUtilsLabelEXT;
	extern PFN_vkCmdEndDebugUtilsLabelEXT vkCmdEndDebugUtilsLabelEXT;
	extern PFN_vkCmdInsertDebugUtilsLabelEXT vkCmdInsertDebugUtilsLabelEXT;
	extern PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT;
	extern PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT;
	extern PFN_vkQueueBeginDebugUtilsLabelEXT vkQueueBeginDebugUtilsLabelEXT;
	extern PFN_vkQueueEndDebugUtilsLabelEXT vkQueueEndDebugUtilsLabelEXT;
	extern PFN_vkQueueInsertDebugUtilsLabelEXT vkQueueInsertDebugUtilsLabelEXT;
	extern PFN_vkSetDebugUtilsObjectNameEXT vkSetDebugUtilsObjectNameEXT;
	extern PFN_vkSetDebugUtilsObjectTagEXT vkSetDebugUtilsObjectTagEXT;
	extern PFN_vkSubmitDebugUtilsMessageEXT vkSubmitDebugUtilsMessageEXT;
#endif 
#if defined(VK_EXT_direct_mode_display)
	extern PFN_vkReleaseDisplayEXT vkReleaseDisplayEXT;
#endif 
#if defined(VK_EXT_discard_rectangles)
	extern PFN_vkCmdSetDiscardRectangleEXT vkCmdSetDiscardRectangleEXT;
#endif 
#if defined(VK_EXT_display_control)
	extern PFN_vkDisplayPowerControlEXT vkDisplayPowerControlEXT;
	extern PFN_vkGetSwapchainCounterEXT vkGetSwapchainCounterEXT;
	extern PFN_vkRegisterDeviceEventEXT vkRegisterDeviceEventEXT;
	extern PFN_vkRegisterDisplayEventEXT vkRegisterDisplayEventEXT;
#endif 
#if defined(VK_EXT_display_surface_counter)
	extern PFN_vkGetPhysicalDeviceSurfaceCapabilities2EXT vkGetPhysicalDeviceSurfaceCapabilities2EXT;
#endif 
#if defined(VK_EXT_external_memory_host)
	extern PFN_vkGetMemoryHostPointerPropertiesEXT vkGetMemoryHostPointerPropertiesEXT;
#endif 
#if defined(VK_EXT_hdr_metadata)
	extern PFN_vkSetHdrMetadataEXT vkSetHdrMetadataEXT;
#endif 
#if defined(VK_EXT_image_drm_format_modifier)
	extern PFN_vkGetImageDrmFormatModifierPropertiesEXT vkGetImageDrmFormatModifierPropertiesEXT;
#endif 
#if defined(VK_EXT_sample_locations)
	extern PFN_vkCmdSetSampleLocationsEXT vkCmdSetSampleLocationsEXT;
	extern PFN_vkGetPhysicalDeviceMultisamplePropertiesEXT vkGetPhysicalDeviceMultisamplePropertiesEXT;
#endif 
#if defined(VK_EXT_transform_feedback)
	extern PFN_vkCmdBeginQueryIndexedEXT vkCmdBeginQueryIndexedEXT;
	extern PFN_vkCmdBeginTransformFeedbackEXT vkCmdBeginTransformFeedbackEXT;
	extern PFN_vkCmdBindTransformFeedbackBuffersEXT vkCmdBindTransformFeedbackBuffersEXT;
	extern PFN_vkCmdDrawIndirectByteCountEXT vkCmdDrawIndirectByteCountEXT;
	extern PFN_vkCmdEndQueryIndexedEXT vkCmdEndQueryIndexedEXT;
	extern PFN_vkCmdEndTransformFeedbackEXT vkCmdEndTransformFeedbackEXT;
#endif 
#if defined(VK_EXT_validation_cache)
	extern PFN_vkCreateValidationCacheEXT vkCreateValidationCacheEXT;
	extern PFN_vkDestroyValidationCacheEXT vkDestroyValidationCacheEXT;
	extern PFN_vkGetValidationCacheDataEXT vkGetValidationCacheDataEXT;
	extern PFN_vkMergeValidationCachesEXT vkMergeValidationCachesEXT;
#endif 
#if defined(VK_FUCHSIA_imagepipe_surface)
	extern PFN_vkCreateImagePipeSurfaceFUCHSIA vkCreateImagePipeSurfaceFUCHSIA;
#endif 
#if defined(VK_GOOGLE_display_timing)
	extern PFN_vkGetPastPresentationTimingGOOGLE vkGetPastPresentationTimingGOOGLE;
	extern PFN_vkGetRefreshCycleDurationGOOGLE vkGetRefreshCycleDurationGOOGLE;
#endif 
#if defined(VK_KHR_android_surface)
	extern PFN_vkCreateAndroidSurfaceKHR vkCreateAndroidSurfaceKHR;
#endif 
#if defined(VK_KHR_bind_memory2)
	extern PFN_vkBindBufferMemory2KHR vkBindBufferMemory2KHR;
	extern PFN_vkBindImageMemory2KHR vkBindImageMemory2KHR;
#endif 
#if defined(VK_KHR_create_renderpass2)
	extern PFN_vkCmdBeginRenderPass2KHR vkCmdBeginRenderPass2KHR;
	extern PFN_vkCmdEndRenderPass2KHR vkCmdEndRenderPass2KHR;
	extern PFN_vkCmdNextSubpass2KHR vkCmdNextSubpass2KHR;
	extern PFN_vkCreateRenderPass2KHR vkCreateRenderPass2KHR;
#endif 
#if defined(VK_KHR_descriptor_update_template)
	extern PFN_vkCreateDescriptorUpdateTemplateKHR vkCreateDescriptorUpdateTemplateKHR;
	extern PFN_vkDestroyDescriptorUpdateTemplateKHR vkDestroyDescriptorUpdateTemplateKHR;
	extern PFN_vkUpdateDescriptorSetWithTemplateKHR vkUpdateDescriptorSetWithTemplateKHR;
#endif 
#if defined(VK_KHR_device_group)
	extern PFN_vkCmdDispatchBaseKHR vkCmdDispatchBaseKHR;
	extern PFN_vkCmdSetDeviceMaskKHR vkCmdSetDeviceMaskKHR;
	extern PFN_vkGetDeviceGroupPeerMemoryFeaturesKHR vkGetDeviceGroupPeerMemoryFeaturesKHR;
#endif 
#if defined(VK_KHR_device_group_creation)
	extern PFN_vkEnumeratePhysicalDeviceGroupsKHR vkEnumeratePhysicalDeviceGroupsKHR;
#endif 
#if defined(VK_KHR_display)
	extern PFN_vkCreateDisplayModeKHR vkCreateDisplayModeKHR;
	extern PFN_vkCreateDisplayPlaneSurfaceKHR vkCreateDisplayPlaneSurfaceKHR;
	extern PFN_vkGetDisplayModePropertiesKHR vkGetDisplayModePropertiesKHR;
	extern PFN_vkGetDisplayPlaneCapabilitiesKHR vkGetDisplayPlaneCapabilitiesKHR;
	extern PFN_vkGetDisplayPlaneSupportedDisplaysKHR vkGetDisplayPlaneSupportedDisplaysKHR;
	extern PFN_vkGetPhysicalDeviceDisplayPlanePropertiesKHR vkGetPhysicalDeviceDisplayPlanePropertiesKHR;
	extern PFN_vkGetPhysicalDeviceDisplayPropertiesKHR vkGetPhysicalDeviceDisplayPropertiesKHR;
#endif 
#if defined(VK_KHR_display_swapchain)
	extern PFN_vkCreateSharedSwapchainsKHR vkCreateSharedSwapchainsKHR;
#endif 
#if defined(VK_KHR_draw_indirect_count)
	extern PFN_vkCmdDrawIndexedIndirectCountKHR vkCmdDrawIndexedIndirectCountKHR;
	extern PFN_vkCmdDrawIndirectCountKHR vkCmdDrawIndirectCountKHR;
#endif 
#if defined(VK_KHR_external_fence_capabilities)
	extern PFN_vkGetPhysicalDeviceExternalFencePropertiesKHR vkGetPhysicalDeviceExternalFencePropertiesKHR;
#endif 
#if defined(VK_KHR_external_fence_fd)
	extern PFN_vkGetFenceFdKHR vkGetFenceFdKHR;
	extern PFN_vkImportFenceFdKHR vkImportFenceFdKHR;
#endif 
#if defined(VK_KHR_external_fence_win32)
	extern PFN_vkGetFenceWin32HandleKHR vkGetFenceWin32HandleKHR;
	extern PFN_vkImportFenceWin32HandleKHR vkImportFenceWin32HandleKHR;
#endif 
#if defined(VK_KHR_external_memory_capabilities)
	extern PFN_vkGetPhysicalDeviceExternalBufferPropertiesKHR vkGetPhysicalDeviceExternalBufferPropertiesKHR;
#endif 
#if defined(VK_KHR_external_memory_fd)
	extern PFN_vkGetMemoryFdKHR vkGetMemoryFdKHR;
	extern PFN_vkGetMemoryFdPropertiesKHR vkGetMemoryFdPropertiesKHR;
#endif 
#if defined(VK_KHR_external_memory_win32)
	extern PFN_vkGetMemoryWin32HandleKHR vkGetMemoryWin32HandleKHR;
	extern PFN_vkGetMemoryWin32HandlePropertiesKHR vkGetMemoryWin32HandlePropertiesKHR;
#endif 
#if defined(VK_KHR_external_semaphore_capabilities)
	extern PFN_vkGetPhysicalDeviceExternalSemaphorePropertiesKHR vkGetPhysicalDeviceExternalSemaphorePropertiesKHR;
#endif 
#if defined(VK_KHR_external_semaphore_fd)
	extern PFN_vkGetSemaphoreFdKHR vkGetSemaphoreFdKHR;
	extern PFN_vkImportSemaphoreFdKHR vkImportSemaphoreFdKHR;
#endif 
#if defined(VK_KHR_external_semaphore_win32)
	extern PFN_vkGetSemaphoreWin32HandleKHR vkGetSemaphoreWin32HandleKHR;
	extern PFN_vkImportSemaphoreWin32HandleKHR vkImportSemaphoreWin32HandleKHR;
#endif 
#if defined(VK_KHR_get_display_properties2)
	extern PFN_vkGetDisplayModeProperties2KHR vkGetDisplayModeProperties2KHR;
	extern PFN_vkGetDisplayPlaneCapabilities2KHR vkGetDisplayPlaneCapabilities2KHR;
	extern PFN_vkGetPhysicalDeviceDisplayPlaneProperties2KHR vkGetPhysicalDeviceDisplayPlaneProperties2KHR;
	extern PFN_vkGetPhysicalDeviceDisplayProperties2KHR vkGetPhysicalDeviceDisplayProperties2KHR;
#endif 
#if defined(VK_KHR_get_memory_requirements2)
	extern PFN_vkGetBufferMemoryRequirements2KHR vkGetBufferMemoryRequirements2KHR;
	extern PFN_vkGetImageMemoryRequirements2KHR vkGetImageMemoryRequirements2KHR;
	extern PFN_vkGetImageSparseMemoryRequirements2KHR vkGetImageSparseMemoryRequirements2KHR;
#endif 
#if defined(VK_KHR_get_physical_device_properties2)
	extern PFN_vkGetPhysicalDeviceFeatures2KHR vkGetPhysicalDeviceFeatures2KHR;
	extern PFN_vkGetPhysicalDeviceFormatProperties2KHR vkGetPhysicalDeviceFormatProperties2KHR;
	extern PFN_vkGetPhysicalDeviceImageFormatProperties2KHR vkGetPhysicalDeviceImageFormatProperties2KHR;
	extern PFN_vkGetPhysicalDeviceMemoryProperties2KHR vkGetPhysicalDeviceMemoryProperties2KHR;
	extern PFN_vkGetPhysicalDeviceProperties2KHR vkGetPhysicalDeviceProperties2KHR;
	extern PFN_vkGetPhysicalDeviceQueueFamilyProperties2KHR vkGetPhysicalDeviceQueueFamilyProperties2KHR;
	extern PFN_vkGetPhysicalDeviceSparseImageFormatProperties2KHR vkGetPhysicalDeviceSparseImageFormatProperties2KHR;
#endif 
#if defined(VK_KHR_get_surface_capabilities2)
	extern PFN_vkGetPhysicalDeviceSurfaceCapabilities2KHR vkGetPhysicalDeviceSurfaceCapabilities2KHR;
	extern PFN_vkGetPhysicalDeviceSurfaceFormats2KHR vkGetPhysicalDeviceSurfaceFormats2KHR;
#endif 
#if defined(VK_KHR_maintenance1)
	extern PFN_vkTrimCommandPoolKHR vkTrimCommandPoolKHR;
#endif 
#if defined(VK_KHR_maintenance3)
	extern PFN_vkGetDescriptorSetLayoutSupportKHR vkGetDescriptorSetLayoutSupportKHR;
#endif 
#if defined(VK_KHR_push_descriptor)
	extern PFN_vkCmdPushDescriptorSetKHR vkCmdPushDescriptorSetKHR;
#endif 
#if defined(VK_KHR_sampler_ycbcr_conversion)
	extern PFN_vkCreateSamplerYcbcrConversionKHR vkCreateSamplerYcbcrConversionKHR;
	extern PFN_vkDestroySamplerYcbcrConversionKHR vkDestroySamplerYcbcrConversionKHR;
#endif 
#if defined(VK_KHR_shared_presentable_image)
	extern PFN_vkGetSwapchainStatusKHR vkGetSwapchainStatusKHR;
#endif 
#if defined(VK_KHR_surface)
	extern PFN_vkDestroySurfaceKHR vkDestroySurfaceKHR;
	extern PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR vkGetPhysicalDeviceSurfaceCapabilitiesKHR;
	extern PFN_vkGetPhysicalDeviceSurfaceFormatsKHR vkGetPhysicalDeviceSurfaceFormatsKHR;
	extern PFN_vkGetPhysicalDeviceSurfacePresentModesKHR vkGetPhysicalDeviceSurfacePresentModesKHR;
	extern PFN_vkGetPhysicalDeviceSurfaceSupportKHR vkGetPhysicalDeviceSurfaceSupportKHR;
#endif 
#if defined(VK_KHR_swapchain)
	extern PFN_vkAcquireNextImageKHR vkAcquireNextImageKHR;
	extern PFN_vkCreateSwapchainKHR vkCreateSwapchainKHR;
	extern PFN_vkDestroySwapchainKHR vkDestroySwapchainKHR;
	extern PFN_vkGetSwapchainImagesKHR vkGetSwapchainImagesKHR;
	extern PFN_vkQueuePresentKHR vkQueuePresentKHR;
#endif 
#if defined(VK_KHR_wayland_surface)
	extern PFN_vkCreateWaylandSurfaceKHR vkCreateWaylandSurfaceKHR;
	extern PFN_vkGetPhysicalDeviceWaylandPresentationSupportKHR vkGetPhysicalDeviceWaylandPresentationSupportKHR;
#endif 
#if defined(VK_KHR_win32_surface)
	extern PFN_vkCreateWin32SurfaceKHR vkCreateWin32SurfaceKHR;
	extern PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR vkGetPhysicalDeviceWin32PresentationSupportKHR;
#endif 
#if defined(VK_KHR_xcb_surface)
	extern PFN_vkCreateXcbSurfaceKHR vkCreateXcbSurfaceKHR;
	extern PFN_vkGetPhysicalDeviceXcbPresentationSupportKHR vkGetPhysicalDeviceXcbPresentationSupportKHR;
#endif 
#if defined(VK_KHR_xlib_surface)
	extern PFN_vkCreateXlibSurfaceKHR vkCreateXlibSurfaceKHR;
	extern PFN_vkGetPhysicalDeviceXlibPresentationSupportKHR vkGetPhysicalDeviceXlibPresentationSupportKHR;
#endif 
#if defined(VK_MVK_ios_surface)
	extern PFN_vkCreateIOSSurfaceMVK vkCreateIOSSurfaceMVK;
#endif 
#if defined(VK_MVK_macos_surface)
	extern PFN_vkCreateMacOSSurfaceMVK vkCreateMacOSSurfaceMVK;
#endif 
#if defined(VK_NN_vi_surface)
	extern PFN_vkCreateViSurfaceNN vkCreateViSurfaceNN;
#endif 
#if defined(VK_NVX_device_generated_commands)
	extern PFN_vkCmdProcessCommandsNVX vkCmdProcessCommandsNVX;
	extern PFN_vkCmdReserveSpaceForCommandsNVX vkCmdReserveSpaceForCommandsNVX;
	extern PFN_vkCreateIndirectCommandsLayoutNVX vkCreateIndirectCommandsLayoutNVX;
	extern PFN_vkCreateObjectTableNVX vkCreateObjectTableNVX;
	extern PFN_vkDestroyIndirectCommandsLayoutNVX vkDestroyIndirectCommandsLayoutNVX;
	extern PFN_vkDestroyObjectTableNVX vkDestroyObjectTableNVX;
	extern PFN_vkGetPhysicalDeviceGeneratedCommandsPropertiesNVX vkGetPhysicalDeviceGeneratedCommandsPropertiesNVX;
	extern PFN_vkRegisterObjectsNVX vkRegisterObjectsNVX;
	extern PFN_vkUnregisterObjectsNVX vkUnregisterObjectsNVX;
#endif 
#if defined(VK_NV_clip_space_w_scaling)
	extern PFN_vkCmdSetViewportWScalingNV vkCmdSetViewportWScalingNV;
#endif 
#if defined(VK_NV_device_diagnostic_checkpoints)
	extern PFN_vkCmdSetCheckpointNV vkCmdSetCheckpointNV;
	extern PFN_vkGetQueueCheckpointDataNV vkGetQueueCheckpointDataNV;
#endif 
#if defined(VK_NV_external_memory_capabilities)
	extern PFN_vkGetPhysicalDeviceExternalImageFormatPropertiesNV vkGetPhysicalDeviceExternalImageFormatPropertiesNV;
#endif 
#if defined(VK_NV_external_memory_win32)
	extern PFN_vkGetMemoryWin32HandleNV vkGetMemoryWin32HandleNV;
#endif 
#if defined(VK_NV_mesh_shader)
	extern PFN_vkCmdDrawMeshTasksIndirectCountNV vkCmdDrawMeshTasksIndirectCountNV;
	extern PFN_vkCmdDrawMeshTasksIndirectNV vkCmdDrawMeshTasksIndirectNV;
	extern PFN_vkCmdDrawMeshTasksNV vkCmdDrawMeshTasksNV;
#endif 
#if defined(VK_NV_ray_tracing)
	extern PFN_vkBindAccelerationStructureMemoryNV vkBindAccelerationStructureMemoryNV;
	extern PFN_vkCmdBuildAccelerationStructureNV vkCmdBuildAccelerationStructureNV;
	extern PFN_vkCmdCopyAccelerationStructureNV vkCmdCopyAccelerationStructureNV;
	extern PFN_vkCmdTraceRaysNV vkCmdTraceRaysNV;
	extern PFN_vkCmdWriteAccelerationStructuresPropertiesNV vkCmdWriteAccelerationStructuresPropertiesNV;
	extern PFN_vkCompileDeferredNV vkCompileDeferredNV;
	extern PFN_vkCreateAccelerationStructureNV vkCreateAccelerationStructureNV;
	extern PFN_vkCreateRayTracingPipelinesNV vkCreateRayTracingPipelinesNV;
	extern PFN_vkDestroyAccelerationStructureNV vkDestroyAccelerationStructureNV;
	extern PFN_vkGetAccelerationStructureHandleNV vkGetAccelerationStructureHandleNV;
	extern PFN_vkGetAccelerationStructureMemoryRequirementsNV vkGetAccelerationStructureMemoryRequirementsNV;
	extern PFN_vkGetRayTracingShaderGroupHandlesNV vkGetRayTracingShaderGroupHandlesNV;
#endif 
#if defined(VK_NV_scissor_exclusive)
	extern PFN_vkCmdSetExclusiveScissorNV vkCmdSetExclusiveScissorNV;
#endif 
#if defined(VK_NV_shading_rate_image)
	extern PFN_vkCmdBindShadingRateImageNV vkCmdBindShadingRateImageNV;
	extern PFN_vkCmdSetCoarseSampleOrderNV vkCmdSetCoarseSampleOrderNV;
	extern PFN_vkCmdSetViewportShadingRatePaletteNV vkCmdSetViewportShadingRatePaletteNV;
#endif 
#if (defined(VK_KHR_descriptor_update_template) && defined(VK_KHR_push_descriptor)) || (defined(VK_KHR_push_descriptor) && defined(VK_VERSION_1_1))
	extern PFN_vkCmdPushDescriptorSetWithTemplateKHR vkCmdPushDescriptorSetWithTemplateKHR;
#endif 
#if (defined(VK_KHR_device_group) && defined(VK_KHR_surface)) || (defined(VK_KHR_swapchain) && defined(VK_VERSION_1_1))
	extern PFN_vkGetDeviceGroupPresentCapabilitiesKHR vkGetDeviceGroupPresentCapabilitiesKHR;
	extern PFN_vkGetDeviceGroupSurfacePresentModesKHR vkGetDeviceGroupSurfacePresentModesKHR;
	extern PFN_vkGetPhysicalDevicePresentRectanglesKHR vkGetPhysicalDevicePresentRectanglesKHR;
#endif 
#if (defined(VK_KHR_device_group) && defined(VK_KHR_swapchain)) || (defined(VK_KHR_swapchain) && defined(VK_VERSION_1_1))
	extern PFN_vkAcquireNextImage2KHR vkAcquireNextImage2KHR;
#endif

#if defined(VK_AMD_display_native_hdr)
	extern PFN_vkSetLocalDimmingAMD vkSetLocalDimmingAMD;
#endif
#if defined(VK_EXT_full_screen_exclusive)
	extern PFN_vkAcquireFullScreenExclusiveModeEXT vkAcquireFullScreenExclusiveModeEXT;
	extern PFN_vkGetPhysicalDeviceSurfacePresentModes2EXT vkGetPhysicalDeviceSurfacePresentModes2EXT;
	extern PFN_vkReleaseFullScreenExclusiveModeEXT vkReleaseFullScreenExclusiveModeEXT;
#endif
#if defined(VK_EXT_headless_surface)
	extern PFN_vkCreateHeadlessSurfaceEXT vkCreateHeadlessSurfaceEXT;
#endif
#if defined(VK_EXT_host_query_reset)
	extern PFN_vkResetQueryPoolEXT vkResetQueryPoolEXT;
#endif
#if defined(VK_EXT_metal_surface)
	extern PFN_vkCreateMetalSurfaceEXT vkCreateMetalSurfaceEXT;
#endif
#if defined(VK_GGP_stream_descriptor_surface)
	extern PFN_vkCreateStreamDescriptorSurfaceGGP vkCreateStreamDescriptorSurfaceGGP;
#endif
#if defined(VK_NVX_image_view_handle)
	extern PFN_vkGetImageViewHandleNVX vkGetImageViewHandleNVX;
#endif
#if defined(VK_NV_cooperative_matrix)
	extern PFN_vkGetPhysicalDeviceCooperativeMatrixPropertiesNV vkGetPhysicalDeviceCooperativeMatrixPropertiesNV;
#endif
#if (defined(VK_EXT_full_screen_exclusive) && defined(VK_VERSION_1_1)) || (defined(VK_KHR_device_group) && defined(VK_EXT_full_screen_exclusive))
	extern PFN_vkGetDeviceGroupSurfacePresentModes2EXT vkGetDeviceGroupSurfacePresentModes2EXT;
#endif

#if defined(AGK_IOS) || defined(AGK_MACOS)
    extern PFN_vkGetMTLDeviceMVK vkGetMTLDeviceMVK;
    extern PFN_vkSetMTLTextureMVK vkSetMTLTextureMVK;
#endif


#endif // AGK_IOS

#endif // DISABLE_VULKAN

#endif // _H_VULKAN_FUNCTIONS
