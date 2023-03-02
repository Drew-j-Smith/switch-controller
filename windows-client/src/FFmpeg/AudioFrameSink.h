#pragma once

#include "FFmpegFrameSink.h"

#include <boost/circular_buffer.hpp>

extern "C" {
#include <libavutil/channel_layout.h>
#include <libavutil/opt.h>
#include <libavutil/samplefmt.h>
#include <libswresample/swresample.h>
}

class AudioFrameSink : public FFmpegFrameSink {
private:
    std::vector<uint8_t> data;
    struct SwrContextDeleter {
        void operator()(SwrContext *s) { swr_free(&s); }
    };
    std::unique_ptr<SwrContext, SwrContextDeleter> swr;

    AVChannelLayout outChannelLayout = AV_CHANNEL_LAYOUT_MONO;
    AVSampleFormat outputSampleFormat = AV_SAMPLE_FMT_FLT;
    int outSampleRate = 48000;

    bool loopRecording = false;
    boost::circular_buffer<uint8_t> circularData;

    void virtualInit(AVCodecContext *decoderContext) override {

        spdlog::info("intitializing AudioFrameSink");

        av_channel_layout_default(&decoderContext->ch_layout,
                                  decoderContext->ch_layout.nb_channels);
        struct SwrContext *temp{nullptr};
        swr_alloc_set_opts2(&temp,             // we're allocating a new context
                            &outChannelLayout, // out_ch_layout
                            outputSampleFormat,          // out_sample_fmt
                            outSampleRate,               // out_sample_rate
                            &decoderContext->ch_layout,  // in_ch_layout
                            decoderContext->sample_fmt,  // in_sample_fmt
                            decoderContext->sample_rate, // in_sample_rate
                            0,                           // log_offset
                            nullptr);                    // log_ctx
        swr = {temp, SwrContextDeleter()};
        int res = swr_init(swr.get());
        if (res < 0) {
            char error[AV_ERROR_MAX_STRING_SIZE];
            av_make_error_string(error, AV_ERROR_MAX_STRING_SIZE, res);
            throw std::runtime_error("Error Initializing AudioFrameSink " +
                                     std::string(error));
        }
        spdlog::info("intitialized AudioFrameSink");
    }

    void virtualOutputFrame(AVFrame *frame) override {
        int64_t out_samples = av_rescale_rnd(
            swr_get_delay(swr.get(), frame->sample_rate) + frame->nb_samples,
            48000, frame->sample_rate, AV_ROUND_UP);
        uint8_t *output;
        int res;

        if (loopRecording) {
            data.resize(static_cast<std::size_t>(
                out_samples * av_get_bytes_per_sample(outputSampleFormat)));
            output = data.data();
        } else {
            data.resize(
                data.size() +
                static_cast<std::size_t>(
                    out_samples * av_get_bytes_per_sample(outputSampleFormat)));
            output = data.data() + data.size() -
                     out_samples * av_get_bytes_per_sample(outputSampleFormat);
        }

        res = swr_convert(swr.get(), &output, static_cast<int>(out_samples),
                          const_cast<const uint8_t **>(frame->extended_data),
                          frame->nb_samples);
        if (res < 0) {
            char error[AV_ERROR_MAX_STRING_SIZE];
            av_make_error_string(error, AV_ERROR_MAX_STRING_SIZE, res);
            throw std::runtime_error(
                "Error converting audio in AudioFrameSink: " +
                std::string(error));
        }

        if (loopRecording) {
            circularData.insert(circularData.end(), data.begin(), data.end());
        }
    }

    void getDataWithoutLock(std::vector<uint8_t> &dataCopy) override {
        if (loopRecording) {
            dataCopy.resize(this->circularData.size());
            std::copy(this->circularData.begin(), this->circularData.end(),
                      dataCopy.begin());
        } else {
            dataCopy.resize(this->data.size());
            std::copy(this->data.begin(), this->data.end(), dataCopy.begin());
        }
    }

public:
    AudioFrameSink() {}

    AudioFrameSink(AVChannelLayout outChannelLayout,
                   AVSampleFormat outputSampleFormat, int outSampleRate,
                   bool loopRecording = false, int64_t loopBufferSize = 0)
        : outChannelLayout(outChannelLayout),
          outputSampleFormat(outputSampleFormat), outSampleRate(outSampleRate),
          loopRecording(loopRecording) {
        if (loopRecording) {
            circularData.resize(static_cast<std::size_t>(
                loopBufferSize * av_get_bytes_per_sample(outputSampleFormat)));
        }
    }

    AVMediaType getType() const override { return AVMEDIA_TYPE_AUDIO; }
};
