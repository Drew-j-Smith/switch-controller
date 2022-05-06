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
    AVFormatContext *formatContext = nullptr;
    AVFrame *frame = nullptr;
    std::map<int, std::unique_ptr<FFmpegDecoder>> decoders;

    std::vector<std::unique_ptr<FFmpegFrameSink>> sinks;
    std::string inputFormatStr;
    std::string deviceNameStr;
    std::map<std::string, std::string> options;

    std::atomic<bool> recording;
    std::thread recordingThread;

    void openStream();

public:
    FFmpegRecorder(std::string inputFormat, std::string deviceName,
                   std::map<std::string, std::string> options,
                   std::vector<std::unique_ptr<FFmpegFrameSink>> &&sinks);
    ~FFmpegRecorder() {
        stop();
        decoders.clear();
        avformat_close_input(&formatContext);
        av_frame_free(&frame);
    };
    void start();
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