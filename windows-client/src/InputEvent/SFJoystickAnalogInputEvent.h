#ifndef SF_JOYSTICK_ANALOG_INPUT_EVENT_H
#define SF_JOYSTICK_ANALOG_INPUT_EVENT_H

#include "pch.h"

#include <SFML/Window/Joystick.hpp>

#include <boost/property_tree/ptree.hpp>

#include "InputEvent.h"

class SfJoystickAnalogInputEvent : public InputEvent
{
private:
    unsigned int joystickIndex = 0;
    sf::Joystick::Axis axis = sf::Joystick::X;
    const double SCALING = 1.4;
public:
    SfJoystickAnalogInputEvent() {};
    SfJoystickAnalogInputEvent(unsigned int joystickIndex, sf::Joystick::Axis axis) { 
        this->joystickIndex = joystickIndex; 
        this->axis = axis;
        assertConnected();
    }
    SfJoystickAnalogInputEvent(const boost::property_tree::ptree & tree) { 
        joystickIndex = tree.get("joystick index", 0);
        axis = (sf::Joystick::Axis) tree.get("axis", 0);
        assertConnected();
    }
    void assertConnected() {
        if (!sf::Joystick::isConnected(joystickIndex))
            std::cerr << "Joystick " << joystickIndex << " was requested but is not connected.\n";
    }

    int getInputValue() const override { 
        if (!sf::Joystick::isConnected(joystickIndex) || !sf::Joystick::hasAxis(joystickIndex, axis))
            return 128;
        int value = (100 + sf::Joystick::getAxisPosition(joystickIndex, axis) * SCALING) / 200.0 * 255.0;
        if (value > 255)
            value = 255;
        if (value < 0)
            value = 0;
        return value;
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