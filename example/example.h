#ifndef EXAMPLE_H
#define EXAMPLE_H

#include <vkmedia.h>
#include <assert.h>

typedef struct _vkm_example_parameters
{
    uint32_t device_index;
    uint32_t width;
    uint32_t height;
    uint32_t fps;
    const char * shader_directory;
} vkm_example_parameters;

typedef struct _vkm_example
{
    VkExtent2D screen_size;

    // Vulkan and CUDA info used by vkm components
    VkInstance vk_instance;
    VkPhysicalDevice vk_physical_device;
    
    // graphics queue
    uint32_t vk_queue_index;
    VkDevice vk_device;
    
    // vkm components
    vkm_server server;
    vkm_application application;
    
    // device and image info retrieved from application
    uint32_t image_count;
    VkImage *vk_images;
    
    // rendering components managed externally
    VkCommandBuffer *vk_command_buffers;
    VkFence  *vk_fences;
    VkFramebuffer *vk_framebuffers;

    VkRenderPass vk_render_pass;
    VkPipeline vk_graphics_pipeline;

    VkBool32 running;
} vkm_example;

void vkm_example_create(
    const vkm_example_parameters *example_parameters,
    vkm_example **example);

void vkm_example_loop(vkm_example *example);

void vkm_example_quit(vkm_example *example);

void vkm_example_destroy(vkm_example *example);

#endif