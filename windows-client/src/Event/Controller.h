#pragma once

#include "pch.h"

#include <boost/program_options.hpp>

#include "Event/ConcreteClasses/ToggleEvent.h"
#include "Event/ConcreteClasses/TurboEvent.h"

using namespace std::literals;

class Controller {
private:
    const std::array<std::function<bool()>, 14> buttons;
    const std::array<std::function<std::array<uint8_t, 2>()>, 3> sticks;

    constexpr static auto buttonMapping = std::array{
        "controls.y"sv,     "controls.b"sv,       "controls.a"sv,
        "controls.x"sv,     "controls.l"sv,       "controls.r"sv,
        "controls.xl"sv,    "controls.xr"sv,      "controls.Select"sv,
        "controls.start"sv, "controls.lClick"sv,  "controls.rClick"sv,
        "controls.home"sv,  "controls.capture"sv,
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
        auto transform = [&](auto button_str) {
            auto button_id = static_cast<unsigned int>(
                vm[std::string{button_str}].as<int>());
            auto event = [modifier, button_id] {
                return sf::Joystick::isButtonPressed(0, button_id) &&
                       !modifier();
            };
            return [turboToggle, event,
                    turboEvent = TurboEvent{event, 20ms}]() mutable {
                return turboToggle() ? turboEvent() : event();
            };
        };
        std::transform(buttonMapping.begin(), buttonMapping.end(),
                       buttons.begin(), transform);
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

    static auto getSticks(const boost::program_options::variables_map &vm) {
        std::array<std::function<std::array<uint8_t, 2>()>, 3> sticks;

        auto left_x_axis =
            static_cast<sf::Joystick::Axis>(vm["controls.leftXAxis"].as<int>());
        auto left_y_axis =
            static_cast<sf::Joystick::Axis>(vm["controls.leftYAxis"].as<int>());
        sticks[Controller::stickIndicies::left] = [=] {
            return std::array<uint8_t, 2>{
                convertSFML_Axis(sf::Joystick::getAxisPosition(0, left_x_axis)),
                convertSFML_Axis(
                    sf::Joystick::getAxisPosition(0, left_y_axis))};
        };

        auto right_x_axis = static_cast<sf::Joystick::Axis>(
            vm["controls.rightXAxis"].as<int>());
        auto right_y_axis = static_cast<sf::Joystick::Axis>(
            vm["controls.rightYAxis"].as<int>());
        sticks[Controller::stickIndicies::right] = [=] {
            return std::array<uint8_t, 2>{
                convertSFML_Axis(
                    sf::Joystick::getAxisPosition(0, right_x_axis)),
                convertSFML_Axis(
                    sf::Joystick::getAxisPosition(0, right_y_axis))};
        };

        auto hat_x_axis =
            static_cast<sf::Joystick::Axis>(vm["controls.hatXAxis"].as<int>());
        auto hat_y_axis =
            static_cast<sf::Joystick::Axis>(vm["controls.hatYAxis"].as<int>());
        sticks[Controller::stickIndicies::hat] = [=] {
            return std::array<uint8_t, 2>{
                convertSFML_Axis(sf::Joystick::getAxisPosition(0, hat_x_axis)),
                convertSFML_Axis(sf::Joystick::getAxisPosition(0, hat_y_axis))};
        };
        return sticks;
    }

public:
    Controller(const boost::program_options::variables_map &vm)
        : buttons(getButtons(vm)), sticks(getSticks(vm)){};

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
