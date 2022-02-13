#ifndef INPUT_EVENT_TEMPLATE_H
#define INPUT_EVENT_TEMPLATE_H

#include "pch.h"

#include "ConstantInputEvent.h"
#include "InputEvent.h"

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

class InputEventTemplate : public InputEvent {
private:
    /**
     * @brief This will be the template that the InputEvent will be based upon.
     * Every field that starts with $ will be replaced in the constructor
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
    const std::string templateStr;

    std::vector<SchemaItem> schema;
    std::shared_ptr<InputEvent> event;

    static std::vector<SchemaItem>
    createSchema(const boost::property_tree::ptree &tree) {
        std::vector<SchemaItem> res;
        for (auto &child : tree) {
            if (child.first.size() > 0 && child.first[0] == '$') {
                std::string schemaTypeStr =
                    child.second.get<std::string>("type");
                std::string description =
                    child.second.get<std::string>("description");

                InputEvent::SchemaItem::SchemaType schemaType;
                if (schemaTypeStr == "String") {
                    schemaType = InputEvent::SchemaItem::SchemaType::String;
                } else if (schemaTypeStr == "Integer") {
                    schemaType = InputEvent::SchemaItem::SchemaType::Integer;
                } else if (schemaTypeStr == "Event") {
                    schemaType = InputEvent::SchemaItem::SchemaType::Event;
                } else /* if (schemaTypeStr == "EventArray") */ {
                    schemaType = InputEvent::SchemaItem::SchemaType::EventArray;
                }

                res.push_back({child.first.substr(1), schemaType, description});
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

    boost::property_tree::ptree createTemplate() {
        boost::property_tree::ptree tree;
        std::stringstream ss;
        ss << templateStr;
        boost::property_tree::read_json(ss, tree);
        return tree;
    }

    static void
    replaceTree(boost::property_tree::ptree &tree,
                const std::pair<const std::string, boost::property_tree::ptree>
                    &replacement) {
        auto findIt = tree.find(replacement.first);
        while (findIt != tree.not_found()) {
            auto replacementName =
                findIt->second.get<std::string>("replacement name");
            tree.erase(tree.to_iterator(findIt));
            tree.add_child(replacementName, replacement.second);
            findIt = tree.find(replacement.first);
        }
        for (auto &it : tree) {
            replaceTree(it.second, replacement);
        }
    }

public:
    InputEventTemplate(const std::string &templateString)
        : templateStr(templateString) {
        schema = createSchema(createTemplate());
    };

    InputEventTemplate(const std::string &templateString,
                       const boost::property_tree::ptree &tree,
                       InputEventFactory &factory)
        : templateStr(templateString) {
        schema = createSchema(createTemplate());

        /**
         * for every item in the first level of the tree
         * if it start with $, it is a target for replacement
         * replace every instance in the template tree
         */
        boost::property_tree::ptree templateTree = createTemplate();
        for (auto &treeChild : tree) {
            if (treeChild.first.size() > 0 && treeChild.first[0] == '$') {
                // TODO this does not work with recursive template classes
                // should remove $ from schema and add it back when searching
                replaceTree(templateTree, treeChild);
            }
        }
        event = factory.create(templateTree);
    };

    int getInputValue() const override { return event->isDigital(); }

    bool isDigital() const override { return event->isDigital(); }

    void update() override {}

    std::vector<SchemaItem> getSchema() const override { return schema; }
};

#endif