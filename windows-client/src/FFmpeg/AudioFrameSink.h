#ifndef AUDIO_FRAME_SINK_H
#define AUDIO_FRAME_SINK_H

#include "FFmpegFrameSink.h"

class AudioFrameSink : public FFmpegFrameSink
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