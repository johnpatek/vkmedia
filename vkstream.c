#include "vkstream.h"

typedef struct
{
    void **blocks;
    size_t block_count;
} vkstream_heap;

static void *vkstream_heap_malloc(vkstream_heap *heap, size_t size)
{
    void *block;
    heap->blocks = realloc(heap->blocks, sizeof(void *) * (heap->block_count + 1));
    block = malloc(size);
    *(heap->blocks + heap->block_count) = block;
    heap->block_count++;
    return block;
}

static void *vkstream_heap_calloc(vkstream_heap *heap, size_t count, size_t size)
{
    void *block;
    heap->blocks = realloc(heap->blocks, sizeof(void *) * (heap->block_count + 1));
    block = calloc(count, size);
    *(heap->blocks + heap->block_count) = block;
    heap->block_count++;
    return block;
}

static void vkstream_heap_clean(vkstream_heap *heap)
{
    size_t block_index;
    for (block_index = 0; block_index < heap->block_count; block_index++)
    {
        free(*(heap->blocks + block_index));
    }
    free(heap->blocks);
}

#define MALLOC(SIZE) vkstream_heap_malloc(&heap, SIZE)
#define CALLOC(COUNT, SIZE) vkstream_heap_calloc(&heap, COUNT, SIZE)

