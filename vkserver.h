#ifndef VKSERVER_H
#define VKSERVER_H

#include <assert.h>

#include <stdio.h>

#include <gst/gst.h>
#include <gst/app/app.h>

#include <gst/rtsp-server/rtsp-server.h>

#include <cuda.h>
#include <ffnvcodec/nvEncodeAPI.h>
#include <stdio.h>

#include <vulkan/vulkan.h>

typedef struct
{
    VkPhysicalDevice vulkan_physical_device;
    VkDevice vulkan_logical_device;
    CUcontext cuda_context;
    VkExtent2D size;
    uint32_t count;
} vkm_swapchain_parameters;

typedef struct
{
    VkSemaphore vk_handle;
    CUexternalSemaphore cu_handle;
} vkm_semaphore;

/**
 * manages shared images
 */
typedef struct
{
    uint32_t count;

    uint32_t vk_index;
    VkImage *vk_images;
    VkDeviceMemory *vk_image_bindings;

    vkm_semaphore *image_semaphores;
    vkm_semaphore *frame_semaphores;

    uint32_t cu_index;
    CUarray *cu_frames;
    CUmipmappedArray *cu_frame_mappings;
    
} vkm_swapchain;

int vkm_swapchain_create(const vkm_swapchain_parameters *swapchain_parameters, vkm_swapchain **swapchain);

void vkm_swapchain_destroy(vkm_swapchain *swapchain);

int vkm_swapchain_get_images(vkm_swapchain *swapchain, VkImage **images);

int vkm_swapchain_get_frames(vkm_swapchain *swapchain, CUarray **frames);

int vkm_swapchain_acquire_image(vkm_swapchain *swapchain, uint32_t *image_index);

int vkm_swapchain_acquire_frame(vkm_swapchain *swapchain, uint32_t *frame_index);

int vkm_swapchain_release_image(vkm_swapchain *swapchain, uint32_t image_index);

int vkm_swapchain_release_frame(vkm_swapchain *swapchain, uint32_t frame_index);

/**
 * vkm_encoder Wrapper for NVIDIA video encoder
 */
typedef struct
{
    NV_ENCODE_API_FUNCTION_LIST api;
    void *encoder;
} vkm_encoder;

void vkm_encoder_producer(CUarray *input);

/**
 * Wrapper for Gstreamer RTSP
 */
typedef struct
{
    GstRTSPServer rtsp_server;
} vkm_server;

#endif