#include "Macro.h"

#include <boost/endian/conversion.hpp>

std::array<uint8_t, 8> Macro::getDataframe(uint64_t time) const {
    Action searchAction = {time, {}};
    auto predicate = [](const Action &a, const Action &b) {
        return a.time < b.time;
    };
    auto res = std::upper_bound(actionVector.begin(), actionVector.end(),
                                searchAction, predicate) -
               1;
    // subtract one to round down instead of up
    return res->data;
}

std::shared_ptr<Macro> Macro::getNextMacro() {
    int macroIndex = decider->value(); // TODO
    if (nextMacroLists.size() == 0)
        return nullptr;
    if (macroIndex < 0 || macroIndex >= nextMacroLists.size())
        return nullptr;
    auto &curr = nextMacroLists[macroIndex];
    if (curr.size() == 0)
        return nullptr;
    if (curr.size() != 1)
        std::rotate(curr.begin(), curr.begin() + 1, curr.end());
    return curr.back().lock();
}