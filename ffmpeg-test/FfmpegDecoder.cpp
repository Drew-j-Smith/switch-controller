
#include "FfmpegDecoder.h"

#include <stdexcept>

extern "C" {
    #include <stdint.h>
    #include <libavutil/samplefmt.h>
    #include <libavutil/timestamp.h>
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libavdevice/avdevice.h>
}

void FFmpegDecoder::openCodecContext()
{
    this->streamIndex = av_find_best_stream(formatContext, this->sink->getType(), -1, -1, NULL, 0);
    if (this->streamIndex < 0) {
        free();
        throw std::runtime_error("Could not find " + std::string(av_get_media_type_string(this->sink->getType())) + " stream");
    }
    AVStream* stream = formatContext->streams[this->streamIndex];

    // find decoder for the stream
    const AVCodec* decoder = avcodec_find_decoder(stream->codecpar->codec_id);
    if (!decoder) {
        free();
        throw std::runtime_error("Failed to find " + std::string(av_get_media_type_string(this->sink->getType())) + " codec");
    }

    // Allocate a codec context for the decoder
    this->decoderContext = avcodec_alloc_context3(decoder);
    if (!this->decoderContext) {
        free();
        throw std::runtime_error("Failed to allocate the " + std::string(av_get_media_type_string(this->sink->getType())) + " codec context");
    }

    // Copy codec parameters from input stream to output codec context
    if (avcodec_parameters_to_context(this->decoderContext, stream->codecpar) < 0) {
        free();
        throw std::runtime_error("Failed to copy " + std::string(av_get_media_type_string(this->sink->getType())) + " codec parameters to decoder context");
    }

    // Init the decoders
    if (avcodec_open2(this->decoderContext, decoder, NULL) < 0) {
        free();
        throw std::runtime_error("Failed to open " + std::string(av_get_media_type_string(this->sink->getType())) + " codec");
    }
}

FFmpegDecoder::FFmpegDecoder(AVFormatContext* formatContext, std::shared_ptr<FfmpegFrameSink> sink) {
    this->formatContext = formatContext;
    this->sink = sink;
    openCodecContext();
    sink->init(this->decoderContext);
}

void FFmpegDecoder::free() {
    avcodec_free_context(&decoderContext);
}

void FFmpegDecoder::decodePacket(const AVPacket* packet, AVFrame* frame) {
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
        sink->outputFrame(frame);
 
        // dereference frame
        av_frame_unref(frame);
    }
}