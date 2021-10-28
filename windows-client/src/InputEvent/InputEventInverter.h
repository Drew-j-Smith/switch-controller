#ifndef INPUT_EVENT_IVERTER_H
#define INPUT_EVENT_IVERTER_H

/**
 * @file InputEventInverter.h
 * @author Drew Smith
 * @brief The InputInverter class inverts any input recieved from a InputEvent class.
 * @date 2021-04-21
 * 
 */

#include "pch.h"

#include "InputEvent.h"
#include "DefaultInputEvent.h"

/**
 * @brief The InputInverter class inverts any input recieved from a InputEvent class.
 * It contains one event.
 * 
 */
class InputEventInverter : public InputEvent
{
private:
    std::shared_ptr<InputEvent> event;
public:
    InputEventInverter() : event(std::make_shared<DefaultInputEvent>()) {};
    /**
     * @brief Construct a new InputEventInverter object.
     * 
     * @param event The event to be negated
     */
    InputEventInverter(std::shared_ptr<InputEvent> event) {
        if (!event->isDigital()) {
            std::cerr << "Invalid type for InputEventInverter: " << typeid(event).name() << std::endl;
            throw std::invalid_argument("Invalid type for InputEventInverter: " + std::string(typeid(event).name()));
        }
        this->event = event;
    };

    int getInputValue() const override {
        if (isDigital()) {
            return !event->getInputValue(); 
        } else {
            return 255 - event->getInputValue();
        }
    };
    bool isDigital() const override { return event->isDigital(); }

    std::shared_ptr<InputEvent> makeShared(const boost::property_tree::ptree & tree, const std::map<std::string, std::shared_ptr<InputEvent>> & eventMap) const override { 
        boost::property_tree::ptree childTree = tree.get_child("event");
        std::shared_ptr<InputEvent> childEvent = eventMap.at(childTree.get<std::string>("type"))->makeShared(childTree, eventMap);
        return std::make_shared<InputEventInverter>(childEvent); 
    }

    void update() override { event->update(); }

    std::string toString() const override { return "InputEventInverter: (" + event->toString() + ")"; }
    boost::property_tree::ptree toPtree() const override { 
        boost::property_tree::ptree result;
        result.add_child("type", boost::property_tree::ptree("InputEventInverter"));
        result.add_child("event", event->toPtree());
        return result;
    }
    bool operator==(const InputEvent& other) const override { 
        if (!InputEvent::operator==(other))
            return false;
        const InputEventInverter* otherPtr = reinterpret_cast<const InputEventInverter*>(&other);
        return *event == *otherPtr->event;
    }
    bool operator!=(const InputEvent& other) const override { 
        return !operator==(other);
    }
};


#endif
