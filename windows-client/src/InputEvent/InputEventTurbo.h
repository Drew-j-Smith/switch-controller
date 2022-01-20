#ifndef INPUT_EVENT_TURBO_H
#define INPUT_EVENT_TURBO_H

#include "pch.h"

#include "InputEventToggle.h"

class InputEventTurbo : public InputEvent
{
private:
    std::shared_ptr<InputEventToggle> toggleEvent;
    std::shared_ptr<InputEvent> event;
public:
    InputEventTurbo(const int loopTime, std::shared_ptr<InputEvent> event) { 
        this->toggleEvent = std::make_shared<InputEventToggle>(loopTime, event);
        this->event = event;
    }

    int getInputValue() const override {
        if (!event->getInputValue())
            toggleEvent->setActive(false);
        return toggleEvent->getInputValue(); 
    };
    bool isDigital() const override { return true; }

    std::shared_ptr<InputEvent> makeShared(const boost::property_tree::ptree & tree, Factory<InputEvent> & factory) const override { return nullptr; }
    std::string getTypeName() const override { return ""; }

    void update() override {}

    std::string toString() const override { return ""; }
    boost::property_tree::ptree toPtree() const override {return boost::property_tree::ptree(); }
};

#endif