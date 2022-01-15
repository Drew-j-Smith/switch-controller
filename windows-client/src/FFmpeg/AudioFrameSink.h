#ifndef AUDIO_FRAME_SINK_H
#define AUDIO_FRAME_SINK_H

#include "FFmpegFrameSink.h"

extern "C" {
    #include <libavutil/opt.h>
    #include <libavutil/channel_layout.h>
    #include <libavutil/samplefmt.h>
    #include <libswresample/swresample.h>
}

class AudioFrameSink : public FFmpegFrameSink
{
private:
    std::vector<uint8_t> data = {};
    SwrContext* swr = nullptr;
    int bytesPerSample;
public:
    AudioFrameSink() {

    }

    ~AudioFrameSink() {
        if (swr != nullptr) {
            swr_free(&swr);
        }
    }

    void virtualInit(AVCodecContext* decoderContext) override {
        data = std::vector<uint8_t>();

        // getting the channel rate or setting it to default
        int channel_layout = decoderContext->channel_layout;
        if (channel_layout == 0) {
            channel_layout = av_get_default_channel_layout(decoderContext->channels);
        }

        // This doesn't work as intended
        // because of this I may switch to a PCM format with constant width 
        bytesPerSample = av_get_bytes_per_sample(decoderContext->sample_fmt);

        swr = swr_alloc_set_opts(NULL,      // we're allocating a new context
            AV_CH_LAYOUT_MONO,              // out_ch_layout
            AV_SAMPLE_FMT_FLT,              // out_sample_fmt
            48000,                          // out_sample_rate
            channel_layout,                 // in_ch_layout
            decoderContext->sample_fmt,     // in_sample_fmt
            decoderContext->sample_rate,    // in_sample_rate
            0,                              // log_offset
            NULL);                          // log_ctx
        int res = swr_init(swr);
        if (res < 0) {
            char error[AV_ERROR_MAX_STRING_SIZE];
            av_make_error_string(error, AV_ERROR_MAX_STRING_SIZE, res);
            std::cout << error << '\n';
            throw std::exception("Error converting audio");
        }
    }

    void virtualOutputFrame(AVFrame* frame) override {
        int out_samples = av_rescale_rnd(swr_get_delay(swr, frame->sample_rate) + frame->nb_samples, 48000, frame->sample_rate, AV_ROUND_UP);
        data.resize(data.size() + out_samples * sizeof(float));
        uint8_t* output = data.data() + data.size() - out_samples * sizeof(float);
        int res = swr_convert(swr, &output, out_samples, (const uint8_t**)frame->extended_data, frame->nb_samples);
        if (out_samples < 0) {
            char error[AV_ERROR_MAX_STRING_SIZE];
            av_make_error_string(error, AV_ERROR_MAX_STRING_SIZE, res);
            std::cout << error << '\n';
            throw std::exception("Error converting audio");
        }
    }

    void getDataWithoutLock(uint8_t* data) override {
        memcpy(data, this->data.data(), this->data.size());
    }

    long long getDataSize() const override {
        auto lock = getDataMutexLock();
        return data.size();
    };

    AVMediaType getType() const override {
        return AVMEDIA_TYPE_AUDIO;
    }
};


#endif