#include "internal.h"

void *vkm_heap_malloc(vkm_heap *heap, size_t size)
{
    void *block;
    heap->blocks = realloc(heap->blocks, sizeof(void *) * (heap->block_count + 1));
    block = malloc(size);
    *(heap->blocks + heap->block_count) = block;
    heap->block_count++;
    return block;
}

void *vkm_heap_calloc(vkm_heap *heap, size_t count, size_t size)
{
    void *block;
    heap->blocks = realloc(heap->blocks, sizeof(void *) * (heap->block_count + 1));
    block = calloc(count, size);
    *(heap->blocks + heap->block_count) = block;
    heap->block_count++;
    return block;
}

void vkm_heap_clean(vkm_heap *heap)
{
    size_t block_index;
    for (block_index = 0; block_index < heap->block_count; block_index++)
    {
        free(*(heap->blocks + block_index));
    }
    free(heap->blocks);
}