#ifndef EVENT_H
#define EVENT_H

/**
 * @file Event.h
 * @author Drew Smith
 * @brief The Event class is used to abstract various digital
 * and analog events.
 * @date 2021-04-21
 *
 */

#include "pch.h"

class Event {
public:
    virtual ~Event() {}

    /**
     * @brief
     * if digital the result will be [0, 1]
     * if analog the result will be [0, 255]
     */
    virtual uint8_t getEventValue() const = 0;

    /**
     * @brief true if digital/false is analog
     */
    virtual bool isDigital() const = 0;

    virtual void update() = 0;
};

#endif