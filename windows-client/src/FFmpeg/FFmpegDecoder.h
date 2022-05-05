#ifndef FFMPEG_DECODER_H
#define FFMPEG_DECODER_H

#include "FFmpegFrameSink.h"

#include "pch.h"

extern "C" {
#include <libavformat/avformat.h>
}

class FFmpegDecoder {
private:
    AVFormatContext *formatContext;
    struct CodecContextDeleter {
        CodecContextDeleter() {}
        void operator()(AVCodecContext *c) const noexcept {
            avcodec_free_context(&c);
        }
    };
    std::unique_ptr<AVCodecContext, CodecContextDeleter> decoderContext;
    std::shared_ptr<FFmpegFrameSink> sink;
    int streamIndex;

public:
    FFmpegDecoder(AVFormatContext *formatContext,
                  std::shared_ptr<FFmpegFrameSink> sink);

    void decodePacket(const AVPacket *packet, AVFrame *frame);
    int getStreamIndex() const { return streamIndex; }

private:
    void openCodecContext();
};

#endif