#define BEGIN_FUNCTION(FUNC, ARGS...) \
    int FUNC(ARGS)                    \
    {                                 \
        vkstream_heap heap = {        \
            .blocks = NULL,           \
            .block_count = 0,         \
        };                            \
        int vkstream_result;          \
        VkResult vk_result;           \
        CUresult cu_result;           \
        vkstream_result = VKSTREAM_SUCCESS;

#define END_FUNCTION                    \
    goto done;                          \
    error:                              \
    vkstream_result = VKSTREAM_FAILURE; \
    done:                               \
    vkstream_heap_clean(&heap);         \
    return vkstream_result;             \
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

static int vkstream_new_instance(vkstream *stream);

static int vkstream_new_physical_device(vkstream *stream, int device_index);

static int vkstream_new_queue_family(vkstream *stream);

static int vkstream_new_cuda_context(vkstream *stream, int device_index);

static int vkstream_new_logical_device(vkstream *stream);

static int vkstream_new_image(vkstream *stream);

static int vkstream_new_image_view(vkstream *stream);

static int vkstream_new_memory_type(
    vkstream *stream,
    uint32_t type_bits,
    VkMemoryPropertyFlags property_flags,
    uint32_t *type);

static int vkstream_new_memory_allocate(vkstream *stream, uint32_t memory_type);

static int vkstream_new_memory_export(vkstream *stream);

static int vkstream_new_memory_import(vkstream *stream);

vkstream *vkstream_new(int device_index)
{
    vkstream *stream;
    uint32_t memory_type;

    stream = calloc(1, sizeof(vkstream));
    if (stream == NULL)
    {
        goto error;
    }

#define VKSTREAM_NEW_FUNC(FUNC, ARGS...)                     \
    if (FUNC(ARGS) != VKSTREAM_SUCCESS)                      \
    {                                                        \
        fprintf(stderr, "vkstream_new: %s failed\n", #FUNC); \
        goto error;                                          \
    }

    VKSTREAM_NEW_FUNC(vkstream_new_instance, stream)
    VKSTREAM_NEW_FUNC(vkstream_new_physical_device, stream, device_index)
    VKSTREAM_NEW_FUNC(vkstream_new_queue_family, stream)
    VKSTREAM_NEW_FUNC(vkstream_new_cuda_context, stream, device_index)
    VKSTREAM_NEW_FUNC(vkstream_new_logical_device, stream)
    VKSTREAM_NEW_FUNC(vkstream_new_image, stream)
    VKSTREAM_NEW_FUNC(
        vkstream_new_memory_type,
        stream,
        stream->vk_memory_requirements.memoryTypeBits,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        &memory_type)
    VKSTREAM_NEW_FUNC(vkstream_new_memory_allocate, stream, memory_type)
    VKSTREAM_NEW_FUNC(vkstream_new_image_view, stream)
    VKSTREAM_NEW_FUNC(vkstream_new_memory_export, stream)
    VKSTREAM_NEW_FUNC(vkstream_new_memory_import, stream)

    goto done;
error:
    vkstream_free(stream);
    stream = NULL;
done:
    return stream;
}

void vkstream_free(vkstream *stream)
{
    if (stream != NULL)
    {
        if (stream->vk_image_view != VK_NULL_HANDLE)
        {
            vkDestroyImageView(stream->vk_device, stream->vk_image_view, NULL);
        }
        if (stream->vk_image_memory != VK_NULL_HANDLE)
        {
            vkFreeMemory(stream->vk_device, stream->vk_image_memory, NULL);
        }
        if (stream->vk_image != VK_NULL_HANDLE)
        {
            vkDestroyImage(stream->vk_device, stream->vk_image, NULL);
        }
        if (stream->vk_device != VK_NULL_HANDLE)
        {
            vkDestroyDevice(stream->vk_device, NULL);
        }
        if (stream->cu_context != NULL)
        {
            cuCtxDestroy(stream->cu_context);
        }
        if (stream->vk_instance != VK_NULL_HANDLE)
        {
            vkDestroyInstance(stream->vk_instance, NULL);
        }
        free(stream);
    }
}

BEGIN_FUNCTION(vkstream_new_instance, vkstream *stream)

const char *validation_layers[] = {
    "VK_LAYER_KHRONOS_validation",
};

const char *instance_extensions[] = {
    VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
    VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME,
    VK_KHR_EXTERNAL_MEMORY_CAPABILITIES_EXTENSION_NAME,
    VK_KHR_EXTERNAL_SEMAPHORE_CAPABILITIES_EXTENSION_NAME,
};

const VkApplicationInfo application_info = {
    .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
    .apiVersion = VK_MAKE_API_VERSION(0, 1, 3, 0),
};

const VkInstanceCreateInfo instance_create_info = {
    .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
    .enabledExtensionCount = sizeof(instance_extensions) / sizeof(instance_extensions[0]),
    .ppEnabledExtensionNames = &instance_extensions[0],
    .enabledLayerCount = sizeof(validation_layers) / sizeof(validation_layers[0]),
    .ppEnabledLayerNames = &validation_layers[0],
};

VK_CHECK_RESULT(vkCreateInstance, &instance_create_info, NULL, &stream->vk_instance)

END_FUNCTION

BEGIN_FUNCTION(vkstream_new_physical_device, vkstream *stream, int device_index)

uint32_t physical_device_index;
uint32_t physical_device_count;
VkPhysicalDevice *physical_devices;

physical_device_index = (uint32_t)device_index;
physical_devices = NULL;

VK_CHECK_RESULT(
    vkEnumeratePhysicalDevices,
    stream->vk_instance,
    &physical_device_count,
    NULL)

if (physical_device_index >= physical_device_count)
{
    fprintf(
        stderr,
        "invalid device index(%d) for given device count(%d)\n",
        physical_device_index,
        physical_device_count);
    goto error;
}

physical_devices = CALLOC(1, sizeof(VkPhysicalDevice));
if (physical_devices == NULL)
{
    fputs("failed to allocate physical devices\n", stderr);
    goto error;
}

VK_CHECK_RESULT(
    vkEnumeratePhysicalDevices,
    stream->vk_instance,
    &physical_device_count,
    physical_devices)

stream->vk_physical_device = *(physical_devices + physical_device_index);

END_FUNCTION

BEGIN_FUNCTION(vkstream_new_queue_family, vkstream *stream)

uint32_t queue_family_index;
uint32_t queue_family_count;
VkBool32 graphics_support;
VkQueueFamilyProperties *queue_families;
VkQueueFamilyProperties queue_family;

vkGetPhysicalDeviceQueueFamilyProperties(
    stream->vk_physical_device,
    &queue_family_count,
    NULL);

queue_families = CALLOC(queue_family_count, sizeof(VkQueueFamilyProperties));
if (queue_families == NULL)
{
    fputs("failed to allocate queue families\n", stderr);
    goto error;
}

vkGetPhysicalDeviceQueueFamilyProperties(
    stream->vk_physical_device,
    &queue_family_count,
    queue_families);

graphics_support = VK_FALSE;
for (
    queue_family_index = 0;
    queue_family_index < queue_family_count;
    queue_family_index++)
{
    queue_family = *(queue_families + queue_family_index);
    if (graphics_support == VK_FALSE && (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT))
    {
        graphics_support = VK_TRUE;
        stream->vk_queue_family = queue_family_index;
    }
}

if (graphics_support == VK_FALSE)
{
    fputs("failed to find graphics queue family\n", stderr);
    goto error;
}

END_FUNCTION

BEGIN_FUNCTION(vkstream_new_cuda_context, vkstream *stream, int device_index)

CU_CHECK_RESULT(cuDeviceGet, &stream->cu_device, device_index)

CU_CHECK_RESULT(cuCtxCreate, &stream->cu_context, 0, stream->cu_device)

END_FUNCTION

BEGIN_FUNCTION(vkstream_new_logical_device, vkstream *stream)

const float queue_priority = 1.0;

const char *device_extensions[] = {
    VK_KHR_EXTERNAL_MEMORY_EXTENSION_NAME,
    VK_KHR_EXTERNAL_MEMORY_FD_EXTENSION_NAME,
    VK_KHR_EXTERNAL_SEMAPHORE_EXTENSION_NAME,
    VK_KHR_EXTERNAL_SEMAPHORE_FD_EXTENSION_NAME,
};

const VkDeviceQueueCreateInfo queue_create_info = {
    .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
    .queueCount = 1,
    .pQueuePriorities = &queue_priority,
    .queueFamilyIndex = stream->vk_queue_family,
};

const VkDeviceCreateInfo device_create_info = {
    .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
    .enabledExtensionCount = sizeof(device_extensions) / sizeof(device_extensions[0]),
    .ppEnabledExtensionNames = &device_extensions[0],
    .queueCreateInfoCount = 1,
    .pQueueCreateInfos = &queue_create_info,
};

VK_CHECK_RESULT(
    vkCreateDevice,
    stream->vk_physical_device,
    &device_create_info,
    NULL,
    &stream->vk_device)

vkGetDeviceQueue(
    stream->vk_device,
    stream->vk_queue_family,
    0,
    &stream->vk_queue);

END_FUNCTION

BEGIN_FUNCTION(vkstream_new_image, vkstream *stream)

const VkExternalMemoryImageCreateInfoKHR external_memory_create_info = {
    .sType = VK_STRUCTURE_TYPE_EXTERNAL_MEMORY_IMAGE_CREATE_INFO_KHR,
    .handleTypes = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_FD_BIT_KHR,
};

const VkImageCreateInfo image_create_info = {
    .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
    .imageType = VK_IMAGE_TYPE_2D,
    .format = VK_FORMAT_R8G8B8A8_UNORM,
    .extent = {
        .width = WIDTH,
        .height = HEIGHT,
        .depth = 1,
    },
    .samples = VK_SAMPLE_COUNT_1_BIT,
    .usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
    .mipLevels = 1,
    .arrayLayers = 1,
    .pNext = &external_memory_create_info,
    .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
    .tiling = VK_IMAGE_TILING_OPTIMAL,
    .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
};

VK_CHECK_RESULT(
    vkCreateImage,
    stream->vk_device,
    &image_create_info,
    NULL,
    &stream->vk_image)

vkGetImageMemoryRequirements(
    stream->vk_device,
    stream->vk_image,
    &stream->vk_memory_requirements);

END_FUNCTION

BEGIN_FUNCTION(
    vkstream_new_memory_type,
    vkstream *stream,
    uint32_t type_bits,
    VkMemoryPropertyFlags property_flags,
    uint32_t *type)

VkPhysicalDeviceMemoryProperties physical_device_memory_properties;
VkBool32 found_memory_type;
uint32_t memory_type_index;

vkGetPhysicalDeviceMemoryProperties(stream->vk_physical_device, &physical_device_memory_properties);

found_memory_type = VK_FALSE;
for (memory_type_index = 0; memory_type_index < physical_device_memory_properties.memoryTypeCount; memory_type_index++)
{
    if ((found_memory_type == VK_FALSE) && ((type_bits & 1) == 1))
    {
        if ((physical_device_memory_properties.memoryTypes[memory_type_index].propertyFlags & property_flags) == property_flags)
        {
            *type = memory_type_index;
            found_memory_type = VK_TRUE;
        }
    }
    type_bits >>= 1;
}

if (found_memory_type == VK_FALSE)
{
    goto error;
}

END_FUNCTION

BEGIN_FUNCTION(vkstream_new_memory_allocate, vkstream *stream, uint32_t memory_type)

const VkExportMemoryAllocateInfoKHR export_memory_allocate_info = {
    .sType = VK_STRUCTURE_TYPE_EXPORT_MEMORY_ALLOCATE_INFO_KHR,
    .handleTypes = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_FD_BIT_KHR,
};

const VkMemoryAllocateInfo memory_allocate_info = {
    .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
    .memoryTypeIndex = memory_type,
    .allocationSize = stream->vk_memory_requirements.size,
    .pNext = &export_memory_allocate_info,
};

VK_CHECK_RESULT(
    vkAllocateMemory,
    stream->vk_device,
    &memory_allocate_info,
    NULL,
    &stream->vk_image_memory)

VK_CHECK_RESULT(
    vkBindImageMemory,
    stream->vk_device,
    stream->vk_image,
    stream->vk_image_memory, 0)

END_FUNCTION

BEGIN_FUNCTION(vkstream_new_image_view, vkstream *stream)

const VkImageViewCreateInfo image_view_create_info = {
    .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
    .format = VK_FORMAT_R8G8B8A8_UNORM,
    .viewType = VK_IMAGE_VIEW_TYPE_2D,
    .subresourceRange = {
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .baseArrayLayer = 0,
        .layerCount = 1,
        .baseMipLevel = 0,
        .levelCount = 1,
    },
    .image = stream->vk_image,
};

VK_CHECK_RESULT(
    vkCreateImageView,
    stream->vk_device,
    &image_view_create_info,
    NULL,
    &stream->vk_image_view)

END_FUNCTION

BEGIN_FUNCTION(vkstream_new_memory_export, vkstream *stream)

PFN_vkGetMemoryFdKHR get_memory_fd_func;

const VkMemoryGetFdInfoKHR memory_get_fd_info = {
    .sType = VK_STRUCTURE_TYPE_MEMORY_GET_FD_INFO_KHR,
    .memory = stream->vk_image_memory,
    .handleType = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_FD_BIT_KHR,
};

get_memory_fd_func = (PFN_vkGetMemoryFdKHR)vkGetDeviceProcAddr(
    stream->vk_device,
    "vkGetMemoryFdKHR");

VK_CHECK_RESULT(
    get_memory_fd_func,
    stream->vk_device,
    &memory_get_fd_info,
    &stream->vk_image_memory_handle)

END_FUNCTION

BEGIN_FUNCTION(vkstream_new_memory_import, vkstream *stream)

CUDA_EXTERNAL_MEMORY_HANDLE_DESC memory_handle_desc = {
    .type = CU_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_FD,
    .handle.fd = stream->vk_image_memory_handle,
    .size = WIDTH * HEIGHT * 4,
};

CU_CHECK_RESULT(
    cuImportExternalMemory,
    &stream->cu_image_memory,
    &memory_handle_desc)

END_FUNCTION