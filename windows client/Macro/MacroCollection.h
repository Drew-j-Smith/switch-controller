#ifndef MACRO_COLLECTION_H
#define MACRO_COLLECTION_H

#include "../pch.h"

#include <boost/property_tree/ptree.hpp>

#include "Macro.h"
#include "../Decider/ImageProcessingDeciderCollection.h"

class MacroCollection
{
private:
    std::vector<std::shared_ptr<Macro>> macros;
    std::shared_ptr<Macro> activeMacro;
    std::chrono::steady_clock::time_point timeSinceMacroActivation;
    std::shared_ptr<ImageProcessingDeciderCollection> deciders;
public:
    MacroCollection(const std::vector<std::shared_ptr<Macro>> & macros, const std::shared_ptr<ImageProcessingDeciderCollection> & deciders);
    MacroCollection(const boost::property_tree::ptree & tree, const std::shared_ptr<ImageProcessingDeciderCollection> & deciders);

    void getData(unsigned char data[8]);
    void activateMacros();
    bool isMacroActive() const { return activeMacro != nullptr; }
    void deactivateMacros() { activeMacro = nullptr; }
    const std::vector<std::shared_ptr<Macro>> getMacros() const { return macros; }
    const std::shared_ptr<Macro> lastMacro() const { return macros.back(); }
    void popLastMacro() { macros.pop_back(); }
    void pushBackMacro(std::shared_ptr<Macro> macro) { macros.push_back(macro); }
};


#endif