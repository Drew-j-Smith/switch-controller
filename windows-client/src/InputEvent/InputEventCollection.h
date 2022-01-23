#ifndef INPUT_EVENT_COLLECTION_H
#define INPUT_EVENT_COLLECTION_H

#include "pch.h"

#include <boost/property_tree/ptree.hpp>

#include "InputEvent.h"

class InputEventCollection : public InputEvent {
private:
    std::vector<std::shared_ptr<InputEvent>> inputEvents;

    bool loadEventType(const boost::property_tree::ptree::const_iterator &it);
    void
    addEventModifiers(const boost::property_tree::ptree::const_iterator &it);

public:
    InputEventCollection(){};
    InputEventCollection(
        const std::vector<std::shared_ptr<InputEvent>> &inputEvents) {
        this->inputEvents = inputEvents;
    };
    InputEventCollection(const boost::property_tree::ptree &tree);
    InputEventCollection(const boost::property_tree::ptree &tree,
                         InputEventFactory &&factory);

    int getInputValue() const override;
    bool isDigital() const override { return true; }

    std::shared_ptr<InputEvent>
    makeShared(const boost::property_tree::ptree &tree,
               Factory<InputEvent> &factory) const override;
    std::string getTypeName() const override { return ""; }

    virtual void update() override {}

    std::string toString() const override { return ""; }
    boost::property_tree::ptree toPtree() const override {
        return boost::property_tree::ptree();
    }
};

#endif