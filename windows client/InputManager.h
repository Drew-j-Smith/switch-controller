#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include "pch.h"

#include <boost/property_tree/ptree.hpp>

#include "InputEventCollection.h"
#include "SfJoystickInputEvent.h"
#include "Macro.h"

class InputManager
{
private:
    const int JOYSTICK_DEADZONE = 30;
    const std::map<std::string, int> buttonMap = {
        {"y", 0},
        {"b", 1},
        {"a", 2},
        {"x", 3},
        {"l", 4},
        {"r", 5},
        {"xl", 6},
        {"xr", 7},
        {"select", 8},
        {"start", 9},
        {"lClick", 10},
        {"rClick", 11},
        {"home", 12},
        {"capture", 13},

        {"leftStickXplus", 14},
        {"leftStickXminus", 15},
        {"leftStickYplus", 16},
        {"leftStickYminus", 17},
        {"leftStickX", 14},
        {"leftStickY", 16},

        {"rightStickXplus", 18},
        {"rightStickXminus", 19},
        {"rightStickYplus", 20},
        {"rightStickYminus", 21},
        {"rightStickX", 18},
        {"rightStickY", 20},

        {"dpadUp", 22},
        {"dpadRight", 23},
        {"dpadDown", 24},
        {"dpadLeft", 25},
        {"dpadX", 22},
        {"dpadY", 23}
    };

    std::shared_ptr<InputEvent> buttons[14];
    std::shared_ptr<InputEvent> controlSticks[8];
    std::shared_ptr<InputEvent> dpad[4];

public:
    InputManager(const boost::property_tree::ptree & tree);

    void getData(unsigned char* data) const;
private:
    unsigned char getControlStickData(int stick) const;
    unsigned char getDpadData() const;
    unsigned char getDpadData(bool up, bool right, bool down, bool left) const;

};

#endif