#ifndef INPUT_EVENT_TURBO_H
#define INPUT_EVENT_TURBO_H

#include "../pch.h"

#include "InputEventToggle.h"

class InputEventTurbo : public InputEvent 
{
private:
    std::shared_ptr<InputEvent> event;
public:
    InputEventTurbo(const int loopTime) { 
        event = std::make_shared<InputEventToggle>(loopTime, std::make_shared<ActiveInputEvent>());
    }

    int getInputValue() override { return event->getInputValue(); };
    bool isDigital() const override { return true; }
};

#endif