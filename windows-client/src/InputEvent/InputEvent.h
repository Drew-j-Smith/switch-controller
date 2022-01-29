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

#include "Utility/InputEventFactory.h"

/**
 * @brief The InputEvent class is used to abstract various digital
 * and analogue input methods.
 *
 */
class InputEvent {
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
     * @brief A SchemaItem is field expected to be present in the ptree
     * constructor. Additionally there must be a field of "type" which is the
     * name of the class (this will NOT be returned in the schema).
     */
    struct SchemaItem {
        // The name of the SchemaItem as it appears in a ptree.
        std::string name;
        // Potential types of a SchemaItem.
        enum SchemaType { String, Integer, Event, EventArray };
        // Children of a SchemaItem as it appears in a ptree.
        SchemaType type;
        // In depth explanation of the SchemaItem.
        std::string description;
    };

    /**
     * @brief Get the Schema of the input event.
     *
     * @return std::vector<SchemaItem> the Schema of the input event
     */
    virtual std::vector<SchemaItem> getSchema() const {
        return std::vector<SchemaItem>(); // TODO temporary definition
    }
};

#endif