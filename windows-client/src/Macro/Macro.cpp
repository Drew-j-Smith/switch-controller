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