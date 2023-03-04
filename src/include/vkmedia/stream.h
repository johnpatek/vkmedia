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

#ifndef VKM_STREAM_H
#define VKM_STREAM_H
#include "swapchain.h"

typedef enum
{
    VKM_FORMAT,
} vkm_stream_format;

typedef struct
{
    VkPhysicalDevice vulkan_physical_device;
    CUdevice cuda_device;
    vkm_stream_format format;
    int width;
    int height;
    int framerate;
} vkm_stream_parameters;

/**
 * @brief vkm_stream simplifies interaction between vkm_swapchain and vkm_encoder
 */
typedef vkm_handle vkm_stream;

int vkm_stream_create(
    const vkm_stream_parameters *stream_parameters,
    vkm_stream *stream);

void vkm_stream_destroy(
    vkm_stream stream);

int vkm_stream_get_swapchain(
    vkm_stream stream,
    vkm_swapchain *swapchain);

int vkm_stream_initialize(
    vkm_stream stream);

int vkm_stream_start(
    vkm_stream stream);

int vkm_stream_stop(
    vkm_stream stream);


#endif