#include "internal.h"

typedef struct _vkm_session_s
{
    vkm_application *application;
    GstBufferPool *buffer_pool;
} vkm_session_s;

typedef struct _vkm_server_s
{
    GstRTSPServer *gst_rtsp_server;
    guint gst_server_source;
    gboolean active;
} vkm_server_s;

#define VKM_SERVER(HANDLE) ((vkm_server_s *)HANDLE)

static void vkm_server_configure(
    GstRTSPMediaFactory *factory,
    GstRTSPMedia *media,
    vkm_application application);

int vkm_server_create(
    const vkm_server_parameters *server_parameters,
    vkm_server *server)
{
    VKM_BEGIN_FUNCTION

    VKM_ASSERT(server != NULL, "server address cannot be NULL")

    *server = calloc(1, sizeof(vkm_server_s));
    VKM_ASSERT(*server != NULL, "failed to allocate server")

    VKM_SERVER(*server)->gst_rtsp_server = gst_rtsp_server_new();
    VKM_ASSERT(
        VKM_SERVER(*server)->gst_rtsp_server != NULL,
        "failed to allocate RTSP server")

    VKM_SERVER(*server)->active = FALSE;

    VKM_END_FUNCTION
}

void vkm_server_destroy(vkm_server server)
{
}

int vkm_server_start(vkm_server server)
{
    VKM_BEGIN_FUNCTION

    VKM_SERVER(server)->gst_server_source = gst_rtsp_server_attach(VKM_SERVER(server)->gst_rtsp_server, NULL);

    VKM_ASSERT(
        VKM_SERVER(server)->gst_server_source > 0,
        "failed to attach RTSP server")

    VKM_END_FUNCTION
}

int vkm_server_stop(vkm_server server)
{
    VKM_BEGIN_FUNCTION

    VKM_ASSERT(
        g_source_remove(
            VKM_SERVER(server)->gst_server_source) == TRUE,
        "failed to detach RTSP server")

    VKM_SERVER(server)->active = FALSE;

    VKM_END_FUNCTION
}

int vkm_server_mount(vkm_server server, const char *path, vkm_application application)
{
    VKM_BEGIN_FUNCTION

    GstRTSPMediaFactory *factory;
    const char *application_id;
    gchar *factory_launch_string;
    GstRTSPMountPoints *mount_points;

    factory = gst_rtsp_media_factory_new();
    VKM_ASSERT(factory != NULL, "failed to allocate media factory")

    VKM_ASSERT(
        vkm_application_get_id(
            application,
            &application_id) == VKM_SUCCESS,
        "failed to get application ID")

    factory_launch_string = g_strdup_printf(
        "( appsrc name=%s ! rtph264pay name=pay0 pt=96 )",
        application_id);

    gst_rtsp_media_factory_set_launch(factory, factory_launch_string);
    g_object_unref(factory_launch_string);

    mount_points = gst_rtsp_server_get_mount_points(
        VKM_SERVER(server)->gst_rtsp_server);
    VKM_ASSERT(factory != NULL, "failed to get server mount points")

    gst_rtsp_mount_points_add_factory(mount_points, path, factory);
    g_object_unref(mount_points);

    VKM_END_FUNCTION
}

int(vkm_server_unmount(vkm_server server, const char *path))
{
    VKM_BEGIN_FUNCTION

    VKM_END_FUNCTION
}

static void vkm_server_configure(
    GstRTSPMediaFactory *factory,
    GstRTSPMedia *media,
    vkm_application application)
{
    const char *application_id;
    GstElement *media_element;
    GstElement *appsrc_element;
    VkExtent2D extent;
    uint32_t fps_num;
    uint32_t fps_den;
    GstCaps *appsrc_caps;

    assert(
        vkm_application_get_id(
            application,
            &application_id) == VKM_SUCCESS);

    media_element = gst_rtsp_media_get_element(media);

    appsrc_element = gst_bin_get_by_name_recurse_up(
        GST_BIN(media_element),
        application_id);

    assert(
        vkm_application_get_extent(
            application,
            &extent) == VKM_SUCCESS);

    assert(
        vkm_application_get_framerate(
            application,
            &fps_num,
            &fps_den) == VKM_SUCCESS);

    appsrc_caps = gst_caps_new_simple(
        "video/x-h264",
        "stream-format", G_TYPE_STRING, "byte-stream",
        "alignment", G_TYPE_STRING, "au",
        "width", G_TYPE_INT, extent.width, "height", G_TYPE_INT, extent.height,
        "framerate", GST_TYPE_FRACTION, fps_num, fps_den, NULL);

    gst_util_set_object_arg(G_OBJECT(appsrc_element), "format", "time");
    gst_app_src_set_emit_signals(GST_APP_SRC(appsrc_element), FALSE);
}
