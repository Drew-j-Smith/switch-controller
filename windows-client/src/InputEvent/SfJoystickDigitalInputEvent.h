#ifndef SF_JOYSTICK_DIGITAL_INPUT_EVENT_H
#define SF_JOYSTICK_DIGITAL_INPUT_EVENT_H

#include "pch.h"

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
        assertConnected();
    }
    SfJoystickDigitalInputEvent(boost::property_tree::ptree tree) { 
        joystickIndex = tree.get("joystick index", 0);
        button = tree.get("button", 0);
        assertConnected();
    }
    SfJoystickDigitalInputEvent(const boost::property_tree::ptree& tree, 
        [[maybe_unused]] const InputEventFactory& factory) {
        joystickIndex = tree.get<int>("joystick index");
        button = tree.get<int>("button", 0);
        assertConnected();
    }
    void assertConnected() {
        if (!sf::Joystick::isConnected(joystickIndex))
            std::cerr << "Joystick " << joystickIndex << " was requested but is not connected.\n";
    }

    int getInputValue() const override {
        if (!sf::Joystick::isConnected(joystickIndex) || sf::Joystick::getButtonCount(joystickIndex) < button)
            return 0;
        return sf::Joystick::isButtonPressed(joystickIndex, button);
    }
    bool isDigital() const override { return true; }

    std::shared_ptr<InputEvent> makeShared(const boost::property_tree::ptree & tree, [[maybe_unused]] Factory<InputEvent> & factory) const override {
        return std::make_shared<SfJoystickDigitalInputEvent>(tree);
    }
    std::string getTypeName() const override { return ""; }

    void update() override {}

    std::string toString() const override { return ""; }
    boost::property_tree::ptree toPtree() const override {return boost::property_tree::ptree(); }
};



#endif