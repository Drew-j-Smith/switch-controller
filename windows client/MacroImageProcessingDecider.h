#ifndef MACRO_IMAGE_PROCESSING_DECIDER_H
#define MACRO_IMAGE_PROCESSING_DECIDER_H

#include "pch.h"

#include <boost/property_tree/ptree.hpp>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "MacroDecider.h"


class MacroImageProcessingDecider : public MacroDecider
{
public:
    struct ImageProcessingInfo {
        cv::Mat templatePic;
        cv::Mat maskPic;
        int     matchMethod;
        double  matchThreshold;
        int     minX;
        int     minY;
        int     maxX;
        int     maxY;
    };

    MacroImageProcessingDecider(const std::string & name, const ImageProcessingInfo & imageProcessingInfo);
    MacroImageProcessingDecider(const boost::property_tree::ptree & tree);

    int nextMacroListIndex() const override;

    void update(const cv::Mat & screenshot);
private:
    ImageProcessingInfo imageProcessingInfo;

    std::atomic<double> matchPointX;
    std::atomic<double> matchPointY;
    std::atomic<double> critalMatchVal;
};

#endif