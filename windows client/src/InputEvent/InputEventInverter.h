#ifndef INPUT_EVENT_IVERTER_H
#define INPUT_EVENT_IVERTER_H

/**
 * @file InputEventInverter.h
 * @author Drew Smith
 * @brief The InputInverter class inverts any input recieved from a InputEvent class.
 * @date 2021-04-21
 * 
 */

#include "pch.h"

#include "InputEvent.h"

/**
 * @brief The InputInverter class inverts any input recieved from a InputEvent class.
 * It contains one event.
 * 
 */
class InputEventInverter : public InputEvent
{
private:
    std::shared_ptr<InputEvent> event;
public:
    /**
     * @brief Construct a new InputEventInverter object.
     * 
     * @param event The event to be negated
     */
    InputEventInverter(std::shared_ptr<InputEvent> event) : event(event) {};

    int getInputValue() const override {
        if (isDigital()) {
            return !event->getInputValue(); 
        } else {
            return 255 - event->getInputValue();
        }
    };
    bool isDigital() const override { return event->isDigital(); }

    using ptree = boost::property_tree::ptree;
    virtual std::shared_ptr<InputEvent> makeShared(const ptree & tree,
        const std::map<std::string, std::shared_ptr<InputEvent> (*)(const ptree & tree)> & map) const override { return nullptr; } 
    std::string getTypeName() const override { return ""; }

    void update() override {}

    std::string toString() const override { return ""; }
    boost::property_tree::ptree toPtree() const override {return boost::property_tree::ptree(); }
    bool operator==(const InputEvent& other) const override { return false; }
};


#endif
