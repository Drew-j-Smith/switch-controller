#ifndef INPUT_EVENT_TOGGLE_H
#define INPUT_EVENT_TOGGLE_H

#include "pch.h"

#include "InputEvent.h"
#include "DefaultInputEvent.h"

class InputEventToggle : public InputEvent
{
private:
    int cooldown;
    bool active = false;
    std::shared_ptr<InputEvent> event;
    std::chrono::steady_clock::time_point lastActivation = std::chrono::steady_clock::now();

    static std::set<InputEventToggle*> toggles;
public:
    InputEventToggle() {
        this->cooldown = 0;
        this->event = std::make_shared<DefaultInputEvent>();
        toggles.insert(this);
    }
    InputEventToggle(const int cooldown, const std::shared_ptr<InputEvent> event) {
        this->cooldown = cooldown;
        this->event = event;
        toggles.insert(this);
    }

    ~InputEventToggle() {
        toggles.erase(this);
    }

    static void updateAll() {
        for (auto event : toggles) {
            event->update();
        }
    }

    void update() override {
        if (event->getInputValue() && std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - lastActivation).count() > cooldown) {
            active = !active;
            lastActivation = std::chrono::steady_clock::now();
        }
    }

    void setActive(const bool active) {
        this->active = active;
    }

    int getInputValue() const override {
        return active;
    };

    bool isDigital() const override { return true; }

    std::shared_ptr<InputEvent> makeShared(const boost::property_tree::ptree & tree, const std::map<std::string, std::shared_ptr<InputEvent>> & eventMap) const override { return nullptr; }
    std::string getTypeName() const override { return ""; }

    std::string toString() const override { return ""; }
    boost::property_tree::ptree toPtree() const override {return boost::property_tree::ptree(); }
    bool operator==(const InputEvent& other) const override { return false; }
};


#endif