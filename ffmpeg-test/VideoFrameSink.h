#ifndef VIDEO_FRAME_SINK_H
#define VIDEO_FRAME_SINK_H

#include "FfmpegFrameSink.h"

class VideoFrameSink : public FfmpegFrameSink
{
private:
    int width, height;
    AVPixelFormat pix_fmt;

    struct SwsContext* sws_ctx = NULL;
    uint8_t* video_data = NULL;
    int video_linesize[1];
public:
    VideoFrameSink() {}

    void init(AVCodecContext* decoderContext) override {
        
    }

    void outputFrame(AVFrame *frame) override {

    }

    AVMediaType getType() const override {
        return AVMEDIA_TYPE_VIDEO;
    }
};


#endif