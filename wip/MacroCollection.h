#ifndef MACRO_COLLECTION_H
#define MACRO_COLLECTION_H

#include "pch.h"

#include <boost/property_tree/ptree.hpp>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include "Macro.h"
#include "MacroImageProcessingDecider.h"

class MacroCollection
{
private:
    std::vector<std::shared_ptr<Macro>> macros;
    std::shared_ptr<Macro> activeMacro;
    std::chrono::steady_clock::time_point timeSinceMacroActivation;
    std::vector<std::shared_ptr<MacroImageProcessingDecider>> deciders;
public:
    MacroCollection(std::vector<std::shared_ptr<Macro>> macros, std::vector<std::shared_ptr<MacroImageProcessingDecider>> deciders);
    MacroCollection(const boost::property_tree::ptree & tree);

    void getData(unsigned char data[8]);
    void activateMacros();
    bool isMacroActive() const { return activeMacro != nullptr; }
    void updateImageProcessing(const cv::Mat & screenshot) const;
};


#endif