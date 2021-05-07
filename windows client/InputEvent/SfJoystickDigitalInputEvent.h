#ifndef SF_JOYSTICK_DIGITAL_INPUT_EVENT_H
#define SF_JOYSTICK_DIGITAL_INPUT_EVENT_H

#include "../pch.h"

#include <SFML/Window/Joystick.hpp>

#include <boost/property_tree/ptree.hpp>

#include "InputEvent.h"

class SfJoystickDigitalInputEvent : public InputEvent
{
private:
    unsigned int joystickIndex = 0;
    unsigned int button = 0;
public:
    SfJoystickDigitalInputEvent() {};
    SfJoystickDigitalInputEvent(unsigned int joystickIndex, unsigned int button) { 
        this->joystickIndex = joystickIndex; 
        this->button = button;
    }
    SfJoystickDigitalInputEvent(boost::property_tree::ptree tree) { 
        joystickIndex = tree.get("joystick index", 0);
        button = tree.get("button", 0);
    }

    int getInputValue() override {
        if (!sf::Joystick::isConnected(joystickIndex) || sf::Joystick::getButtonCount(joystickIndex) < button)
            return 0;
        return sf::Joystick::isButtonPressed(joystickIndex, button);
    }
    bool isDigital() const override { return true; }
};



#endif