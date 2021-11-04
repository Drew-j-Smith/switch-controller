#ifndef VIDEO_FRAME_SINK_H
#define VIDEO_FRAME_SINK_H

#include "FfmpegFrameSink.h"

class VideoFrameSink : public FfmpegFrameSink
{
private:

public:
    VideoFrameSink();
    int outputFrame(AVFrame *frame) {

    }

    AVMediaType getType() {
        return AVMEDIA_TYPE_VIDEO;
    }
};


#endif