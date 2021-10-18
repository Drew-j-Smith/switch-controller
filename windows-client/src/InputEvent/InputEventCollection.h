#ifndef INPUT_EVENT_COLLECTION_H
#define INPUT_EVENT_COLLECTION_H

#include "pch.h"

#include <boost/property_tree/ptree.hpp>

#include "InputEvent.h"

class InputEventCollection : public InputEvent
{
private:
    std::vector<std::shared_ptr<InputEvent>> inputEvents;

    bool loadEventType(const boost::property_tree::ptree::const_iterator & it);
    void addEventModifiers(const boost::property_tree::ptree::const_iterator & it);
public:
    InputEventCollection() {};
    InputEventCollection(const std::vector<std::shared_ptr<InputEvent>> & inputEvents) { this->inputEvents = inputEvents; };
    InputEventCollection(const boost::property_tree::ptree & tree);

    int getInputValue() const override;
    bool isDigital() const override { return true; }

    std::shared_ptr<InputEvent> makeShared(const boost::property_tree::ptree & tree, const std::map<std::string, std::shared_ptr<InputEvent>> & eventMap) const override { return nullptr; }
    std::string getTypeName() const override { return ""; }

    virtual void update() {}

    std::string toString() const override { return ""; }
    boost::property_tree::ptree toPtree() const override {return boost::property_tree::ptree(); }
    bool operator==(const InputEvent& other) const override { return false; }
};

#endif