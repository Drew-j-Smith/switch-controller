#ifndef FFMPEG_RECORDER_H
#define FFMPEG_RECORDER_H

#include "FfmpegFrameSink.h"

extern "C" {
    #include <libavformat/avformat.h>
}

#include <string>
#include <vector>
#include <map>

class FfmpegRecorder
{
private:
    int decode_packet(AVCodecContext *dec, const AVPacket *pkt, AVFrame* frame);
    int open_codec_context(int *stream_idx, AVCodecContext **dec_ctx, AVFormatContext *fmt_ctx, enum AVMediaType type);
public:
    FfmpegRecorder(std::string inputFormat, std::string deviceName, std::map<std::string, std::string> options, std::vector<std::shared_ptr<FfmpegFrameSink>> sinks);
    ~FfmpegRecorder();
    void start();
    void stop();
};


#endif