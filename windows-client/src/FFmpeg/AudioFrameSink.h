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
    std::vector<uint8_t> data;
    SwrContext* swr = nullptr;

    int64_t outChannelLayout = AV_CH_LAYOUT_MONO;
    AVSampleFormat outputSampleFormat = AV_SAMPLE_FMT_FLT;
    int outSampleRate = 48000;

    bool loopRecording = false;
    std::vector<uint8_t> dataBuffer;
    int64_t loopBufferPosition = 0;
public:
    AudioFrameSink() {}

    AudioFrameSink(int64_t outChannelLayout, AVSampleFormat outputSampleFormat, int outSampleRate, bool loopRecording = false, int64_t loopBufferSize = 0) :
        outChannelLayout(outChannelLayout),
        outputSampleFormat(outputSampleFormat),
        outSampleRate(outSampleRate),
        loopRecording(loopRecording) {
        if (loopRecording) {
            // I don't know the best way to do this but I am just going 
            // to set the buffer to be 1 sec. of audio data
            dataBuffer.resize(outSampleRate * av_get_bytes_per_sample(outputSampleFormat));
            data.resize(loopBufferSize * av_get_bytes_per_sample(outputSampleFormat));
        }
    }

    ~AudioFrameSink() override {
        swr_free(&swr);
    }

    void virtualInit(AVCodecContext* decoderContext) override {
        // getting the channel layout or setting it to default
        uint64_t channel_layout = decoderContext->channel_layout;
        if (channel_layout == 0) {
            channel_layout = av_get_default_channel_layout(decoderContext->channels);
        }

        swr = swr_alloc_set_opts(nullptr,      // we're allocating a new context
            outChannelLayout,               // out_ch_layout
            outputSampleFormat,             // out_sample_fmt
            outSampleRate,                  // out_sample_rate
            channel_layout,                 // in_ch_layout
            decoderContext->sample_fmt,     // in_sample_fmt
            decoderContext->sample_rate,    // in_sample_rate
            0,                              // log_offset
            nullptr);                          // log_ctx
        int res = swr_init(swr);
        if (res < 0) {
            char error[AV_ERROR_MAX_STRING_SIZE];
            av_make_error_string(error, AV_ERROR_MAX_STRING_SIZE, res);
            std::cerr << "Error Initializing AudioFrameSink\n" << error << '\n';
            throw std::exception("Error Initializing AudioFrameSink");
        }
    }

    void virtualOutputFrame(AVFrame* frame) override {
        int64_t out_samples = av_rescale_rnd(swr_get_delay(swr, frame->sample_rate) + frame->nb_samples, 48000, frame->sample_rate, AV_ROUND_UP);
        uint8_t* output;
        int res;

        if (loopRecording) {
            output = dataBuffer.data();
        } else {
            data.resize(data.size() + out_samples * av_get_bytes_per_sample(outputSampleFormat));
            output = data.data() + data.size() - out_samples * av_get_bytes_per_sample(outputSampleFormat);
        }

        res = swr_convert(swr, &output, (int)out_samples, (const uint8_t**)frame->extended_data, frame->nb_samples);
        if (res < 0) {
            char error[AV_ERROR_MAX_STRING_SIZE];
            av_make_error_string(error, AV_ERROR_MAX_STRING_SIZE, res);
            std::cerr << "Error converting audio in AudioFrameSink" << error << '\n';
            throw std::exception("Error converting audio");
        }

        if (loopRecording) {
            if (loopBufferPosition + out_samples * av_get_bytes_per_sample(outputSampleFormat) >= (int64_t)data.size()) {
                std::copy(dataBuffer.begin(),
                          dataBuffer.begin() + (data.size() - loopBufferPosition),
                          data.begin() + loopBufferPosition);
                std::copy(dataBuffer.begin() + (data.size() - loopBufferPosition),
                          dataBuffer.begin() + out_samples * av_get_bytes_per_sample(outputSampleFormat),
                          data.begin());
                loopBufferPosition -= data.size();
            } else {
                std::copy(dataBuffer.begin(),
                          dataBuffer.begin() + out_samples * av_get_bytes_per_sample(outputSampleFormat),
                          data.begin() + loopBufferPosition);
            }
            loopBufferPosition += out_samples * av_get_bytes_per_sample(outputSampleFormat);
        }
    }

    void getDataWithoutLock(std::vector<uint8_t>& dataCopy) override {
        dataCopy.resize(this->data.size());
        if (loopRecording) {
            std::copy(this->data.begin() + loopBufferPosition, this->data.end(), dataCopy.begin());
            std::copy(this->data.begin(), this->data.begin() + loopBufferPosition, dataCopy.begin() + (this->data.size() - loopBufferPosition));
        } else {
            std::copy(this->data.begin(), this->data.end(), data.begin());
        }
    }

    AVMediaType getType() const override {
        return AVMEDIA_TYPE_AUDIO;
    }
};


#endif