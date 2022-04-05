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
    enum Type {
        Analog,  // value() is an element of [0, 255]
        Digital, // value() is an element of [0, 1]
    };

    virtual ~Event() {}

protected:
    Type m_type;
    Event(Type type) : m_type(type) {}

public:
    virtual uint8_t value() const = 0;

    Type type() const { return m_type; }

    virtual void update() = 0;
};

#endif