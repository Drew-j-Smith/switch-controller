#pragma once

#include "pch.h"

#include "FFmpeg/AudioFrameSink.h"
#include "FFmpeg/FFmpegRecorder.h"
#include "FFmpeg/VideoFrameSink.h"

struct GameCaptureConfig {
    int videoFrameSinkOffset = 0;
    int audioFrameSinkOffset = 1;
    std::vector<std::unique_ptr<FFmpegFrameSink>> sinks{};
    std::unique_ptr<FFmpegRecorder> recorder;

    GameCaptureConfig(
        std::string inputFormat = "dshow",
        std::string deviceName =
            "video=Game Capture HD60 S:audio=Game Capture HD60 S Audio",
        std::map<std::string, std::string> ffmpegOptions = {{"pixel_format",
                                                             "bgr24"}})

    {
        sinks.push_back(std::make_unique<VideoFrameSink>());
        sinks.push_back(std::make_unique<AudioFrameSink>(
            AV_CH_LAYOUT_MONO, AV_SAMPLE_FMT_S16, 48000, true, 48000));

        recorder = {std::make_unique<FFmpegRecorder>(inputFormat, deviceName,
                                                     ffmpegOptions, sinks)};
    }
};