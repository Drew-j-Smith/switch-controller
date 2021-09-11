#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include "pch.h"

#include <boost/property_tree/ptree.hpp>

#include "InputEvent.h"

class InputManager
{
private:
    const int JOYSTICK_DEADZONE = 30;

    // 0-13 buttons
    // 14-21 controlSticks
    // 22-25 dpad
    // 26 turbo button
    std::shared_ptr<InputEvent> inputs[27];

public:
    InputManager(const boost::property_tree::ptree & tree, const int turboButtonLoopTime);

    void getData(unsigned char* data) const;
private:
    void loadInputEvent(const std::pair<const std::string, boost::property_tree::ptree> & it);
    int testInMap(const std::map<std::string, int> & map, const std::string & key);

    unsigned char getControlStickData(int stick) const;
    unsigned char getDpadData() const;
    unsigned char getDpadData(bool up, bool right, bool down, bool left) const;

};

#endif