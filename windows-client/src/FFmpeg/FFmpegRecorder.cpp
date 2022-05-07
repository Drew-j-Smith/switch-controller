#include "FFmpegRecorder.h"

#include "pch.h"

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
    std::vector<std::unique_ptr<FFmpegFrameSink>> &&sinks)
    : sinks(std::move(sinks)) {
    std::set<AVMediaType> types;
    bool overlap = false;
    for (auto &sink : sinks) {
        if (types.find(sink->getType()) == types.end()) {
            types.insert(sink->getType());
        } else {
            overlap = true;
        }
    }
    if (overlap) {
        throw std::invalid_argument(
            "Only one frame sink of each type may be used");
    }
    if (this->sinks.size() == 0) {
        throw std::invalid_argument("The sink size must be greater than 0");
    }

    this->inputFormatStr = inputFormat;
    this->deviceNameStr = deviceName;
    this->options = options;
}

std::unique_ptr<AVFormatContext, decltype(FFmpegRecorder::formatContextDeleter)>
FFmpegRecorder::openStream() {
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
    AVFormatContext *formatContextPtr = nullptr;
    if (avformat_open_input(&formatContextPtr, deviceNameStr.c_str(),
                            inputFormat, &optionsDic) < 0) {
        throw std::runtime_error("Could not open stream " + deviceNameStr);
    }

    auto formatContext =
        std::unique_ptr<AVFormatContext,
                        decltype(FFmpegRecorder::formatContextDeleter)>{
            formatContextPtr, formatContextDeleter};

    // retrieve stream information
    if (avformat_find_stream_info(formatContext.get(), nullptr) < 0) {
        throw std::runtime_error("Could not find stream information for " +
                                 deviceNameStr);
    }

    // open stream ctx for each frame sink
    for (auto &sink : sinks) {
        std::unique_ptr<FFmpegDecoder> decoder =
            std::make_unique<FFmpegDecoder>(formatContext.get(), sink);
        int streamIndex = decoder->getStreamIndex();
        decoders.insert({streamIndex, std::move(decoder)});
    }

    if (decoders.size() == 0) {
        throw std::runtime_error(
            "No frame sinks were loaded in FFmpegRecorder");
    }

    // print stream info
    // av_dump_format(formatContext, 0, deviceName.c_str(), 0);

    return formatContext;
}

void FFmpegRecorder::start() {
    recording.store(true);

    recordingThread = std::thread([&]() {
        try {
            auto formatContext = openStream();

            AVPacket pkt;

            constexpr auto FrameDeleter = [](AVFrame *f) { av_frame_free(&f); };
            auto frame = std::unique_ptr<AVFrame, decltype(FrameDeleter)>{
                av_frame_alloc(), FrameDeleter};

            if (!frame) {
                throw std::runtime_error("Could not allocate frame");
            }
            // read until there are no more frames or canceled
            while (recording.load() &&
                   av_read_frame(formatContext.get(), &pkt) >= 0) {
                // check if the pack goes to a frame sink
                if (decoders.find(pkt.stream_index) != decoders.end()) {
                    auto &decoder = decoders.at(pkt.stream_index);
                    decoder->decodePacket(&pkt, frame.get());
                }
                av_packet_unref(&pkt);
            }

            // flushing decoders
            for (auto &it : decoders) {
                it.second->decodePacket(nullptr, frame.get());
            }

        } catch (std::exception &e) {
            std::cerr << "Uncaught exception in FFmpeg Recorder: " +
                             std::string(e.what()) + "\n";
            throw;
        }
    });

    for (auto &sink : sinks) {
        sink->waitForInit();
    }
}