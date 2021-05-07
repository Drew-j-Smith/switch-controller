#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include "../pch.h"

#include <boost/property_tree/ptree.hpp>

#include "InputEvent.h"

class InputManager
{
private:
    const int JOYSTICK_DEADZONE = 30;

    std::shared_ptr<InputEvent> buttons[14];
    std::shared_ptr<InputEvent> controlSticks[8];
    std::shared_ptr<InputEvent> dpad[4];

public:
    InputManager(const boost::property_tree::ptree & tree, const int turboButtonLoopTime);

    void getData(unsigned char* data) const;
private:
    unsigned char getControlStickData(int stick) const;
    unsigned char getDpadData() const;
    unsigned char getDpadData(bool up, bool right, bool down, bool left) const;

};

#endif