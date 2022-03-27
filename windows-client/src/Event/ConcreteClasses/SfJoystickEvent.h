#ifndef SF_JOYSTICK_DIGITAL_EVENT_H
#define SF_JOYSTICK_DIGITAL_EVENT_H

/**
 * @file SfJoystickEvent.h
 * @brief the SfJoystickEvent is used to capture joystick events from SFML
 * @date 2022-03-27
 *
 */

#include "pch.h"

#include <SFML/Window/Joystick.hpp>

#include "Event/Event.h"

class SfJoystickEvent : public Event {
private:
    bool isStick = false;
    unsigned int joystickIndex = 0;
    union {
        unsigned int button = 0;
        sf::Joystick::Axis axis;
    };
    static constexpr double SCALING = 1.4;

public:
    SfJoystickEvent(){};
    SfJoystickEvent(unsigned int joystickIndex, sf::Joystick::Axis axis)
        : isStick(true), joystickIndex(joystickIndex), axis(axis) {
        assertConnected();
    }
    SfJoystickEvent(unsigned int joystickIndex, unsigned int button)
        : isStick(false), joystickIndex(joystickIndex), button(button) {
        assertConnected();
    }
    void assertConnected() {
        if (!sf::Joystick::isConnected(joystickIndex)) {
            std::cerr << "Joystick " + std::to_string(joystickIndex) +
                             " was requested but is not connected.\n";
        }
    }

    uint8_t getEventValue() const override {
        if (!sf::Joystick::isConnected(joystickIndex) ||
            (!isStick &&
             sf::Joystick::getButtonCount(joystickIndex) < button) ||
            (isStick && !sf::Joystick::hasAxis(joystickIndex, axis)))
            return isStick ? 128 : 0;

        if (isStick) {
            // SFML works on a [-100, 100] scale
            // this scales it to [0, 255] scale
            int scaled = (int)((100 + sf::Joystick::getAxisPosition(
                                          joystickIndex, axis) *
                                          SCALING) /
                               200.0 * 255.0);
            return (uint8_t)std::clamp(scaled, 0, 255);
        } else {
            return sf::Joystick::isButtonPressed(joystickIndex, button);
        }
    }

    bool isDigital() const override { return !isStick; }

    void update() override {}
};

#endif