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
    unsigned int joystickIndex;
    union {
        unsigned int button;
        sf::Joystick::Axis axis;
    };
    static constexpr double SCALING = 1.4;

public:
    SfJoystickEvent(unsigned int joystickIndex = 0,
                    sf::Joystick::Axis axis = sf::Joystick::Axis::X)
        : Event(Analog), joystickIndex(joystickIndex), axis(axis) {
        assertConnected();
    }
    SfJoystickEvent(unsigned int joystickIndex, unsigned int button = 0)
        : Event(Digital), joystickIndex(joystickIndex), button(button) {
        assertConnected();
    }
    void assertConnected() {
        if (!sf::Joystick::isConnected(joystickIndex)) {
            std::cerr << "Joystick " + std::to_string(joystickIndex) +
                             " was requested but is not connected.\n";
        }
    }

    uint8_t value() const override {
        if (!sf::Joystick::isConnected(joystickIndex) ||
            (m_type == Digital &&
             sf::Joystick::getButtonCount(joystickIndex) < button) ||
            (m_type == Analog && !sf::Joystick::hasAxis(joystickIndex, axis)))
            return m_type == Analog ? 128 : 0;

        if (m_type == Analog) {
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

    void update() override {}
};

#endif