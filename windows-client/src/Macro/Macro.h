#ifndef MACRO_H
#define MACRO_H

#include "pch.h"

#include "Action.h"
#include "Event/ConcreteClasses/ConstantEvent.h"
#include "Event/Event.h"

class Macro {
public:
    Macro() {} // TODO? potentially remove default constructor
    Macro(const std::vector<Action> &actionVector,
          const std::shared_ptr<Event> &activateEvent,
          const std::shared_ptr<Event> &decider,
          const std::vector<std::vector<std::weak_ptr<Macro>>> &nextMacroLists)
        : actionVector(actionVector), activateEvent(activateEvent),
          decider(decider), nextMacroLists(nextMacroLists) {}

    std::vector<Action> actionVector;
    std::shared_ptr<Event> activateEvent;
    std::shared_ptr<Event> decider;
    std::vector<std::vector<std::weak_ptr<Macro>>> nextMacroLists;

    std::shared_ptr<Macro> getNextMacro();

    std::optional<std::array<uint8_t, 8>> getDataframe(uint64_t time) const;
};

#endif
