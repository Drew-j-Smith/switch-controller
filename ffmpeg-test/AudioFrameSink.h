#ifndef AUDIO_FRAME_SINK_H
#define AUDIO_FRAME_SINK_H

#include "FfmpegFrameSink.h"

class AudioFrameSink : public FfmpegFrameSink
{
private:

public:
    AudioFrameSink();
    int outputFrame(AVFrame *frame) {

    }

    AVMediaType getType() {
        return AVMEDIA_TYPE_AUDIO;
    }
};


#endif