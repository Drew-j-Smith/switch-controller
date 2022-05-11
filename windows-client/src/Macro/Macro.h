#ifndef MACRO_H
#define MACRO_H

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

    Macro(const Macro &&other)
        : actionVector(std::move(other.actionVector)),
          activateEvent(std::move(other.activateEvent)),
          decider(std::move(other.decider)) {}
    Macro operator=(const Macro &&other) { return Macro(std::move(other)); }

    std::shared_ptr<Macro> getNextMacro() const {
        return decider ? decider().lock() : nullptr;
    }

    std::optional<std::array<uint8_t, 8>> getDataframe(uint64_t time) const;

    bool active() const { return activateEvent ? activateEvent() : false; }

private:
    std::vector<Action> actionVector;
    std::function<bool()> activateEvent;
    std::function<std::weak_ptr<Macro>()> decider;
};

#endif
