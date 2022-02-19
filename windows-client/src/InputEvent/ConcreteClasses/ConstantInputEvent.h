#ifndef CONSTANT_INPUT_EVENT_H
#define CONSTANT_INPUT_EVENT_H

/**
 * @file ConstantInputEvent.h
 * @author Drew Smith
 * @brief The ConstantInputEvent is a basic implementation of InputEvent
 * which returns a constant value.
 * @date 2021-10-17
 *
 */

#include "pch.h"

#include "InputEvent/InputEvent.h"

#include <boost/log/trivial.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/stacktrace.hpp>

/**
 * @brief The ConstantInputEvent is a basic implementation of InputEvent
 * which returns a constant value.
 *
 */
class ConstantInputEvent : public InputEvent {
private:
    int inputValue = 0;
    bool digital = true;

public:
    ConstantInputEvent(){};
    ConstantInputEvent(int inputValue, bool isDigital)
        : inputValue(inputValue), digital(isDigital){};
    ConstantInputEvent(const boost::property_tree::ptree &tree,
                       [[maybe_unused]] const InputEventFactory &factory) {
        try {
            digital = tree.get<bool>("isDigital");
            inputValue = tree.get<int>("inputValue");
        } catch (std::exception &e) {
            std::stringstream ss;
            ss << e.what();
            ss << "\nUnable to parse constant input event from ptree:\n";
            boost::property_tree::write_json(ss, tree);
            ss << boost::stacktrace::stacktrace();
            BOOST_LOG_TRIVIAL(error) << ss.str();
            inputValue = 0;
            digital = true;
        }
    }
    int getInputValue() const override { return inputValue; }
    bool isDigital() const override { return digital; }

    void update() override {}

    std::vector<SchemaItem> getSchema() const override {
        return {
            {"isDigital", SchemaItem::Integer,
             "isDigital determines if the input event is digital or analog."},
            {"inputValue", SchemaItem::Integer,
             "inputValue determines what the constant value is."}};
    }
};

#endif