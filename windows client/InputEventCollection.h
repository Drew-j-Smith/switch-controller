#ifndef INPUT_EVENT_COLLECTION_H
#define INPUT_EVENT_COLLECTION_H

#include "pch.h"

#include <boost/property_tree/ptree.hpp>

#include "InputEvent.h"
#include "SfKeyboardInputEvent.h"
#include "SfJoystickInputEvent.h"
#include "InputEventInverter.h"
#include "InputEventToggle.h"

class InputEventCollection : public InputEvent
{
private:
    std::vector<std::shared_ptr<InputEvent>> inputEvents;
public:
    InputEventCollection() {};
    InputEventCollection(const std::vector<std::shared_ptr<InputEvent>> & inputEvents) { this->inputEvents = inputEvents; };
    InputEventCollection(const boost::property_tree::ptree & tree) {
        for (auto it = tree.begin(); it != tree.end(); ++it) {
            if (it->second.get("type", "") == "Default") {
                inputEvents.clear();
                return;
            }
            else if (it->second.get("type", "") == "SF Keyboard Input") {
                inputEvents.push_back(std::make_shared<SfKeyboardInputEvent>(it->second));
            }
            else if (it->second.get("type", "") == "SF Digital Joystick Input") {
                inputEvents.push_back(std::make_shared<SfJoystickDigitalInputEvent>(it->second));
            }
            if (it->second.get("negated", false)) {
                std::shared_ptr<InputEvent> invertedEvent = std::make_shared<InputEventInverter>(inputEvents.back());
                inputEvents.pop_back();
                inputEvents.push_back(invertedEvent);
            }
            if (it->second.get("toggle", false)) {
                std::shared_ptr<InputEvent> toggleEvent = std::make_shared<InputEventToggle>(it->second.get("toggle cooldown", 1000), inputEvents.back());
                inputEvents.pop_back();
                inputEvents.push_back(toggleEvent);
            }
        }
    };

    int getInputValue() override { 
        if (inputEvents.size() == 0)
            return 0;

        for (int i = 0; i < inputEvents.size(); i++) 
            if (!inputEvents[i]->getInputValue())
                return 0;

        return 1;
    };
    bool isDigital() const override { return true; }
};

#endif