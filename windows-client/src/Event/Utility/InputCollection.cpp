#include "InputCollection.h"

#include "Event/ConcreteClasses/ConstantEvent.h"

InputCollection::InputCollection(
    const std::map<std::string, std::shared_ptr<Event>> &map)
    : map(map) {

    auto recordEventIt = map.find("record");
    std::shared_ptr<Event> recordEvent = std::make_shared<ConstantEvent>();
    if (recordEventIt != map.end()) {
        recordEvent = recordEventIt->second;
    }
    auto playEventIt = map.find("playLastRecorded");
    std::shared_ptr<Event> playEvent = std::make_shared<ConstantEvent>();
    if (playEventIt != map.end()) {
        playEvent = playEventIt->second;
    }
    recorder = std::make_shared<MacroRecorder>(recordEvent, playEvent);
}

std::array<uint8_t, 8> InputCollection::getData() const {
    std::array<uint8_t, 8> res;
    res[0] = 85;
    res[1] = (getInputValue("y") << 0) | (getInputValue("b") << 1) |
             (getInputValue("a") << 2) | (getInputValue("x") << 3) |
             (getInputValue("l") << 4) | (getInputValue("r") << 5) |
             (getInputValue("xl") << 6) | (getInputValue("xr") << 7);
    res[2] = (getInputValue("select") << 0) | (getInputValue("start") << 1) |
             (getInputValue("lClick") << 2) | (getInputValue("rClick") << 3) |
             (getInputValue("home") << 4) | (getInputValue("capture") << 5);
    if (map.find("leftStickX") != map.end()) {
        setStickValue(res[3], res[4], getInputValue("leftStickX"),
                      getInputValue("leftStickY"));
        setStickValue(res[5], res[6], getInputValue("rightStickX"),
                      getInputValue("rightStickY"));
    } else {
        res[3] = 128 + getInputValue("leftStickXplus") * 127 -
                 getInputValue("leftStickXminus") * 128;
        res[4] = 128 + getInputValue("leftStickYplus") * 127 -
                 getInputValue("leftStickYminus") * 128;
        res[5] = 128 + getInputValue("rightStickXplus") * 127 -
                 getInputValue("rightStickXminus") * 128;
        res[6] = 128 + getInputValue("rightStickYplus") * 127 -
                 getInputValue("rightStickYminus") * 128;
    }
    res[7] = getDpadData();
    return res;
}

void InputCollection::setStickValue(uint8_t &xres, uint8_t &yres,
                                    uint8_t xvalue, uint8_t yvalue) const {
    if (xvalue > 128 - JOYSTICK_DEADZONE && xvalue < 128 + JOYSTICK_DEADZONE &&
        yvalue > 128 - JOYSTICK_DEADZONE && yvalue < 128 + JOYSTICK_DEADZONE) {
        xres = 128;
        yres = 128;
    } else {
        xres = xvalue;
        yres = yvalue;
    }
}

unsigned char InputCollection::getDpadData() const {
    if (map.find("dpadUp") != map.end()) {
        return getDpadData(getInputValue("dpadUp"), getInputValue("dpadRight"),
                           getInputValue("dpadDown"),
                           getInputValue("dpadLeft"));
    } else {
        return getDpadData(getInputValue("dpadY") > 128 + JOYSTICK_DEADZONE,
                           getInputValue("dpadX") > 128 + JOYSTICK_DEADZONE,
                           getInputValue("dpadY") < 128 - JOYSTICK_DEADZONE,
                           getInputValue("dpadX") < 128 - JOYSTICK_DEADZONE);
    }
}

unsigned char InputCollection::getDpadData(bool up, bool right, bool down,
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