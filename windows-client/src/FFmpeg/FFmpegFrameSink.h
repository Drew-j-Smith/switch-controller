#ifndef FFMPEG_FRAME_SINK_H
#define FFMPEG_FRAME_SINK_H

// TODO
// FFmpeg has a lot of warnings
// Changing level to 2 on MSCV
// This does have the potential to break 
// in the future and on other compilers
#ifdef _MSC_VER
#pragma warning( push, 2 )
#endif

extern "C" {
    #include <libavformat/avformat.h>
}

#ifdef _MSC_VER
#pragma warning( pop )
#endif

#include "pch.h"

class FFmpegFrameSink
{
private:
    mutable std::mutex initMutex;
    std::condition_variable initCV;
    bool initialized = false;

    mutable std::mutex dataMutex;
    std::condition_variable dataCV;
    long long lastFrame = 0;

protected:
    virtual void virtualInit(AVCodecContext* decoderContext) = 0;
    virtual void virtualOutputFrame(AVFrame* frame) = 0;
    virtual void getDataWithoutLock(std::vector<uint8_t>& data) = 0;

    std::unique_ptr<std::unique_lock<std::mutex>> getDataMutexLock() const {
        return std::make_unique<std::unique_lock<std::mutex>>(dataMutex);
    }

public:
    void init(AVCodecContext* decoderContext) {
        virtualInit(decoderContext);
        std::lock_guard<std::mutex> lock(initMutex);
        this->initialized = true;
        initCV.notify_all();
    }

    void waitForInit() {
        std::unique_lock<std::mutex> lock(initMutex);
        while (!initialized) {
            initCV.wait(lock);
        }
    }

    void outputFrame(AVFrame* frame) {
        std::lock_guard<std::mutex> lock(dataMutex);
        virtualOutputFrame(frame);
        lastFrame++;
        dataCV.notify_all();
    }

    long long getData(std::vector<uint8_t>& data) {
        std::lock_guard<std::mutex> lock(dataMutex);
        getDataWithoutLock(data);
        return lastFrame;
    }

    long long getNextData(std::vector<uint8_t>& data, long long lastFrameSeen) {
        std::unique_lock<std::mutex> lock(dataMutex);
        while (this->lastFrame == lastFrameSeen) {
            dataCV.wait(lock);
        }
        getDataWithoutLock(data);
        return this->lastFrame;
    }

    virtual AVMediaType getType() const = 0;
};


#endif