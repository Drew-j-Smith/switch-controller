#ifndef ARDUINO_MACROS_H
#define ARDUINO_MACROS_H

#include "pch.h"

#include <boost/property_tree/ptree.hpp>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include "ArduinoMacro.h"
#include "MacroImageProcessingDecider.h"

class Macros
{
private:
    std::vector<std::shared_ptr<Macro>> macros;
    std::shared_ptr<Macro> activeMacro;
    std::chrono::steady_clock::time_point timeSinceMacroActivation;
    std::vector<std::shared_ptr<MacroImageProcessingDecider>> deciders;
public:
    Macros(std::vector<std::shared_ptr<Macro>> macros, std::vector<std::shared_ptr<MacroImageProcessingDecider>> deciders);
    Macros(const boost::property_tree::ptree & tree);

    void getData(unsigned char data[8]);
    void activateMacros();
    bool isMacroActive() const { return activeMacro != nullptr; }
    void updateImageProcessing(const cv::Mat & screenshot) const;
};


#endif