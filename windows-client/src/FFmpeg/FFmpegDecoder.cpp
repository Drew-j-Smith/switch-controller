
#include "FFmpegDecoder.h"

#include <stdexcept>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavdevice/avdevice.h>
#include <libavformat/avformat.h>
#include <libavutil/samplefmt.h>
#include <libavutil/timestamp.h>
#include <stdint.h>
}

void FFmpegDecoder::openCodecContext() {
    this->streamIndex = av_find_best_stream(
        formatContext, this->sink->getType(), -1, -1, nullptr, 0);
    if (this->streamIndex < 0) {
        throw std::runtime_error(
            "Could not find " +
            std::string(av_get_media_type_string(this->sink->getType())) +
            " stream in FFmpeg Decoder");
    }
    AVStream *stream = formatContext->streams[this->streamIndex];

    // find decoder for the stream
    const AVCodec *decoder = avcodec_find_decoder(stream->codecpar->codec_id);
    if (!decoder) {
        throw std::runtime_error(
            "Failed to find " +
            std::string(av_get_media_type_string(this->sink->getType())) +
            " codec in FFmpeg Decoder");
    }

    // Allocate a codec context for the decoder
    this->decoderContext = avcodec_alloc_context3(decoder);
    if (!this->decoderContext) {
        throw std::runtime_error(
            "Failed to allocate the " +
            std::string(av_get_media_type_string(this->sink->getType())) +
            " codec context in FFmpeg Decoder");
    }

    // Copy codec parameters from input stream to output codec context
    if (avcodec_parameters_to_context(this->decoderContext, stream->codecpar) <
        0) {
        throw std::runtime_error(
            "Failed to copy " +
            std::string(av_get_media_type_string(this->sink->getType())) +
            " codec parameters to decoder context in FFmpeg Decoder");
    }

    // Init the decoders
    if (avcodec_open2(this->decoderContext, decoder, nullptr) < 0) {
        throw std::runtime_error(
            "Failed to open " +
            std::string(av_get_media_type_string(this->sink->getType())) +
            " codec in FFmpeg Decoder");
    }
}

FFmpegDecoder::FFmpegDecoder(AVFormatContext *formatContext,
                             std::shared_ptr<FFmpegFrameSink> sink) {
    this->formatContext = formatContext;
    this->sink = sink;
    openCodecContext();
    sink->init(this->decoderContext);
}

void FFmpegDecoder::decodePacket(const AVPacket *packet, AVFrame *frame) {
    int ret = 0;

    // submit the packet to the decoder
    ret = avcodec_send_packet(decoderContext, packet);
    if (ret < 0) {
        char error[AV_ERROR_MAX_STRING_SIZE];
        av_make_error_string(error, AV_ERROR_MAX_STRING_SIZE, ret);
        throw std::runtime_error("Error submitting a packet for decoding: " +
                                 std::string(error));
    }

    // get all the available frames from the decoder
    while (ret >= 0) {
        ret = avcodec_receive_frame(decoderContext, frame);
        if (ret < 0) {
            // those two return values are special and mean there is no output
            // frame available, but there were no errors during decoding
            if (ret == AVERROR_EOF || ret == AVERROR(EAGAIN))
                return;

            char error[AV_ERROR_MAX_STRING_SIZE];
            av_make_error_string(error, AV_ERROR_MAX_STRING_SIZE, ret);
            throw std::runtime_error("Error during decoding (" +
                                     std::string(error) +
                                     ")  in FFmpeg Decoder");
        }

        // send frame to frame sink
        sink->outputFrame(frame);

        // dereference frame
        av_frame_unref(frame);
    }
}