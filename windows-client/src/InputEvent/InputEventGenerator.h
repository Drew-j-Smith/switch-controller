#ifndef INPUT_EVENT_GENERATOR_H
#define INPUT_EVENT_GENERATOR_H


#include "pch.h"

#include "InputEvent.h"

#include <boost/property_tree/ptree.hpp>

class InputEventGenerator
{
private:
    std::map<std::string, std::shared_ptr<InputEvent>> eventMap;
public:
    InputEventGenerator() {}

    template<class T>
    void registerInputEvent() {
        std::shared_ptr<InputEvent> event = std::make_shared<T>();
        eventMap.insert({std::string(typeid(*event).name()), event});
    }

    std::shared_ptr<InputEvent> generateInputEvent(const boost::property_tree::ptree & tree) {
        std::string type = tree.get("type", "TYPE_PROPORTY_NOT_FOUND");
        if (eventMap.find(type) != eventMap.end())
            return (eventMap.at(type))->makeShared(tree, eventMap);
        std::cerr << "Input event type: \"" << type << "\" was not found in InputEventGenerator.\n";
        return nullptr;
    }

    bool operator==(const InputEventGenerator& other) {
        if (eventMap.size() != other.eventMap.size()) {
            return false;
        }
        
        for (auto it = eventMap.begin(); it != eventMap.end(); ++it) {
            if (other.eventMap.find(it->first) == other.eventMap.end()) {
                return false;
            }
        }
        
        return true;
    }
};



#endif