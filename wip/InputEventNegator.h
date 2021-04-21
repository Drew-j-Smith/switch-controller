#ifndef INPUT_EVENT_NEGATOR_H
#define INPUT_EVENT_NEGATOR_H

/**
 * @file InputEventNegator.h
 * @author Drew Smith
 * @brief The InputNegator class inverts any input recieved from a InputEvent class.
 * @date 2021-04-21
 * 
 */

#include "pch.h"

#include "InputEvent.h"

/**
 * @brief The InputNegator class inverts any input recieved from a InputEvent class.
 * It contains one event.
 * 
 */
class InputEventNegator : public InputEvent
{
private:
    std::shared_ptr<InputEvent> event;
public:
    /**
     * @brief Construct a new InputEventNegator object.
     * 
     * @param event The event to be negated
     */
    InputEventNegator(std::shared_ptr<InputEvent> event) : event(event) {};

    int getInputValue() const override {
        if (isDigital()) {
            return !event->getInputValue(); 
        } else {
            return 255 - event->getInputValue();
        }
    };
    bool isDigital() const override { return event->isDigital(); }
};


#endif
