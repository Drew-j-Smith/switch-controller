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
    for (auto streamIdCtxSink : streamIdCtxSinkMap) {
        auto streamCtx = streamIdCtxSink.second.second;
        avcodec_free_context(&streamCtx);
    }
    streamIdCtxSinkMap.clear();
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
        streamIdCtxSinkMap.insert({streamIndex, {sink, decoderContext}});
    }

    if(streamIdCtxSinkMap.size() == 0)
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

void FfmpegRecorder::decodePacket(AVCodecContext *decoderContext, const AVPacket *packet, AVFrame* frame, std::shared_ptr<FfmpegFrameSink> sink) {
    int ret = 0;
 
    // submit the packet to the decoder
    ret = avcodec_send_packet(decoderContext, packet);
    if (ret < 0) {
        free();
        char error[AV_ERROR_MAX_STRING_SIZE];
        throw std::runtime_error("Error submitting a packet for decoding (" + std::string(av_make_error_string(error, AV_ERROR_MAX_STRING_SIZE, ret)) + ")");
    }
 
    // get all the available frames from the decoder
    while (ret >= 0) {
        ret = avcodec_receive_frame(decoderContext, frame);
        if (ret < 0) {
            // those two return values are special and mean there is no output
            // frame available, but there were no errors during decoding
            if (ret == AVERROR_EOF || ret == AVERROR(EAGAIN))
                return;

            free();
            char error[AV_ERROR_MAX_STRING_SIZE];
            throw std::runtime_error("Error during decoding (" + std::string(av_make_error_string(error, AV_ERROR_MAX_STRING_SIZE, ret)) + ")");
        }
 
        // send frame to frame sink
        auto streamIdCtxSink = streamIdCtxSinkMap.at(decoderContext->codec->type);
        streamIdCtxSink.first->outputFrame(frame);
 
        // dereference frame
        av_frame_unref(frame);
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
        std::cout << "1\n";
        while (recording.load() && av_read_frame(formatContext, &pkt) >= 0) {
            // check if the pack goes to a frame sink
            std::cout << "2\n";
            if (streamIdCtxSinkMap.find(pkt.stream_index) != streamIdCtxSinkMap.end()) {
                auto streamIdCtxSink = streamIdCtxSinkMap.at(pkt.stream_index);
                decodePacket(streamIdCtxSink.second, &pkt, frame, streamIdCtxSink.first);
            }
            av_packet_unref(&pkt);
        }

        // flushing decoders
        for (auto it : streamIdCtxSinkMap) {
            decodePacket(it.second.second, NULL, frame, it.second.first);
        }

        free();
    });
}