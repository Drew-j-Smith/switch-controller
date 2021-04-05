#ifndef SF_KEYBOARD_INPUT_EVENT_H
#define SF_KEYBOARD_INPUT_EVENT_H

#include "pch.h"

#include <SFML/Window/Keyboard.hpp>

#include "InputEvent.h"

class SfKeyboardInputEvent : public InputEvent
{
private:
    sf::Keyboard::Key key = sf::Keyboard::Unknown;
public:
    SfKeyboardInputEvent() {};
    SfKeyboardInputEvent(sf::Keyboard::Key key) { this->key = key; };

    int getInputValue() const override { return sf::Keyboard::isKeyPressed(key); };
    bool isDigital() const override { return true; }
};

#endif