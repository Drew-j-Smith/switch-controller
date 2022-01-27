
#include "pch.h"

#include "Utility/InputEventPrompt.h"

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

void CreateConfig() {
    boost::property_tree::ptree testEvent = promptForInputEvent(
        "test input event",
        {{"test", InputEvent::SchemaItem::Event, "test input event"}},
        createSchema());
    boost::property_tree::write_json(std::cout, testEvent);
}