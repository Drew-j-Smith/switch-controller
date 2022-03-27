#ifndef EVENT_COLLECTION_H
#define EVENT_COLLECTION_H

/**
 * @file EventCollection.h
 * @brief The EventCollection contains a list of Events and performs operation
 * on them to get its event
 * @date 2022-03-27
 *
 */

#include "pch.h"

#include "Event/Event.h"

class EventCollection : public Event {
public:
    enum Operators { And, Or, Not, Xor };

private:
    std::vector<std::shared_ptr<Event>> events;
    Operators op = Operators::And;

public:
    EventCollection(){};
    EventCollection(std::vector<std::shared_ptr<Event>> events, Operators op)
        : events(events), op(op){};

    uint8_t getEventValue() const override {
        uint8_t res = 0;

        for (unsigned int i = 0; i < events.size(); i++) {
            switch (op) {
            case Operators::And:
                if (i == 0)
                    res = 1;
                res = res && events[i]->getEventValue();
                break;
            case Operators::Or:
                res = res || events[i]->getEventValue();
                break;
            case Operators::Not:
                res = !events[i]->getEventValue();
                break;
            case Operators::Xor:
                res = (res && events[i]->getEventValue()) ||
                      (!res && !events[i]->getEventValue());
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