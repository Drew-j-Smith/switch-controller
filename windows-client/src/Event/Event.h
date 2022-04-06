#ifndef EVENT_H
#define EVENT_H

/**
 * @brief The Event class is used to abstract various digital
 * and analog events.
 */

#include "pch.h"

class Event {
public:
    virtual ~Event() {}

    virtual uint8_t value() const = 0;
};

#endif