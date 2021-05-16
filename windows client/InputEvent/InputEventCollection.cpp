#include "InputEventCollection.h"

#include "SfKeyboardInputEvent.h"
#include "SFJoystickDigitalInputEvent.h"
#include "InputEventInverter.h"
#include "InputEventToggle.h"
#include "InputEventTurbo.h"

InputEventCollection::InputEventCollection(const boost::property_tree::ptree & tree) {
    for (auto it = tree.begin(); it != tree.end(); ++it) {
        if (loadEventType(it))
            addEventModifiers(it);
    }
}

bool InputEventCollection::loadEventType(const boost::property_tree::ptree::const_iterator & it) {
    if (it->second.get("type", "") == "Default") {
        inputEvents.push_back(std::make_shared<DefaultInputEvent>());
        return true;
    }
    if (it->second.get("type", "") == "SF Keyboard Input") {
        inputEvents.push_back(std::make_shared<SfKeyboardInputEvent>(it->second));
        return true;
    }
    if (it->second.get("type", "") == "SF Digital Joystick Input") {
        inputEvents.push_back(std::make_shared<SfJoystickDigitalInputEvent>(it->second));
        return true;
    }

    std::cerr << "Unkown type in macro collection \"" << it->second.get("type", "") << "\"\n";
    return false;
}

void InputEventCollection::addEventModifiers(const boost::property_tree::ptree::const_iterator & it) {
    if (it->second.get("negated", false)) {
        inputEvents.back() = std::make_shared<InputEventInverter>(inputEvents.back());
    }
    if (it->second.get("toggle", false)) {
        inputEvents.back() = std::make_shared<InputEventToggle>(1000, inputEvents.back());
    }
    if (it->second.get("turbo", false)) {
        inputEvents.back() = std::make_shared<InputEventTurbo>(it->second.get("turbo loop", 500), inputEvents.back());
    }
}

int InputEventCollection::getInputValue() const { 
    if (inputEvents.size() == 0)
        return 0;

    for (int i = 0; i < inputEvents.size(); i++) 
        if (!inputEvents[i]->getInputValue())
            return 0;

    return 1;
}