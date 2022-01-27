#include "InputEventPrompt.h"

#include "InputEvent/ConstantInputEvent.h"
#include "InputEvent/InputEventCollection.h"
#include "InputEvent/InputEventInverter.h"
#include "InputEvent/InputEventSwitch.h"
#include "InputEvent/InputEventToggle.h"
#include "InputEvent/InputEventTurbo.h"
#include "InputEvent/SfJoystickAnalogInputEvent.h"
#include "InputEvent/SfJoystickDigitalInputEvent.h"
#include "InputEvent/SfKeyboardInputEvent.h"

std::map<std::string, std::vector<InputEvent::SchemaItem>> createSchema() {
    std::map<std::string, std::vector<InputEvent::SchemaItem>> schemaList;
#define AC_ADD_SCHEMA(class) schemaList[#class] = class().getSchema();
    AC_ADD_SCHEMA(ConstantInputEvent);
    AC_ADD_SCHEMA(InputEventCollection);
    AC_ADD_SCHEMA(InputEventInverter);
    AC_ADD_SCHEMA(InputEventSwitch);
    AC_ADD_SCHEMA(InputEventToggle);
    AC_ADD_SCHEMA(InputEventTurbo);
    AC_ADD_SCHEMA(SfJoystickAnalogInputEvent);
    AC_ADD_SCHEMA(SfJoystickDigitalInputEvent);
    AC_ADD_SCHEMA(SfKeyboardInputEvent);
    return schemaList;
}

std::string promptForStringType() {
    std::string strline;
    std::cout << "Enter a string:";
    std::getline(std::cin, strline);
    return strline;
}

int promptForIntegerType() {
    std::string strline;
    int intValue;
    std::cout << "Enter a Integer:";
    std::cin >> intValue;
    std::getline(std::cin, strline);
    return intValue;
}

boost::property_tree::ptree promptForEventType(
    const std::map<std::string, std::vector<InputEvent::SchemaItem>>
        &schemaList) {

    std::string strline;
    auto it = schemaList.end();
    while (it == schemaList.end()) {
        std::cout << "Here are the list of availible input events:\n";
        for (auto event : schemaList) {
            std::cout << "  " << event.first << '\n';
        }
        std::cout << "Enter an InputEvent:";
        std::getline(std::cin, strline);
        it = schemaList.find(strline);
    }
    boost::property_tree::ptree ptreeEvent =
        promptForInputEvent(it->first, it->second, schemaList);
    ptreeEvent.add("type", it->first);
    return ptreeEvent;
}

boost::property_tree::ptree promptForEventArrayType(
    const std::map<std::string, std::vector<InputEvent::SchemaItem>>
        &schemaList) {
    std::string strline;
    boost::property_tree::ptree ptreeEventArray;
    while (strline == "y") {
        std::cout << "Add another InputEvent? (y/n):";
        ptreeEventArray.add_child("", promptForEventType(schemaList));
        std::getline(std::cin, strline);
    }
    return ptreeEventArray;
}

boost::property_tree::ptree promptForInputEvent(
    const std::string name, const std::vector<InputEvent::SchemaItem> &schema,
    const std::map<std::string, std::vector<InputEvent::SchemaItem>>
        &schemaList) {

    boost::property_tree::ptree res;

    std::cout << "Creating config for " << name << '\n';

    for (auto &schemaItem : schema) {

        std::cout << "Name:" << schemaItem.name << '\n';
        std::cout << "Description:" << schemaItem.description << '\n';

        switch (schemaItem.type) {

        case InputEvent::SchemaItem::SchemaType::String: {
            res.add(schemaItem.name, promptForStringType());
            break;
        }

        case InputEvent::SchemaItem::SchemaType::Integer: {
            res.add(schemaItem.name, promptForIntegerType());
            break;
        }

        case InputEvent::SchemaItem::SchemaType::Event: {
            res.add_child(schemaItem.name, promptForEventType(schemaList));
            break;
        }

        case InputEvent::SchemaItem::SchemaType::EventArray: {
            res.add_child(schemaItem.name, promptForEventArrayType(schemaList));
            break;
        }

        default:
            break;
        }
    }
    return res;
}