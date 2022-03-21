#ifndef FFMPEG_FRAME_SINK_H
#define FFMPEG_FRAME_SINK_H

// TODO
// FFmpeg has a lot of warnings
// Changing level to 2 on MSCV
// This does have the potential to break
// in the future and on other compilers
#ifdef _MSC_VER
#pragma warning(push, 2)
#endif

extern "C" {
#include <libavformat/avformat.h>
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#include "pch.h"

#include <boost/lockfree/queue.hpp>

class FFmpegFrameSink {
private:
    bool initialized = false;
    mutable std::mutex mutex;
    std::condition_variable conditionVariable;
    long long lastFrame = 0;

    std::vector<std::shared_ptr<std::vector<uint8_t>>> dataArr;
    std::map<std::vector<uint8_t> *, std::shared_ptr<std::atomic<uint64_t>>>
        dataCount;
    std::vector<uint8_t> *previousData;
    boost::lockfree::queue<std::vector<uint8_t> *,
                           boost::lockfree::capacity<10>>
        availablePointers;

    long long getDataInternal(std::vector<uint8_t> *&data) const {
        data = previousData;
        dataCount.at(previousData)->fetch_add(1);
        return lastFrame;
    }

protected:
    virtual void getDataVirtual(AVFrame *frame, std::vector<uint8_t> &data) = 0;

public:
    virtual ~FFmpegFrameSink() {}

    virtual void init(AVCodecContext *decoderContext) = 0;

    void waitForInit() {
        std::unique_lock<std::mutex> lock(mutex);
        while (!initialized) {
            conditionVariable.wait(lock);
        }
    }

    void outputFrame(AVFrame *frame) {
        std::lock_guard<std::mutex> lock(mutex);

        if (initialized) {
            returnPointer(previousData);
        } else {
            // ensuring that previousData is never null
            initialized = true;
        }

        if (!availablePointers.pop(previousData)) {
            dataArr.push_back(std::make_shared<std::vector<uint8_t>>());
            previousData = dataArr.back().get();
            dataCount[previousData] = std::make_shared<std::atomic<uint64_t>>();
        }

        dataCount[previousData]->store(1);
        lastFrame++;
        getDataVirtual(frame, *previousData);
        conditionVariable.notify_all();
    }

    void returnPointer(std::vector<uint8_t> *data) {
        if (dataCount.at(data)->fetch_sub(1) == 1) {
            availablePointers.push(data);
        }
    }

    long long getData(std::vector<uint8_t> *&data) {
        std::lock_guard<std::mutex> lock(mutex);
        return getDataInternal(data);
    }

    long long getNextData(std::vector<uint8_t> *&data,
                          long long lastFrameSeen) {
        std::unique_lock<std::mutex> lock(mutex);
        while (this->lastFrame == lastFrameSeen) {
            conditionVariable.wait(lock);
        }
        return getDataInternal(data);
    }

    virtual AVMediaType getType() const = 0;
};

#endif