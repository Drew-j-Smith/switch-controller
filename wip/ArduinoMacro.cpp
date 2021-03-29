#include "ArduinoMacro.h"


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
static std::shared_ptr<Macro> cycleVector(std::vector<std::weak_ptr<Macro>> macroVector){
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

void Macro::matchImage(const cv::Mat sceenshot){
    if(imageProcesssingStatus == ImageProcessingStatus::fromOther)
        return;

    //TODO
}

//helper function to Macro::isImageMatch
static const Macro* getImageProcessingMacro(const Macro* m) {
    if (m->getImageProcesssingStatus() == Macro::ImageProcessingStatus::fromOther)
        return m->getSharedImgProcMacro().get();
    else 
        return m;
}

bool Macro::isImageMatch() const{
    //TODO
    if (getImageProcessingMacro(this)->getMacroInfo()->matchMethod == cv::TM_SQDIFF
        || getImageProcessingMacro(this)->getMacroInfo()->matchMethod == cv::TM_SQDIFF_NORMED) {
        return getCritalMatchVal() < macroInfo.matchThreshold && 
            getMatchPoint().x >= macroInfo.minX && 
            getMatchPoint().y >= macroInfo.minY && 
            getMatchPoint().x <= macroInfo.maxX && 
            getMatchPoint().y <= macroInfo.maxY;
    }
    else {
        return macroInfo.matchThreshold < getCritalMatchVal() && 
            getMatchPoint().x >= macroInfo.minX &&
            getMatchPoint().y >= macroInfo.minY &&
            getMatchPoint().x <= macroInfo.maxX &&
            getMatchPoint().y <= macroInfo.maxY;
    }
}