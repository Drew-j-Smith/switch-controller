#ifndef INPUT_EVENT_H
#define INPUT_EVENT_H

/**
 * @file InputEvent.h
 * @author Drew Smith
 * @brief The InputEvent class is used to abstract various digital
 * and analogue input methods.
 * @date 2021-04-21
 *
 */

#include "pch.h"

#include <boost/property_tree/ptree.hpp>

#include "Utility/FactoryObject.h"
#include "Utility/InputEventFactory.h"

/**
 * @brief The InputEvent class is used to abstract various digital
 * and analogue input methods.
 *
 */
class InputEvent : public FactoryObject<InputEvent> {
public:
    virtual ~InputEvent() {}

    /**
     * @brief Gets the input value from the object.
     * if the input is digital the result will be [0, 1]
     * if the input is analog the result will be [0, 255]
     *
     * @return int
     */
    virtual int getInputValue() const = 0;

    /**
     * @brief The value decides if the input acts as a digital or analogue input
     *
     * @return true getInputValue will return [0, 1]
     * @return false getInputValue will return [0, 255]
     */
    virtual bool isDigital() const = 0;

    virtual void update() = 0;

    /**
     * @brief Get the Schema of the input event
     *
     * @details The schema will have the same shape that the input event will
     * expect in its constructor. The names of the nodes are expected to be the
     * same while the child of the nodes represent the data expected.
     * There are 4 types of data (as of now):
     * 1. String
     * 2. Integer
     * 3. Array (the array will have a child with the expected type)
     * 4. InputEvent (the InputEvent must be a valid ptree to construct another
     * InputEvent)
     *
     * @return boost::property_tree::ptree the Schema of the input event
     */
    virtual boost::property_tree::ptree getSchema() {
        return boost::property_tree::ptree(); // TODO temporary definition
    }
};

#endif