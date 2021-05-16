#include "InputManager.h"

#include "InputEventCollection.h"
#include "SfJoystickAnalogInputEvent.h"
#include "InputEventSwitch.h"
#include "InputEventTurbo.h"

InputManager::InputManager(const boost::property_tree::ptree & tree, const int turboButtonLoopTime) {
    std::shared_ptr<InputEvent> defaultInput = std::make_shared<DefaultInputEvent>();

    for (int i = 0; i < 27; i++) {
        inputs[i] = defaultInput;
    }

    for (auto & it : tree) {
        loadInputEvent(it);
    }

    int turboLoopTime = tree.get("turbo loop time", 15);
    for (int i = 0; i < 14; i++) {
        std::shared_ptr<InputEvent> turboEvent = std::make_shared<InputEventTurbo>(turboLoopTime, inputs[i]);
        inputs[i] = std::make_shared<InputEventSwitch>(turboEvent, inputs[i], inputs[26]);
    }
}

const std::set<std::string> unusedButtons = {
    "record",
    "playLastRecorded",
    "stopMacros"
};

const std::map<std::string, int> digitalInputMap = { 
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
    {"rightStickXplus", 18},
    {"rightStickXminus", 19},
    {"rightStickYplus", 20},
    {"rightStickYminus", 21},
    {"dpadUp", 22},
    {"dpadRight", 23},
    {"dpadDown", 24},
    {"dpadLeft", 25}
};

const std::map<std::string, int> analogInputMap = {
    {"leftStickX", 14},
    {"leftStickY", 16},
    {"rightStickX", 18},
    {"rightStickY", 20},
    {"dpadX", 22},
    {"dpadY", 23}
};

void InputManager::loadInputEvent(const std::pair<const std::string, boost::property_tree::ptree> & it) {
    std::shared_ptr<InputEvent>* event;

    if (unusedButtons.find(it.first) != unusedButtons.end())
        return;

    if (testInMap(digitalInputMap, it.first, event)) {
        *event = std::make_shared<InputEventCollection>(it.second);
        return;
    }

    if (testInMap(analogInputMap, it.first, event)) {
        *event = std::make_shared<SfJoystickAnalogInputEvent>(it.second);
        return;
    }

    if (it.first == "turboButtonToggle") {
        inputs[26] = std::make_shared<InputEventToggle>(500, std::make_shared<InputEventCollection>(it.second));
        return;
    }

    std::cerr << "Unknown button type \"" << it.first << "\"\n";
}


bool InputManager::testInMap(const std::map<std::string, int> & map, const std::string & key, std::shared_ptr<InputEvent>* & event) {
    if (map.find(key) != map.end()) {
        event = &inputs[map.at(key)];
        return true;
    }
    return false;
}

void InputManager::getData(unsigned char* data) const {
    data[0] = 85;
    data[1] = 0;
    data[2] = 0;
    for (int i = 0; i < 8; i++) {
        if (inputs[i]->getInputValue())
            data[1] |= (1 << i);
    }
    for (int i = 0; i < 6; i++) {
        if (inputs[i + 8]->getInputValue())
            data[2] |= (1 << i);
    }
    for (int i = 0; i < 4; i++) {
        data[i + 3] = getControlStickData(i);
    }
    data[7] = getDpadData();
}

unsigned char InputManager::getControlStickData(int stick) const {
    const std::shared_ptr<InputEvent>* controlSticks = &inputs[14];
    if (controlSticks[stick * 2]->isDigital()) {
        return controlSticks[stick * 2]->getInputValue() * 127
            - !(controlSticks[stick * 2 + 1]->getInputValue()) * 128;
    } else {
        if (controlSticks[0 + (stick >= 2) * 4]->getInputValue() > 128 - JOYSTICK_DEADZONE && controlSticks[0 + (stick >= 2) * 4]->getInputValue() < 128 + JOYSTICK_DEADZONE &&
            controlSticks[2 + (stick >= 2) * 4]->getInputValue() > 128 - JOYSTICK_DEADZONE && controlSticks[2 + (stick >= 2) * 4]->getInputValue() < 128 + JOYSTICK_DEADZONE)
            return 128;
        else 
            return controlSticks[stick * 2]->getInputValue();
    }
}
    

unsigned char InputManager::getDpadData() const {
    const std::shared_ptr<InputEvent>* dpad = &inputs[22];
    if (dpad[0]->isDigital()) {
        return getDpadData(dpad[0]->getInputValue(), dpad[1]->getInputValue(), dpad[2]->getInputValue(), dpad[3]->getInputValue());
    } else {
        return getDpadData(dpad[0]->getInputValue() > 128 + JOYSTICK_DEADZONE, dpad[1]->getInputValue() > 128 + JOYSTICK_DEADZONE,
            dpad[0]->getInputValue() < 128 - JOYSTICK_DEADZONE, dpad[1]->getInputValue() < 128 - JOYSTICK_DEADZONE);
    }
}

unsigned char InputManager::getDpadData(bool up, bool right, bool down, bool left) const {
    unsigned char result = 8;
	if (up && !down){
		result = 0; //up
	}
	if (down && !up){
		result = 4; //down
	}
	if (right && !left){
		if(result == 0) result = 1; //up right
		if(result == 8) result = 2; //right
		if(result == 4) result = 3; //down right
	}
	if (left && !right){
		if(result == 4) result = 5; //down left
		if(result == 8) result = 6; //left
		if(result == 0) result = 7; //up left
	}
    return result;
}
