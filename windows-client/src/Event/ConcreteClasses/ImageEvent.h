#pragma once

#include "pch.h"

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include "FFmpeg/VideoFrameSink.h"

class ImageEvent {
public:
    ImageEvent(const cv::Mat &templatePic, const cv::Mat &maskPic,
               const int matchMethod, const double matchThreshold,
               const cv::Rect imageCrop, VideoFrameSink *videoFrameSink);

    uint8_t value() const;

private:
    const cv::Mat templatePic;
    const cv::Mat maskPic;
    const int matchMethod;
    const double matchThreshold;
    const cv::Rect imageCrop;

    VideoFrameSink *videoFrameSink;
    mutable std::vector<uint8_t> videoData;
};
