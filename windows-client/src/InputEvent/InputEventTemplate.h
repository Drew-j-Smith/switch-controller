#ifndef INPUT_EVENT_TEMPLATE_H
#define INPUT_EVENT_TEMPLATE_H

#include "pch.h"

#include "InputEvent/InputEvent.h"

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

template <class T> class InputEventTemplate : public InputEvent {
private:
    static constexpr std::string_view myString = T::stringView;

    std::vector<SchemaItem> schema = createTemplate();
    std::shared_ptr<InputEvent> event;

    static std::vector<SchemaItem> createTemplate() { return {}; };

public:
    InputEventTemplate(){};

    InputEventTemplate(boost::property_tree::ptree tree,
                       InputEventFactory &factory){};

    int getInputValue() const override { return event->isDigital(); }

    bool isDigital() const override { return event->isDigital(); }

    void update() override {}

    virtual std::vector<SchemaItem> getSchema() const override {
        return schema; // TODO temporary definition
    }
};

#endif