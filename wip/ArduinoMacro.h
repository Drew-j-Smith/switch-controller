#ifndef ARDUINO_MACRO
#define ARDUNIO_MACRO

#include "pch.h"

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "CharStream.h"

class Macro
{
public:
    enum ImageProcessingStatus { disabled = 0, enabled, fromOther };
    struct MacroInfo {
        std::string name;
        int         keyboardButton;
        cv::Mat     templatePic;
        cv::Mat     maskPic;
        int         matchMethod;
        double      matchThreshold;
        int         minX;
        int         minY;
        int         maxX;
        int         maxY;
        int         searchMinX;
        int         searchMinY;
        int         searchMaxX;
        int         searchMaxY;
    };

    Macro();
    Macro(const MacroInfo macroInfo, const ImageProcessingStatus ImageProcessingStatus,
    const std::vector<std::weak_ptr<Macro>> macroSuccessList, const std::vector<std::weak_ptr<Macro>> macroFailList,
    const std::vector<std::weak_ptr<Macro>> macroDefaultList, const std::shared_ptr<Macro> sharedImgProcMacro);
    void loadMacro(boost::property_tree::ptree macro, std::map<std::string, std::shared_ptr<Macro>>);
private:
    CharStream<15> data;
    std::shared_ptr<std::atomic_bool> imageMatch = std::shared_ptr<std::atomic_bool>(new std::atomic_bool);
    ImageProcessingStatus imageProcesssingStatus = ImageProcessingStatus::disabled;
    MacroInfo macroInfo = {"", 0, cv::Mat(), cv::Mat(), 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0};

    std::vector<std::weak_ptr<Macro>> macroSuccessList;
    std::vector<std::weak_ptr<Macro>> macroFailList;
    std::vector<std::weak_ptr<Macro>> macroDefaultList;
public:
    void loadData(const std::string &, bool isHex = true);
    void saveData(const std::string &, bool asHex = true);

    void getData(const unsigned long, unsigned char[8]) const;
    void appendData(const unsigned long, unsigned char[8]);
    std::shared_ptr<Macro> getNextMacro() const;

    void matchImage(const cv::Mat);


    std::shared_ptr<std::atomic_bool> getImageMatch() const { return imageMatch;                          }
    unsigned long         lastTime()                  const { return *(unsigned long*)data.back().data(); }
    MacroInfo             getMacroInfo()              const { return macroInfo;                           }
    ImageProcessingStatus getImageProcesssingStatus() const { return imageProcesssingStatus;              }

    void setMacroInfo(MacroInfo macroInfo) { this->macroInfo = macroInfo; }
    void setImageProcesssingStatus(ImageProcessingStatus imageProcessingStatus) { this->imageProcesssingStatus = imageProcessingStatus; }
    
};


#endif
