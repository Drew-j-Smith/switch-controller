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
               const int minX, const int minY, const int maxX, const int maxY,
               const std::shared_ptr<VideoFrameSink> &videoFrameSink);

    void update() override {}

    uint8_t value() const override;

private:
    cv::Mat templatePic;
    cv::Mat maskPic;
    int matchMethod;
    double matchThreshold;
    int minX;
    int minY;
    int maxX;
    int maxY;

    std::shared_ptr<VideoFrameSink> videoFrameSink;
    mutable std::vector<uint8_t> videoData;
};

#endif