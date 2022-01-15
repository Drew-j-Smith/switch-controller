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
		std::cout << decoderContext->channel_layout << '\n' << decoderContext->sample_fmt << '\n' << decoderContext->sample_rate << '\n';
        swr = swr_alloc_set_opts(NULL,      // we're allocating a new context
            AV_CH_LAYOUT_MONO,              // out_ch_layout
            AV_SAMPLE_FMT_FLT,              // out_sample_fmt
            48000,                          // out_sample_rate
			AV_CH_LAYOUT_STEREO,            // in_ch_layout TODO
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