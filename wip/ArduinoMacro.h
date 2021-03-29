#ifndef ARDUINO_MACRO_H
#define ARDUINO_MACRO_H

#include "pch.h"

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "CharStream.h"

class Macro {
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

    Macro() {};
    Macro(MacroInfo macroInfo, ImageProcessingStatus ImageProcessingStatus,
    std::vector<std::weak_ptr<Macro>> macroSuccessList, std::vector<std::weak_ptr<Macro>> macroFailList,
    std::vector<std::weak_ptr<Macro>> macroDefaultList, std::weak_ptr<Macro> sharedImgProcMacro);

    void loadMacro(boost::property_tree::ptree macro, std::map<std::string, std::shared_ptr<Macro>> macroList,
    std::map<std::string, cv::Mat> pictureList);
private:
    CharStream<15> data;
    ImageProcessingStatus imageProcesssingStatus = ImageProcessingStatus::disabled;
    MacroInfo macroInfo = {"", 0, cv::Mat(), cv::Mat(), 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0};

    std::atomic<cv::Point> matchPoint;
    std::atomic<double> critalMatchVal;
    std::shared_ptr<Macro> sharedImgProcMacro;

    std::vector<std::weak_ptr<Macro>> macroSuccessList;
    std::vector<std::weak_ptr<Macro>> macroFailList;
    std::vector<std::weak_ptr<Macro>> macroDefaultList;
public:
    void loadData(const std::string & filename, bool isHex = true);
    void saveData(const std::string & filename, bool asHex = true);

    const CharStream<15> getData() const { return data; }
    void appendData(const unsigned long long, unsigned char[8]);

    //thread safe methods
    void getDataframe(const unsigned long long, unsigned char[8]) const;
    std::shared_ptr<Macro> getNextMacro() const;
    std::shared_ptr<Macro> getSharedImgProcMacro() const{ return sharedImgProcMacro; }

    void matchImage(const cv::Mat);
    bool isImageMatch() const;
    cv::Point getMatchPoint() const {
        if (imageProcesssingStatus == ImageProcessingStatus::fromOther) return sharedImgProcMacro->getMatchPoint();
        return matchPoint.load();
    }
    double getCritalMatchVal() const {
        if (imageProcesssingStatus == ImageProcessingStatus::fromOther) return sharedImgProcMacro->getCritalMatchVal();
        return critalMatchVal.load(); 
    }

    void setMatchPoint(const cv::Point matchPoint) { this->matchPoint.store(matchPoint);         }
    void setCritalMatchVal(double critalMatchVal)  { this->critalMatchVal.store(critalMatchVal); }

    unsigned long long    getTime(int index)          const { return *(unsigned long long*)data[index].data(); }
    unsigned long long    lastTime()                  const { return *(unsigned long long*)data.back().data(); }
    const MacroInfo*      getMacroInfo()              const { return &macroInfo;                               }
    ImageProcessingStatus getImageProcesssingStatus() const { return imageProcesssingStatus;                   }
    //end thread safe methods

    void setMacroInfo(MacroInfo macroInfo) { this->macroInfo = macroInfo; }
    void setImageProcesssingStatus(ImageProcessingStatus imageProcessingStatus) { this->imageProcesssingStatus = imageProcessingStatus; }
    
};


#endif
