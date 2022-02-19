#ifndef INPUT_EVENT_PROMPT_H
#define INPUT_EVENT_PROMPT_H

#include "pch.h"

#include "InputEvent/InputEvent.h"

#include <boost/property_tree/ptree.hpp>

boost::property_tree::ptree promptForInputEvent(
    const std::string name, const std::vector<InputEvent::SchemaItem> &schema,
    const std::map<std::string, std::vector<InputEvent::SchemaItem>>
        &schemaList);
std::map<std::string, std::vector<InputEvent::SchemaItem>>
createSchema(const boost::property_tree::ptree &templates);
std::string promptForStringType();
int promptForIntegerType();
boost::property_tree::ptree promptForEventType(
    const std::map<std::string, std::vector<InputEvent::SchemaItem>>
        &schemaList);
boost::property_tree::ptree promptForEventArrayType(
    const std::map<std::string, std::vector<InputEvent::SchemaItem>>
        &schemaList);

#endif