#ifndef FFMPEG_RECORDER_H
#define FFMPEG_RECORDER_H

#include "FFmpegDecoder.h"
#include "FFmpegFrameSink.h"

extern "C" {
#include <libavformat/avformat.h>
}

#include "pch.h"

class FFmpegRecorder {
public:
    FFmpegRecorder(const std::string &inputFormat,
                   const std::string &deviceName,
                   const std::map<std::string, std::string> &options,
                   std::vector<std::unique_ptr<FFmpegFrameSink>> &sinks);
    ~FFmpegRecorder() {
        m_recording.store(false);
        if (m_thread.joinable()) {
            m_thread.join();
        }
    }
    FFmpegRecorder(FFmpegRecorder &other) = delete;
    FFmpegRecorder(FFmpegRecorder &&other) = delete;

    void joinThread() { m_thread.join(); }

private:
    std::thread m_thread;
    std::atomic_bool m_recording;
};

#endif