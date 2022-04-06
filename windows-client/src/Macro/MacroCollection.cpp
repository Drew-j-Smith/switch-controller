#include "MacroCollection.h"

std::array<uint8_t, 8>
MacroCollection::getData(const std::array<uint8_t, 8> &dataToMerge) {
    std::array<uint8_t, 8> res = dataToMerge;
    if (activeMacros.size()) {
        auto now = std::chrono::steady_clock::now();
        std::vector<std::shared_ptr<Macro>> toAdd;
        std::vector<std::shared_ptr<Macro>> toRemove;

        for (auto it : activeMacros) {
            auto diff = now - it.second;
            uint64_t time =
                std::chrono::duration_cast<std::chrono::milliseconds>(diff)
                    .count();
            res = it.first->getDataframe(time, res);

            if (it.first->lastTime() < time) {
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

void MacroCollection::activateMacros() {
    for (auto m : macros) {
        if (activeMacros.find(m) == activeMacros.end() &&
            m->getActivateEvent()->value()) {
            activeMacros.insert({m, std::chrono::steady_clock::now()});
        }
    }
}