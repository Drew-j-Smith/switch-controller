#ifndef SF_KEYBOARD_EVENT_H
#define SF_KEYBOARD_EVENT_H

/**
 * @file SfKeyboardEvent.h
 * @brief The SfKeyboardEvent class is used to capture keyboard events from SFML
 * @date 2022-03-27
 *
 */

#include "pch.h"

#include <SFML/Window/Keyboard.hpp>

#include "Event/Event.h"

class SfKeyboardEvent : public Event {
private:
    sf::Keyboard::Key key;

public:
    SfKeyboardEvent(sf::Keyboard::Key key = sf::Keyboard::Unknown) : key(key){};

    uint8_t value() const override { return sf::Keyboard::isKeyPressed(key); }

    void update() override {}
};

#endif