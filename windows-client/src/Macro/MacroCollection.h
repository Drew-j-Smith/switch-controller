#ifndef MACRO_COLLECTION_H
#define MACRO_COLLECTION_H

#include "pch.h"

#include <boost/property_tree/ptree.hpp>

#include "Decider/DeciderCollectionBase.h"
#include "Macro.h"

class MacroCollection {
private:
    std::vector<std::shared_ptr<Macro>> macros;
    std::map<std::shared_ptr<Macro>, std::chrono::steady_clock::time_point>
        activeMacros;
    std::shared_ptr<DeciderCollectionBase> deciders;

public:
    MacroCollection(const std::vector<std::shared_ptr<Macro>> &macros,
                    const std::shared_ptr<DeciderCollectionBase> &deciders);
    MacroCollection(const boost::property_tree::ptree &tree,
                    const std::shared_ptr<DeciderCollectionBase> &deciders,
                    InputEventFactory &factory);

    std::array<uint8_t, 8> getData(const std::array<uint8_t, 8> &dataToMerge);
    void activateMacros();
    bool isMacroActive() const { return activeMacros.size(); }
    void deactivateMacros() { activeMacros.clear(); }
    const std::vector<std::shared_ptr<Macro>> getMacros() const {
        return macros;
    }
    const std::shared_ptr<Macro> lastMacro() const { return macros.back(); }
    void popLastMacro() { macros.pop_back(); }
    void pushBackMacro(std::shared_ptr<Macro> macro) {
        macros.push_back(macro);
    }
};

#endif