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

#include <assert.h>

#include <stdio.h>

#include <gst/gst.h>
#include <gst/app/app.h>
#include <gst/video/video.h>
#include <gst/rtsp-server/rtsp-server.h>

typedef struct
{
    GstClock *clock;
    GstElement *generator_pipe;
    GstElement *vid_appsink;
    GstElement *vid_appsrc;
    GThread *thread;
} MyContext;

static void push_video_thread(MyContext *ctx)
{
    GstSample *sample;
    GstBuffer *buffer;
    GstSegment *segment;
    GstFlowReturn ret;
    GstClockTime pts, dts;
    
    assert(gst_app_src_get_emit_signals(GST_APP_SRC(ctx->vid_appsrc)) == FALSE);

    while (!gst_app_sink_is_eos(GST_APP_SINK(ctx->vid_appsink)))
    {
        sample = gst_app_sink_pull_sample(GST_APP_SINK(ctx->vid_appsink));
        if (G_UNLIKELY(sample == NULL))
        {
            continue;
        }
        buffer = gst_sample_get_buffer(sample);
        if (G_UNLIKELY(buffer == NULL))
        {
            continue;
        }
        segment = gst_sample_get_segment(sample);
        if (G_UNLIKELY(segment == NULL))
        {
            continue;
        }
        pts = GST_BUFFER_PTS(buffer);

        dts = GST_BUFFER_DTS(buffer);

        /* Make writable so we can adjust the timestamps */
        buffer = gst_buffer_copy(buffer);
        GST_BUFFER_PTS(buffer) = gst_segment_to_running_time(segment, GST_FORMAT_TIME, pts);
        GST_BUFFER_DTS(buffer) = gst_segment_to_running_time(segment, GST_FORMAT_TIME, dts);

        gst_app_src_push_buffer(GST_APP_SRC(ctx->vid_appsrc), buffer);
        gst_sample_unref(sample);
    }
}

static void
ctx_free(MyContext *ctx)
{
    gst_element_set_state(ctx->generator_pipe, GST_STATE_NULL);

    gst_object_unref(ctx->generator_pipe);
    gst_object_unref(ctx->vid_appsrc);
    gst_object_unref(ctx->vid_appsink);
}

static void initialize_pipeline(MyContext *ctx)
{
    GstElement *element, *appsrc, *appsink;
    GstCaps *caps;

    ctx->generator_pipe =
        gst_parse_launch(
            "videotestsrc is-live=true ! queue ! nvh264enc preset=5 zerolatency=true ! appsink name=vid max-buffers=2 drop=true ",
            NULL);

    caps = gst_caps_new_simple(
        "video/x-h264",
        "stream-format", G_TYPE_STRING, "byte-stream",
        "alignment", G_TYPE_STRING, "au",
        "width", G_TYPE_INT, 1920, "height", G_TYPE_INT, 1080,
        "framerate", GST_TYPE_FRACTION, 30, 1, NULL);

    ctx->vid_appsink = appsink =
        gst_bin_get_by_name(GST_BIN(ctx->generator_pipe), "vid");

    g_object_set(G_OBJECT(appsink), "caps", caps, NULL);

    gst_app_sink_set_emit_signals(GST_APP_SINK(appsink), FALSE);

    gst_caps_unref(caps);

    if (gst_element_set_state(ctx->generator_pipe, GST_STATE_PAUSED) == GST_STATE_CHANGE_ASYNC)
    {
        puts("waiting...");
        gst_element_get_state(
            ctx->generator_pipe,
            NULL,
            NULL,
            GST_CLOCK_TIME_NONE);
        puts("state changed");
    }
}

/* called when a new media pipeline is constructed. We can query the
 * pipeline and configure our appsrc */
static void media_configure(
    GstRTSPMediaFactory *factory,
    GstRTSPMedia *media,
    MyContext *ctx)
{
    GstElement *element, *appsrc, *appsink;
    GstCaps *caps;

    /* This pipeline generates H264 video and PCM audio. The appsinks are kept small so that if delivery is slow,
     * encoded buffers are dropped as needed. There's slightly more buffers (32) allowed for audio */

    /* make sure the data is freed when the media is gone */
    g_object_set_data_full(G_OBJECT(media), "rtsp-extra-data", ctx,
                           (GDestroyNotify)ctx_free);

    /* get the element (bin) used for providing the streams of the media */
    element = gst_rtsp_media_get_element(media);

    /* Find the 2 app sources (video / audio), and configure them, connect to the
     * signals to request data */
    /* configure the caps of the video */
    caps = gst_caps_new_simple("video/x-h264",
                               "stream-format", G_TYPE_STRING, "byte-stream",
                               "alignment", G_TYPE_STRING, "au",
                               "width", G_TYPE_INT, 1920, "height", G_TYPE_INT, 1080,
                               "framerate", GST_TYPE_FRACTION, 30, 1, NULL);
    ctx->vid_appsrc = appsrc =
        gst_bin_get_by_name_recurse_up(GST_BIN(element), "videosrc");

    gst_util_set_object_arg(G_OBJECT(appsrc), "format", "time");
    g_object_set(G_OBJECT(appsrc), "caps", caps, NULL);

    gst_app_src_set_stream_type(GST_APP_SRC(appsrc), GST_APP_STREAM_TYPE_STREAM);
    /* install the callback that will be called when a buffer is needed */
    gst_app_src_set_emit_signals(GST_APP_SRC(appsrc), FALSE);
    // g_signal_connect(appsrc, "need-data", (GCallback)push_video_data, ctx);

    gst_caps_unref(caps);

    ctx->thread = g_thread_new("push", (GThreadFunc)&push_video_thread, ctx);

    if (gst_element_set_state(ctx->generator_pipe, GST_STATE_PLAYING) == GST_STATE_CHANGE_ASYNC)
    {
        puts("waiting...");
        gst_element_get_state(
            ctx->generator_pipe,
            NULL,
            NULL,
            GST_CLOCK_TIME_NONE);
        puts("state changed");
    }

    gst_object_unref(element);
}

int main(int argc, char *argv[])
{
    MyContext ctx;
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
                                      "( appsrc name=videosrc ! rtph264pay name=pay0 pt=96 )");

    initialize_pipeline(&ctx);

    /* notify when our media is ready, This is called whenever someone asks for
     * the media and a new pipeline with our appsrc is created */
    g_signal_connect(
        factory,
        "media-configure",
        (GCallback)media_configure,
        &ctx);

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
