#ifndef FFMPEG_RECORDER_H
#define FFMPEG_RECORDER_H

#include "FFmpegFrameSink.h"
#include "FFmpegDecoder.h"

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

class FFmpegRecorder
{
private:
    AVFormatContext* formatContext = nullptr;
    AVFrame* frame = nullptr;
    std::map<int, std::shared_ptr<FFmpegDecoder>> decoders;

    std::vector<std::shared_ptr<FFmpegFrameSink>> sinks;
    std::string inputFormatStr; std::string deviceNameStr; std::map<std::string, std::string> options;

    std::atomic<bool> recording;
    std::thread recordingThread;

    void openStream();

    void free();
public:
    FFmpegRecorder(std::string inputFormat, std::string deviceName, std::map<std::string, std::string> options, std::vector<std::shared_ptr<FFmpegFrameSink>> sinks);
    ~FFmpegRecorder() { stop(); };
    void start();
    void stop() {
        recording.store(false);
        join();
        free();
    };
    void join() {
        if (recordingThread.joinable()) {
            recordingThread.join();
        }
    }
};


#endif