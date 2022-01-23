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
            
            std::cerr << "Error loading SfKeyboardInputEvent key \"" << keycode << "\".\n";
            std::cerr << "Error: \"" << e.what() << "\"\n";
            key = sf::Keyboard::Unknown;
        }
    }
    SfKeyboardInputEvent(const boost::property_tree::ptree& tree, [[maybe_unused]] const InputEventFactory& factory) {

        std::string keycode = tree.get<std::string>("key");
        if(isalpha(keycode.at(0))){
            key = (sf::Keyboard::Key) (tolower(keycode.at(0)) - 'a');
            return;
        }
        key = (sf::Keyboard::Key) stoi(keycode);
    }

    int getInputValue() const override { return sf::Keyboard::isKeyPressed(key); }
    bool isDigital() const override { return true; }

    std::shared_ptr<InputEvent> makeShared(const boost::property_tree::ptree & tree, [[maybe_unused]] Factory<InputEvent> & factory) const override {
        return std::make_shared<SfKeyboardInputEvent>(tree);
    }
    std::string getTypeName() const override { return ""; }

    void update() override {}

    std::string toString() const override { return ""; }
    boost::property_tree::ptree toPtree() const override {return boost::property_tree::ptree(); }
};

#endif