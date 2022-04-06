#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

/**
 * @brief The InputCollection class takes inputs and converts them into a
 * character array which can be sent to the arduino
 */
#include "pch.h"

#include "Event/Event.h"
#include "Macro/MacroRecorder.h"

class InputCollection {
private:
    const int JOYSTICK_DEADZONE = 30;

    std::map<std::string, std::shared_ptr<Event>> map;
    std::shared_ptr<MacroRecorder> recorder;

public:
    InputCollection(const std::map<std::string, std::shared_ptr<Event>> &map);

    int getStopEventValue() const { return getInputValue("stopMacros"); };
    std::shared_ptr<MacroRecorder> getRecorder() { return recorder; }

    std::array<uint8_t, 8> getData() const;

private:
    uint8_t getInputValue(const std::string &key) const {
        if (map.find(key) != map.end()) {
            return (uint8_t)map.at(key)->value();
        }
        return 0;
    }

    void setStickValue(uint8_t &xres, uint8_t &yres, uint8_t xvalue,
                       uint8_t yvalue) const;

    unsigned char getDpadData() const;

    unsigned char getDpadData(bool up, bool right, bool down, bool left) const;
};

#endif