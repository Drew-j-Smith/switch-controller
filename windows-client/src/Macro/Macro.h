#ifndef MACRO_H
#define MACRO_H

#include "pch.h"

#include "Action.h"

class Macro {
public:
    Macro() {} // TODO? potentially remove default constructor
    Macro(const std::vector<Action> &actionVector,
          const std::function<bool()> &activateEvent,
          const std::function<std::size_t()> &decider,
          const std::vector<std::vector<std::weak_ptr<Macro>>> &nextMacroLists)
        : actionVector(actionVector), activateEvent(activateEvent),
          decider(decider), nextMacroLists(nextMacroLists) {}

    Macro(const Macro &other) = delete;

    Macro(const Macro &&other)
        : actionVector(std::move(other.actionVector)),
          activateEvent(std::move(other.activateEvent)),
          decider(std::move(other.decider)),
          nextMacroLists(std::move(other.nextMacroLists)) {}
    Macro operator=(const Macro &&other) { return Macro(std::move(other)); }

    std::shared_ptr<Macro> getNextMacro();

    std::optional<std::array<uint8_t, 8>> getDataframe(uint64_t time) const;

    const std::function<bool()> &getActivateEvent() { return activateEvent; }

private:
    std::vector<Action> actionVector;
    std::function<bool()> activateEvent;
    std::function<std::size_t()> decider;
    std::vector<std::vector<std::weak_ptr<Macro>>> nextMacroLists;
};

#endif
