#pragma once

#include "pch.h"

#include "Action.h"

class Macro {
public:
    Macro() {} // TODO? potentially remove default constructor
    Macro(const std::vector<Action> &actionVector,
          const std::function<bool()> &activateEvent,
          const std::function<std::weak_ptr<Macro>()> &decider)
        : actionVector(actionVector), activateEvent(activateEvent),
          decider(decider) {}

    Macro(const Macro &other) = delete;

    Macro(Macro &&other) = default;
    Macro &operator=(Macro &&other) = default;

    std::shared_ptr<Macro> getNextMacro() const {
        return decider ? decider().lock() : nullptr;
    }

    std::optional<std::array<uint8_t, 8>> getDataframe(uint64_t time) const {
        if (actionVector.size() == 0 || time >= actionVector.back().time) {
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

    bool active() const { return activateEvent ? activateEvent() : false; }

private:
    std::vector<Action> actionVector;
    std::function<bool()> activateEvent;
    std::function<std::weak_ptr<Macro>()> decider;
};
