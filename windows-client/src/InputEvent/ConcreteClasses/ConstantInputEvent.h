#ifndef CONSTANT_INPUT_EVENT_H
#define CONSTANT_INPUT_EVENT_H

/**
 * @file ConstantInputEvent.h
 * @author Drew Smith
 * @brief The ConstantInputEvent is a basic implementation of InputEvent
 * which returns a constant value.
 * @date 2021-10-17
 *
 */

#include "pch.h"

#include "InputEvent/InputEvent.h"

#include <boost/log/trivial.hpp>
#include <boost/stacktrace.hpp>

/**
 * @brief The ConstantInputEvent is a basic implementation of InputEvent
 * which returns a constant value.
 *
 */
class ConstantInputEvent : public InputEvent {
private:
    int inputValue = 0;
    bool digital = true;

public:
    ConstantInputEvent(){};
    ConstantInputEvent(int inputValue, bool isDigital)
        : inputValue(inputValue), digital(isDigital){};

    int getInputValue() const override { return inputValue; }
    bool isDigital() const override { return digital; }

    void update() override {}
};

#endif