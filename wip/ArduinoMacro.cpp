#include "ArduinoMacro.h"

#define AC_DISPLAY_IMAGE_MATCH 1

Macro::Macro(MacroInfo macroInfo, ImageProcessingStatus ImageProcessingStatus,
    std::vector<std::weak_ptr<Macro>> macroSuccessList, std::vector<std::weak_ptr<Macro>> macroFailList,
    std::vector<std::weak_ptr<Macro>> macroDefaultList, std::weak_ptr<Macro> sharedImgProcMacro) :
    imageProcesssingStatus(ImageProcessingStatus),
    macroInfo(macroInfo),
    sharedImgProcMacro(sharedImgProcMacro),
    macroSuccessList(macroSuccessList),
    macroFailList(macroFailList),
    macroDefaultList(macroDefaultList)
{

}

void Macro::loadMacro(boost::property_tree::ptree macro, std::map<std::string, std::shared_ptr<Macro>> macroList,
    std::map<std::string, cv::Mat> pictureList) {
    //TODO
}

void Macro::loadData(const std::string & filename, bool isHex){
    data.load(filename, isHex);
}

void Macro::saveData(const std::string & filename, bool asHex){
    data.save(filename, asHex);
}

void Macro::getDataframe(const unsigned long long time, unsigned char data[8]) const{
    int low = 0;
    int high = this->data.size();
    int mid;
    while(high - low > 1){
        mid = (low + high) / 2;
        if(getTime(mid) > time)
            high = mid;
        else
            low = mid;
    }
    data[0] = 85;
    memcpy(data + 1, this->data[low].data() + 8, 7);
}

void Macro::appendData(const unsigned long long time, unsigned char data[8]){
    std::array<unsigned char, 15> dataframe;
    memcpy(dataframe.data(), &time, 8);
    memcpy(dataframe.data() + 8, data + 1, 7);
    this->data.push_back(dataframe);
}

//helper function to Macro::getNextMacro()
std::shared_ptr<Macro> Macro::cycleVector(std::vector<std::weak_ptr<Macro>> macroVector){
    if(macroVector.size() == 0)
        return std::shared_ptr<Macro>(nullptr);
    else if(macroVector.size() != 1)
        std::rotate(macroVector.begin(), macroVector.begin() + 1, macroVector.end());
    return macroVector.back().lock();
}

std::shared_ptr<Macro> Macro::getNextMacro() const{
    if(imageProcesssingStatus){
        if(isImageMatch())
            return cycleVector(macroSuccessList);
        else
            return cycleVector(macroFailList);
    }
    else
        return cycleVector(macroDefaultList);
}

void Macro::matchImage(const cv::Mat & screenshot){
    if(imageProcesssingStatus != ImageProcessingStatus::enabled)
        return;

    cv::Rect rectCrop = cv::Rect(macroInfo.searchMaxX,
        macroInfo.searchMinY,
        macroInfo.searchMaxX - macroInfo.searchMinX,
        macroInfo.searchMaxY - macroInfo.searchMinY);
    cv::Mat submat = cv::Mat(screenshot, rectCrop);

    int result_cols = submat.cols - macroInfo.templatePic.cols + 1;
    int result_rows = submat.rows - macroInfo.templatePic.rows + 1;
    cv::Mat result(result_rows, result_cols, CV_32FC1);
    if ((cv::TM_CCORR == macroInfo.matchMethod || macroInfo.matchMethod == cv::TM_CCORR_NORMED)
        && macroInfo.maskPic.cols > 0 && macroInfo.maskPic.rows > 0) {
        cv::matchTemplate(submat, macroInfo.templatePic, result, macroInfo.matchMethod, macroInfo.maskPic);
    }
    else {
        cv::matchTemplate(submat, macroInfo.templatePic, result, macroInfo.matchMethod);
    }

    double minVal; double maxVal;
    cv::Point minLoc; cv::Point maxLoc;
    cv::Point matchPoint;
    minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);
    if (macroInfo.matchMethod == cv::TM_SQDIFF || macroInfo.matchMethod == cv::TM_SQDIFF_NORMED) {
        matchPoint = minLoc;
        critalMatchVal.store(minVal);
    }
    else {
        matchPoint = maxLoc;
        critalMatchVal.store(minVal);
    }
    #if AC_DISPLAY_IMAGE_MATCH == 1
        rectangle(submat, matchPoint, cv::Point(matchPoint.x + macroInfo.templatePic.cols,
            matchPoint.y + macroInfo.templatePic.rows), cv::Scalar::all(0), 2, 8, 0);
        rectangle(result, matchPoint, cv::Point(matchPoint.x + macroInfo.templatePic.cols,
            matchPoint.y + macroInfo.templatePic.rows), cv::Scalar::all(0), 2, 8, 0);

        cv::namedWindow(macroInfo.name + "-1", cv::WINDOW_AUTOSIZE);
        cv::imshow(macroInfo.name + "-1", submat);
        cv::namedWindow(macroInfo.name + "-2", cv::WINDOW_AUTOSIZE);
        cv::imshow(macroInfo.name + "-2", result);
        cv::waitKey(0);
    #endif

    this->matchPoint.store(cv::Point(matchPoint.x + macroInfo.searchMinX, matchPoint.y + macroInfo.searchMinY));
}

bool Macro::isImageMatch() const{
    //TODO
    cv::Point matchPoint = getMatchPoint();
    if (getImageProcessingMacro()->getMacroInfo()->matchMethod == cv::TM_SQDIFF
        || getImageProcessingMacro()->getMacroInfo()->matchMethod == cv::TM_SQDIFF_NORMED) {
        return getCritalMatchVal() < macroInfo.matchThreshold && 
            matchPoint.x >= macroInfo.minX && 
            matchPoint.y >= macroInfo.minY && 
            matchPoint.x <= macroInfo.maxX && 
            matchPoint.y <= macroInfo.maxY;
    }
    else {
        return macroInfo.matchThreshold < getCritalMatchVal() && 
            matchPoint.x >= macroInfo.minX &&
            matchPoint.y >= macroInfo.minY &&
            matchPoint.x <= macroInfo.maxX &&
            matchPoint.y <= macroInfo.maxY;
    }
}