#pragma once

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
    std::jthread m_thread;
};
