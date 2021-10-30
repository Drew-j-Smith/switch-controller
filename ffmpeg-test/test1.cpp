/*
 * Copyright (c) 2012 Stefano Sabatini
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
 
/**
 * @file
 * Demuxing and decoding example.
 *
 * Show how to use the libavformat and libavcodec API to demux and
 * decode audio and video data.
 * @example demuxing_decoding.c
 */

#include <iostream> 

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

 extern "C" {
    #include <stdint.h>
    #include <libavutil/imgutils.h>
    #include <libavutil/samplefmt.h>
    #include <libavutil/timestamp.h>
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libswscale/swscale.h>
    #include <libavdevice/avdevice.h>
 }
 
static AVFormatContext *fmt_ctx = NULL;
static AVCodecContext *video_dec_ctx = NULL, *audio_dec_ctx;
static int width, height;
static enum AVPixelFormat pix_fmt;
static AVStream *video_stream = NULL, *audio_stream = NULL;
 
static int video_stream_idx = -1, audio_stream_idx = -1;
static AVFrame *frame = NULL;
static AVPacket *pkt = NULL;

static struct SwsContext* sws_ctx = NULL;
static uint8_t* video_data = NULL;
static int video_linesize[1];

 
static int output_video_frame(AVFrame *frame)
{
    if (frame->width != width || frame->height != height ||
        frame->format != pix_fmt) {
        std::cout << "error";
        return -1;
    }

    sws_ctx = sws_getCachedContext(sws_ctx, width, height,
                                  (AVPixelFormat)frame->format, width, height,
                                  AV_PIX_FMT_BGR24, 0, 0, 0, 0);
    sws_scale(sws_ctx, frame->data, frame->linesize, 0, height, &video_data, video_linesize);

    cv::Mat mat = cv::Mat(frame->height, frame->width, CV_8UC3, video_data);
    cv::imshow("test", mat);
    cv::waitKey(1);
    return 0;
}
 
static int output_audio_frame(AVFrame *frame)
{
    size_t unpadded_linesize = frame->nb_samples * av_get_bytes_per_sample((AVSampleFormat)frame->format);

    // TODO do something with the audio data
    // fwrite(frame->extended_data[0], 1, unpadded_linesize, audio_dst_file);
 
    return 0;
}
 
static int decode_packet(AVCodecContext *dec, const AVPacket *pkt)
{
    int ret = 0;
 
    // submit the packet to the decoder
    ret = avcodec_send_packet(dec, pkt);
    if (ret < 0) {
        char error[AV_ERROR_MAX_STRING_SIZE];
        fprintf(stderr, "Error submitting a packet for decoding (%s)\n", av_make_error_string(error, AV_ERROR_MAX_STRING_SIZE, ret));
        return ret;
    }
 
    // get all the available frames from the decoder
    while (ret >= 0) {
        ret = avcodec_receive_frame(dec, frame);
        if (ret < 0) {
            // those two return values are special and mean there is no output
            // frame available, but there were no errors during decoding
            if (ret == AVERROR_EOF || ret == AVERROR(EAGAIN))
                return 0;
 
            char error[AV_ERROR_MAX_STRING_SIZE];
            fprintf(stderr, "Error during decoding (%s)\n", av_make_error_string(error, AV_ERROR_MAX_STRING_SIZE, ret));
            return ret;
        }
 
        // write the frame data to output file
        if (dec->codec->type == AVMEDIA_TYPE_VIDEO)
            ret = output_video_frame(frame);
        else
            ret = output_audio_frame(frame);
 
        av_frame_unref(frame);
        if (ret < 0)
            return ret;
    }
 
    return 0;
}
 
