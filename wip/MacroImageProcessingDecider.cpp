#include "MacroImageProcessingDecider.h"

#define AC_DISPLAY_IMAGE_MATCH 1

MacroImageProcessingDecider::MacroImageProcessingDecider(const std::string & name, const ImageProcessingInfo & imageProcessingInfo) {
    this->name = name;
    this->imageProcessingInfo = imageProcessingInfo;
    matchPointX.store(0);
    matchPointY.store(0);
    critalMatchVal.store(0);
}

MacroImageProcessingDecider::MacroImageProcessingDecider(const boost::property_tree::ptree & tree){
    name = tree.get("name", "");
    imageProcessingInfo.templatePic = cv::imread(tree.get("template pic filename", ""));
    if (tree.get("mask pic filename", "") != "")
        imageProcessingInfo.maskPic = cv::imread(tree.get("mask pic filename", ""));
    imageProcessingInfo.matchMethod = tree.get("match method", 0);
    imageProcessingInfo.matchThreshold = tree.get("match threshold", 0.0);
    imageProcessingInfo.minX = tree.get("min x", 0);
    imageProcessingInfo.minY = tree.get("min y", 0);
    imageProcessingInfo.maxX = tree.get("max x", 0);
    imageProcessingInfo.maxY = tree.get("max y", 0);
    matchPointX.store(0);
    matchPointY.store(0);
    critalMatchVal.store(0);
}

int MacroImageProcessingDecider::nextMacroListIndex() const {
    double matchX = matchPointX.load();
    double matchY = matchPointY.load();
    if (imageProcessingInfo.matchMethod == cv::TM_SQDIFF
        || imageProcessingInfo.matchMethod == cv::TM_SQDIFF_NORMED) {
        return critalMatchVal.load() < imageProcessingInfo.matchThreshold && 
            matchX >= imageProcessingInfo.minX && 
            matchY >= imageProcessingInfo.minY && 
            matchX <= imageProcessingInfo.maxX && 
            matchY <= imageProcessingInfo.maxY;
    }
    else {
        return imageProcessingInfo.matchThreshold < critalMatchVal.load() && 
            matchX >= imageProcessingInfo.minX &&
            matchY >= imageProcessingInfo.minY &&
            matchX <= imageProcessingInfo.maxX &&
            matchY <= imageProcessingInfo.maxY;
    }
}

void MacroImageProcessingDecider::update(const cv::Mat & screenshot){
    cv::Rect rectCrop = cv::Rect(imageProcessingInfo.minX,
        imageProcessingInfo.minY,
        imageProcessingInfo.maxX - imageProcessingInfo.minX,
        imageProcessingInfo.maxY - imageProcessingInfo.minY);
    cv::Mat submat = cv::Mat(screenshot, rectCrop);

    int result_cols = submat.cols - imageProcessingInfo.templatePic.cols + 1;
    int result_rows = submat.rows - imageProcessingInfo.templatePic.rows + 1;
    cv::Mat result(result_rows, result_cols, CV_32FC1);
    if ((cv::TM_CCORR == imageProcessingInfo.matchMethod || imageProcessingInfo.matchMethod == cv::TM_CCORR_NORMED)
        && imageProcessingInfo.maskPic.cols > 0 && imageProcessingInfo.maskPic.rows > 0) {
        cv::matchTemplate(submat, imageProcessingInfo.templatePic, result, imageProcessingInfo.matchMethod, imageProcessingInfo.maskPic);
    }
    else {
        cv::matchTemplate(submat, imageProcessingInfo.templatePic, result, imageProcessingInfo.matchMethod);
    }

    double minVal; double maxVal;
    cv::Point minLoc; cv::Point maxLoc;
    cv::Point matchPoint;
    minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);
    if (imageProcessingInfo.matchMethod == cv::TM_SQDIFF || imageProcessingInfo.matchMethod == cv::TM_SQDIFF_NORMED) {
        matchPoint = minLoc;
        critalMatchVal.store(minVal);
    }
    else {
        matchPoint = maxLoc;
        critalMatchVal.store(maxVal);
    }
    #if AC_DISPLAY_IMAGE_MATCH == 1
        rectangle(submat, matchPoint, cv::Point(matchPoint.x + imageProcessingInfo.templatePic.cols,
            matchPoint.y + imageProcessingInfo.templatePic.rows), cv::Scalar::all(0), 2, 8, 0);
        rectangle(result, matchPoint, cv::Point(matchPoint.x + imageProcessingInfo.templatePic.cols,
            matchPoint.y + imageProcessingInfo.templatePic.rows), cv::Scalar::all(0), 2, 8, 0);

        cv::namedWindow(name + "-1", cv::WINDOW_AUTOSIZE);
        cv::imshow(name + "-1", submat);
        cv::waitKey(1);
        cv::namedWindow(name + "-2", cv::WINDOW_AUTOSIZE);
        cv::imshow(name + "-2", result);
        cv::waitKey(1);
    #endif

    matchPointX.store(matchPoint.x + imageProcessingInfo.minX);
    matchPointY.store(matchPoint.y + imageProcessingInfo.minY);
}