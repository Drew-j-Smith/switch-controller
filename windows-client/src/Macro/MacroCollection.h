#pragma once

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
    MacroCollection(const MacroCollection &other) = delete;
    MacroCollection(MacroCollection &&other) = default;
    MacroCollection &operator=(MacroCollection &&other) = default;

    std::array<uint8_t, 8>
    getData(std::array<uint8_t, 8> intitial,
            const std::function<std::array<uint8_t, 8>(std::array<uint8_t, 8>,
                                                       std::array<uint8_t, 8>)>
                mergeFunction) {

        std::array<uint8_t, 8> res = intitial;
        if (activeMacros.size()) {
            auto now = std::chrono::steady_clock::now();
            std::vector<std::shared_ptr<Macro>> toAdd;
            std::vector<std::shared_ptr<Macro>> toRemove;

            for (auto it : activeMacros) {
                auto diff = now - it.second;
                uint64_t time = static_cast<uint64_t>(
                    std::chrono::duration_cast<std::chrono::milliseconds>(diff)
                        .count());
                if (auto data = it.first->getDataframe(time)) {
                    res = mergeFunction(res, data.value());
                } else {
                    toAdd.push_back(it.first->getNextMacro());
                    toRemove.push_back(it.first);
                }
            }

            for (auto m : toRemove)
                activeMacros.erase(m);

            for (auto m : toAdd)
                if (m)
                    activeMacros.insert({m, std::chrono::steady_clock::now()});
        }
        return res;
    }
    void activateMacros() {
        for (auto m : macros) {
            if (activeMacros.find(m) == activeMacros.end() && m->active()) {
                activeMacros.insert({m, std::chrono::steady_clock::now()});
            }
        }
    }
    bool isMacroActive() const { return activeMacros.size(); }
    void deactivateMacros() { activeMacros.clear(); }
    void pushBackMacro(std::shared_ptr<Macro> macro) {
        macros.push_back(macro);
    }
};
