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

/**
 * @brief The InputEvent class is used to abstract various digital
 * and analogue input methods.
 * 
 */
class InputEvent
{
public:
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

    // static std::shared_ptr<InputEvent> makeShared(const boost::property_tree::ptree & tree) {
    //     throw "Unsuported Operation: Cannot intialize abstact class InputEvent";
    //     return nullptr;
    // }
};


#endif