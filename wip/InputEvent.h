#ifndef INPUT_EVENT_H
#define INPUT_EVENT_H

#include "pch.h"

class InputEvent
{
public:
    /*
     * Returns the value of an input.
     * if the input is digital the result will be [0, 1]
     * if the input is analog the result will be [0, 255]
    */
    virtual int getInputValue() const = 0;
    virtual bool isDigital() const = 0;
};

class DefaultInputEvent : public InputEvent
{
public:
    DefaultInputEvent() {};
    int getInputValue() const override { return 0; };
    bool isDigital() const override { return true; }
};

#endif