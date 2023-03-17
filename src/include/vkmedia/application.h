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

#ifndef VKM_APPLICATION_H
#define VKM_APPLICATION_H

#include "core.h"

typedef struct _vkm_application_parameters
{
    VkPhysicalDevice pysical_device;
    
    uint32_t width;

    uint32_t height;

    uint32_t fps_num;

    uint32_t fps_den;

    uint32_t image_count;

    uint32_t device_extension_count;

    const char ** device_extensions;

    uint32_t queue_family_count;

    uint32_t *queue_families;

} vkm_application_parameters;

typedef vkm_handle vkm_application;

typedef void (*vkm_bitstream_callback)(uint8_t *, size_t, void *);

int vkm_application_create(
    const vkm_application_parameters *application_parameters,
    vkm_application *application);

void vkm_application_destroy(
    vkm_application application);

int vkm_application_get_id(
    vkm_application application,
    const char **id);

int vkm_application_get_extent(
    vkm_application application,
    VkExtent2D *extent);

int vkm_application_get_framerate(
    vkm_application application,
    uint32_t *fps_num,
    uint32_t *fps_den);

int vkm_application_start(
    vkm_application application);

int vkm_application_stop(
    vkm_application application);

int vkm_application_get_images(
    vkm_application application,
    VkImage **images,
    uint32_t *image_count);

int vkm_application_acquire_image(
    vkm_application application,
    uint32_t *image_index,
    VkSemaphore *wait_semaphore,
    VkSemaphore *signal_semaphore);

#endif