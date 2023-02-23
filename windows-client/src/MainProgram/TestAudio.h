#pragma once

#include "pch.h"

#include "FFmpeg/AudioFrameSink.h"
#include "FFmpeg/FFmpegRecorder.h"
#include "FFmpeg/VideoFrameSink.h"

void TestAudio() {
    std::string inputFormat;
    std::string deviceName;
    std::string recordTimeStr;
    char loopRecord;
    int64_t bufferSize = 0;

    // dshow
    // audio=Game Capture HD60 S Audio
    // ffplay -f s16le -ar 48k -ac 1 test.pcm

    std::cout << "Enter the input format (empty to load a file):\n";
    std::getline(std::cin, inputFormat);

    std::cout << "Enter the device/filename:\n";
    std::getline(std::cin, deviceName);

    std::cout << "Enter the recording time in seconds (empty to read "
                 "an entire file)\n";
    std::getline(std::cin, recordTimeStr);

    std::cout << "Use loop recording (y/n):\n";
    std::cin >> loopRecord;

    if (loopRecord == 'y') {
        std::cout << "Enter the buffer size in seconds:\n";
        std::cin >> bufferSize;
    }

    std::map<std::string, std::string> ffmpegOptions = {};
    std::vector<std::unique_ptr<FFmpegFrameSink>> sinks;
    sinks.push_back(std::make_unique<AudioFrameSink>(
        AV_CH_LAYOUT_MONO, AV_SAMPLE_FMT_S16, 48000, loopRecord == 'y',
        48000 * bufferSize));
    auto audioSink = sinks[0].get();

    {
        FFmpegRecorder recorder(inputFormat, deviceName, ffmpegOptions, sinks);

        if (recordTimeStr.length() > 0) {
            std::this_thread::sleep_for(
                std::chrono::seconds(std::stoi(recordTimeStr)));
        } else {
            recorder.joinThread();
        }
    }

    std::vector<uint8_t> data;
    audioSink->getData(data);

    std::cout << "Enter the output filename:\n";
    std::string audioFilename;
    std::getline(std::cin, audioFilename);
    std::getline(std::cin, audioFilename);

    std::ofstream outfile(audioFilename, std::ios::out | std::ios::binary);
    outfile.write(reinterpret_cast<char *>(data.data()),
                  static_cast<std::streamsize>(data.size()));
}