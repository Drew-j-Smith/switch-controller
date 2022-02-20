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
        try {
            std::string keycode = tree.get<std::string>("key");
            if (isalpha(keycode.at(0))) {
                key = (sf::Keyboard::Key)(tolower(keycode.at(0)) - 'a');
                return;
            }
            key = (sf::Keyboard::Key)stoi(keycode);
        } catch (std::exception &e) {
            std::stringstream ss;
            ss << e.what();
            ss << "\nUnable to parse SF keyboard input event from ptree:\n";
            boost::property_tree::write_json(ss, tree);
            ss << boost::stacktrace::stacktrace();
            BOOST_LOG_TRIVIAL(error) << ss.str();
            key = sf::Keyboard::Unknown;
        }
    }

    int getInputValue() const override {
        return sf::Keyboard::isKeyPressed(key);
    }
    bool isDigital() const override { return true; }

    void update() override {}

    std::vector<SchemaItem> getSchema() const override {
        return {{"key", SchemaItem::String,
                 "Its an integer key code used by SFML (see "
                 "https://www.sfml-dev.org/documentation/2.5.1/"
                 "classsf_1_1Keyboard.php) or a letter a-z that will be "
                 "converted to that letter key code"}};
    }
};

#endif