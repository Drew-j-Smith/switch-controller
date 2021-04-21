#ifndef MACRO_COLLECTION_H
#define MACRO_COLLECTION_H

#include "pch.h"

#include <boost/property_tree/ptree.hpp>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include "Macro.h"
#include "MacroImageProcessingDeciderCollection.h"

class MacroCollection
{
private:
    std::vector<std::shared_ptr<Macro>> macros;
    std::shared_ptr<Macro> activeMacro;
    std::chrono::steady_clock::time_point timeSinceMacroActivation;
    std::shared_ptr<MacroImageProcessingDeciderCollection> deciders;
public:
    MacroCollection(const std::vector<std::shared_ptr<Macro>> & macros, const std::shared_ptr<MacroImageProcessingDeciderCollection> & deciders);
    MacroCollection(const boost::property_tree::ptree & tree, const std::shared_ptr<MacroImageProcessingDeciderCollection> & deciders);

    void getData(unsigned char data[8]);
    void activateMacros();
    bool isMacroActive() const { return activeMacro != nullptr; }
};


#endif