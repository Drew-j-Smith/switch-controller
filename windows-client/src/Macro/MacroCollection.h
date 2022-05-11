#ifndef MACRO_COLLECTION_H
#define MACRO_COLLECTION_H

#include "pch.h"

#include "Macro.h"

class MacroCollection {
private:
    std::vector<std::shared_ptr<Macro>> macros;
    std::map<std::shared_ptr<Macro>, std::chrono::steady_clock::time_point>
        activeMacros;

public:
    MacroCollection(const std::vector<std::shared_ptr<Macro>> &macros)
        : macros(macros){};

    std::array<uint8_t, 8>
    getData(std::array<uint8_t, 8> intitial,
            const std::function<std::array<uint8_t, 8>(std::array<uint8_t, 8>,
                                                       std::array<uint8_t, 8>)>
                mergeFunction);
    void activateMacros();
    bool isMacroActive() const { return activeMacros.size(); }
    void deactivateMacros() { activeMacros.clear(); }
    void pushBackMacro(std::shared_ptr<Macro> macro) {
        macros.push_back(macro);
    }
};

#endif