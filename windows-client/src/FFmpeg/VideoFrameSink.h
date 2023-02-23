#pragma once

#include "FFmpegFrameSink.h"

#include "pch.h"

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

extern "C" {
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}

class VideoFrameSink : public FFmpegFrameSink {
private:
    int width = 0;
    int height = 0;
    AVPixelFormat pixelFormat;

    struct swsContextDeleter {
        void operator()(SwsContext *s) const noexcept { sws_freeContext(s); }
    };
    std::unique_ptr<SwsContext, swsContextDeleter> swsContext;
    std::vector<uint8_t> data;
    int linesize[4];

    AVPixelFormat outputPixelFormat = AV_PIX_FMT_BGR24;
    int outputWidth = 1920;
    int outputHeight = 1080;

    void virtualInit(AVCodecContext *decoderContext) override {
        width = decoderContext->width;
        height = decoderContext->height;
        pixelFormat = decoderContext->pix_fmt;

        int res =
            av_image_fill_linesizes(linesize, outputPixelFormat, outputWidth);
        if (res < 0) {
            char error[AV_ERROR_MAX_STRING_SIZE];
            av_make_error_string(error, AV_ERROR_MAX_STRING_SIZE, res);
            throw std::runtime_error(
                "Error getting linesize in VideoFrameSink: " +
                std::string(error));
        }

        res = av_image_get_buffer_size(outputPixelFormat, outputWidth,
                                       outputHeight, 1);
        if (res < 0) {
            char error[AV_ERROR_MAX_STRING_SIZE];
            av_make_error_string(error, AV_ERROR_MAX_STRING_SIZE, res);
            throw std::runtime_error(
                "Error getting plane size in VideoFrameSink: " +
                std::string(error));
        }
        data.resize(res);

        swsContext = {sws_getCachedContext(nullptr, width, height, pixelFormat,
                                           outputWidth, outputHeight,
                                           outputPixelFormat, 0, nullptr,
                                           nullptr, nullptr),
                      swsContextDeleter()};
    }

    void virtualOutputFrame(AVFrame *frame) override {
        if (frame->width != width || frame->height != height ||
            frame->format != pixelFormat) {
            throw std::runtime_error(
                "Cannot support changing input format in VideoFrameSink");
        }

        uint8_t *dataPointer = data.data();
        int res = sws_scale(swsContext.get(), frame->data, frame->linesize, 0,
                            height, &dataPointer, linesize);
        if (res < 0) {
            char error[AV_ERROR_MAX_STRING_SIZE];
            av_make_error_string(error, AV_ERROR_MAX_STRING_SIZE, res);
            throw std::runtime_error(
                "Error converting image in VideoFrameSink: " +
                std::string(error));
        }
    }

    void getDataWithoutLock(std::vector<uint8_t> &dataCopy) override {
        dataCopy.resize(this->data.size());
        std::copy(this->data.begin(), this->data.end(), dataCopy.begin());
    }

public:
    VideoFrameSink() {}

    AVMediaType getType() const override { return AVMEDIA_TYPE_VIDEO; }

    int getWidth() const { return outputWidth; }
    int getHeight() const { return outputHeight; }
};
