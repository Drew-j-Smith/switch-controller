#ifndef INPUT_EVENT_TOGGLE_H
#define INPUT_EVENT_TOGGLE_H

#include "pch.h"

#include "InputEvent.h"

class InputEventToggle : public InputEvent
{
private:
    int cooldown;
    bool active = false;
    std::shared_ptr<InputEvent> event;
    std::chrono::steady_clock::time_point lastActivation = std::chrono::steady_clock::now();
public:
    InputEventToggle(const int cooldown, const std::shared_ptr<InputEvent> event) {
        this->cooldown = cooldown;
        this->event = event;
    }


    int getInputValue() override {
        if (event->getInputValue() && std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - lastActivation).count() > cooldown) {
            active = !active;
            lastActivation = std::chrono::steady_clock::now();
        }
        return active;
    };

    bool isDigital() const override { return true; }
};


#endif