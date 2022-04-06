#ifndef EVENT_TOGGLE_H
#define EVENT_TOGGLE_H

/**
 * @file EventToggle.h
 * @brief The Event Toggle class will take an event, and while it is active,
 * toggle between 0 and 1 with an specified interval
 * @date 2022-03-27
 *
 */

#include "pch.h"

#include "ConstantEvent.h"
#include "Event/Event.h"

class EventToggle : public Event {
private:
    int cooldown;
    mutable bool active = false;
    std::shared_ptr<Event> event;
    mutable std::chrono::steady_clock::time_point lastActivation =
        std::chrono::steady_clock::now();

public:
    EventToggle(const int cooldown = 0, const std::shared_ptr<Event> event =
                                            std::make_shared<ConstantEvent>())
        : cooldown(cooldown), event(event) {}

    uint8_t value() const override {
        // TODO? acepting that this can change almost at random
        if (event->value() &&
            std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now() - lastActivation)
                    .count() > cooldown) {
            active = !active;
            lastActivation = std::chrono::steady_clock::now();
        }
        return active;
    };
};

#endif