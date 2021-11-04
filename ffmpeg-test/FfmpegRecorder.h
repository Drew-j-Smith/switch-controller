#ifndef FFMPEG_RECORDER_H
#define FFMPEG_RECORDER_H

#include "FfmpegFrameSink.h"

extern "C" {
    #include <libavformat/avformat.h>
}

#include <string>
#include <vector>
#include <map>
#include <atomic>
#include <thread>
#include <set>
#include <cassert>

class FfmpegRecorder
{
private:
    AVFormatContext* formatContext = NULL;
    AVFrame* frame = NULL;
    std::map<int, std::pair<std::shared_ptr<FfmpegFrameSink>, AVCodecContext*>> streamIdCtxSinkMap;

    std::vector<std::shared_ptr<FfmpegFrameSink>> sinks;
    std::string inputFormat; std::string deviceName; std::map<std::string, std::string> options;

    std::atomic<bool> recording;
    std::thread recordingThread;

    void decodePacket(AVCodecContext* decoderContext, const AVPacket* packet, AVFrame* frame, std::shared_ptr<FfmpegFrameSink> sink);
    void openStream(std::string inputFormatStr, std::string deviceName, std::map<std::string, std::string> optionsMap, std::vector<std::shared_ptr<FfmpegFrameSink>> sinks);
    void openCodecContext(int* streamIndex, AVCodecContext** decoderContext, AVFormatContext* formatContex, enum AVMediaType type);

    bool checkOverlap(std::vector<std::shared_ptr<FfmpegFrameSink>> sinks);

    void free();
public:
    FfmpegRecorder(std::string inputFormat, std::string deviceName, std::map<std::string, std::string> options, std::vector<std::shared_ptr<FfmpegFrameSink>> sinks);
    ~FfmpegRecorder() { free(); };
    void start();
    void stop() { recording.store(true); } ;
};


#endif