#ifndef SCREENSHOT_UTILITY_H
#define SCREENSHOT_UTILITY_H

#include <opencv2/core.hpp>

class ScreenshotUtility {
public:
    virtual void screenshot(cv::Mat &) = 0;
};


#endif