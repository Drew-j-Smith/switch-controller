#ifndef MACRO_DECIDER_H
#define MACRO_DECIDER_H

#include "pch.h"

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <boost/property_tree/ptree.hpp>

class MacroDecider
{
public:
    MacroDecider() { name = ""; }
    MacroDecider(const std::string & name) { this->name = name; }

    std::string getName() { return name; }
    virtual int nextMacroListIndex() const { return 0; }
protected:
    std::string name;
};

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

    MacroImageProcessingDecider(const std::string & name, const ImageProcessingInfo & imageProcessingInfo) {
        this->name = name;
        this->imageProcessingInfo = imageProcessingInfo;
        matchPointX.store(0);
        matchPointY.store(0);
        critalMatchVal.store(0);
    }

    MacroImageProcessingDecider(const boost::property_tree::ptree & tree);

    int nextMacroListIndex() const override;

    void update(cv::Mat &);
private:
    ImageProcessingInfo imageProcessingInfo;

    std::atomic<double> matchPointX;
    std::atomic<double> matchPointY;
    std::atomic<double> critalMatchVal;
};

#endif