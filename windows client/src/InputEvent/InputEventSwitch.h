#ifndef INPUT_EVENT_SWITCH_H
#define INPUT_EVENT_SWITCH_H

#include "pch.h"

#include "InputEvent.h"

class InputEventSwitch : public InputEvent 
{
private:
    std::shared_ptr<InputEvent> event1;
    std::shared_ptr<InputEvent> event2;
    std::shared_ptr<InputEvent> deciderEvent;
public:
    InputEventSwitch(std::shared_ptr<InputEvent> event1, std::shared_ptr<InputEvent> event2, std::shared_ptr<InputEvent> deciderEvent) {
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

    bool isDigital() const override { return false; }

    std::shared_ptr<InputEvent> makeShared(const boost::property_tree::ptree & tree, const std::map<std::string, std::shared_ptr<InputEvent>> & eventMap) const override { return nullptr; }
    std::string getTypeName() const override { return ""; }

    void update() override {}

    std::string toString() const override { return ""; }
    boost::property_tree::ptree toPtree() const override {return boost::property_tree::ptree(); }
    bool operator==(const InputEvent& other) const override { return false; }
};

#endif