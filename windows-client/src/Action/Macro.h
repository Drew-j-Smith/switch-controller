#ifndef MACRO2_H // TODO rename to Macro
#define MACRO2_H

#include "pch.h"

#include "Action.h"
#include "ActionProvider.h"
#include "Event/Event.h"

class Macro2 {
private:
    std::shared_ptr<ActionProvider> base;
    std::shared_ptr<ActionProvider> curr;
    std::shared_ptr<Event> start;
    std::shared_ptr<Event> cancel;

    bool active;

public:
    Macro2(const std::shared_ptr<ActionProvider> &provider,
           std::shared_ptr<Event> start, std::shared_ptr<Event> cancel)
        : base(provider), curr(provider), start(start), cancel(cancel){};

    Action getAction() {
        assert(active);
        if (!curr->isActive()) {
            curr = curr->getNextProvider().get(); // TODO
            curr->start();
        }
        return curr->getAction();
    };
    bool isActive() {
        if (active && cancel->value()) {
            // reset
            active = false;
            curr->stop();
            curr = base;
        } else if (start->value()) {
            // start
            active = true;
            curr->start();
        }
        return active;
    };
};

#endif