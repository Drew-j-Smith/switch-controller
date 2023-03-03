#pragma once

#include "pch.h"

#include "FFmpeg/AudioFrameSink.h"
#include "FFmpeg/FFmpegRecorder.h"
#include "FFmpeg/VideoFrameSink.h"

#include <boost/program_options.hpp>

void TestAudio(const boost::program_options::variables_map &vm,
               const std::map<std::string, std::string> &ffmpegOptions) {
    auto inputFormat = vm["inputFormat"].as<std::string>();
    auto deviceName = vm["deviceName"].as<std::string>();
    int recordTime = vm["testAudio"].as<int>();
    int loopRecordTime = vm["audioLoopTime"].as<int>();

    // ffplay -f s16le -ar 48k -ac 1 test.pcm

    std::vector<std::unique_ptr<FFmpegFrameSink>> sinks;
    AVChannelLayout channel_layout = AV_CHANNEL_LAYOUT_MONO;
    sinks.push_back(std::make_unique<AudioFrameSink>(
        channel_layout, AV_SAMPLE_FMT_S16, 48000, loopRecordTime != -1,
        48000 * (loopRecordTime != -1 ? loopRecordTime : recordTime)));
    auto audioSink = sinks[0].get();

    {
        FFmpegRecorder recorder(inputFormat, deviceName, ffmpegOptions, sinks);
        std::this_thread::sleep_for(std::chrono::seconds(recordTime));
    }

    std::vector<uint8_t> data;
    audioSink->getData(data);

    std::cout << "Enter the output filename:\n";
    std::string audioFilename;
    std::getline(std::cin, audioFilename);

    std::ofstream outfile(audioFilename, std::ios::out | std::ios::binary);
    outfile.write(reinterpret_cast<char *>(data.data()),
                  static_cast<std::streamsize>(data.size()));
}