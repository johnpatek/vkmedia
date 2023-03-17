#include "internal.h"
#include <libavutil/buffer.h>
typedef struct _vkm_application_s
{
    VkDevice _device;
    VkQueue _encoder_queue;

    uint32_t _image_count;
    VkImage *_images;

    VkVideoSessionKHR _session

} vkm_application_s;

#define VKM_APPLICATION(HANDLE) ((vkm_application_s *)HANDLE)

static int vkm_application_create_device(
    vkm_application application, 
    const vkm_application_parameters *application_parameters);

static int vkm_application_create_images(
    vkm_application application, 
    const vkm_application_parameters *application_parameters);


VKM_BEGIN_FUNCTION(
    vkm_application_create(
        const vkm_application_parameters *application_parameters,
        vkm_application *application))

VKM_END_FUNCTION

void vkm_application_destroy(
    vkm_application application)
{
}

VKM_BEGIN_FUNCTION(
    vkm_application_start(
        vkm_application application))

VKM_END_FUNCTION

VKM_BEGIN_FUNCTION(
    vkm_application_get_id(
        vkm_application application,
        const char **id))

VKM_END_FUNCTION

VKM_BEGIN_FUNCTION(
    vkm_application_get_extent(
        vkm_application application,
        VkExtent2D *extent))

VKM_END_FUNCTION

VKM_BEGIN_FUNCTION(
    vkm_application_get_framerate(
        vkm_application application,
        uint32_t *fps_num,
        uint32_t *fps_den))

VKM_END_FUNCTION

VKM_BEGIN_FUNCTION(
    vkm_application_stop(
        vkm_application application))

VKM_END_FUNCTION

VKM_BEGIN_FUNCTION(
    vkm_application_get_images(
        vkm_application application,
        VkImage **images,
        uint32_t *image_count))

VKM_END_FUNCTION

VKM_BEGIN_FUNCTION(
    vkm_application_acquire_image(
        vkm_application application,
        uint32_t *image_index,
        VkSemaphore *wait_semaphore,
        VkSemaphore *signal_semaphore))

VKM_END_FUNCTION