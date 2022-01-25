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

#include "InputEvent.h"

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
        digital = tree.get<bool>("isDigital");
        inputValue = tree.get<int>("inputValue");
    }
    int getInputValue() const override { return inputValue; }
    bool isDigital() const override { return digital; }

    std::shared_ptr<InputEvent>
    makeShared([[maybe_unused]] const boost::property_tree::ptree &tree,
               [[maybe_unused]] Factory<InputEvent> &factory) const override {
        // TODO not changing this method because it will be removed in the
        // future
        return std::make_shared<ConstantInputEvent>();
    }
    std::string getTypeName() const override { return "ConstantInputEvent"; }

    void update() override {}

    std::string toString() const override { return "ConstantInputEvent"; }
    boost::property_tree::ptree toPtree() const override {
        // TODO not changing this method because it will be removed in the
        // future
        boost::property_tree::ptree result;
        result.add_child("type",
                         boost::property_tree::ptree("ConstantInputEvent"));
        return result;
    }

    std::vector<SchemaItem> getSchema() const override {
        return {
            {"isDigital", SchemaItem::Integer,
             "isDigital determines if the input event is digital or analog."},
            {"inputValue", SchemaItem::Integer,
             "inputValue determines what the constant value is."}};
    }
};

#endif