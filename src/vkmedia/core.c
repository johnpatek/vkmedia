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

#include "internal.h"

int vkm_initialize()
{
    VKM_BEGIN_FUNCTION
    
    gst_init(NULL, NULL);

    VKM_END_FUNCTION
}

void vkm_shutdown()
{
    gst_deinit();
}

int vkm_enumerate_instance_extentions(const char **extensions, uint32_t *extension_count)
{
    VKM_BEGIN_FUNCTION
    const char *instance_extensions[] = {
        VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
        VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME,
        VK_KHR_EXTERNAL_MEMORY_CAPABILITIES_EXTENSION_NAME,
        VK_KHR_EXTERNAL_SEMAPHORE_CAPABILITIES_EXTENSION_NAME,
    };
    uint32_t index;
    if (extensions != NULL)
    {
        for (index = 0; index < *extension_count; index++)
        {
            *(extensions + index) = *(instance_extensions + index);
        }
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
}

int vkm_enumerate_device_extentions(
    const char **extensions,
    uint32_t *extension_count)
{
    VKM_BEGIN_FUNCTION
    const char *device_extensions[] = {
        VK_EXT_YCBCR_2PLANE_444_FORMATS_EXTENSION_NAME,
        VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME,
        VK_KHR_VIDEO_QUEUE_EXTENSION_NAME,
        VK_KHR_VIDEO_ENCODE_QUEUE_EXTENSION_NAME,
        VK_EXT_VIDEO_ENCODE_H264_EXTENSION_NAME,
    };
    uint32_t index;
    if (extensions != NULL)
    {
        for (index = 0; index < *extension_count; index++)
        {
            *(extensions + index) = *(device_extensions + index);
        }
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
}

int vkm_find_queue_family(
    VkPhysicalDevice physical_device,
    VkQueueFlagBits required_bits,
    uint32_t *index)
{
    VKM_BEGIN_FUNCTION

    uint32_t queue_family_property_index;
    uint32_t queue_family_property_count;
    VkQueueFamilyProperties *queue_family_properties;
    VkBool32 found_queue_family;

    vkGetPhysicalDeviceQueueFamilyProperties(
        physical_device,
        &queue_family_property_count,
        NULL);

    queue_family_properties = VKM_CALLOC(queue_family_property_count, sizeof(VkQueueFamilyProperties));
    VKM_ASSERT(queue_family_properties != NULL, "failed to allocate queue family properties");

    vkGetPhysicalDeviceQueueFamilyProperties(
        physical_device,
        &queue_family_property_count,
        queue_family_properties);

    found_queue_family = VK_FALSE;
    for (queue_family_property_index = 0; queue_family_property_index < queue_family_property_count; queue_family_property_index++)
    {
        if ((found_queue_family == VK_FALSE) && ((queue_family_properties + queue_family_property_index)->queueFlags & required_bits))
        {
            found_queue_family = VK_TRUE;
            *index = queue_family_property_index;
        }
    }

    VKM_ASSERT(found_queue_family == VK_TRUE, "failed to find suitable queue family");
    VKM_END_FUNCTION
}

int vkm_get_physical_device(
    VkInstance vulkan_instance,
    int index,
    VkPhysicalDevice *vulkan_physical_device)
{
    VKM_BEGIN_FUNCTION
    uint32_t physical_device_count;
    VkPhysicalDevice *physical_devices;

    VK_CHECK_RESULT(vkEnumeratePhysicalDevices(vulkan_instance, &physical_device_count, NULL))

    VKM_ASSERT(index < physical_device_count, "device index out of bounds")

    physical_devices = VKM_CALLOC(physical_device_count, sizeof(VkPhysicalDevice));

    VKM_ASSERT(physical_devices != NULL, "failed to allocate physical devices")

    VK_CHECK_RESULT(vkEnumeratePhysicalDevices(vulkan_instance, &physical_device_count, physical_devices))

    *vulkan_physical_device = *(physical_devices + index);

    VKM_END_FUNCTION
}