#pragma once

#include "pch.h"

#include <boost/program_options.hpp>

#include "Event/ConcreteClasses/EventToggle.h"

using namespace std::literals;

class Controller {
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

    constexpr static auto buttonMapping = std::array{
        std::pair{"controls.y"sv, y},
        std::pair{"controls.b"sv, b},
        std::pair{"controls.a"sv, a},
        std::pair{"controls.x"sv, x},
        std::pair{"controls.l"sv, l},
        std::pair{"controls.r"sv, r},
        std::pair{"controls.xl"sv, xl},
        std::pair{"controls.xr"sv, xr},
        std::pair{"controls.Select"sv, Select},
        std::pair{"controls.start"sv, start},
        std::pair{"controls.lClick"sv, lClick},
        std::pair{"controls.rClick"sv, rClick},
        std::pair{"controls.home"sv, home},
        std::pair{"controls.capture"sv, capture},
    };

    enum stickIndicies { left, right, hat };

    constexpr static int JOYSTICK_DEADZONE = 30;

    static auto getButtons(const boost::program_options::variables_map &vm) {
        auto modifierButton =
            static_cast<unsigned int>(vm["controls.modifier"].as<int>());
        auto modifier = [modifierButton] {
            return sf::Joystick::isButtonPressed(0, modifierButton);
        };
        auto turboToggle = ToggleEvent{
            [modifier, turboButton = static_cast<unsigned int>(
                           vm["controls.turbo"].as<int>())] {
                return sf::Joystick::isButtonPressed(0, turboButton) &&
                       modifier();
            },
            1000ms};

        std::array<std::function<bool()>, 14> buttons;
        for (std::size_t i = 0; i < 14; i++) {
            auto event =
                [modifier,
                 button = static_cast<unsigned int>(
                     vm[std::string{buttonMapping[i].first}].as<int>())] {
                    return sf::Joystick::isButtonPressed(0, button) &&
                           !modifier();
                };
            buttons[buttonMapping[i].second] =
                [turboToggle, event,
                 turboEvent = TurboEvent{event, 20ms}]() mutable {
                    return turboToggle() ? turboEvent() : event();
                };
        }
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
        sticks[Controller::stickIndicies::left] = [] {
            return std::array<uint8_t, 2>{
                convertSFML_Axis(
                    sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::X)),
                convertSFML_Axis(
                    sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::Y))};
        };
        sticks[Controller::stickIndicies::right] = [] {
            return std::array<uint8_t, 2>{
                convertSFML_Axis(
                    sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::U)),
                convertSFML_Axis(
                    sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::V))};
        };
        sticks[Controller::stickIndicies::hat] = [] {
            return std::array<uint8_t, 2>{
                convertSFML_Axis(
                    sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::PovX)),
                convertSFML_Axis(sf::Joystick::getAxisPosition(
                    0, sf::Joystick::Axis::PovY))};
        };
        return sticks;
    }

    Controller(const boost::program_options::variables_map &vm)
        : buttons(getButtons(vm)), sticks(getSticks()){};

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
        if (sticks[0] > 128 - Controller::JOYSTICK_DEADZONE &&
            sticks[0] < 128 + Controller::JOYSTICK_DEADZONE &&
            sticks[1] > 128 - Controller::JOYSTICK_DEADZONE &&
            sticks[1] < 128 + Controller::JOYSTICK_DEADZONE) {
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
