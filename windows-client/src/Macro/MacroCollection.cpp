#include "MacroCollection.h"

std::array<uint8_t, 8> MacroCollection::getData(
    std::array<uint8_t, 8> intitial,
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
            uint64_t time =
                std::chrono::duration_cast<std::chrono::milliseconds>(diff)
                    .count();
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

void MacroCollection::activateMacros() {
    for (auto m : macros) {
        if (activeMacros.find(m) == activeMacros.end() && m->active()) { // TODO
            activeMacros.insert({m, std::chrono::steady_clock::now()});
        }
    }
}