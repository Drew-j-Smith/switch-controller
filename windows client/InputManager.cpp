#include "InputManager.h"


InputManager::InputManager(const boost::property_tree::ptree & tree) {
    std::shared_ptr<InputEvent> defaultInput = std::make_shared<DefaultInputEvent>();
    for (int i = 0; i < 14; i++) {
        buttons[i] = defaultInput;
    }
    for (int i = 0; i < 8; i++) {
        controlSticks[i] = defaultInput;
    }
    for (int i = 0; i < 4; i++) {
        dpad[i] = defaultInput;
    }
    record = defaultInput;

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
            record = std::make_shared<InputEventCollection>(it.second);
        }
        else if (index == 27) {
            this->lastRecordedMacro->setInputEvent(std::make_shared<InputEventCollection>(it.second));
        }
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
        if (stick < 2){
            if (controlSticks[0]->getInputValue() > 128 - JOYSTICK_DEADZONE && controlSticks[0]->getInputValue() < 128 + JOYSTICK_DEADZONE &&
                controlSticks[2]->getInputValue() > 128 - JOYSTICK_DEADZONE && controlSticks[2]->getInputValue() < 128 + JOYSTICK_DEADZONE)
                return 128;
            else 
                return controlSticks[stick * 2]->getInputValue();
        }
        else {
            if (controlSticks[4]->getInputValue() > 128 - JOYSTICK_DEADZONE && controlSticks[4]->getInputValue() < 128 + JOYSTICK_DEADZONE &&
                controlSticks[6]->getInputValue() > 128 - JOYSTICK_DEADZONE && controlSticks[6]->getInputValue() < 128 + JOYSTICK_DEADZONE)
                return 128;
            else 
                return controlSticks[stick * 2]->getInputValue();
        }
        
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
    result = 8;
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


void InputManager::updateRecording() {
    
    auto now = std::chrono::steady_clock::now();
    
    if (record->getInputValue() && std::chrono::duration_cast<std::chrono::milliseconds>(now - activationTime).count() > RECORDING_BUTTON_COOLDOWN) {
        activationTime = now;
        if (recording) {
            //getting the time as a string
            auto t = std::time(nullptr);
            auto tm = *std::localtime(&t);
            std::ostringstream oss;
            oss << std::put_time(&tm, "%d-%m-%Y %H-%M-%S");
            std::string str = oss.str();
            std::cout << "Saved recording to \"" << str << "\"" << std::endl;

            auto macroData = currentRecordingMacro->getData();
            macroData.save(str);
            lastRecordedMacro->setData(macroData);
            currentRecordingMacro->setData({});
        }
        recording = !recording;
    }
    if (recording) {
        unsigned char data[8];
        getData(data);
        currentRecordingMacro->appendData(std::chrono::duration_cast<std::chrono::milliseconds>(now - activationTime).count(), data);
    }
}
