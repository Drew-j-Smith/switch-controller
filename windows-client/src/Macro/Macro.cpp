#include "Macro.h"

#include <boost/endian/conversion.hpp>

std::optional<std::array<uint8_t, 8>> Macro::getDataframe(uint64_t time) const {

    if (time > actionVector.back().time) {
        return {};
    }

    Action searchAction = {time, {}};
    auto predicate = [](const Action &a, const Action &b) {
        return a.time < b.time;
    };
    // round up
    auto res = std::upper_bound(actionVector.begin(), actionVector.end(),
                                searchAction, predicate);
    return res->data;
}

std::shared_ptr<Macro> Macro::getNextMacro() {
    std::size_t macroIndex = decider(); // TODO
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