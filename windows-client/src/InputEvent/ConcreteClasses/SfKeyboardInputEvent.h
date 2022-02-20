#ifndef SF_KEYBOARD_INPUT_EVENT_H
#define SF_KEYBOARD_INPUT_EVENT_H

#include "pch.h"

#include <SFML/Window/Keyboard.hpp>

#include "InputEvent/InputEvent.h"

class SfKeyboardInputEvent : public InputEvent {
private:
    sf::Keyboard::Key key = sf::Keyboard::Unknown;

public:
    SfKeyboardInputEvent(){};
    SfKeyboardInputEvent(const boost::property_tree::ptree &tree,
                         [[maybe_unused]] const InputEventFactory &factory) {

        std::string keycode = tree.get<std::string>("key");
        if (isalpha(keycode.at(0))) {
            key = (sf::Keyboard::Key)(tolower(keycode.at(0)) - 'a');
            return;
        }
        key = (sf::Keyboard::Key)stoi(keycode);
    }

    int getInputValue() const override {
        return sf::Keyboard::isKeyPressed(key);
    }
    bool isDigital() const override { return true; }

    void update() override {}
};

#endif