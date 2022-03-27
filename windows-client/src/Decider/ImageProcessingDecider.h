#ifndef IMAGE_PROCESSING_DECIDER_H
#define IMAGE_PROCESSING_DECIDER_H

#include "pch.h"

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include "FFmpeg/VideoFrameSink.h"

#include "Decider.h"

class ImageProcessingDecider : public Decider {
public:
    ImageProcessingDecider(
        const cv::Mat &templatePic, const cv::Mat &maskPic,
        const int matchMethod, const double matchThreshold, const int minX,
        const int minY, const int maxX, const int maxY,
        const std::shared_ptr<VideoFrameSink> &videoFrameSink);

    int nextListIndex() const override;

    void update() override;

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

    std::atomic<double> matchPointX;
    std::atomic<double> matchPointY;
    std::atomic<double> critalMatchVal;
};

#endif