static int open_codec_context(int *stream_idx,
                              AVCodecContext **dec_ctx, AVFormatContext *fmt_ctx, enum AVMediaType type)
{
    int ret, stream_index;
    AVStream *st;
    const AVCodec *dec = NULL;
 
    ret = av_find_best_stream(fmt_ctx, type, -1, -1, NULL, 0);
    if (ret < 0) {
        fprintf(stderr, "Could not find %s stream for input\n",
                av_get_media_type_string(type));
        return ret;
    } else {
        stream_index = ret;
        st = fmt_ctx->streams[stream_index];
 
        /* find decoder for the stream */
        dec = avcodec_find_decoder(st->codecpar->codec_id);
        if (!dec) {
            fprintf(stderr, "Failed to find %s codec\n",
                    av_get_media_type_string(type));
            return AVERROR(EINVAL);
        }
 
        /* Allocate a codec context for the decoder */
        *dec_ctx = avcodec_alloc_context3(dec);
        if (!*dec_ctx) {
            fprintf(stderr, "Failed to allocate the %s codec context\n",
                    av_get_media_type_string(type));
            return AVERROR(ENOMEM);
        }
 
        /* Copy codec parameters from input stream to output codec context */
        if ((ret = avcodec_parameters_to_context(*dec_ctx, st->codecpar)) < 0) {
            fprintf(stderr, "Failed to copy %s codec parameters to decoder context\n",
                    av_get_media_type_string(type));
            return ret;
        }
 
        /* Init the decoders */
        if ((ret = avcodec_open2(*dec_ctx, dec, NULL)) < 0) {
            fprintf(stderr, "Failed to open %s codec\n",
                    av_get_media_type_string(type));
            return ret;
        }
        *stream_idx = stream_index;
    }
 
    return 0;
}
 
int main (int argc, char **argv)
{
    avdevice_register_all(); // for device 
    avcodec_register_all();
    av_register_all();
    char *dev_name = "video=Game Capture HD60 S";
    AVInputFormat *inputFormat =av_find_input_format("dshow");
    AVDictionary *options = NULL;
    // av_dict_set(&options, "framerate", "30", 0);
    // av_dict_set(&options, "video_size", "1280x960", 0);

    int ret = 0;
 
 
    /* open input file, and allocate format context */
    if (avformat_open_input(&fmt_ctx, dev_name, inputFormat, &options) < 0) {
        fprintf(stderr, "Could not open stream %s\n", dev_name);
        exit(1);
    }
 
    /* retrieve stream information */
    if (avformat_find_stream_info(fmt_ctx, NULL) < 0) {
        fprintf(stderr, "Could not find stream information\n");
        exit(1);
    }
 
    if (open_codec_context(&video_stream_idx, &video_dec_ctx, fmt_ctx, AVMEDIA_TYPE_VIDEO) >= 0) {
        video_stream = fmt_ctx->streams[video_stream_idx];
 
        /* allocate image where the decoded image will be put */
        width = video_dec_ctx->width;
        height = video_dec_ctx->height;
        pix_fmt = video_dec_ctx->pix_fmt;
        video_data = new uint8_t[width * height * 8];
        video_linesize[0] = 3 * width;
    }
 
    if (open_codec_context(&audio_stream_idx, &audio_dec_ctx, fmt_ctx, AVMEDIA_TYPE_AUDIO) >= 0) {
        audio_stream = fmt_ctx->streams[audio_stream_idx];
    }
 
    /* dump input information to stderr */
    av_dump_format(fmt_ctx, 0, dev_name, 0);
 
    if (!audio_stream && !video_stream) {
        fprintf(stderr, "Could not find audio or video stream in the input, aborting\n");
        ret = 1;
        goto end;
    }
 
    frame = av_frame_alloc();
    if (!frame) {
        fprintf(stderr, "Could not allocate frame\n");
        ret = AVERROR(ENOMEM);
        goto end;
    }
 
    pkt = av_packet_alloc();
    if (!pkt) {
        fprintf(stderr, "Could not allocate packet\n");
        ret = AVERROR(ENOMEM);
        goto end;
    }

 
    /* read frames from the file */
    while (av_read_frame(fmt_ctx, pkt) >= 0) {
        // check if the packet belongs to a stream we are interested in, otherwise
        // skip it
        if (pkt->stream_index == video_stream_idx)
            ret = decode_packet(video_dec_ctx, pkt);
        else if (pkt->stream_index == audio_stream_idx)
            ret = decode_packet(audio_dec_ctx, pkt);
        av_packet_unref(pkt);
        if (ret < 0)
            break;
    }
 
    /* flush the decoders */
    if (video_dec_ctx)
        decode_packet(video_dec_ctx, NULL);
    if (audio_dec_ctx)
        decode_packet(audio_dec_ctx, NULL);
 

 
end:
    avcodec_free_context(&video_dec_ctx);
    avcodec_free_context(&audio_dec_ctx);
    avformat_close_input(&fmt_ctx);
    av_packet_free(&pkt);
    av_frame_free(&frame);
    delete[] video_data;
    sws_freeContext(sws_ctx);
 
    return ret < 0;
}
