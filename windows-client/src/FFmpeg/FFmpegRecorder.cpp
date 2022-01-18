#include "FFmpegRecorder.h"

#include <stdexcept>
#include <iostream>

 extern "C" {
    #include <stdint.h>
    #include <libavutil/samplefmt.h>
    #include <libavutil/timestamp.h>
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libavdevice/avdevice.h>
}

FFmpegRecorder::FFmpegRecorder(std::string inputFormat, std::string deviceName, std::map<std::string, std::string> options, std::vector<std::shared_ptr<FFmpegFrameSink>> sinks) {
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
        std::string errStr = "Only one frame sink of each type may be used";
        std::cerr << errStr << '\n';
        throw std::runtime_error(errStr);
    }
    if (sinks.size() == 0) {
        std::string errStr = "The sink size must be greater than 0";
        std::cerr << errStr << '\n';
        throw std::runtime_error(errStr);
    }

    this->inputFormatStr = inputFormat;
    this->deviceNameStr = deviceName;
    this->options = options;
    this->sinks = sinks;
}

void FFmpegRecorder::free() {
    recording.store(false);
    decoders.clear();
    avformat_close_input(&formatContext);
    av_frame_free(&frame);
}

void FFmpegRecorder::openStream() {
    avdevice_register_all();

    // finding input format and setting options dict
    AVInputFormat* inputFormat = nullptr;
    if (inputFormatStr.size() > 0) { // If no input format is set, load a file
        inputFormat = av_find_input_format(inputFormatStr.c_str());
    }
    AVDictionary* optionsDic = nullptr;
    for (auto& pair : options) {
        av_dict_set(&optionsDic, pair.first.c_str(), pair.second.c_str(), 0);
    }
 
    // open input file, and allocate format context
    if (avformat_open_input(&formatContext, deviceNameStr.c_str(), inputFormat, &optionsDic) < 0) {
        free();
        std::string errStr = "Could not open stream " + deviceNameStr + " in FFmpeg Recorder";
        std::cerr << errStr << '\n';
        throw std::runtime_error(errStr);
    }
 
    // retrieve stream information
    if (avformat_find_stream_info(formatContext, nullptr) < 0) {
        free();
        std::string errStr = "Could not find stream information for " + deviceNameStr + " in FFmpeg Recorder";
        std::cerr << errStr << '\n';
        throw std::runtime_error(errStr);
    }
    
    // open stream ctx for each frame sink
    for (auto sink : sinks) {
        std::shared_ptr<FFmpegDecoder> decoder = std::make_shared<FFmpegDecoder>(formatContext, sink);
        decoders.insert({decoder->getStreamIndex(), decoder});
    }

    if(decoders.size() == 0) {
        std::string errStr = "No frame sinks were loaded in FFmpegRecorder";
        std::cerr << errStr << '\n';
        throw std::runtime_error(errStr);
    }
 
    // print stream info
    // av_dump_format(formatContext, 0, deviceName.c_str(), 0);
}


void FFmpegRecorder::start() {
    recording.store(true);

    recordingThread = std::thread([&](){
        openStream();

        AVPacket pkt;
        frame = av_frame_alloc();
        if (!frame) {
            free();
            std::cerr << "Could not allocate frame in FFmpeg Recorder\n";
            throw std::runtime_error("Could not allocate frame");
        }
        // read until there are no more frames or canceled
        while (recording.load() && av_read_frame(formatContext, &pkt) >= 0) {
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
    });
}