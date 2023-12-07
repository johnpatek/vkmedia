#include <vulkan/vulkan.h>
#include <cuda.h>
#include <stdio.h>

static void print_device_info(VkPhysicalDevice *devices, uint32_t index)
{
    char name[100];
    CUuuid uuid;
    CUdevice device;
    VkPhysicalDeviceProperties2 properties = {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2};
    VkPhysicalDeviceIDProperties id_properties = {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ID_PROPERTIES};
    properties.pNext = &id_properties;
    vkGetPhysicalDeviceProperties2(*(devices + index), &properties);
    uint16_t *vi = (uint16_t*)&id_properties.deviceUUID;
    printf("%d\n", *vi);
    cuDeviceGet(&device, index);
    cuDeviceGetName(name,100,device);
    
    cuDeviceGetUuid(&uuid,device);
    uint16_t *ui = (uint16_t*)&uuid;
    printf("%d\n", *ui);
}

static void print_devices(VkInstance instance)
{
    uint32_t count;
    VkPhysicalDevice *data;
    uint32_t index;
    vkEnumeratePhysicalDevices(instance, &count, NULL);
    data = calloc(count, sizeof(VkPhysicalDevice));
    vkEnumeratePhysicalDevices(instance, &count, data);
    for (index = 0; index < count; index++)
    {
        print_device_info(data, index);
    }
    free(data);
}

int main()
{
    cuInit(0);
    const char *validation_layers[] = {
        "VK_LAYER_KHRONOS_validation",
    };
    VkApplicationInfo app_info = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .apiVersion = VK_MAKE_API_VERSION(0, 1, 2, 0),
    };
    VkInstanceCreateInfo ci = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .enabledLayerCount = 1,
        .ppEnabledLayerNames = &validation_layers[0],
        .pApplicationInfo = &app_info,
    };
    VkInstance instance;
    VkResult result = vkCreateInstance(&ci, NULL, &instance);
    if (result != VK_SUCCESS)
    {
        printf("failed(%d)\n", result);
    }
    print_devices(instance);

    vkDestroyInstance(instance, NULL);
    return 0;
}