#ifndef CONSTANT_EVENT_H
#define CONSTANT_EVENT_H

/**
 * @brief The ConstantEvent is a basic implementation of Event
 * which returns a constant value.
 */

#include "pch.h"

#include "Event/Event.h"

class ConstantEvent : public Event {
private:
    uint8_t eventValue;

public:
    ConstantEvent(uint8_t eventValue = 0) : eventValue(eventValue){};

    uint8_t value() const override { return eventValue; }
};

#endif