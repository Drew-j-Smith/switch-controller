#ifndef INPUT_EVENT_FACTORY_H
#define INPUT_EVENT_FACTORY_H

class InputEvent;
#include "InputEvent/InputEvent.h"

// Another false positive on MSCV
// TODO
// could break in the future

#ifdef _MSC_VER
#pragma warning(push, 2)
#endif

#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/functional/factory.hpp>

#ifdef _MSC_VER
#pragma warning(pop)
#endif

class InputEventFactory {
  private:
    std::map<std::string, boost::function<std::shared_ptr<InputEvent>(
                              boost::property_tree::ptree, InputEventFactory)>>
        factories;
    std::set<std::shared_ptr<InputEvent>> createdEvents;

  public:
    InputEventFactory();

    std::shared_ptr<InputEvent>
    create(const boost::property_tree::ptree &tree) {
        // TODO test if events are the same
        std::string name = tree.get<std::string>("type");
        auto event = factories[name](tree, boost::ref(*this));
        createdEvents.insert(event);
        return event;
    }
};

#endif