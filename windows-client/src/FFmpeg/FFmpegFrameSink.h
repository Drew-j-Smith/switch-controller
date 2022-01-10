#ifndef FFMPEG_FRAME_SINK_H
#define FFMPEG_FRAME_SINK_H

extern "C" {
    #include <libavformat/avformat.h>
}

#include "pch.h"

class FFmpegFrameSink
{
protected:
    std::mutex initMutex;
    std::condition_variable initCV;
    bool initialized = false;
public:
    void waitForInit() {
        std::unique_lock<std::mutex> lock(initMutex);
        while (!initialized) {
            initCV.wait(lock);
        }
        lock.unlock();
    }
    void setInitialized(bool initialized) {
        std::lock_guard<std::mutex> lk(initMutex);
        this->initialized = initialized;
        initCV.notify_all();
    }

    virtual void init(AVCodecContext* decoderContext) = 0;
    virtual void outputFrame(AVFrame* frame) = 0;
    virtual AVMediaType getType() const = 0;
};


#endif