#ifndef INPUT_EVENT_DECIDER_H
#define INPUT_EVENT_DECIDER_H

#include "../pch.h"

#include "../InputEvent/InputEvent.h"
#include "Decider.h"

class InputEventDecider : public Decider
{
private:
    std::shared_ptr<InputEvent> event;
public:
    InputEventDecider(std::string name, std::shared_ptr<InputEvent> event);

    int nextListIndex() const override { return event->getInputValue(); };
};


#endif