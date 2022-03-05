#ifndef INPUT_EVENT_TEMPLATE_H
#define INPUT_EVENT_TEMPLATE_H

#include "pch.h"

#include "ConstantInputEvent.h"
#include "InputEvent/InputEvent.h"

class InputEventTemplate : public InputEvent {
private:
    /**
     * @brief This will describe the template that the InputEvent will be based
     * upon. Every field that starts with $ will be replaced in the constructor
     * each field that starts with $ is expected to have 3 children.
     *
     *
     * "replacement name" - Every instance of the field that starts with $ will
     * be replaced with this name
     *
     * "description" - A description, see InputEvent::SchemaItem
     *
     * "type" - the schema type, see InputEvent::SchemaItem.
     *
     */

    std::vector<SchemaItem> schema;
    std::shared_ptr<InputEvent> event;

    static std::vector<SchemaItem>
    createSchema(const boost::property_tree::ptree &tree) {
        std::vector<SchemaItem> res;
        for (auto &child : tree) {
            if (child.first.size() > 0 && child.first[0] == '$') {
                try {
                    std::string schemaTypeStr =
                        child.second.get<std::string>("type");
                    std::string description =
                        child.second.get<std::string>("description");

                    InputEvent::SchemaItem::SchemaType schemaType;
                    if (schemaTypeStr == "String") {
                        schemaType = InputEvent::SchemaItem::SchemaType::String;
                    } else if (schemaTypeStr == "Integer") {
                        schemaType =
                            InputEvent::SchemaItem::SchemaType::Integer;
                    } else if (schemaTypeStr == "Event") {
                        schemaType = InputEvent::SchemaItem::SchemaType::Event;
                    } else /* if (schemaTypeStr == "EventArray") */ {
                        schemaType =
                            InputEvent::SchemaItem::SchemaType::EventArray;
                    }

                    res.push_back(
                        {child.first.substr(1), schemaType, description});
                } catch (std::exception &e) {
                    std::stringstream ss;
                    ss << e.what();
                    ss << "\nUnable to parse schema item from ptree item \"" +
                              child.first + "\"\n";
                    boost::property_tree::write_json(ss, child.second);
                    ss << boost::stacktrace::stacktrace();
                    BOOST_LOG_TRIVIAL(error) << ss.str();
                }
            }

            auto childSchema = createSchema(child.second);
            for (auto &childSchemaItem : childSchema) {
                auto it = std::find_if(
                    res.begin(), res.end(), [&](InputEvent::SchemaItem &item) {
                        return item.name == childSchemaItem.name;
                    });
                if (it == res.end()) {
                    res.push_back(childSchemaItem);
                }
            }
        }
        return res;
    };

public:
    static boost::property_tree::ptree
    createTree(const boost::property_tree::ptree &templateTree,
               const boost::property_tree::ptree &replacementTree) {
        if (templateTree.size() == 0) {
            return templateTree;
        }
        boost::property_tree::ptree res;
        for (auto &child : templateTree) {
            auto findIt = replacementTree.find(child.first.substr(1));
            if (findIt == replacementTree.not_found()) {
                res.add_child(child.first,
                              createTree(child.second, replacementTree));
            } else {
                try {
                    auto replacementName =
                        child.second.get<std::string>("replacement name");
                    res.add_child(replacementName, findIt->second);
                } catch (std::exception &e) {
                    std::stringstream ss;
                    ss << e.what();
                    ss << "\nUnable to find replacement name for template item "
                          "\"" +
                              child.first + "\"\n";
                    boost::property_tree::write_json(ss, child.second);
                    ss << "With template tree:\n";
                    boost::property_tree::write_json(ss, replacementTree);
                    ss << boost::stacktrace::stacktrace();
                    BOOST_LOG_TRIVIAL(error) << ss.str();
                }
            }
        }
        return res;
    }

    InputEventTemplate(const boost::property_tree::ptree &templateTree) {
        schema = createSchema(templateTree);
    };

    InputEventTemplate(const boost::property_tree::ptree &templateTree,
                       const boost::property_tree::ptree &tree,
                       InputEventFactory &factory) {
        schema = createSchema(templateTree);
        event = factory.create(createTree(templateTree, tree));
    };

    int getInputValue() const override { return event->getInputValue(); }

    bool isDigital() const override { return event->isDigital(); }

    void update() override {}

    std::vector<SchemaItem> getSchema() const override { return schema; }

    bool operator==(const InputEvent &other) const override {
        if (typeid(*this) != typeid(other))
            return false;
        auto localOther = dynamic_cast<const InputEventTemplate &>(other);
        return event == localOther.event;
    }
};

#endif