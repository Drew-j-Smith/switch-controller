#include "ImageProcessingDecider.h"

#define AC_DISPLAY_IMAGE_MATCH 1

ImageProcessingDecider::ImageProcessingDecider(const std::string & name, cv::Mat & templatePic,
    const cv::Mat & maskPic,
    const int     matchMethod,
    const double  matchThreshold,
    const int     minX,
    const int     minY,
    const int     maxX,
    const int     maxY) {

    this->name = name;

    this->templatePic = templatePic;
    this->maskPic = maskPic;
    this->matchMethod = matchMethod;
    this->matchThreshold = matchThreshold;
    this->minX = minX;
    this->minY = minY;
    this->maxX = maxX;
    this->maxY = maxY;

    matchPointX.store(0);
    matchPointY.store(0);
    critalMatchVal.store(0);
}

ImageProcessingDecider::ImageProcessingDecider(const boost::property_tree::ptree & tree){
    name = "";
    templatePic = cv::Mat();
    maskPic = cv::Mat();
    matchMethod = 0;
    matchThreshold = 0;
    minX = 0;
    minY = 0;
    maxX = 0;
    maxY = 0;
    for (auto it : tree) {
        if (it.first == "name") name = it.second.get_value<std::string>();
        else if (it.first == "template pic filename") templatePic = cv::imread(it.second.get_value<std::string>());
        else if (it.first == "mask pic filename") maskPic = cv::imread(it.second.get_value<std::string>());
        else if (it.first == "match method") matchMethod = it.second.get_value<int>();
        else if (it.first == "match threshold") matchThreshold = it.second.get_value<double>();
        else if (it.first == "min x") minX = it.second.get_value<int>();
        else if (it.first == "min y") minY = it.second.get_value<int>();
        else if (it.first == "max x") maxX = it.second.get_value<int>();
        else if (it.first == "max y") maxY = it.second.get_value<int>();
        else if (it.first != "type") {
            std::cerr << "Unrecognized field \"" << it.first << "\" when loading ImageProcessingDecider\n";
        }
    }
    matchPointX.store(0);
    matchPointY.store(0);
    critalMatchVal.store(0);
}

int ImageProcessingDecider::nextListIndex() const {
    double matchX = matchPointX.load();
    double matchY = matchPointY.load();
    if (matchMethod == cv::TM_SQDIFF
        || matchMethod == cv::TM_SQDIFF_NORMED) {
        return critalMatchVal.load() < matchThreshold && 
            matchX >= minX && matchY >= minY && 
            matchX <= maxX && matchY <= maxY;
    }
    else {
        return matchThreshold < critalMatchVal.load() && 
            matchX >= minX && matchY >= minY &&
            matchX <= maxX && matchY <= maxY;
    }
}

void ImageProcessingDecider::update(const cv::Mat & screenshot){
    cv::Rect rectCrop = cv::Rect(minX,
        minY,
        maxX - minX,
        maxY - minY);
    cv::Mat submat = cv::Mat(screenshot, rectCrop);

    int result_cols = submat.cols - templatePic.cols + 1;
    int result_rows = submat.rows - templatePic.rows + 1;
    cv::Mat result(result_rows, result_cols, CV_32FC1);
    if ((cv::TM_CCORR == matchMethod || matchMethod == cv::TM_CCORR_NORMED)
        && maskPic.cols > 0 && maskPic.rows > 0) {
        cv::matchTemplate(submat, templatePic, result, matchMethod, maskPic);
    }
    else {
        cv::matchTemplate(submat, templatePic, result, matchMethod);
    }

    double minVal; double maxVal;
    cv::Point minLoc; cv::Point maxLoc;
    cv::Point matchPoint;
    minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);
    if (matchMethod == cv::TM_SQDIFF || matchMethod == cv::TM_SQDIFF_NORMED) {
        matchPoint = minLoc;
        critalMatchVal.store(minVal);
    }
    else {
        matchPoint = maxLoc;
        critalMatchVal.store(maxVal);
    }
    #if AC_DISPLAY_IMAGE_MATCH == 1
        rectangle(submat, matchPoint, cv::Point(matchPoint.x + templatePic.cols,
            matchPoint.y + templatePic.rows), cv::Scalar::all(0), 2, 8, 0);
        rectangle(result, matchPoint, cv::Point(matchPoint.x + templatePic.cols,
            matchPoint.y + templatePic.rows), cv::Scalar::all(0), 2, 8, 0);

        cv::namedWindow(name + "-1", cv::WINDOW_AUTOSIZE);
        cv::imshow(name + "-1", submat);
        cv::waitKey(1);
        cv::namedWindow(name + "-2", cv::WINDOW_AUTOSIZE);
        cv::imshow(name + "-2", result);
        cv::waitKey(1);
    #endif

    matchPointX.store(matchPoint.x + minX);
    matchPointY.store(matchPoint.y + minY);
}