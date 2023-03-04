/*
** Copyright 2023 John R. Patek Sr.
**
** Permission is hereby granted, free of charge, to any person obtaining a copy
** of this software and associated documentation files (the "Software"), to deal
** in the Software without restriction, including without limitation the rights
** to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
** copies of the Software, and to permit persons to whom the Software is
** furnished to do so, subject to the following conditions:
**
** The above copyright notice and this permission notice shall be included in all
** copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
** AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
** OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
** SOFTWARE.
*/

#ifndef VKM_SWAPCHAIN_H
#define VKM_SWAPCHAIN_H

#include "core.h"

typedef struct
{
    VkPhysicalDevice vulkan_physical_device;
    VkDevice vulkan_logical_device;
    CUcontext cuda_context;
    VkFormat format;
    VkExtent2D size;
    uint32_t count;
} vkm_swapchain_parameters;

/**
 * @brief vkm_swapchain manages images shared between Vulkan and CUDA
 */
typedef vkm_handle vkm_swapchain;

/**
 * @brief create a new swapchain from a set of parameters
 *
 * @param swapchain_parameters a pointer to a vkm_swapchain_parameters structure
 * controlling creation of the swapchain.
 * @param points to a vkm_swapchain handle in which the resulting swapchain is
 * returned.
 * @return 0 on success, non-zero on failure.
 */
int vkm_swapchain_create(
    const vkm_swapchain_parameters *swapchain_parameters, 
    vkm_swapchain *swapchain);

void vkm_swapchain_destroy(
    vkm_swapchain swapchain);

int vkm_swapchain_get_count(
    vkm_swapchain swapchain,
    uint32_t *count);

int vkm_swapchain_get_images(
    vkm_swapchain swapchain, 
    VkImage **images);

int vkm_swapchain_get_frames(
    vkm_swapchain swapchain, 
    CUarray **frames);

int vkm_swapchain_acquire_image(
    vkm_swapchain swapchain, 
    uint32_t *image_index, 
    VkSemaphore *wait_semaphore, 
    VkSemaphore *signal_semaphore);

int vkm_swapchain_acquire_frame(
    vkm_swapchain swapchain, 
    uint32_t *frame_index, 
    CUexternalSemaphore *wait_semaphore, 
    CUexternalSemaphore *signal_semaphore);

#endif