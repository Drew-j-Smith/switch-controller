
#include "InputEventFactory.h"
#include "InputEvent/InputEventTypes.h"

template <class... classes>
static void addClasses(
    const std::vector<std::string> &names, InputEventFactory *factory,
    std::map<std::string, boost::function<std::shared_ptr<InputEvent>(
                              boost::property_tree::ptree)>> &factories) {
    std::vector<boost::function<std::shared_ptr<InputEvent>(
        boost::property_tree::ptree)>>
        factoryVec = {boost::bind(boost::factory<std::shared_ptr<classes>>(),
                                  _1, boost::ref(*factory))...};
    for (int i = 0; i < (int)names.size(); i++) {
        factories[names[i]] = factoryVec[i];
    }
}

InputEventFactory::InputEventFactory(
    const std::map<std::string, boost::property_tree::ptree>
        &inputEventTemplates) {
    std::vector<std::string> names = {AC_INPUT_EVENT_NAMES};
    for (auto templateString : inputEventTemplates) {
        factories[templateString.first] =
            boost::bind(boost::factory<std::shared_ptr<InputEventTemplate>>(),
                        templateString.second, _1, boost::ref(*this));
    }
    addClasses<AC_INPUT_EVENT_TYPES>(names, this, this->factories);
}