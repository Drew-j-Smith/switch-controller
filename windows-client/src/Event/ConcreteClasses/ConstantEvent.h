#ifndef CONSTANT_EVENT_H
#define CONSTANT_EVENT_H

/**
 * @file ConstantEvent.h
 * @author Drew Smith
 * @brief The ConstantEvent is a basic implementation of Event
 * which returns a constant value.
 * @date 2021-10-17
 *
 */

#include "pch.h"

#include "Event/Event.h"

class ConstantEvent : public Event {
private:
    uint8_t eventValue = 0;
    bool digital = true;

public:
    ConstantEvent(){};
    ConstantEvent(uint8_t eventValue, bool isDigital)
        : eventValue(eventValue), digital(isDigital){};

    uint8_t getEventValue() const override { return eventValue; }
    bool isDigital() const override { return digital; }

    void update() override {}
};

#endif