#ifndef FFMPEG_RECORDER_H
#define FFMPEG_RECORDER_H

#include "FFmpegDecoder.h"
#include "FFmpegFrameSink.h"

extern "C" {
#include <libavformat/avformat.h>
}

#include "pch.h"

class FFmpegRecorder {
private:
    std::map<int, std::unique_ptr<FFmpegDecoder>> decoders;

    std::vector<std::unique_ptr<FFmpegFrameSink>> sinks;
    std::string inputFormatStr;
    std::string deviceNameStr;
    std::map<std::string, std::string> options;

    std::atomic<bool> recording;
    std::thread recordingThread;

    constexpr static auto formatContextDeleter = [](AVFormatContext *c) {
        avformat_close_input(&c);
    };
    std::unique_ptr<AVFormatContext, decltype(formatContextDeleter)>
    openStream();

public:
    FFmpegRecorder(std::string inputFormat, std::string deviceName,
                   std::map<std::string, std::string> options,
                   std::vector<std::unique_ptr<FFmpegFrameSink>> &&sinks);
    ~FFmpegRecorder() { stop(); };
    void stop() {
        recording.store(false);
        join();
    };
    void join() {
        if (recordingThread.joinable()) {
            recordingThread.join();
        }
    }
};

#endif