/* GStreamer
 * Copyright (C) 2008 Wim Taymans <wim.taymans at gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include <cuda.h>
#include <stdio.h>
#include <stdlib.h>

#include <gst/gst.h>
#include <gst/cuda/gstcuda.h>
#include <gst/app/app.h>
#include <gst/rtsp-server/rtsp-server.h>

typedef struct
{
    
    GstClockTime timestamp;
    GstBufferPool *buffer_pool;
    uint32_t pixel_storage;
    GThread *push_buffer_thread;
    GstElement *appsrc;
} MyContext;

typedef struct
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} rgba_pixel;

static void draw(GstBuffer *buffer, int size, MyContext *ctx)
{
    GstMapInfo info;
    uint8_t new_value = (uint8_t)(rand() % 256);
    rgba_pixel *pixel = (rgba_pixel *)&ctx->pixel_storage;
    uint32_t *buffer_pixels;
    switch (rand() % 3)
    {
    case 0:
        pixel->r = new_value;
        break;
    case 1:
        pixel->g = new_value;
        break;
    case 2:
        pixel->b = new_value;
        break;
    }
    gst_buffer_map(buffer, &info, GST_MAP_WRITE);
    buffer_pixels = (uint32_t *)info.data;
    for (int index = 0; index < size; index++)
    {
        buffer_pixels[index] = ctx->pixel_storage;
    }
    gst_buffer_unmap(buffer, &info);
}

static void push_data(MyContext *ctx)
{
    GstBuffer *buffer;
    const int size = 385 * 288;
    GstFlowReturn ret;

    while (gst_buffer_pool_acquire_buffer(ctx->buffer_pool, &buffer, NULL) == GST_FLOW_OK)
    {
        if (buffer != NULL)
        {
            draw(buffer, size, ctx);

            /* increment the timestamp every 1/2 second */
            GST_BUFFER_PTS(buffer) = ctx->timestamp;
            GST_BUFFER_DURATION(buffer) = gst_util_uint64_scale_int(1, GST_SECOND, 3);
            ctx->timestamp += GST_BUFFER_DURATION(buffer);

            gst_app_src_push_buffer(GST_APP_SRC(ctx->appsrc), buffer);
        }
    }
}

/* called when a new media pipeline is constructed. We can query the
 * pipeline and configure our appsrc */
static void
media_configure(GstRTSPMediaFactory *factory, GstRTSPMedia *media,
                gpointer user_data)
{
    srand(time(0));
    GstElement *element, *appsrc;
    MyContext *ctx;
    rgba_pixel *init_pixel;

    const int size = 385 * 288 * 4;

    /* get the element used for providing the streams of the media */
    element = gst_rtsp_media_get_element(media);

    /* get our appsrc, we named it 'mysrc' with the name property */
    appsrc = gst_bin_get_by_name_recurse_up(GST_BIN(element), "mysrc");

    /* this instructs appsrc that we will be dealing with timed buffer */
    gst_util_set_object_arg(G_OBJECT(appsrc), "format", "time");
    /* configure the caps of the video */
    g_object_set(G_OBJECT(appsrc), "caps",
                 gst_caps_new_simple("video/x-raw",
                                     "format", G_TYPE_STRING, "RGBA",
                                     "width", G_TYPE_INT, 384,
                                     "height", G_TYPE_INT, 288,
                                     "framerate", GST_TYPE_FRACTION, 3, 1, NULL),
                 "block", TRUE,
                 "emit-signals",FALSE,
                 NULL);

    ctx = g_new0(MyContext, 1);
    ctx->timestamp = 0;
    init_pixel = (rgba_pixel *)&ctx->pixel_storage;
    init_pixel->r = 0;
    init_pixel->g = 0;
    init_pixel->b = 0;
    init_pixel->a = 200;
    ctx->appsrc = appsrc;
    
    GstStructure *config;
    ctx->buffer_pool = gst_buffer_pool_new();
    config = gst_buffer_pool_get_config(ctx->buffer_pool);
    gst_buffer_pool_config_set_params(config, NULL, size, 4, 0);
    gst_buffer_pool_set_config(ctx->buffer_pool, config);
    gst_buffer_pool_set_active(ctx->buffer_pool, TRUE);
    /* make sure ther datais freed when the media is gone */
    g_object_set_data_full(G_OBJECT(media), "my-extra-data", ctx,
                           (GDestroyNotify)g_free);

    /* install the callback that will be called when a buffer is needed */
    // g_signal_connect(appsrc, "need-data", (GCallback)need_data, ctx);

    ctx->push_buffer_thread = g_thread_new("push_buffer", (GThreadFunc)push_data, ctx);
    gst_object_unref(element);
}

int main(int argc, char *argv[])
{
    GMainLoop *loop;
    GstRTSPServer *server;
    GstRTSPMountPoints *mounts;
    GstRTSPMediaFactory *factory;

    gst_init(&argc, &argv);

    loop = g_main_loop_new(NULL, FALSE);

    /* create a server instance */
    server = gst_rtsp_server_new();

    /* get the mount points for this server, every server has a default object
     * that be used to map uri mount points to media factories */
    mounts = gst_rtsp_server_get_mount_points(server);

    /* make a media factory for a test stream. The default media factory can use
     * gst-launch syntax to create pipelines.
     * any launch line works as long as it contains elements named pay%d. Each
     * element with pay%d names will be a stream */
    factory = gst_rtsp_media_factory_new();
    gst_rtsp_media_factory_set_launch(factory,
                                      "( appsrc name=mysrc is-live=true ! queue ! nvh264enc ! rtph264pay name=pay0 pt=96 )");
    gst_rtsp_media_factory_set_enable_rtcp(factory, FALSE);
    gst_rtsp_media_factory_set_do_retransmission(factory, FALSE);
    gst_rtsp_media_factory_set_shared(factory, TRUE);
    gst_rtsp_media_factory_set_suspend_mode(factory, GST_RTSP_SUSPEND_MODE_NONE);
    gst_rtsp_media_factory_set_dscp_qos(factory, 24);
    /* notify when our media is ready, This is called whenever someone asks for
     * the media and a new pipeline with our appsrc is created */
    g_signal_connect(factory, "media-configure", (GCallback)media_configure,
                     NULL);

    /* attach the test factory to the /test url */
    gst_rtsp_mount_points_add_factory(mounts, "/test", factory);

    /* don't need the ref to the mounts anymore */
    g_object_unref(mounts);

    /* attach the server to the default maincontext */
    gst_rtsp_server_attach(server, NULL);

    /* start serving */
    g_print("stream ready at rtsp://127.0.0.1:8554/test\n");
    g_main_loop_run(loop);

    return 0;
}
