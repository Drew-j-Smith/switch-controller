#include "MacroImageProcessingDecider.h"

#define AC_DISPLAY_IMAGE_MATCH 1

MacroImageProcessingDecider::MacroImageProcessingDecider(const std::string & name, cv::Mat & templatePic,
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

MacroImageProcessingDecider::MacroImageProcessingDecider(const boost::property_tree::ptree & tree){
    name = tree.get("name", "");
    templatePic = cv::imread(tree.get("template pic filename", ""));
    if (tree.get("mask pic filename", "") != "")
        maskPic = cv::imread(tree.get("mask pic filename", ""));
    matchMethod = tree.get("match method", 0);
    matchThreshold = tree.get("match threshold", 0.0);
    minX = tree.get("min x", 0);
    minY = tree.get("min y", 0);
    maxX = tree.get("max x", 0);
    maxY = tree.get("max y", 0);
    matchPointX.store(0);
    matchPointY.store(0);
    critalMatchVal.store(0);
}

int MacroImageProcessingDecider::nextMacroListIndex() const {
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

void MacroImageProcessingDecider::update(const cv::Mat & screenshot){
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