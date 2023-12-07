#include "example.h"

// create helper functions
static void vkm_example_create_instance(vkm_example *example);
static void vkm_example_create_devices(vkm_example *example, uint32_t device_index);
static void vkm_example_create_image_views(vkm_example *example);
static void vkm_example_create_shaders(vkm_example *example, const char *shader_directory);
static void vkm_example_create_framebuffers(vkm_example *example);

// loop helper functions
static void vkm_example_loop_record(vkm_example *example, VkFramebuffer framebuffer, VkCommandBuffer command_buffer);

void vkm_example_create(
    const vkm_example_parameters *example_parameters, vkm_example **example)
{
    vkm_application_parameters application_parameters = {
        .width = example_parameters->width,
        .height = example_parameters->height,
        .fps_num = example_parameters->fps,
        .fps_den = 1,
        .queue_family_count = 1,
    };

    *example = calloc(1, sizeof(vkm_example));
    assert(example != NULL);

    (*example)->screen_size.width = example_parameters->width;
    (*example)->screen_size.height = example_parameters->height;

    assert(vkm_initialize() == VKM_SUCCESS);

    vkm_example_create_instance(*example);
    vkm_example_create_devices(*example, example_parameters->device_index);

    application_parameters.queue_families = &(*example)->vk_queue_index;
    assert(
        vkm_application_create(
            &application_parameters,
            &(*example)->application) == VKM_SUCCESS);
}

void vkm_example_loop(vkm_example *example)
{
    const VkPipelineStageFlags wait_dest_stage_mask = VK_PIPELINE_STAGE_TRANSFER_BIT;
    VkSubmitInfo submit_info = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pWaitDstStageMask = &wait_dest_stage_mask,
        .commandBufferCount = 1,
        .waitSemaphoreCount = 1,
        .signalSemaphoreCount = 1,
    };
    uint32_t image_index;
    VkFence fence;
    VkCommandBuffer command_buffer;
    VkFramebuffer framebuffer;

    VkSemaphore wait_semaphore;
    VkSemaphore signal_semaphore;

    example->running = VK_TRUE;

    while (example->running == VK_TRUE)
    {
        vkm_application_acquire_image(
            example->application,
            &image_index,
            &wait_semaphore,
            &signal_semaphore);

        fence = *(example->vk_fences + image_index);
        command_buffer = *(example->vk_command_buffers + image_index);
        framebuffer = *(example->vk_framebuffers + image_index);

        vkWaitForFences(example->vk_device, 1, &fence, VK_TRUE, UINT64_MAX);
        vkResetFences(example->vk_device, 1, &fence);

        vkm_example_loop_record(example, framebuffer, command_buffer);

        submit_info.pCommandBuffers = &command_buffer;
        submit_info.pWaitSemaphores = &wait_semaphore;
        submit_info.pSignalSemaphores = &signal_semaphore;
    }
}

void vkm_example_quit(vkm_example *example)
{
    example->running = VK_FALSE;
}

void vkm_example_destroy(vkm_example *example)
{
}

void vkm_example_create_instance(vkm_example *example)
{
    const char *validation_layers[] = {
        "VK_LAYER_KHRONOS_validation",
    };
    VkApplicationInfo application_info = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .apiVersion = VK_MAKE_API_VERSION(0, 1, 3, 0),
    };
    VkInstanceCreateInfo instance_create_info = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &application_info,
        .enabledLayerCount = sizeof(validation_layers) / sizeof(validation_layers[0]),
        .ppEnabledLayerNames = &validation_layers[0],
    };
    uint32_t instance_extension_count;
    const char **instance_extensions;

    assert(vkm_enumerate_instance_extentions(NULL, &instance_extension_count) == VKM_SUCCESS);

    instance_extensions = calloc(instance_extension_count, sizeof(char *));
    assert(instance_extensions != NULL);

    assert(vkm_enumerate_instance_extentions(instance_extensions, &instance_extension_count) == VKM_SUCCESS);

    instance_create_info.enabledExtensionCount = instance_extension_count;
    instance_create_info.ppEnabledExtensionNames = instance_extensions;
    assert(vkCreateInstance(&instance_create_info, NULL, &example->vk_instance) == VK_SUCCESS);

    free(instance_extensions);
}

