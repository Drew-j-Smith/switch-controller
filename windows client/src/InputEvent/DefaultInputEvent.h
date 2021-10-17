#ifndef DEFAULT_INPUT_EVENT_H
#define DEFAULT_INPUT_EVENT_H

/**
 * @file DefaultInputEvent.h
 * @author Drew Smith
 * @brief The DefaultInputEvent is a basic implementation of InputEvent
 * which is a digital input that always returns 0.
 * @date 2021-10-17
 * 
 */

#include "pch.h"

#include "InputEvent.h"

/**
 * @brief The DefaultInputEvent is a basic implementation of InputEvent
 * which is a digital input that always returns 0.
 * 
 */
class DefaultInputEvent : public InputEvent
{
public:
    DefaultInputEvent() {};
    int getInputValue() const override { return 0; };
    bool isDigital() const override { return true; }
};

/**
 * @brief The ActiveInputEvent is a basic implementation of InputEvent
 * which is a digital input that always returns 1.
 * 
 */
class ActiveInputEvent : public InputEvent
{
public:
    ActiveInputEvent() {};
    int getInputValue() const override { return 1; };
    bool isDigital() const override { return true; }
};


#endif