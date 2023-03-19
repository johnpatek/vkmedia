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

#ifndef VKSERVER_H
#define VKSERVER_H

#include <vkmedia.h>
#include <assert.h>
#include <gst/app/app.h>
#include <gst/rtsp-server/rtsp-server.h>

typedef struct _vkm_context
{
    int data;
} vkm_context;


typedef struct _vkm_heap
{
    void **blocks;
    size_t block_count;
} vkm_heap;

void *vkm_heap_malloc(vkm_heap *heap, size_t size);
void *vkm_heap_calloc(vkm_heap *heap, size_t count, size_t size);
void vkm_heap_clean(vkm_heap *heap);

#define VKM_MALLOC(SIZE) vkm_heap_malloc(&heap, SIZE)
#define VKM_CALLOC(COUNT, SIZE) vkm_heap_calloc(&heap, COUNT, SIZE)

#define VKM_BEGIN_FUNCTION    \
    vkm_heap heap = {         \
        .blocks = NULL,       \
        .block_count = 0,     \
    };                        \
    int vkm_result;           \
    VkResult vk_result;       \
    vkm_result = VKM_SUCCESS; \
    vk_result = VK_SUCCESS;

#define VKM_END_FUNCTION      \
    goto done;                \
    error:                    \
    vkm_result = VKM_FAILURE; \
    done:                     \
    vkm_heap_clean(&heap);    \
    return vkm_result;

#define VKM_ASSERT(COND, MESSAGE) \
    if (!(COND))                  \
    {                             \
        fprintf(                  \
            stderr,               \
            "%s:%d: %s: %s\n",    \
            __FILE__,             \
            __LINE__,             \
            __func__,             \
            MESSAGE);             \
        goto error;               \
    }

#define VK_CHECK_RESULT(FUNC)             \
    vk_result = FUNC;                     \
    if (vk_result != VK_SUCCESS)          \
    {                                     \
        fprintf(                          \
            stderr,                       \
            "%s:%d: %s: %s failed(%d)\n", \
            __FILE__,                     \
            __LINE__,                     \
            __func__,                     \
            #FUNC,                        \
            vk_result);                   \
        goto error;                       \
    }

typedef struct
{

} vkm_encoder_s;

#define VKM_ENCODER(HANDLE) ((vkm_encoder_s *)HANDLE)

#endif
