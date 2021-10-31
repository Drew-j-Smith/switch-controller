#ifndef INPUT_EVENT_GENERATOR_H
#define INPUT_EVENT_GENERATOR_H


#include "pch.h"

#include "InputEvent.h"
#include "Utility/PtreeConstructableConstuctor.h"

#include "DefaultInputEvent.h"
#include "InputEventCollection.h"
#include "InputEventInverter.h"
#include "InputEventToggle.h"
#include "InputEventSwitch.h"
#include "SfJoystickAnalogInputEvent.h"
#include "SfJoystickDigitalInputEvent.h"
#include "SfKeyboardInputEvent.h"

#include <boost/property_tree/ptree.hpp>

class InputEventGenerator : public PtreeConstructableConstructor<InputEvent>
{
public:
    InputEventGenerator() {
        registerClass<DefaultInputEvent>();
        registerClass<ActiveInputEvent>();
        registerClass<InputEventCollection>();
        registerClass<InputEventInverter>();
        registerClass<InputEventToggle>();
        registerClass<InputEventSwitch>();
        registerClass<SfJoystickAnalogInputEvent>();
        registerClass<SfJoystickDigitalInputEvent>();
        registerClass<SfKeyboardInputEvent>();
    }

    std::shared_ptr<InputEvent> generateObject(const boost::property_tree::ptree & tree) {
        std::string type = tree.get<std::string>("type");
        if (map.find(type) != map.end())
            return (map.at(type))->makeShared(tree, map);
        std::cerr << "Input event type: \"" << type << "\" was not found in InputEventGenerator.\n";
        return nullptr;
    }
};



#endif