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
    if (checkOverlap(sinks)) {
        std::string errStr = "Only one frame sink of each type may be used";
        std::cerr << errStr << '\n';
        throw std::runtime_error(errStr);
    }
    if (sinks.size() == 0) {
        std::string errStr = "The sink size must be greater than 0";
        std::cerr << errStr << '\n';
        throw std::runtime_error(errStr);
    }
    
    this->inputFormat = inputFormat;
    this->deviceName = deviceName;
    this->options = options;
    this->sinks = sinks;
}

bool FFmpegRecorder::checkOverlap(std::vector<std::shared_ptr<FFmpegFrameSink>> sinks) {
    std::set<AVMediaType> types;
    for (auto sink : sinks) {
        if (types.find(sink->getType()) == types.end()) {
            types.insert(sink->getType());
        } else {
            return true;
        }
    }
    return false;
}

void FFmpegRecorder::free() {
    recording.store(false);
    decoders.clear();
    avformat_close_input(&formatContext);
    av_frame_free(&frame);
}

void FFmpegRecorder::openStream(std::string inputFormatStr, std::string deviceName, std::map<std::string, std::string> optionsMap, std::vector<std::shared_ptr<FFmpegFrameSink>> sinks) {
    avdevice_register_all();

    // finding input format and setting options dict
    AVInputFormat* inputFormat = nullptr;
    if (inputFormatStr.size() > 0) { // If no input format is set, load a file
        inputFormat = av_find_input_format(inputFormatStr.c_str());
    }
    AVDictionary* options = nullptr;
    for (auto& pair : optionsMap) {
        av_dict_set(&options, pair.first.c_str(), pair.second.c_str(), 0);
    }
 
    // open input file, and allocate format context
    if (avformat_open_input(&formatContext, deviceName.c_str(), inputFormat, &options) < 0) {
        free();
        std::string errStr = "Could not open stream " + deviceName + " in FFmpeg Recorder";
        std::cerr << errStr << '\n';
        throw std::runtime_error(errStr);
    }
 
    // retrieve stream information
    if (avformat_find_stream_info(formatContext, nullptr) < 0) {
        free();
        std::string errStr = "Could not find stream information for " + deviceName + " in FFmpeg Recorder";
        std::cerr << errStr << '\n';
        throw std::runtime_error(errStr);
    }
    
    // open stream ctx for each frame sink
    for (auto sink : sinks) {
        try {
            std::shared_ptr<FFmpegDecoder> decoder = std::make_shared<FFmpegDecoder>(formatContext, sink);
            decoders.insert({decoder->getStreamIndex(), decoder});
        }
        catch (std::exception& e) {
            free();
            throw;
        }
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
        openStream(inputFormat, deviceName, options, sinks);

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
                try {
                    decoder->decodePacket(&pkt, frame);
                }
                catch (std::runtime_error& e) {
                    free();
                    throw;
                }
                
            }
            av_packet_unref(&pkt);
        }

        // flushing decoders
        for (auto it : decoders) {
            try { 
                it.second->decodePacket(nullptr, frame);
            }
            catch (std::runtime_error& e) {
                free();
                throw;
            }
        }

        free();
    });
}