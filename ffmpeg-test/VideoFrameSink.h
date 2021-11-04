#ifndef VIDEO_FRAME_SINK_H
#define VIDEO_FRAME_SINK_H

#include "FfmpegFrameSink.h"

class VideoFrameSink : public FfmpegFrameSink
{
private:

public:
    VideoFrameSink() {

    }

    void outputFrame(AVFrame *frame) override {

    }

    AVMediaType getType() const override {
        return AVMEDIA_TYPE_VIDEO;
    }
};


#endif