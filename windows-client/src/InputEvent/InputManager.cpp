#include "InputManager.h"

#include "DefaultInputEvent.h"
#include "InputEventCollection.h"
#include "InputEventSwitch.h"
#include "InputEventTurbo.h"
#include "SfJoystickAnalogInputEvent.h"

InputManager::InputManager(const boost::property_tree::ptree &tree,
                           const int turboButtonLoopTime) {
    // set all inputs to be a default input
    std::shared_ptr<InputEvent> defaultInput =
        std::make_shared<DefaultInputEvent>();
    for (int i = 0; i < 27; i++) {
        inputs[i] = defaultInput;
    }

    // go througth the entire list and try to load inputs
    for (auto &it : tree) {
        loadInputEvent(it);
    }

    // wrap each event in a turbo event which
    // which keeps the input the same unless inputs[26] is pressed
    for (int i = 0; i < 14; i++) {
        std::shared_ptr<InputEvent> turboEvent =
            std::make_shared<InputEventTurbo>(turboButtonLoopTime, inputs[i]);
        inputs[i] = std::make_shared<InputEventSwitch>(turboEvent, inputs[i],
                                                       inputs[26]);
    }
}

static const std::set<std::string> unusedButtons = {
    "record", "playLastRecorded", "stopMacros"};

static const std::map<std::string, int> digitalInputMap = {
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
    {"dpadLeft", 25}};

static const std::map<std::string, int> analogInputMap = {
    {"leftStickX", 14},  {"leftStickY", 16}, {"rightStickX", 18},
    {"rightStickY", 20}, {"dpadX", 22},      {"dpadY", 23}};

void InputManager::loadInputEvent(
    const std::pair<const std::string, boost::property_tree::ptree> &it) {
    // trys to match the name to a button
    // once a match is found, the button is loaded and the function returns

    // buttons that are used other places but not in this class
    if (unusedButtons.find(it.first) != unusedButtons.end())
        return;

    int index = testInMap(digitalInputMap, it.first);
    if (index >= 0) {
        inputs[index] = std::make_shared<InputEventCollection>(it.second);
        return;
    }

    index = testInMap(analogInputMap, it.first);
    if (index >= 0) {
        inputs[index] = std::make_shared<SfJoystickAnalogInputEvent>(it.second);
        return;
    }

    if (it.first == "turboButtonToggle") {
        inputs[26] = std::make_shared<InputEventToggle>(
            500, std::make_shared<InputEventCollection>(it.second));
        return;
    }

    // if a button could not be matched
    std::cerr << "Unknown button type \"" << it.first
              << "\" loaded into input manager.\n";
    std::cerr << "Consult InputManager.cpp for a list of valid buttons.\n";
}

int InputManager::testInMap(const std::map<std::string, int> &map,
                            const std::string &key) {
    if (map.find(key) != map.end()) {
        return map.at(key);
    }
    return -1;
}

std::array<uint8_t, 8> InputManager::getData() const {
    std::array<uint8_t, 8> res;
    res[0] = 85;
    res[1] = 0;
    res[2] = 0;
    for (int i = 0; i < 8; i++) {
        if (inputs[i]->getInputValue())
            res[1] |= (1 << i);
    }
    for (int i = 0; i < 6; i++) {
        if (inputs[i + 8]->getInputValue())
            res[2] |= (1 << i);
    }
    for (int i = 0; i < 4; i++) {
        res[i + 3] = getControlStickData(i);
    }
    res[7] = getDpadData();
    return res;
}

bool InputManager::isInDeadzone(int stick) const {
    const int offset = 14;             // control sticks start at inputs[14]
    const int actualStick = stick * 2; // there are 2 inputs per control stick
    int stickData = inputs[offset + actualStick]->getInputValue();
    return stickData > 128 - JOYSTICK_DEADZONE &&
           stickData < 128 + JOYSTICK_DEADZONE;
}

// TODO make readable
unsigned char InputManager::getControlStickData(int stick) const {
    const int offset = 14;             // control sticks start at inputs[14]
    const int actualStick = stick * 2; // there are 2 inputs per control stick

    if (inputs[offset + actualStick]->isDigital()) {
        return (
            unsigned char)(128 +
                           inputs[offset + actualStick]->getInputValue() * 127 -
                           inputs[offset + actualStick + 1]->getInputValue() *
                               128);
    } else {
        if (isInDeadzone((stick >= 2) * 2) &&
            isInDeadzone((stick >= 2) * 2 + 1)) {
            return 128;
        } else
            return (unsigned char)inputs[offset + actualStick]->getInputValue();
    }
}

unsigned char InputManager::getDpadData() const {
    const int offset = 22; // Dpad data starts at inputs[22]
    if (inputs[offset]->isDigital()) {
        return getDpadData(inputs[offset]->getInputValue(),
                           inputs[offset + 1]->getInputValue(),
                           inputs[offset + 2]->getInputValue(),
                           inputs[offset + 3]->getInputValue());
    } else {
        return getDpadData(
            inputs[offset]->getInputValue() > 128 + JOYSTICK_DEADZONE,
            inputs[offset + 1]->getInputValue() > 128 + JOYSTICK_DEADZONE,
            inputs[offset]->getInputValue() < 128 - JOYSTICK_DEADZONE,
            inputs[offset + 1]->getInputValue() < 128 - JOYSTICK_DEADZONE);
    }
}

unsigned char InputManager::getDpadData(bool up, bool right, bool down,
                                        bool left) const {
    unsigned char result = 8;
    if (up && !down) {
        result = 0; // up
    }
    if (down && !up) {
        result = 4; // down
    }
    if (right && !left) {
        if (result == 0)
            result = 1; // up right
        if (result == 8)
            result = 2; // right
        if (result == 4)
            result = 3; // down right
    }
    if (left && !right) {
        if (result == 4)
            result = 5; // down left
        if (result == 8)
            result = 6; // left
        if (result == 0)
            result = 7; // up left
    }
    return result;
}
