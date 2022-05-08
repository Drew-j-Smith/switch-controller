#ifndef EVENT_TOGGLE_H
#define EVENT_TOGGLE_H

/**
 * @brief The Event Toggle class will take an event, and while it is active,
 * toggle between 0 and 1 with an specified interval
 */

#include "pch.h"

#include "ConstantEvent.h"
#include "Event/Event.h"

class EventToggle : public Event {
private:
    int cooldown;
    std::shared_ptr<Event> event;

public:
    EventToggle(const int cooldown = 0, const std::shared_ptr<Event> event =
                                            std::make_shared<ConstantEvent>())
        : cooldown(cooldown), event(event) {}

    uint8_t value() const override {
        return event->value() &&
               std::chrono::duration_cast<std::chrono::milliseconds>(
                   std::chrono::steady_clock::now().time_since_epoch())
                       .count() /
                   cooldown % 2;
    };
};

#endif