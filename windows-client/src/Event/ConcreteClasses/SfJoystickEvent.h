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
        switch (type) {
        case Stick: {
            double pos = sf::Joystick::getAxisPosition(joystickIndex, axis);
            // SFML works on a [-100, 100] scale
            // this scales it to [0, 255] scale
            const double SFML_RATIO = 255.0 / 200.0;
            const double SCALE_FACTOR = 1.4;
            const double SCALE = SCALE_FACTOR * SFML_RATIO;
            const int OFFSET = 128;

            int scaled = (int)(pos * SCALE + OFFSET);
            return (uint8_t)std::clamp(scaled, 0, 255);
        }
        case Button: {
            return sf::Joystick::isButtonPressed(joystickIndex, button);
        }
        default:
            return 0;
        }
    }

    void update() override {}
};

#endif