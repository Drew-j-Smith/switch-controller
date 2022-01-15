#ifndef VIDEO_FRAME_SINK_H
#define VIDEO_FRAME_SINK_H

#include "FFmpegFrameSink.h"

#include "pch.h"

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

extern "C" {
    #include <libswscale/swscale.h>
}

class VideoFrameSink : public FFmpegFrameSink
{
private:
    int width = 0;
    int height = 0;
    AVPixelFormat pixelFormat;

    std::mutex mutex;

    struct SwsContext* swsContext;
    uint8_t* data;
    int linesize[1];

    void virtualInit(AVCodecContext* decoderContext) override {
        width = decoderContext->width;
        height = decoderContext->height;
        pixelFormat = decoderContext->pix_fmt;
        data = new uint8_t[getDataSize()];
        memset(data, 0, getDataSize());
        linesize[0] = 3 * width; // TODO get with ffmpeg
        swsContext = sws_getCachedContext(nullptr,
            width,
            height,
            pixelFormat,
            width,
            height,
            AV_PIX_FMT_BGR24, // TODO pick output format
            0,
            0,
            0,
            0);
    }

    void virtualOutputFrame(AVFrame* frame) override {
        if (frame->width != width || frame->height != height || frame->format != pixelFormat) {
            throw std::runtime_error("Cannot support changing input format");
        }

        int res = sws_scale(swsContext, frame->data, frame->linesize, 0, height, &data, linesize);
        if (res < 0) {
            char error[AV_ERROR_MAX_STRING_SIZE];
            av_make_error_string(error, AV_ERROR_MAX_STRING_SIZE, res);
            std::cerr << "Error converting audio in VideoFrameSink" << error << '\n';
            throw std::runtime_error("Error converting image in VideoFrameSink");
        }
    }

    void getDataWithoutLock(uint8_t* data) override {
        memcpy(data, this->data, getDataSize());
    }

public:
    VideoFrameSink() {}

    ~VideoFrameSink() {
        if (data)
            delete[] data;
    }

    AVMediaType getType() const override {
        return AVMEDIA_TYPE_VIDEO;
    }

    long long getDataSize() const override {
        return width * height * 3;
    }

    int getWidth() const { return this->width; }
    int getHeight() const { return this->height; }
};


#endif