#ifndef FFMPEG_FRAME_SINK_H
#define FFMPEG_FRAME_SINK_H

extern "C" {
    #include <libavformat/avformat.h>
}

class FfmpegFrameSink
{
public:
    virtual int outputFrame(AVFrame *frame);
    virtual AVMediaType getType() const;
};


#endif