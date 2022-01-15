
#include "FFmpegDecoder.h"

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
    this->streamIndex = av_find_best_stream(formatContext, this->sink->getType(), -1, -1, nullptr, 0);
    if (this->streamIndex < 0) {
        free();
        std::string errStr = "Could not find " + std::string(av_get_media_type_string(this->sink->getType())) + " stream in FFmpeg Decoder";
        std::cerr << errStr << '\n';
        throw std::runtime_error(errStr);
    }
    AVStream* stream = formatContext->streams[this->streamIndex];

    // find decoder for the stream
    const AVCodec* decoder = avcodec_find_decoder(stream->codecpar->codec_id);
    if (!decoder) {
        free();
        std::string errStr = "Failed to find " + std::string(av_get_media_type_string(this->sink->getType())) + " codec in FFmpeg Decoder";
        std::cerr << errStr << '\n';
        throw std::runtime_error(errStr);
    }

    // Allocate a codec context for the decoder
    this->decoderContext = avcodec_alloc_context3(decoder);
    if (!this->decoderContext) {
        free();
        std::string errStr = "Failed to allocate the " + std::string(av_get_media_type_string(this->sink->getType())) + " codec context in FFmpeg Decoder";
        std::cerr << errStr << '\n';
        throw std::runtime_error(errStr);
    }

    // Copy codec parameters from input stream to output codec context
    if (avcodec_parameters_to_context(this->decoderContext, stream->codecpar) < 0) {
        free();
        std::string errStr = "Failed to copy " + std::string(av_get_media_type_string(this->sink->getType())) + " codec parameters to decoder context in FFmpeg Decoder";
        std::cerr << errStr << '\n';
        throw std::runtime_error(errStr);
    }

    // Init the decoders
    if (avcodec_open2(this->decoderContext, decoder, nullptr) < 0) {
        free();
        std::string errStr = "Failed to open " + std::string(av_get_media_type_string(this->sink->getType())) + " codec in FFmpeg Decoder";
        std::cerr << errStr << '\n';
        throw std::runtime_error(errStr);
    }
}

FFmpegDecoder::FFmpegDecoder(AVFormatContext* formatContext, std::shared_ptr<FFmpegFrameSink> sink) {
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
        std::string errStr(av_make_error_string(error, AV_ERROR_MAX_STRING_SIZE, ret));
        std::cerr << "Error submitting a packet for decoding (" << errStr << ")\n";
        throw std::runtime_error("Error submitting a packet for decoding (" + errStr + ")");
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
            std::string errStr(av_make_error_string(error, AV_ERROR_MAX_STRING_SIZE, ret));
            std::cerr << "Error during decoding (" << errStr << ")  in FFmpeg Decoder\n";
            throw std::runtime_error("Error during decoding (" + errStr + ")  in FFmpeg Decoder");
        }
 
        // send frame to frame sink
        sink->outputFrame(frame);
 
        // dereference frame
        av_frame_unref(frame);
    }
}