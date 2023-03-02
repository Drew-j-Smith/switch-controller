#pragma once

#include "pch.h"

#include "GameCaptureConfig.h"

#include <SFML/Window/Joystick.hpp>

#include "Event/ConcreteClasses/EventToggle.h"
#include "Event/InputCollection.h"
#include "Macro/MacroRecorder.h"

// The following layout has only been test for the nintendo switch pro
// controller on windows

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

// capture button
bool toggle() { return sf::Joystick::isButtonPressed(0, 13); };

auto getButtons() {
    std::array<std::function<bool()>, 14> buttons;
    buttons[InputCollection::buttonIndicies::b] = ToggleEvent{
        [] { return sf::Joystick::isButtonPressed(0, 0) && !toggle(); }, 20ms};
    buttons[InputCollection::buttonIndicies::a] = [] {
        return sf::Joystick::isButtonPressed(0, 1) && !toggle();
    };
    buttons[InputCollection::buttonIndicies::y] = [] {
        return sf::Joystick::isButtonPressed(0, 2) && !toggle();
    };
    buttons[InputCollection::buttonIndicies::x] = [] {
        return sf::Joystick::isButtonPressed(0, 3) && !toggle();
    };
    buttons[InputCollection::buttonIndicies::l] = [] {
        return sf::Joystick::isButtonPressed(0, 4) && !toggle();
    };
    buttons[InputCollection::buttonIndicies::r] = [] {
        return sf::Joystick::isButtonPressed(0, 5) && !toggle();
    };
    buttons[InputCollection::buttonIndicies::xl] = [] {
        return sf::Joystick::isButtonPressed(0, 6) && !toggle();
    };
    buttons[InputCollection::buttonIndicies::xr] = [] {
        return sf::Joystick::isButtonPressed(0, 7) && !toggle();
    };
    buttons[InputCollection::buttonIndicies::Select] = [] {
        return sf::Joystick::isButtonPressed(0, 8) && !toggle();
    };
    buttons[InputCollection::buttonIndicies::start] = [] {
        return sf::Joystick::isButtonPressed(0, 9) && !toggle();
    };
    buttons[InputCollection::buttonIndicies::lClick] = [] {
        return sf::Joystick::isButtonPressed(0, 10) && !toggle();
    };
    buttons[InputCollection::buttonIndicies::rClick] = [] {
        return sf::Joystick::isButtonPressed(0, 11) && !toggle();
    };
    buttons[InputCollection::buttonIndicies::home] = [] {
        return sf::Joystick::isButtonPressed(0, 12) && !toggle();
    };
    buttons[InputCollection::buttonIndicies::capture] = [] {
        return sf::Joystick::isButtonPressed(0, 13) && !toggle();
    };
    return buttons;
}

auto getSticks() {
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
            convertSFML_Axis(
                sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::PovY))};
    };
    return sticks;
}

struct InputConfig {
    bool stopMacros() {
        return sf::Joystick::isButtonPressed(0, 3) && toggle();
    }
    GameCaptureConfig gameCaptureConfig;
    InputCollection inputCollection;
    MacroRecorder macroRecorder;
    InputConfig()
        : gameCaptureConfig{}, inputCollection{getButtons(), getSticks()},
          macroRecorder{
              [] { return sf::Joystick::isButtonPressed(0, 1) && toggle(); },
              [] { return sf::Joystick::isButtonPressed(0, 0) && toggle(); }} {}
};
