#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

/**
 * @file InputCollection.h
 * @author Drew Smith
 * @brief This file is the header for the InputCollection class which
 * takes inputs and converts them into a character array which can be sent to
 * the arduino
 * @date 2021-09-11
 *
 */
#include "pch.h"

#include "Event/Event.h"
#include "Macro/MacroRecorder.h"

/**
 * @brief This class takes inputs and converts them into a character array which
 * can be sent to the arduino
 *
 * @invariant The elements in inputs will not be null
 */
class InputCollection {
private:
    /**
     * @brief used to determine how large the deadzone on control sticks will be
     * control stick range is -128 to 127
     * A value of 30 means that any value between -30 and 30 will register as 0
     *
     */
    const int JOYSTICK_DEADZONE = 30;

    std::map<std::string, std::shared_ptr<Event>> map;
    std::shared_ptr<MacroRecorder> recorder;

public:
    InputCollection(const std::map<std::string, std::shared_ptr<Event>> &map);

    int getStopEventValue() const { return getInputValue("stopMacros"); };
    std::shared_ptr<MacroRecorder> getRecorder() { return recorder; }

    /**
     * @brief Gets the user's inputs in the form of a uint8_t array
     *
     */
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