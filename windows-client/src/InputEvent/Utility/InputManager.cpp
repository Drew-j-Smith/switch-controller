#include "InputManager.h"

#include <boost/log/trivial.hpp>
#include <boost/stacktrace/stacktrace.hpp>

#include "InputEvent/ConcreteClasses/ConstantInputEvent.h"

InputManager::InputManager(
    const std::map<std::string, std::shared_ptr<InputEvent>> map) {
    // set all inputs to be a default input
    std::shared_ptr<InputEvent> defaultInput =
        std::make_shared<ConstantInputEvent>();
    for (auto &event : buttonEvents) {
        event = defaultInput;
    }
    for (auto &event : controlStickEvents) {
        event = defaultInput;
    }
    for (auto &event : dpadEvents) {
        event = defaultInput;
    }

    // go througth the entire list and try to load inputs
    for (auto &it : map) {
        loadInputEvent(it);
    }
}

static const std::set<std::string> unusedButtons = {
    "record", "playLastRecorded", "stopMacros"};

static const std::map<std::string, int> buttonMap = {
    {"y", 0},       {"b", 1},       {"a", 2},     {"x", 3},       {"l", 4},
    {"r", 5},       {"xl", 6},      {"xr", 7},    {"select", 8},  {"start", 9},
    {"lClick", 10}, {"rClick", 11}, {"home", 12}, {"capture", 13}};

static const std::map<std::string, int> controlStickMap = {
    {"leftStickXplus", 0},  {"leftStickXminus", 1},  {"leftStickYplus", 2},
    {"leftStickYminus", 3}, {"rightStickXplus", 4},  {"rightStickXminus", 5},
    {"rightStickYplus", 6}, {"rightStickYminus", 7}, {"leftStickX", 0},
    {"leftStickY", 2},      {"rightStickX", 4},      {"rightStickY", 6}};

static const std::map<std::string, int> dpadMap = {
    {"dpadUp", 0},   {"dpadRight", 1}, {"dpadDown", 2},
    {"dpadLeft", 3}, {"dpadX", 0},     {"dpadY", 2}};

void InputManager::loadInputEvent(
    const std::pair<const std::string, std::shared_ptr<InputEvent>> &it) {
    // trys to match the name to a button
    // once a match is found, the button is loaded and the function returns

    // buttons that are used other places but not in this class
    if (unusedButtons.find(it.first) != unusedButtons.end())
        return;

    int index = testInMap(buttonMap, it.first);
    if (index >= 0) {
        buttonEvents[index] = it.second;
        return;
    }

    index = testInMap(controlStickMap, it.first);
    if (index >= 0) {
        controlStickEvents[index] = it.second;
        return;
    }

    index = testInMap(dpadMap, it.first);
    if (index >= 0) {
        dpadEvents[index] = it.second;
        return;
    }

    // if a button could not be matched
    BOOST_LOG_TRIVIAL(warning)
        << "Unknown button type \"" + it.first +
               "\" loaded into input manager.\n"
               "Consult InputManager.cpp for a list of valid buttons.\n" +
               boost::stacktrace::to_string(boost::stacktrace::stacktrace());
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
        if (buttonEvents[i]->getInputValue())
            res[1] |= (1 << i);
    }
    for (int i = 0; i < 6; i++) {
        if (buttonEvents[i + 8]->getInputValue())
            res[2] |= (1 << i);
    }
    for (int i = 0; i < 4; i++) {
        res[i + 3] = getControlStickData(i);
    }
    res[7] = getDpadData();
    return res;
}

bool InputManager::isInDeadzone(int stick) const {
    int stickData = controlStickEvents[stick * 2]->getInputValue();
    return stickData > 128 - JOYSTICK_DEADZONE &&
           stickData < 128 + JOYSTICK_DEADZONE;
}

uint8_t InputManager::getControlStickData(int stick) const {
    if (controlStickEvents[stick * 2]->isDigital()) {
        return (uint8_t)(128 +
                         controlStickEvents[stick * 2]->getInputValue() * 127 -
                         controlStickEvents[stick * 2 + 1]->getInputValue() *
                             128);
    } else {
        if (isInDeadzone(stick < 2 ? 0 : 2) &&
            isInDeadzone(stick < 2 ? 1 : 3)) {
            return 128;
        } else
            return (uint8_t)controlStickEvents[stick * 2]->getInputValue();
    }
}

unsigned char InputManager::getDpadData() const {
    if (dpadEvents[0]->isDigital()) {
        return getDpadData(
            dpadEvents[0]->getInputValue(), dpadEvents[1]->getInputValue(),
            dpadEvents[2]->getInputValue(), dpadEvents[3]->getInputValue());
    } else {
        return getDpadData(
            dpadEvents[0]->getInputValue() > 128 + JOYSTICK_DEADZONE,
            dpadEvents[1]->getInputValue() > 128 + JOYSTICK_DEADZONE,
            dpadEvents[0]->getInputValue() < 128 - JOYSTICK_DEADZONE,
            dpadEvents[1]->getInputValue() < 128 - JOYSTICK_DEADZONE);
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