#ifndef FFMPEG_FRAME_SINK_H
#define FFMPEG_FRAME_SINK_H

extern "C" {
    #include <libavformat/avformat.h>
}

class FFmpegFrameSink
{
public:
    virtual void init(AVCodecContext* decoderContext) = 0;
    virtual void outputFrame(AVFrame* frame) = 0;
    virtual AVMediaType getType() const = 0;
};


#endif