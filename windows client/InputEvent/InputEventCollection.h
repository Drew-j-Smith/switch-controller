#ifndef INPUT_EVENT_COLLECTION_H
#define INPUT_EVENT_COLLECTION_H

#include "../pch.h"

#include <boost/property_tree/ptree.hpp>

#include "InputEvent.h"

class InputEventCollection : public InputEvent
{
private:
    std::vector<std::shared_ptr<InputEvent>> inputEvents;
public:
    InputEventCollection() {};
    InputEventCollection(const std::vector<std::shared_ptr<InputEvent>> & inputEvents) { this->inputEvents = inputEvents; };
    InputEventCollection(const boost::property_tree::ptree & tree);

    int getInputValue() const override;
    bool isDigital() const override { return true; }
};

#endif