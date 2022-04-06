#include "ImageEvent.h"

#define AC_DISPLAY_IMAGE_MATCH 0

ImageEvent::ImageEvent(const cv::Mat &templatePic, const cv::Mat &maskPic,
                       const int matchMethod, const double matchThreshold,
                       const int minX, const int minY, const int maxX,
                       const int maxY,
                       const std::shared_ptr<VideoFrameSink> &videoFrameSink)
    : templatePic(templatePic), maskPic(maskPic), matchMethod(matchMethod),
      matchThreshold(matchThreshold), minX(minX), minY(minY), maxX(maxX),
      maxY(maxY), videoFrameSink(videoFrameSink) {}

uint8_t ImageEvent::value() const {

    this->videoFrameSink->waitForInit();
    std::vector<uint8_t> data; // TODO create class instance
    this->videoFrameSink->getData(data);

    cv::Mat screenshot =
        cv::Mat(this->videoFrameSink->getHeight(),
                this->videoFrameSink->getWidth(), CV_8UC3, data.data());

    cv::Rect rectCrop = cv::Rect(minX, minY, maxX - minX, maxY - minY);
    cv::Mat submat = cv::Mat(screenshot, rectCrop);

    int result_cols = submat.cols - templatePic.cols + 1;
    int result_rows = submat.rows - templatePic.rows + 1;
    cv::Mat result(result_rows, result_cols, CV_32FC1);
    if ((cv::TM_CCORR == matchMethod || matchMethod == cv::TM_CCORR_NORMED) &&
        maskPic.cols > 0 && maskPic.rows > 0) {
        cv::matchTemplate(submat, templatePic, result, matchMethod, maskPic);
    } else {
        cv::matchTemplate(submat, templatePic, result, matchMethod);
    }

    double minVal;
    double maxVal;
    double critalMatchVal;
    cv::Point minLoc;
    cv::Point maxLoc;
    cv::Point matchPoint;
    minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);
    if (matchMethod == cv::TM_SQDIFF || matchMethod == cv::TM_SQDIFF_NORMED) {
        matchPoint = minLoc;
        critalMatchVal = minVal;
    } else {
        matchPoint = maxLoc;
        critalMatchVal = maxVal;
    }
#if AC_DISPLAY_IMAGE_MATCH == 1
    rectangle(submat, matchPoint,
              cv::Point(matchPoint.x + templatePic.cols,
                        matchPoint.y + templatePic.rows),
              cv::Scalar::all(0), 2, 8, 0);
    rectangle(result, matchPoint,
              cv::Point(matchPoint.x + templatePic.cols,
                        matchPoint.y + templatePic.rows),
              cv::Scalar::all(0), 2, 8, 0);

    cv::namedWindow(name + "-1", cv::WINDOW_AUTOSIZE);
    cv::imshow(name + "-1", submat);
    cv::waitKey(1);
    cv::namedWindow(name + "-2", cv::WINDOW_AUTOSIZE);
    cv::imshow(name + "-2", result);
    cv::waitKey(1);
#endif

    if (matchMethod == cv::TM_SQDIFF || matchMethod == cv::TM_SQDIFF_NORMED) {
        return critalMatchVal < matchThreshold;
    } else {
        return matchThreshold < critalMatchVal;
    }
}