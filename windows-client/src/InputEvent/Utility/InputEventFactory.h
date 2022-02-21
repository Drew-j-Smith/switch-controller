#ifndef INPUT_EVENT_FACTORY_H
#define INPUT_EVENT_FACTORY_H

class InputEvent;
#include "InputEvent/InputEvent.h"

#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/functional/factory.hpp>

class InputEventFactory {
private:
    std::map<std::string, boost::function<std::shared_ptr<InputEvent>(
                              boost::property_tree::ptree)>>
        factories;
    std::set<std::shared_ptr<InputEvent>> createdEvents;

public:
    InputEventFactory(const std::map<std::string, boost::property_tree::ptree>
                          &inputEventTemplates);

    std::shared_ptr<InputEvent>
    create(const boost::property_tree::ptree &tree) {
        // TODO test if events are the same
        std::string name = tree.get<std::string>("type");
        auto event = factories[name](tree);
        createdEvents.insert(event);
        return event;
    }
};

#endif