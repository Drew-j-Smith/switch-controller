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

    template <class T, class... args>
    void addClass(int count, const std::vector<std::string> &names) {
        factories[names[count]] = boost::bind(
            boost::factory<std::shared_ptr<T>>(), _1, boost::ref(*this));
        if constexpr (sizeof...(args) > 0) {
            addClass<args...>(count + 1, names);
        }
    }

public:
    InputEventFactory();

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