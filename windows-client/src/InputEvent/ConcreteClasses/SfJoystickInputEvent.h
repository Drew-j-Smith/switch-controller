#ifndef SF_JOYSTICK_DIGITAL_INPUT_EVENT_H
#define SF_JOYSTICK_DIGITAL_INPUT_EVENT_H

#include "pch.h"

#include <SFML/Window/Joystick.hpp>

#include <boost/log/trivial.hpp>
#include <boost/stacktrace/stacktrace.hpp>

#include "InputEvent/InputEvent.h"

class SfJoystickInputEvent : public InputEvent {
private:
    bool isStick = false;
    unsigned int joystickIndex = 0;
    union {
        unsigned int button = 0;
        sf::Joystick::Axis axis;
    };
    static constexpr double SCALING = 1.4;

public:
    SfJoystickInputEvent(){};
    SfJoystickInputEvent(const boost::property_tree::ptree &tree,
                         [[maybe_unused]] const InputEventFactory &factory) {
        try {
            isStick = tree.get<int>("isStick");
            joystickIndex = tree.get<int>("joystick index");
            if (isStick) {
                axis = (sf::Joystick::Axis)tree.get<int>("axis/button");
            } else {
                button = tree.get<int>("axis/button");
            }
        } catch (std::exception &e) {
            std::stringstream ss;
            ss << e.what();
            ss << "\nUnable to parse SF joystick input event from ptree:\n";
            boost::property_tree::write_json(ss, tree);
            ss << boost::stacktrace::stacktrace();
            BOOST_LOG_TRIVIAL(error) << ss.str();
            isStick = false;
            joystickIndex = 0;
            button = 0;
        }
        assertConnected();
    }
    void assertConnected() {
        if (!sf::Joystick::isConnected(joystickIndex)) {
            BOOST_LOG_TRIVIAL(warning)
                << "Joystick " + std::to_string(joystickIndex) +
                       " was requested but is not connected.\n" +
                       boost::stacktrace::to_string(
                           boost::stacktrace::stacktrace());
        }
    }

    int getInputValue() const override {
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
            return std::clamp(scaled, 0, 255);
        } else {
            return sf::Joystick::isButtonPressed(joystickIndex, button);
        }
    }

    bool isDigital() const override { return !isStick; }

    void update() override {}

    std::vector<SchemaItem> getSchema() const override {
        return {
            {"isStick", SchemaItem::Integer,
             "isDigital determines if the input event is a stick or a button."},
            {"axis/button", SchemaItem::Integer,
             "axis/button is the axis or button used, depending on the value "
             "of isStick"},
            {"joystick index", SchemaItem::Integer,
             "joystick index is the index of the joystick"}};
    }
};

#endif