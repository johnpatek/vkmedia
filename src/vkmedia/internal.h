#ifndef VKSERVER_H
#define VKSERVER_H

#include "vkmedia.h"

typedef struct
{
    void **blocks;
    size_t block_count;
} vkm_heap;

void *vkm_heap_malloc(vkm_heap *heap, size_t size);
void *vkm_heap_calloc(vkm_heap *heap, size_t count, size_t size);
void vkm_heap_clean(vkm_heap *heap);

#define VKM_MALLOC(SIZE) vkm_heap_malloc(&heap, SIZE)
#define VKM_CALLOC(COUNT, SIZE) vkm_heap_calloc(&heap, COUNT, SIZE)

#define VKM_BEGIN_FUNCTION(FUNC, ARGS...) \
    int FUNC(ARGS)                        \
    {                                     \
        vkm_heap heap = {                 \
            .blocks = NULL,               \
            .block_count = 0,             \
        };                                \
        int vkm_result;                   \
        VkResult vk_result;               \
        CUresult cu_result;               \
        vkm_result = VKM_SUCCESS;

#define VKM_END_FUNCTION      \
    goto done;                \
    error:                    \
    vkm_result = VKM_FAILURE; \
    done:                     \
    vkm_heap_clean(&heap);    \
    return vkm_result;        \
    }

#define VK_CHECK_RESULT(FUNC, ARGS...)    \
    vk_result = FUNC(ARGS);               \
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

#define CU_CHECK_RESULT(FUNC, ARGS...)    \
    cu_result = FUNC(ARGS);               \
    if (cu_result != CUDA_SUCCESS)        \
    {                                     \
        fprintf(                          \
            stderr,                       \
            "%s:%d: %s: %s failed(%d)\n", \
            __FILE__,                     \
            __LINE__,                     \
            __func__,                     \
            #FUNC,                        \
            cu_result);                   \
        goto error;                       \
    }

#endif

