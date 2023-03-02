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

constexpr static auto formatContextDeleter = [](AVFormatContext *c) {
    avformat_close_input(&c);
};

static auto openStream(const std::string &inputFormatStr,
                       const std::string &deviceNameStr,
                       const std::map<std::string, std::string> &options,
                       std::vector<std::unique_ptr<FFmpegFrameSink>> &sinks) {
    avdevice_register_all();

    // finding input format and setting options dict
    const AVInputFormat *inputFormat = nullptr;
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
        std::unique_ptr<AVFormatContext, decltype(formatContextDeleter)>{
            formatContextPtr, formatContextDeleter};

    // retrieve stream information
    if (avformat_find_stream_info(formatContext.get(), nullptr) < 0) {
        throw std::runtime_error("Could not find stream information for " +
                                 deviceNameStr);
    }

    // open stream ctx for each frame sink
    std::map<int, std::unique_ptr<FFmpegDecoder>> decoders;
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

    return std::make_tuple(std::move(formatContext), std::move(decoders));
}

FFmpegRecorder::FFmpegRecorder(
    const std::string &inputFormat, const std::string &deviceName,
    const std::map<std::string, std::string> &options,
    std::vector<std::unique_ptr<FFmpegFrameSink>> &sinks)
    : m_recording(true) {
    if (std::unique(sinks.begin(), sinks.end(),
                    [](const std::unique_ptr<FFmpegFrameSink> &p1,
                       const std::unique_ptr<FFmpegFrameSink> &p2) {
                        return p1->getType() == p2->getType();
                    }) != sinks.end()) {
        throw std::invalid_argument(
            "Only one frame sink of each type may be used");
    }
    if (sinks.size() == 0) {
        throw std::invalid_argument("The sink size must be greater than 0");
    }

    m_thread = std::thread([&]() {
        try {
            auto [formatContext, decoders] =
                openStream(inputFormat, deviceName, options, sinks);

            AVPacket pkt;

            constexpr auto FrameDeleter = [](AVFrame *f) { av_frame_free(&f); };
            auto frame = std::unique_ptr<AVFrame, decltype(FrameDeleter)>{
                av_frame_alloc(), FrameDeleter};

            if (!frame) {
                throw std::runtime_error("Could not allocate frame");
            }
            // read until there are no more frames or canceled
            while (m_recording.load() &&
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
            spdlog::error("Uncaught exception in FFmpeg Recorder: {}",
                          e.what());
            std::terminate();
        }
    });

    for (auto &sink : sinks) {
        sink->waitForInit();
    }
}