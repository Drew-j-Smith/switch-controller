#ifndef SF_JOYSTICK_INPUT_EVENT_H
#define SF_JOYSTICK_INPUT_EVENT_H

#include "pch.h"

#include <SFML/Window/Joystick.hpp>

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
    };

    int getInputValue() const override { 
        if (!sf::Joystick::isConnected(joystickIndex) || sf::Joystick::getButtonCount(joystickIndex) < button)
            return 0;
        return sf::Joystick::isButtonPressed(joystickIndex, button);
    };
    bool isDigital() const override { return true; }
};

class SfJoystickAnalogInputEvent : public InputEvent
{
private:
    unsigned int joystickIndex = 0;
    sf::Joystick::Axis axis = sf::Joystick::X;
public:
    SfJoystickAnalogInputEvent() {};
    SfJoystickAnalogInputEvent(unsigned int joystickIndex, sf::Joystick::Axis axis) { 
        this->joystickIndex = joystickIndex; 
        this->axis = axis;
    };

    int getInputValue() const override { 
        if (!sf::Joystick::isConnected(joystickIndex) || !sf::Joystick::hasAxis(joystickIndex, axis))
            return 0;
        return (100 + sf::Joystick::getAxisPosition(joystickIndex, axis)) / 200.0 * 255.0;
    };
    bool isDigital() const override { return false; }
};

#endif