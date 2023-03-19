#include "internal.h"

typedef struct _vkm_video_api_s
{
    PFN_vkCreateVideoSessionKHR vkCreateVideoSessionKHR;
    PFN_vkDestroyVideoSessionKHR vkDestroyVideoSessionKHR;
    PFN_vkCmdBeginVideoCodingKHR vkCmdBeginVideoCodingKHR;
    PFN_vkCmdEndVideoCodingKHR vkCmdEndVideoCodingKHR;
    PFN_vkCmdControlVideoCodingKHR vkCmdControlVideoCodingKHR;
    PFN_vkCmdEncodeVideoKHR vkCmdEncodeVideoKHR;
} vkm_video_api_s;

typedef struct _vkm_application_s
{
    VkDevice _device;

    vkm_video_api_s *api;

    uint32_t _image_count;
    VkImage *_images;

    VkCommandPool _encoder_command_pool;
    VkQueue _encoder_queue;
    VkVideoProfileInfoKHR _encoder_profile;
    VkVideoSessionKHR _encoder_session;

} vkm_application_s;

#define VKM_APPLICATION(HANDLE) ((vkm_application_s *)HANDLE)

static int vkm_application_create_device(
    vkm_application application,
    const vkm_application_parameters *application_parameters);

static int vkm_application_create_images(
    vkm_application application,
    const vkm_application_parameters *application_parameters);

static int vkm_application_create_encoder(
    vkm_application application,
    const vkm_application_parameters *application_parameters);

int vkm_application_create(
    const vkm_application_parameters *application_parameters,
    vkm_application *application)
{
    VKM_BEGIN_FUNCTION
    VKM_ASSERT(
        vkm_application_create_device(
            *application,
            application_parameters) == VKM_SUCCESS,
        "failed to create device")
    VKM_END_FUNCTION
}

void vkm_application_destroy(
    vkm_application application)
{
}

int vkm_application_start(
    vkm_application application)
{
    VKM_BEGIN_FUNCTION

    VKM_END_FUNCTION
}

int vkm_application_get_id(vkm_application application,
                           const char **id)
{
    VKM_BEGIN_FUNCTION

    VKM_END_FUNCTION
}

int vkm_application_get_extent(
    vkm_application application,
    VkExtent2D *extent)
{
    VKM_BEGIN_FUNCTION

    VKM_END_FUNCTION
}

int vkm_application_get_framerate(
    vkm_application application,
    uint32_t *fps_num,
    uint32_t *fps_den)
{
    VKM_BEGIN_FUNCTION

    VKM_END_FUNCTION
}

int vkm_application_stop(
    vkm_application application)
{
    VKM_BEGIN_FUNCTION

    VKM_END_FUNCTION
}
int vkm_application_get_images(
    vkm_application application,
    VkImage **images,
    uint32_t *image_count)
{
    VKM_BEGIN_FUNCTION

    VKM_END_FUNCTION
}
int vkm_application_acquire_image(
    vkm_application application,
    uint32_t *image_index,
    VkSemaphore *wait_semaphore,
    VkSemaphore *signal_semaphore)
{
    VKM_BEGIN_FUNCTION

    VKM_END_FUNCTION
}
int vkm_application_create_encoder(
    vkm_application application,
    const vkm_application_parameters *application_parameters)
{
    VKM_BEGIN_FUNCTION

    VKM_END_FUNCTION
}

int vkm_application_create_device(
    vkm_application application,
    const vkm_application_parameters *application_parameters)
{
    VKM_BEGIN_FUNCTION

    VKM_END_FUNCTION
}