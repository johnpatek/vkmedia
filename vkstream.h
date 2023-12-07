#ifndef VKSTREAM_H
#define VKSTREAM_H
#include <cuda.h>
#include <vulkan/vulkan.h>

#include <stdio.h>

#define VKSTREAM_SUCCESS 0
#define VKSTREAM_FAILURE 1

#define WIDTH 640
#define HEIGHT 480

typedef struct
{
    VkInstance vk_instance;

    VkPhysicalDevice vk_physical_device;
    uint32_t vk_queue_family;

    CUdevice cu_device;
    CUcontext cu_context;

    VkDevice vk_device;
    VkQueue vk_queue;

    VkImage vk_image;
    VkMemoryRequirements vk_memory_requirements;

    VkImageView vk_image_view;

    VkDeviceMemory vk_image_memory;
    int vk_image_memory_handle;

    CUexternalMemory cu_image_memory;
} vkstream;

typedef struct
{
    CUmipmappedArray cu_mipmapped_array;
} vkstream_frame;

vkstream *vkstream_new(int device_index);

void vkstream_free(vkstream *stream);

#endif