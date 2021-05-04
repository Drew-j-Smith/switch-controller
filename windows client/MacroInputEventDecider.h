#ifndef MACRO_INPUT_EVENT_DECIDER_H
#define MACRO_INPUT_EVENT_DECIDER_H

#include "pch.h"

#include "InputEvent.h"
#include "MacroDecider.h"

class MacroInputEventDecider : public MacroDecider
{
private:
    std::shared_ptr<InputEvent> event;
public:
    MacroInputEventDecider(std::string name, std::shared_ptr<InputEvent> event);

    int nextMacroListIndex() const override { return event->getInputValue(); };
};


#endif