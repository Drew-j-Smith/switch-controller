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

/**
 * @brief The ConstantEvent is a basic implementation of Event
 * which returns a constant value.
 *
 */
class ConstantEvent : public Event {
private:
    int inputValue = 0;
    bool digital = true;

public:
    ConstantEvent(){};
    ConstantEvent(int inputValue, bool isDigital)
        : inputValue(inputValue), digital(isDigital){};

    int getInputValue() const override { return inputValue; }
    bool isDigital() const override { return digital; }

    void update() override {}
};

#endif