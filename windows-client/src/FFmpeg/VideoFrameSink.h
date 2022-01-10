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

    struct SwsContext* swsContext = NULL;
    uint8_t* data = NULL;
    int linesize[1];
public:
    VideoFrameSink() {}

    ~VideoFrameSink() {
        if (data)
            delete[] data;
    }

    void init(AVCodecContext* decoderContext) override {
        width = decoderContext->width;
        height = decoderContext->height;
        pixelFormat = decoderContext->pix_fmt;
        data = new uint8_t[getDataSize()];
        memset(data, 0, getDataSize());
        linesize[0] = 3 * width;
		swsContext = sws_getCachedContext(swsContext, width, height,
			pixelFormat, width, height,
			AV_PIX_FMT_BGR24, 0, 0, 0, 0);
        setInitialized(true);
    }

    void outputFrame(AVFrame *frame) override {
        if (frame->width != width || frame->height != height || frame->format != pixelFormat) {
            throw std::runtime_error("Cannot support changing input format");
        }

        std::lock_guard<std::mutex> lock(mutex);
        sws_scale(swsContext, frame->data, frame->linesize, 0, height, &data, linesize);
    }

    AVMediaType getType() const override {
        return AVMEDIA_TYPE_VIDEO;
    }

    int getWidth() const { return this->width; }
    int getHeight() const { return this->height; }

    void getData(uint8_t* data) {
        std::lock_guard<std::mutex> lock(mutex);
        memcpy(data, this->data, getDataSize());
    }
    int getDataSize() const {
        return width * height * 3;
    }
};


#endif