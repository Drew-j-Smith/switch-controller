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

    virtual uint8_t value() const = 0;

    virtual void update() = 0;
};

#endif