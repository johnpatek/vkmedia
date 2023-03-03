#include "internal.h"

VKM_BEGIN_FUNCTION(vkm_enumerate_instance_extentions, const char **extensions, uint32_t *extension_count)
const char *instance_extensions[] = {
    VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
    VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME,
    VK_KHR_EXTERNAL_MEMORY_CAPABILITIES_EXTENSION_NAME,
    VK_KHR_EXTERNAL_SEMAPHORE_CAPABILITIES_EXTENSION_NAME,
};
if (extensions != NULL)
{
    memcpy(&extensions[0], &instance_extensions[0], sizeof(instance_extensions) / sizeof(instance_extensions[0]));
}
else if (extension_count != NULL)
{
    *extension_count = sizeof(instance_extensions) / sizeof(instance_extensions[0]);
}
else
{
    goto error;
}
VKM_END_FUNCTION

VKM_BEGIN_FUNCTION(vkm_enumerate_device_extentions, const char **extensions, uint32_t *extension_count)
const char *device_extensions[] = {
    VK_KHR_EXTERNAL_MEMORY_EXTENSION_NAME,
    VK_KHR_EXTERNAL_MEMORY_FD_EXTENSION_NAME,
#ifdef _WIN32
#warning "Windows is not currently supported"
    VK_KHR_EXTERNAL_MEMORY_WIN32_EXTENSION_NAME,
    VK_KHR_EXTERNAL_SEMAPHORE_WIN32_EXTENSION_NAME
#else
    VK_KHR_EXTERNAL_MEMORY_FD_EXTENSION_NAME,
    VK_KHR_EXTERNAL_SEMAPHORE_FD_EXTENSION_NAME
#endif
};
if (extensions != NULL)
{
    memcpy(&extensions[0], &device_extensions[0], sizeof(device_extensions) / sizeof(device_extensions[0]));
}
else if (extension_count != NULL)
{
    *extension_count = sizeof(device_extensions) / sizeof(device_extensions[0]);
}
else
{
    goto error;
}
VKM_END_FUNCTION
