#ifndef IMAGE_PROCESSING_DECIDER_H
#define IMAGE_PROCESSING_DECIDER_H

#include "../pch.h"

#include <boost/property_tree/ptree.hpp>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "Decider.h"


class ImageProcessingDecider : public Decider
{
public:
    

    ImageProcessingDecider(const std::string & name, cv::Mat & templatePic,
        const cv::Mat & maskPic,
        const int     matchMethod,
        const double  matchThreshold,
        const int     minX,
        const int     minY,
        const int     maxX,
        const int     maxY);
    ImageProcessingDecider(const boost::property_tree::ptree & tree);

    int nextListIndex() const override;

    void update(const cv::Mat & screenshot);
private:
    cv::Mat templatePic;
    cv::Mat maskPic;
    int     matchMethod;
    double  matchThreshold;
    int     minX;
    int     minY;
    int     maxX;
    int     maxY;

    std::atomic<double> matchPointX;
    std::atomic<double> matchPointY;
    std::atomic<double> critalMatchVal;
};

#endif