#ifndef SF_KEYBOARD_INPUT_EVENT_H
#define SF_KEYBOARD_INPUT_EVENT_H

#include "pch.h"

#include <SFML/Window/Keyboard.hpp>

#include <boost/property_tree/ptree.hpp>

#include "InputEvent.h"

class SfKeyboardInputEvent : public InputEvent
{
private:
    sf::Keyboard::Key key = sf::Keyboard::Unknown;
public:
    SfKeyboardInputEvent() {};
    SfKeyboardInputEvent(sf::Keyboard::Key key) { this->key = key; };
    SfKeyboardInputEvent(const boost::property_tree::ptree & tree) {
        std::string keycode = tree.get("key", "-1");
        if(isalpha(keycode.at(0))){
            key = (sf::Keyboard::Key) (tolower(keycode.at(0)) - 'a');
            return;
        }

        try{
            key = (sf::Keyboard::Key) stoi(keycode);
        }
        catch(const std::exception& e){
            std::cerr << e.what() << '\n';
            std::cerr << "Error loading key \"" << keycode << "\"\n";
            sf::Keyboard::Key key = sf::Keyboard::Unknown;
        }
    }

    int getInputValue() const override { return sf::Keyboard::isKeyPressed(key); }
    bool isDigital() const override { return true; }
};

#endif