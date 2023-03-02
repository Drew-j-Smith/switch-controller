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

    constexpr static std::array<uint8_t, 2>
    digtalToAnalogConversion(bool up, bool right, bool down, bool left) {
        return {uint8_t(128 + 127 * up - 128 * down),
                uint8_t(128 + 127 * right - 128 * left)};
    }

    constexpr static std::array<bool, 4>
    analogToDigitalConversion(std::array<uint8_t, 2> sticks) {
        return {sticks[1] > 128 + JOYSTICK_DEADZONE,
                sticks[0] > 128 + JOYSTICK_DEADZONE,
                sticks[1] < 128 - JOYSTICK_DEADZONE,
                sticks[0] < 128 - JOYSTICK_DEADZONE};
    }

    constexpr static std::array<uint8_t, 2>
    adjustStickValueForDeadzone(std::array<uint8_t, 2> sticks) {
        if (sticks[0] > 128 - InputCollection::JOYSTICK_DEADZONE &&
            sticks[0] < 128 + InputCollection::JOYSTICK_DEADZONE &&
            sticks[1] > 128 - InputCollection::JOYSTICK_DEADZONE &&
            sticks[1] < 128 + InputCollection::JOYSTICK_DEADZONE) {
            return {128, 128};
        } else {
            return sticks;
        }
    }

    constexpr static uint8_t convertSticksToHat(std::array<uint8_t, 2> sticks) {
        auto [up, right, down, left] = analogToDigitalConversion(sticks);

        if (right && up)
            return 1; // up right
        if (right && down)
            return 3; // down right
        if (left && down)
            return 5; // down left
        if (left && up)
            return 7; // up left
        if (up)
            return 0; // up
        if (right)
            return 2; // right
        if (down)
            return 4; // down
        if (left)
            return 6; // left

        return 8;
    }

    std::array<uint8_t, 8> getData() const {
        std::array<uint8_t, 8> res;
        res[0] = 85;
        res[1] = 0;
        res[2] = 0;
        for (std::size_t i = 0; i < 8; i++) {
            res[1] |= (buttons[i]() << i);
        }
        for (std::size_t i = 0; i < 6; i++) {
            res[2] |= (buttons[i + 8]() << i);
        }
        auto [leftX, leftY] =
            adjustStickValueForDeadzone(sticks[stickIndicies::left]());
        res[3] = leftX;
        res[4] = leftY;
        auto [rightX, rightY] =
            adjustStickValueForDeadzone(sticks[stickIndicies::right]());
        res[5] = rightX;
        res[6] = rightY;
        res[7] = convertSticksToHat(sticks[stickIndicies::hat]());
        return res;
    }
};
