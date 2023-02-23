#include "InputCollection.h"

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
    auto [up, right, down, left] =
        InputCollection::AnalogToDigitalConversion(sticks);

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

std::array<uint8_t, 8> InputCollection::getData() const {
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
