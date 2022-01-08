
#include "FfmpegRecorder.h"
#include "AudioFrameSink.h"
#include "VideoFrameSink.h"

#include <iostream>

int main() {
    std::string inputFormat = "dshow";
    std::string deviceName = "video=Game Capture HD60 S";
    std::map<std::string, std::string> options = {{"pixel_format", "bgr24"}};
    std::vector<std::shared_ptr<FfmpegFrameSink>> sinks;
    std::shared_ptr<FfmpegFrameSink> videoSink = std::make_shared<VideoFrameSink>();
    sinks.push_back(videoSink);

    av_log_set_level(AV_LOG_QUIET);

    FfmpegRecorder recorder(inputFormat, deviceName, options, sinks);
    recorder.start();

    while (true) { std::this_thread::sleep_for(std::chrono::minutes(1)); }
}