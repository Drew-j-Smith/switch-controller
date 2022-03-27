#ifndef EVENT_COLLECTION_H
#define EVENT_COLLECTION_H

#include "pch.h"

#include "Event/Event.h"

class EventCollection : public Event {
public:
    enum Operators { And, Or, Not, Xor };

private:
    std::vector<std::shared_ptr<Event>> inputEvents;
    Operators op = Operators::And;

public:
    EventCollection(){};
    EventCollection(std::vector<std::shared_ptr<Event>> inputEvents,
                    Operators op)
        : inputEvents(inputEvents), op(op){};

    int getInputValue() const override {
        int res = 0;

        for (int i = 0; i < (int)inputEvents.size(); i++) {
            switch (op) {
            case Operators::And:
                if (i == 0)
                    res = 1;
                res = res && inputEvents[i]->getInputValue();
                break;
            case Operators::Or:
                res = res || inputEvents[i]->getInputValue();
                break;
            case Operators::Not:
                res = !inputEvents[i]->getInputValue();
                break;
            case Operators::Xor:
                res = (res && inputEvents[i]->getInputValue()) ||
                      (!res && !inputEvents[i]->getInputValue());
                break;
            default:
                break;
            }
        }

        return res;
    };

    bool isDigital() const override { return true; }

    virtual void update() override {}
};

#endif