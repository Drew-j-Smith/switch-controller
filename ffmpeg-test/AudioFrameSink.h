#ifndef AUDIO_FRAME_SINK_H
#define AUDIO_FRAME_SINK_H

#include "FfmpegFrameSink.h"

class AudioFrameSink : public FfmpegFrameSink
{
private:

public:
    AudioFrameSink() {

    }


    void init(AVCodecContext* decoderContext) override {

    }

    void outputFrame(AVFrame *frame) override {

    }

    AVMediaType getType() const override {
        return AVMEDIA_TYPE_AUDIO;
    }
};


#endif