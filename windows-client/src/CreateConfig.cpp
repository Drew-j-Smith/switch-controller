
#include "pch.h"

#include "InputEvent/InputManager.h"
#include "Utility/InputEventPrompt.h"


#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

void CreateConfig() {
    boost::property_tree::ptree testEvent = promptForInputEvent(
        "Controls", InputManager::getSchema(), createSchema());
    boost::property_tree::write_json(std::cout, testEvent);
}