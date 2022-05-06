#ifndef IMAGE_EVENT_H
#define IMAGE_EVENT_H

#include "pch.h"

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include "FFmpeg/VideoFrameSink.h"

#include "Event/Event.h"

class ImageEvent : public Event {
public:
    ImageEvent(const cv::Mat &templatePic, const cv::Mat &maskPic,
               const int matchMethod, const double matchThreshold,
               const cv::Rect imageCrop, VideoFrameSink *videoFrameSink);

    uint8_t value() const override;

private:
    const cv::Mat templatePic;
    const cv::Mat maskPic;
    const int matchMethod;
    const double matchThreshold;
    const cv::Rect imageCrop;

    VideoFrameSink *videoFrameSink;
    mutable std::vector<uint8_t> videoData;
};

#endif