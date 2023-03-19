#ifndef EXAMPLE_UTIL_H
#define EXAMPLE_UTIL_H

#include <vulkan/vulkan.h>

VkShaderModule load_shader_module(VkDevice device, const char * path);

VkQueue find_graphics_queue(VkPhysicalDevice physical_device, VkDevice device);

#endif