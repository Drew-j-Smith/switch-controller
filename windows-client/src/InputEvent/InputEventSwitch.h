#ifndef INPUT_EVENT_SWITCH_H
#define INPUT_EVENT_SWITCH_H

#include "pch.h"

#include "InputEvent.h"
#include "DefaultInputEvent.h"

class InputEventSwitch : public InputEvent 
{
private:
    std::shared_ptr<InputEvent> event1;
    std::shared_ptr<InputEvent> event2;
    std::shared_ptr<InputEvent> deciderEvent;
public:
    InputEventSwitch() {
        this->event1 = std::make_shared<DefaultInputEvent>();
        this->event2 = event1;
        this->deciderEvent = event1;
    }
    InputEventSwitch(std::shared_ptr<InputEvent> event1, std::shared_ptr<InputEvent> event2, std::shared_ptr<InputEvent> deciderEvent) {
        if (event1->isDigital() != event2->isDigital() || !deciderEvent->isDigital()) {
            throw std::invalid_argument("Invalid input types for InputEventSwitch: " +
            event1->getTypeName() + " " + event2->getTypeName() + " " + deciderEvent->getTypeName());
        }
        this->event1 = event1;
        this->event2 = event2;
        this->deciderEvent = deciderEvent;
    }

    int getInputValue() const override {
        if (deciderEvent->getInputValue())
            return event1->getInputValue();
        else
            return event2->getInputValue();
    }

    bool isDigital() const override { return event1->isDigital(); }

    std::shared_ptr<InputEvent> makeShared(const boost::property_tree::ptree & tree, Factory<InputEvent> & factory) const override {
        boost::property_tree::ptree newEvent1 = tree.get_child("event 1");
        boost::property_tree::ptree newEvent2 = tree.get_child("event 2");
        boost::property_tree::ptree newDeciderEvent = tree.get_child("decider event");
        return std::make_shared<InputEventSwitch>(factory.generateObject(newEvent1), factory.generateObject(newEvent2), factory.generateObject(newDeciderEvent));
    }
    std::string getTypeName() const override { return ""; }

    void update() override {}

    std::string toString() const override { return ""; }
    boost::property_tree::ptree toPtree() const override {return boost::property_tree::ptree(); }
};

#endif