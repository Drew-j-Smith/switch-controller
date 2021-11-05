#include "FfmpegRecorder.h"

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

FfmpegRecorder::FfmpegRecorder(std::string inputFormat, std::string deviceName, std::map<std::string, std::string> options, std::vector<std::shared_ptr<FfmpegFrameSink>> sinks) {
    if (checkOverlap(sinks))
        throw std::runtime_error("Only one frame sink of each type may be used");
    if (sinks.size() == 0)
        throw std::runtime_error("The sink size must be greater than 0");
    
    this->inputFormat = inputFormat;
    this->deviceName = deviceName;
    this->options = options;
    this->sinks = sinks;
}

bool FfmpegRecorder::checkOverlap(std::vector<std::shared_ptr<FfmpegFrameSink>> sinks) {
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

void FfmpegRecorder::free() {
    recording.store(false);
    decoders.clear();
    avformat_close_input(&formatContext);
    av_frame_free(&frame);
}

void FfmpegRecorder::openStream(std::string inputFormatStr, std::string deviceName, std::map<std::string, std::string> optionsMap, std::vector<std::shared_ptr<FfmpegFrameSink>> sinks) {
    avdevice_register_all();

    // finding input format and setting options dict
    AVInputFormat* inputFormat = NULL;
    if (inputFormatStr.size() > 0) { // If no input format is set, load a file
        inputFormat = av_find_input_format(inputFormatStr.c_str());
    }
    AVDictionary* options = NULL;
    for (auto& pair : optionsMap) {
        av_dict_set(&options, pair.first.c_str(), pair.second.c_str(), 0);
    }
 
    // open input file, and allocate format context
    if (avformat_open_input(&formatContext, deviceName.c_str(), inputFormat, &options) < 0) {
        free();
        throw std::runtime_error("Could not open stream");
    }
 
    // retrieve stream information
    if (avformat_find_stream_info(formatContext, NULL) < 0) {
        free();
        throw std::runtime_error("Could not find stream information");
    }
    
    // open stream ctx for each frame sink
    int streamIndex = -1;
    AVCodecContext* decoderContext = NULL;
    for (auto sink : sinks) {
        openCodecContext(&streamIndex, &decoderContext, formatContext, sink->getType());
        decoders.insert({streamIndex, std::make_shared<FFmpegDecoder>(formatContext, decoderContext, sink)});
        sink->init(decoderContext);
    }

    if(decoders.size() == 0)
        throw std::runtime_error("No frame sinks were loaded");
 
    // print stream info
    // av_dump_format(formatContext, 0, deviceName.c_str(), 0);
}

void FfmpegRecorder::openCodecContext(int *streamIndex, AVCodecContext **decoderContext, AVFormatContext *formatContext, enum AVMediaType type)
{
    *streamIndex = av_find_best_stream(formatContext, type, -1, -1, NULL, 0);
    if (*streamIndex < 0) {
        free();
        throw std::runtime_error("Could not find " + std::string(av_get_media_type_string(type)) + " stream");
    }
    AVStream* stream = formatContext->streams[*streamIndex];

    // find decoder for the stream
    const AVCodec* decoder = avcodec_find_decoder(stream->codecpar->codec_id);
    if (!decoder) {
        free();
        throw std::runtime_error("Failed to find " + std::string(av_get_media_type_string(type)) + " codec");
    }

    // Allocate a codec context for the decoder
    *decoderContext = avcodec_alloc_context3(decoder);
    if (!*decoderContext) {
        free();
        throw std::runtime_error("Failed to allocate the " + std::string(av_get_media_type_string(type)) + " codec context");
    }

    // Copy codec parameters from input stream to output codec context
    if (avcodec_parameters_to_context(*decoderContext, stream->codecpar) < 0) {
        free();
        throw std::runtime_error("Failed to copy " + std::string(av_get_media_type_string(type)) + " codec parameters to decoder context");
    }

    // Init the decoders
    if (avcodec_open2(*decoderContext, decoder, NULL) < 0) {
        free();
        throw std::runtime_error("Failed to open " + std::string(av_get_media_type_string(type)) + " codec");
    }
}


void FfmpegRecorder::start() {
    openStream(inputFormat, deviceName, options, sinks);

    AVPacket pkt;
    frame = av_frame_alloc();
    if (!frame) {
        free();
        throw std::runtime_error("Could not allocate frame");
    }
    recording.store(true);

    recordingThread = std::thread([&](){
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
            it.second->decodePacket(NULL, frame);
        }

        free();
    });
}