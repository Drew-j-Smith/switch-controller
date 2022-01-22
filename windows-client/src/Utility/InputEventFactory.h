#ifndef INPUT_EVENT_FACTORY_H
#define INPUT_EVENT_FACTORY_H

class InputEvent;
#include "InputEvent/InputEvent.h"

#include <boost/function.hpp>
#include <boost/functional/factory.hpp>
#include <boost/bind.hpp>

class InputEventFactory
{
private:
    std::map<std::string, boost::function<std::shared_ptr<InputEvent>(boost::property_tree::ptree, InputEventFactory)>> factories;
    std::set<std::shared_ptr<InputEvent>> createdEvents;
public:
    InputEventFactory();

    std::shared_ptr<InputEvent> create(std::string name, const boost::property_tree::ptree& tree) {
        // TODO test if events are the same
        auto event = factories[name](tree, *this);
        createdEvents.insert(event);
        return event;
    }
};

#endif