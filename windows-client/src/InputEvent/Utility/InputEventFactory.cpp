
#include "InputEventFactory.h"
#include "InputEvent/InputEventTypes.h"

InputEventFactory::InputEventFactory(
    const std::map<std::string, boost::property_tree::ptree>
        &inputEventTemplates) {
    std::vector<std::string> names = {AC_INPUT_EVENT_NAMES};
    for (auto templateString : inputEventTemplates) {
        factories[templateString.first] =
            boost::bind(boost::factory<std::shared_ptr<InputEventTemplate>>(),
                        templateString.second, _1, boost::ref(*this));
    }
    addClasses<AC_INPUT_EVENT_TYPES>(names);
}