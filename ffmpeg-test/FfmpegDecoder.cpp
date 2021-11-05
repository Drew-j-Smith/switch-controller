
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

FFmpegDecoder::FFmpegDecoder(AVFormatContext* formatContext, AVCodecContext* decoderContext, std::shared_ptr<FfmpegFrameSink> sink) {
    this->formatContext = formatContext;
    this->decoderContext = decoderContext;
    this->sink = sink;
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