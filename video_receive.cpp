#include <gst/gst.h>

GstElement *pipeline;
GstElement *udp_source;
GstElement *src_cap;
GstElement *rtp_buffer;
GstElement *depayloader;
GstElement *parser;
GstElement *omx_decoder;
GstElement *video_converter;
GstElement *video_crop;
GstElement *sink;

GstBus *bus;
GstStateChangeReturn ret;

int create_pipeline() {
    pipeline = gst_pipeline_new("video_receive");
    udp_source = gst_element_factory_make("udpsrc", "vidsrc");
    src_cap = gst_element_factory_make("capsfilter", NULL);
    rtp_buffer = gst_element_factory_make("rtpjitterbuffer", "rtp_buffer");
    depayloader = gst_element_factory_make("rtph264depay", "depayloader");
    parser = gst_element_factory_make("h264parse", "parser");
    omx_decoder = gst_element_factory_make("omxh264dec", "decoder");
    video_converter = gst_element_factory_make("videoconvert", "video_conv");
    video_crop = gst_element_factory_make("videocrop", "video_crop");
    sink = gst_element_factory_make("autovideosink", "videosink");
    

    if(!pipeline || !udp_source || !src_cap || !rtp_buffer || 
       !depayloader || !parser || !omx_decoder || !video_converter || !video_crop || !sink) {
        g_printerr("Error creating the pipeline\n");
        return -1;
    }


    g_object_set(G_OBJECT(udp_source), "port", 5801, NULL);
    
    GstCaps *source_cap;
    source_cap = gst_caps_new_simple("application/x-rtp",
            "payload", G_TYPE_INT, 96,
            "encoding-name", G_TYPE_STRING, "H264",
            "clock-rate", G_TYPE_INT, 900000, NULL);

    gst_caps_unref(source_cap);

    g_object_set(G_OBJECT(rtp_buffer), "latency", 50, NULL);
    g_object_set(G_OBJECT(video_crop), "bottom", 150, NULL);
    g_object_set(G_OBJECT(sink), "sync", false, NULL);

    gst_bin_add_many(GST_BIN(pipeline), udp_source, src_cap, rtp_buffer, depayloader, parser, omx_decoder,
            video_converter, video_crop, sink, NULL);

    if(gst_element_link_many(udp_source, src_cap, rtp_buffer, depayloader, parser, omx_decoder, video_converter, video_crop, sink, NULL) != TRUE) {
        g_printerr("Failed to link\n");
        return -1;
    }
    return 0;
}

void destroy_pipeline() {
    gst_element_set_state(pipeline, GST_STATE_NULL);
    g_object_unref(pipeline);
}

int main(int argc, char **argv) {

    GMainLoop *loop;

    gst_init(&argc, &argv);
    loop = g_main_loop_new(NULL, FALSE);
    
    if(create_pipeline() < 0) {
        g_printerr("Unable to set the pipeline to the playing state. \n");
        gst_object_unref(pipeline);
        return -1;
    }

    gst_element_set_state(pipeline, GST_STATE_PLAYING);
    g_main_loop_run(loop);

    ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
    if(ret == GST_STATE_CHANGE_FAILURE) {
        g_printerr("Unable to ste the pipeline to the playing state.\n");
        gst_object_unref(pipeline);
        return -1;
    }

    destroy_pipeline();
    g_main_loop_unref(loop);

    return 0;
}
