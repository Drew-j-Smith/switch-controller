#ifndef FFMPEG_DECODER_H
#define FFMPEG_DECODER_H

#include "FFmpegFrameSink.h"

#include "Util/Delter.h"

#include "pch.h"

extern "C" {
#include <libavformat/avformat.h>
}

class FFmpegDecoder {
private:
    AVFormatContext *formatContext;
    unique_dbl_ptr_w_deleter<AVCodecContext, avcodec_free_context>
        decoderContext;
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