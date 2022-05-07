#ifndef FFMPEG_RECORDER_H
#define FFMPEG_RECORDER_H

#include "FFmpegDecoder.h"
#include "FFmpegFrameSink.h"

extern "C" {
#include <libavformat/avformat.h>
}

#include "pch.h"

struct FFmpegRecorderThreadDeleter {
    void
    operator()(std::tuple<std::thread, std::unique_ptr<std::atomic_bool>> *t) {
        auto &[thread, recording] = *t;
        recording->store(false);
        if (thread.joinable()) {
            thread.join();
        }
        delete t;
    }
};

using FFmpegRecorder =
    std::unique_ptr<std::tuple<std::thread, std::unique_ptr<std::atomic_bool>>,
                    FFmpegRecorderThreadDeleter>;

FFmpegRecorder
createFFmpegRecorder(const std::string &inputFormat,
                     const std::string &deviceName,
                     const std::map<std::string, std::string> &options,
                     std::vector<std::unique_ptr<FFmpegFrameSink>> &sinks);

#endif