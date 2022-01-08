#ifndef FFMPEG_DECODER_H
#define FFMPEG_DECODER_H

#include "FfmpegFrameSink.h"

#include <iostream>

extern "C" {
    #include <libavformat/avformat.h>
}

class FFmpegDecoder
{
private:
    AVFormatContext* formatContext;
    AVCodecContext* decoderContext;
    std::shared_ptr<FfmpegFrameSink> sink;
    int streamIndex;
public:
    FFmpegDecoder(AVFormatContext* formatContext, std::shared_ptr<FfmpegFrameSink> sink);
    ~FFmpegDecoder() { free(); };

    void free();

    void decodePacket(const AVPacket* packet, AVFrame* frame);
    int getStreamIndex() const { return streamIndex; }
private:
    void openCodecContext();
};




#endif