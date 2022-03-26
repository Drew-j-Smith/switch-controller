#ifndef INPUT_EVENT_TOGGLE_H
#define INPUT_EVENT_TOGGLE_H

#include "pch.h"

#include "ConstantInputEvent.h"
#include "InputEvent/InputEvent.h"

class InputEventToggle : public InputEvent {
private:
    int cooldown = 0;
    bool active = false;
    std::shared_ptr<InputEvent> event = std::make_shared<ConstantInputEvent>();
    std::chrono::steady_clock::time_point lastActivation =
        std::chrono::steady_clock::now();

    static std::set<InputEventToggle *> toggles;

public:
    InputEventToggle() { toggles.insert(this); }
    InputEventToggle(const int cooldown,
                     const std::shared_ptr<InputEvent> event) {
        this->cooldown = cooldown;
        this->event = event;
        toggles.insert(this);
    }

    ~InputEventToggle() { toggles.erase(this); }

    static void updateAll() {
        for (auto event : toggles) {
            event->update();
        }
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