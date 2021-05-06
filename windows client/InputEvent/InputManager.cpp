#include "InputManager.h"


InputManager::InputManager(const boost::property_tree::ptree & tree) {
    std::shared_ptr<InputEvent> defaultInput = std::make_shared<DefaultInputEvent>();
    std::shared_ptr<InputEvent> turboSwitch = defaultInput;
    for (int i = 0; i < 14; i++) {
        buttons[i] = defaultInput;
    }
    for (int i = 0; i < 8; i++) {
        controlSticks[i] = defaultInput;
    }
    for (int i = 0; i < 4; i++) {
        dpad[i] = defaultInput;
    }

    for (auto it : tree) {
        if (buttonMap.find(it.first) == buttonMap.end())
            continue;
        int index = buttonMap.at(it.first);
        if (index < 14) {
            buttons[index] = std::make_shared<InputEventCollection>(it.second);
        }
        else if (index < 22) {
            if (it.first == "leftStickX" || it.first == "leftStickY" || it.first == "rightStickX" || it.first == "rightStickY") {
                controlSticks[index - 14] = std::make_shared<SfJoystickAnalogInputEvent>(it.second);
            }
            else {
                controlSticks[index - 14] = std::make_shared<InputEventCollection>(it.second);
            }
        }
        else if (index < 26) {
            if (it.first == "dpadX" || it.first == "dpadY") {
                dpad[index - 22] = std::make_shared<SfJoystickAnalogInputEvent>(it.second);
            }
            else {
                dpad[index - 22] = std::make_shared<InputEventCollection>(it.second);
            }
        }
        else if (index == 26) {
            turboSwitch = std::make_shared<InputEventToggle>(500, std::make_shared<InputEventCollection>(it.second));
        }
    }

    for (int i = 0; i < 14; i++) {
        buttons[i] = std::make_shared<InputEventSwitch>(std::make_shared<InputEventTurbo>(tree.get("turbo loop time", 15), buttons[i]), buttons[i], turboSwitch);
    }
}

void InputManager::getData(unsigned char* data) const {
    data[0] = 85;
    data[1] = 0;
    data[2] = 0;
    for (int i = 0; i < 8; i++) {
        if (buttons[i]->getInputValue())
            data[1] |= (1 << i);
    }
    for (int i = 0; i < 6; i++) {
        if (buttons[i + 8]->getInputValue())
            data[2] |= (1 << i);
    }
    for (int i = 0; i < 4; i++) {
        data[i + 3] = getControlStickData(i);
    }
    data[7] = getDpadData();
}

unsigned char InputManager::getControlStickData(int stick) const {
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
