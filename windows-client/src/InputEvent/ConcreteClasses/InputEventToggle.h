#ifndef INPUT_EVENT_TOGGLE_H
#define INPUT_EVENT_TOGGLE_H

#include "pch.h"

#include "ConstantInputEvent.h"
#include "InputEvent/InputEvent.h"

class InputEventToggle : public InputEvent {
private:
    int cooldown;
    bool active = false;
    std::shared_ptr<InputEvent> event;
    std::chrono::steady_clock::time_point lastActivation =
        std::chrono::steady_clock::now();

    static std::set<InputEventToggle *> toggles;

public:
    InputEventToggle() {
        this->cooldown = 0;
        this->event = std::make_shared<ConstantInputEvent>();
        toggles.insert(this);
    }
    InputEventToggle(const int cooldown,
                     const std::shared_ptr<InputEvent> event) {
        this->cooldown = cooldown;
        this->event = event;
        toggles.insert(this);
    }
    InputEventToggle(const boost::property_tree::ptree &tree,
                     InputEventFactory &factory) {
        cooldown = tree.get<int>("button cooldown");
        boost::property_tree::ptree childTree = tree.get_child("event");
        event = factory.create(childTree);
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

    void setActive(const bool newActive) { this->active = newActive; }

    int getInputValue() const override { return active; };

    bool isDigital() const override { return true; }
};

#endif