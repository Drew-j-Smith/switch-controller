#ifndef EVENT_H
#define EVENT_H

/**
 * @file Event.h
 * @author Drew Smith
 * @brief The Event class is used to abstract various digital
 * and analog input methods.
 * @date 2021-04-21
 *
 */

#include "pch.h"

/**
 * @brief The Event class is used to abstract various digital
 * and analog input methods.
 *
 */
class Event {
public:
    virtual ~Event() {}

    /**
     * @brief Gets the input value from the object.
     * if the input is digital the result will be [0, 1]
     * if the input is analog the result will be [0, 255]
     *
     * @return int
     */
    virtual int getInputValue() const = 0;

    /**
     * @brief The value decides if the input acts as a digital or analog input
     *
     * @return true getInputValue will return [0, 1]
     * @return false getInputValue will return [0, 255]
     */
    virtual bool isDigital() const = 0;

    virtual void update() = 0;
};

#endif