void vkm_example_create_devices(vkm_example *example, uint32_t device_index)
{
    VkDeviceCreateInfo device_create_info = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
    };
    uint32_t device_extension_count;
    const char **device_extensions;

    assert(example->vk_instance != VK_NULL_HANDLE);

    assert(
        vkm_get_physical_device(
            example->vk_instance,
            0,
            &example->vk_physical_device) == VKM_SUCCESS);

    assert(
        vkm_find_queue_family(
            example->vk_physical_device,
            VK_QUEUE_GRAPHICS_BIT,
            &example->vk_queue_index) == VKM_SUCCESS);

    assert(
        vkm_enumerate_device_extentions(
            NULL,
            &device_extension_count) == VKM_SUCCESS);

    device_extensions = calloc(device_extension_count, sizeof(char *));

    assert(
        vkm_enumerate_device_extentions(
            device_extensions,
            &device_extension_count) == VKM_SUCCESS);
}

void vkm_example_create_image_views(vkm_example *example)
{
    VkImageViewCreateInfo image_view_create_info = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = VK_FORMAT_R8G8B8A8_UNORM,
        .subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1,
        },
    };
    uint32_t image_view_index;
    for (image_view_index = 0; image_view_index < example->image_count; image_view_index++)
    {
        image_view_create_info.image = *(example->vk_images + image_view_index);
    }
}

void vkm_example_create_shaders(vkm_example *example, const char *shader_directory)
{
    char filename[256];
    sprintf(filename, "%s/%s", shader_directory, "vert.spv");
    example->vk_vertex_shader = load_shader_module(example->vk_device, filename);
    sprintf(filename, "%s/%s", shader_directory, "frag.spv");
    example->vk_fragment_shader = load_shader_module(example->vk_device, filename);
}

void vkm_example_create_framebuffers(vkm_example *example)
{
    
}

void vkm_example_loop_record(vkm_example *example, VkFramebuffer framebuffer, VkCommandBuffer command_buffer)
{
    const VkClearValue clear_color = {
        {
            {0.0f, 0.0f, 0.0f, 1.0f},
        },
    };
    VkCommandBufferBeginInfo command_buffer_begin_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
        .pNext = NULL,
        .pInheritanceInfo = NULL,
    };
    const VkRenderPassBeginInfo render_pass_begin_info = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .clearValueCount = 1,
        .pClearValues = &clear_color,
        .framebuffer = framebuffer,
        .renderPass = example->vk_render_pass,
        .renderArea = {
            .offset = 0,
            .extent = example->screen_size,
        },
    };
    const VkViewport viewport = {
        .x = 0.0f,
        .y = 0.0f,
        .width = (float)example->screen_size.width,
        .height = (float)example->screen_size.height,
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
    };
    const VkRect2D scissor = {
        .offset = {0, 0},
        .extent = example->screen_size,
    };
    assert(vkResetCommandBuffer(command_buffer, 0) == VK_SUCCESS);
    assert(vkBeginCommandBuffer(command_buffer, &command_buffer_begin_info) == VK_SUCCESS);
    vkCmdBeginRenderPass(command_buffer, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, example->vk_graphics_pipeline);
    vkCmdSetViewport(command_buffer, 0, 1, &viewport);
    vkCmdSetScissor(command_buffer, 0, 1, &scissor);
    vkCmdDraw(command_buffer, 3, 1, 0, 0);
    vkCmdEndRenderPass(command_buffer);
    assert(vkEndCommandBuffer(command_buffer) == VK_SUCCESS);
}