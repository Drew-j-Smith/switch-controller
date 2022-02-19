
#include "pch.h"

#include "InputEvent/Utility/InputEventPrompt.h"
#include "InputEvent/Utility/InputManager.h"

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

void CreateConfig() {
    boost::property_tree::ptree testEvent = promptForInputEvent(
        "Controls", InputManager::getSchema(), createSchema({}));
    boost::property_tree::write_json(std::cout, testEvent);
}