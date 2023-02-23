#pragma once

/**
 * @brief The InputCollection class takes inputs and converts them into a
 * character array which can be sent to the arduino
 */
#include "pch.h"

class InputCollection {
private:
    const std::array<std::function<bool()>, 14> buttons;
    const std::array<std::function<std::array<uint8_t, 2>()>, 3> sticks;

public:
    enum buttonIndicies {
        y,
        b,
        a,
        x,
        l,
        r,
        xl,
        xr,
        Select,
        start,
        lClick,
        rClick,
        home,
        capture
    };

    enum stickIndicies { left, right, hat };

    constexpr static int JOYSTICK_DEADZONE = 30;

    InputCollection(
        const std::array<std::function<bool()>, 14> &buttons,
        const std::array<std::function<std::array<uint8_t, 2>()>, 3> &sticks)
        : buttons(buttons), sticks(sticks){};

    std::array<uint8_t, 8> getData() const;

    constexpr static std::array<uint8_t, 2>
    digtalToAnalogConversion(bool up, bool right, bool down, bool left) {
        return {uint8_t(128 + 127 * up - 128 * down),
                uint8_t(128 + 127 * right - 128 * left)};
    }

    constexpr static std::array<bool, 4>
    AnalogToDigitalConversion(std::array<uint8_t, 2> sticks) {
        return {sticks[1] > 128 + JOYSTICK_DEADZONE,
                sticks[0] > 128 + JOYSTICK_DEADZONE,
                sticks[1] < 128 - JOYSTICK_DEADZONE,
                sticks[0] < 128 - JOYSTICK_DEADZONE};
    }
};
