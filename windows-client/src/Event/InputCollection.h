#pragma once

/**
 * @brief The InputCollection class takes inputs and converts them into a
 * character array which can be sent to the arduino
 */
#include "pch.h"

#include <boost/program_options.hpp>

#include "Event/ConcreteClasses/EventToggle.h"

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

    static auto getButtons() {
        // capture button
        constexpr auto toggle = [] {
            return sf::Joystick::isButtonPressed(0, 13);
        };

        std::array<std::function<bool()>, 14> buttons;
        buttons[InputCollection::buttonIndicies::b] = ToggleEvent{
            [toggle] {
                return sf::Joystick::isButtonPressed(0, 0) && !toggle();
            },
            20ms};
        buttons[InputCollection::buttonIndicies::a] = [toggle] {
            return sf::Joystick::isButtonPressed(0, 1) && !toggle();
        };
        buttons[InputCollection::buttonIndicies::y] = [toggle] {
            return sf::Joystick::isButtonPressed(0, 2) && !toggle();
        };
        buttons[InputCollection::buttonIndicies::x] = [toggle] {
            return sf::Joystick::isButtonPressed(0, 3) && !toggle();
        };
        buttons[InputCollection::buttonIndicies::l] = [toggle] {
            return sf::Joystick::isButtonPressed(0, 4) && !toggle();
        };
        buttons[InputCollection::buttonIndicies::r] = [toggle] {
            return sf::Joystick::isButtonPressed(0, 5) && !toggle();
        };
        buttons[InputCollection::buttonIndicies::xl] = [toggle] {
            return sf::Joystick::isButtonPressed(0, 6) && !toggle();
        };
        buttons[InputCollection::buttonIndicies::xr] = [toggle] {
            return sf::Joystick::isButtonPressed(0, 7) && !toggle();
        };
        buttons[InputCollection::buttonIndicies::Select] = [toggle] {
            return sf::Joystick::isButtonPressed(0, 8) && !toggle();
        };
        buttons[InputCollection::buttonIndicies::start] = [toggle] {
            return sf::Joystick::isButtonPressed(0, 9) && !toggle();
        };
        buttons[InputCollection::buttonIndicies::lClick] = [toggle] {
            return sf::Joystick::isButtonPressed(0, 10) && !toggle();
        };
        buttons[InputCollection::buttonIndicies::rClick] = [toggle] {
            return sf::Joystick::isButtonPressed(0, 11) && !toggle();
        };
        buttons[InputCollection::buttonIndicies::home] = [toggle] {
            return sf::Joystick::isButtonPressed(0, 12) && !toggle();
        };
        buttons[InputCollection::buttonIndicies::capture] = [toggle] {
            return sf::Joystick::isButtonPressed(0, 13) && !toggle();
        };
        return buttons;
    }

    constexpr static auto convertSFML_Axis(float pos) {
        // SFML works on a [-100, 100] scale
        // this scales it to [0, 255] scale
        float SFML_RATIO = 255.0f / 200.0f;
        float SCALE_FACTOR = 1.4f;
        float SCALE = SCALE_FACTOR * SFML_RATIO;
        int OFFSET = 128;

        int scaled = static_cast<int>(pos * SCALE) + OFFSET;
        return static_cast<uint8_t>(std::clamp(scaled, 0, 255));
    }

    static auto getSticks() {
        std::array<std::function<std::array<uint8_t, 2>()>, 3> sticks;
        sticks[InputCollection::stickIndicies::left] = [] {
            return std::array<uint8_t, 2>{
                convertSFML_Axis(
                    sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::X)),
                convertSFML_Axis(
                    sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::Y))};
        };
        sticks[InputCollection::stickIndicies::right] = [] {
            return std::array<uint8_t, 2>{
                convertSFML_Axis(
                    sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::U)),
                convertSFML_Axis(
                    sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::V))};
        };
        sticks[InputCollection::stickIndicies::hat] = [] {
            return std::array<uint8_t, 2>{
                convertSFML_Axis(
                    sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::PovX)),
                convertSFML_Axis(sf::Joystick::getAxisPosition(
                    0, sf::Joystick::Axis::PovY))};
        };
        return sticks;
    }

    InputCollection(const boost::program_options::variables_map &vm)
        : buttons(getButtons()), sticks(getSticks()){};

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
