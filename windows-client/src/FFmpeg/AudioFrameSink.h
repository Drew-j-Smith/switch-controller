#ifndef AUDIO_FRAME_SINK_H
#define AUDIO_FRAME_SINK_H

#include "ErrorTypes/FFmpegError.h"
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
    SwrContext *swr = nullptr;

    int64_t outChannelLayout = AV_CH_LAYOUT_MONO;
    AVSampleFormat outputSampleFormat = AV_SAMPLE_FMT_FLT;
    int outSampleRate = 48000;

    bool loopRecording = false;
    boost::circular_buffer<uint8_t> circularData;

    void virtualInit(AVCodecContext *decoderContext) override {
        // getting the channel layout or setting it to default
        uint64_t channel_layout = decoderContext->channel_layout;
        if (channel_layout == 0) {
            channel_layout =
                av_get_default_channel_layout(decoderContext->channels);
        }

        swr = swr_alloc_set_opts(nullptr, // we're allocating a new context
                                 outChannelLayout,            // out_ch_layout
                                 outputSampleFormat,          // out_sample_fmt
                                 outSampleRate,               // out_sample_rate
                                 channel_layout,              // in_ch_layout
                                 decoderContext->sample_fmt,  // in_sample_fmt
                                 decoderContext->sample_rate, // in_sample_rate
                                 0,                           // log_offset
                                 nullptr);                    // log_ctx
        int res = swr_init(swr);
        if (res < 0) {
            char error[AV_ERROR_MAX_STRING_SIZE];
            av_make_error_string(error, AV_ERROR_MAX_STRING_SIZE, res);
            throw FFmpegInitError("Error Initializing AudioFrameSink " +
                                  std::string(error));
        }
    }

    void virtualOutputFrame(AVFrame *frame) override {
        int64_t out_samples = av_rescale_rnd(
            swr_get_delay(swr, frame->sample_rate) + frame->nb_samples, 48000,
            frame->sample_rate, AV_ROUND_UP);
        uint8_t *output;
        int res;

        if (loopRecording) {
            data.resize(out_samples *
                        av_get_bytes_per_sample(outputSampleFormat));
            output = data.data();
        } else {
            data.resize(data.size() + out_samples * av_get_bytes_per_sample(
                                                        outputSampleFormat));
            output = data.data() + data.size() -
                     out_samples * av_get_bytes_per_sample(outputSampleFormat);
        }

        res = swr_convert(swr, &output, (int)out_samples,
                          (const uint8_t **)frame->extended_data,
                          frame->nb_samples);
        if (res < 0) {
            char error[AV_ERROR_MAX_STRING_SIZE];
            av_make_error_string(error, AV_ERROR_MAX_STRING_SIZE, res);
            throw FFmpegRuntimeError(
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

    AudioFrameSink(int64_t outChannelLayout, AVSampleFormat outputSampleFormat,
                   int outSampleRate, bool loopRecording = false,
                   int64_t loopBufferSize = 0)
        : outChannelLayout(outChannelLayout),
          outputSampleFormat(outputSampleFormat), outSampleRate(outSampleRate),
          loopRecording(loopRecording) {
        if (loopRecording) {
            circularData.resize(loopBufferSize *
                                av_get_bytes_per_sample(outputSampleFormat));
        }
    }

    ~AudioFrameSink() override { swr_free(&swr); }

    AVMediaType getType() const override { return AVMEDIA_TYPE_AUDIO; }
};

#endif