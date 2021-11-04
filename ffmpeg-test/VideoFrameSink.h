#ifndef VIDEO_FRAME_SINK_H
#define VIDEO_FRAME_SINK_H

#include "FfmpegFrameSink.h"

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

extern "C" {
    #include <libswscale/swscale.h>
}

class VideoFrameSink : public FfmpegFrameSink
{
private:
    int width, height;
    AVPixelFormat pixelFormat;

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
        data = new uint8_t[width * height * 8];
        linesize[0] = 3 * width;
		swsContext = sws_getCachedContext(swsContext, width, height,
			pixelFormat, width, height,
			AV_PIX_FMT_BGR24, 0, 0, 0, 0);
    }

    void outputFrame(AVFrame *frame) override {
        if (frame->width != width || frame->height != height || frame->format != pixelFormat) {
            throw std::runtime_error("Cannot support changing input format");
        }

        sws_scale(swsContext, frame->data, frame->linesize, 0, height, &data, linesize);

        // cv::Mat mat = cv::Mat(frame->height, frame->width, CV_8UC3, data);
        // cv::imshow("test", mat);
        // cv::waitKey(1);
    }

    AVMediaType getType() const override {
        return AVMEDIA_TYPE_VIDEO;
    }
};


#endif