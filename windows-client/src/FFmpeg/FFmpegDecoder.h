#pragma once

#include "FFmpegFrameSink.h"

#include "pch.h"

extern "C" {
#include <libavformat/avformat.h>
}

class FFmpegDecoder {
private:
    AVFormatContext *formatContext;
    struct CodecContextDeleter {
        void operator()(AVCodecContext *s) { avcodec_free_context(&s); }
    };
    std::unique_ptr<AVCodecContext, CodecContextDeleter> decoderContext;
    FFmpegFrameSink *sink;
    int streamIndex;

public:
    FFmpegDecoder(AVFormatContext *formatContext,
                  std::unique_ptr<FFmpegFrameSink> &sink);

    void decodePacket(const AVPacket *packet, AVFrame *frame);
    int getStreamIndex() const { return streamIndex; }

private:
    void openCodecContext();
};
