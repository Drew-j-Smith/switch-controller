#ifndef EVENT_TOGGLE_H
#define EVENT_TOGGLE_H

#include "pch.h"

#include "ConstantEvent.h"
#include "Event/Event.h"

class EventToggle : public Event {
private:
    int cooldown = 0;
    bool active = false;
    std::shared_ptr<Event> event = std::make_shared<ConstantEvent>();
    std::chrono::steady_clock::time_point lastActivation =
        std::chrono::steady_clock::now();

public:
    EventToggle() {}
    EventToggle(const int cooldown, const std::shared_ptr<Event> event) {
        this->cooldown = cooldown;
        this->event = event;
    }

    void update() override {
        if (event->getInputValue() &&
            std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now() - lastActivation)
                    .count() > cooldown) {
            active = !active;
            lastActivation = std::chrono::steady_clock::now();
        }
    }

    int getInputValue() const override { return active; };

    bool isDigital() const override { return true; }
};

#endif