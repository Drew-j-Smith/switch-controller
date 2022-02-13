#include "InputEventPrompt.h"

#include "InputEvent/InputEventTypes.h"

template <class... classes>
static std::map<std::string, std::vector<InputEvent::SchemaItem>>
addClasses(const std::vector<std::string> &names) {

    std::vector<std::shared_ptr<InputEvent>> schemas = {
        std::make_shared<classes>()...};
    std::map<std::string, std::vector<InputEvent::SchemaItem>> res;
    for (int i = 0; i < names.size(); i++) {
        res[names[i]] = schemas[i]->getSchema();
    }
    return res;
}

std::map<std::string, std::vector<InputEvent::SchemaItem>> createSchema() {
    std::vector<std::string> names = {AC_INPUT_EVENT_NAMES};
    std::map<std::string, std::vector<InputEvent::SchemaItem>> schemaList =
        addClasses<AC_INPUT_EVENT_TYPES>(names);
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
        std::cout << "Enter an InputEvent (or an empty string to not include "
                     "an event):";
        std::getline(std::cin, strline);
        if (strline.length() == 0) {
            return boost::property_tree::ptree();
        }
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
    std::cout << "Add another InputEvent? (y/n):";
    std::getline(std::cin, strline);
    boost::property_tree::ptree ptreeEventArray;
    while (strline == "y") {
        ptreeEventArray.add_child(" ", promptForEventType(schemaList));
        std::cout << "Add another InputEvent? (y/n):";
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
            auto event = promptForEventType(schemaList);
            if (event.size() > 0) {
                res.add_child(schemaItem.name, event);
            }
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