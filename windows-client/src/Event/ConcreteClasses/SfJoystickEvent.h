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
    enum Type { Button, Stick };
    Type type;
    unsigned int joystickIndex;
    union {
        unsigned int button;
        sf::Joystick::Axis axis;
    };
    static constexpr double SCALING = 1.4;

public:
    SfJoystickEvent(unsigned int joystickIndex = 0,
                    sf::Joystick::Axis axis = sf::Joystick::Axis::X)
        : type(Stick), joystickIndex(joystickIndex), axis(axis) {
        assertConnected();
    }
    SfJoystickEvent(unsigned int joystickIndex, unsigned int button = 0)
        : type(Button), joystickIndex(joystickIndex), button(button) {
        assertConnected();
    }
    void assertConnected() {
        if (!sf::Joystick::isConnected(joystickIndex)) {
            std::cerr << "Joystick " + std::to_string(joystickIndex) +
                             " was requested but is not connected.\n";
        }
    }

    uint8_t value() const override {
        uint8_t res = type == Stick ? 128 : 0;

        if (!sf::Joystick::isConnected(joystickIndex))
            return res;

        switch (type) {
        case Stick: {
            if (sf::Joystick::hasAxis(joystickIndex, axis))
                break;
            // SFML works on a [-100, 100] scale
            // this scales it to [0, 255] scale
            int scaled = (int)((100 + sf::Joystick::getAxisPosition(
                                          joystickIndex, axis) *
                                          SCALING) /
                               200.0 * 255.0);

            res = (uint8_t)std::clamp(scaled, 0, 255);
            break;
        }
        case Button: {
            if (sf::Joystick::getButtonCount(joystickIndex) < button)
                break;

            res = sf::Joystick::isButtonPressed(joystickIndex, button);
            break;
        }
        }
        return res;
    }

    void update() override {}
};

#endif