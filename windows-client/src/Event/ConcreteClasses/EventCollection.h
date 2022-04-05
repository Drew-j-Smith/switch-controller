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
    Operators op;

public:
    EventCollection(std::vector<std::shared_ptr<Event>> events = {},
                    Operators op = And)
        : events(events), op(op){};

    uint8_t value() const override {
        uint8_t res = 0;

        for (unsigned int i = 0; i < events.size(); i++) {
            switch (op) {
            case Operators::And:
                if (i == 0)
                    res = 1;
                res = res && events[i]->value();
                break;
            case Operators::Or:
                res = res || events[i]->value();
                break;
            case Operators::Not:
                res = !events[i]->value();
                break;
            case Operators::Xor:
                res = (res && events[i]->value()) ||
                      (!res && !events[i]->value());
                break;
            default:
                break;
            }
        }

        return res;
    };

    virtual void update() override {}
};

#endif