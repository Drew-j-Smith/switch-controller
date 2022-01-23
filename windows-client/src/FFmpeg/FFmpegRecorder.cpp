#include "FFmpegRecorder.h"

#include <iostream>
#include <stdexcept>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavdevice/avdevice.h>
#include <libavformat/avformat.h>
#include <libavutil/samplefmt.h>
#include <libavutil/timestamp.h>
#include <stdint.h>
}

FFmpegRecorder::FFmpegRecorder(
    std::string inputFormat, std::string deviceName,
    std::map<std::string, std::string> options,
    std::vector<std::shared_ptr<FFmpegFrameSink>> sinks) {
    std::set<AVMediaType> types;
    bool overlap = false;
    for (auto sink : sinks) {
        if (types.find(sink->getType()) == types.end()) {
            types.insert(sink->getType());
        } else {
            overlap = true;
        }
    }
    if (overlap) {
        throw std::runtime_error(
            "Only one frame sink of each type may be used");
    }
    if (sinks.size() == 0) {
        throw std::runtime_error("The sink size must be greater than 0");
    }

    this->inputFormatStr = inputFormat;
    this->deviceNameStr = deviceName;
    this->options = options;
    this->sinks = sinks;
}

void FFmpegRecorder::openStream() {
    avdevice_register_all();

    // finding input format and setting options dict
    AVInputFormat *inputFormat = nullptr;
    if (inputFormatStr.size() > 0) { // If no input format is set, load a file
        inputFormat = av_find_input_format(inputFormatStr.c_str());
    }
    AVDictionary *optionsDic = nullptr;
    for (auto &pair : options) {
        av_dict_set(&optionsDic, pair.first.c_str(), pair.second.c_str(), 0);
    }

    // open input file, and allocate format context
    if (avformat_open_input(&formatContext, deviceNameStr.c_str(), inputFormat,
                            &optionsDic) < 0) {
        throw std::runtime_error("Could not open stream " + deviceNameStr);
    }

    // retrieve stream information
    if (avformat_find_stream_info(formatContext, nullptr) < 0) {
        throw std::runtime_error("Could not find stream information for " +
                                 deviceNameStr);
    }

    // open stream ctx for each frame sink
    for (auto sink : sinks) {
        std::shared_ptr<FFmpegDecoder> decoder =
            std::make_shared<FFmpegDecoder>(formatContext, sink);
        decoders.insert({decoder->getStreamIndex(), decoder});
    }

    if (decoders.size() == 0) {
        throw std::runtime_error(
            "No frame sinks were loaded in FFmpegRecorder");
    }

    // print stream info
    // av_dump_format(formatContext, 0, deviceName.c_str(), 0);
}

void FFmpegRecorder::start() {
    recording.store(true);

    recordingThread = std::thread([&]() {
        try {
            openStream();

            AVPacket pkt;
            frame = av_frame_alloc();
            if (!frame) {
                throw std::runtime_error("Could not allocate frame");
            }
            // read until there are no more frames or canceled
            while (recording.load() &&
                   av_read_frame(formatContext, &pkt) >= 0) {
                // check if the pack goes to a frame sink
                if (decoders.find(pkt.stream_index) != decoders.end()) {
                    auto decoder = decoders.at(pkt.stream_index);
                    decoder->decodePacket(&pkt, frame);
                }
                av_packet_unref(&pkt);
            }

            // flushing decoders
            for (auto it : decoders) {
                it.second->decodePacket(nullptr, frame);
            }

            // need to call these methods here... for some reason
            // if these methods aren't called in this thread it crashes
            decoders.clear();
            avformat_close_input(&formatContext);
            av_frame_free(&frame);

        } catch (std::exception &e) {
            std::cerr << e.what() << '\n';
            throw;
        }
    });
}