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
    InputEventTurbo(const boost::property_tree::ptree& tree, InputEventFactory&& factory) {
        int loopTime = tree.get<int>("loop time");
        boost::property_tree::ptree childTree = tree.get_child("event");
        event = factory.create(childTree);
        toggleEvent = std::make_shared<InputEventToggle>(loopTime, event); //TODO this should not be calling make_shared
    }

    int getInputValue() const override {
        if (!event->getInputValue())
            toggleEvent->setActive(false);
        return toggleEvent->getInputValue(); 
    };
    bool isDigital() const override { return true; }

    std::shared_ptr<InputEvent> makeShared(const boost::property_tree::ptree & tree, Factory<InputEvent> & factory) const override {
        boost::property_tree::ptree childTree = tree.get_child("event");
        int loopTime = tree.get<int>("loop time");
        return std::make_shared<InputEventTurbo>(loopTime, factory.generateObject(childTree));
    }
    std::string getTypeName() const override { return ""; }

    void update() override {}

    std::string toString() const override { return ""; }
    boost::property_tree::ptree toPtree() const override {return boost::property_tree::ptree(); }
};

#endif