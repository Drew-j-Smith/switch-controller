#ifndef INPUT_EVENT_NEGATOR_H
#define INPUT_EVENT_NEGATOR_H

#include "pch.h"

#include "InputEvent.h"

class InputEventNegator : public InputEvent
{
private:
    std::shared_ptr<InputEvent> event;
public:
    InputEventNegator(std::shared_ptr<InputEvent> event) : event(event) {};

    int getInputValue() const override {
        if (isDigital()) {
            return !event->getInputValue(); 
        } else {
            return 255 - event->getInputValue();
        }
    };
    bool isDigital() const override { return event->isDigital(); }
};


#endif
