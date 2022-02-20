#ifndef INPUT_EVENT_COLLECTION_H
#define INPUT_EVENT_COLLECTION_H

#include "pch.h"

#include "InputEvent/InputEvent.h"

class InputEventCollection : public InputEvent {
public:
    enum Operators { And, Or, Not, Xor };

private:
    std::vector<std::shared_ptr<InputEvent>> inputEvents;
    Operators op = Operators::And;

public:
    InputEventCollection(){};
    InputEventCollection(const boost::property_tree::ptree &tree,
                         InputEventFactory &factory) {
        boost::property_tree::ptree childTree = tree.get_child("events");
        for (auto event : childTree) {
            inputEvents.push_back(factory.create(event.second));
        }
        std::string opStr = tree.get<std::string>("operator");
        if (opStr == "and") {
            op = Operators::And;
        }
        if (opStr == "or") {
            op = Operators::Or;
        }
        if (opStr == "not") {
            op = Operators::Not;
        }
        if (opStr == "xor") {
            op = Operators::Xor;
        }
    }

    int getInputValue() const override {
        int res = 0;

        for (int i = 0; i < (int)inputEvents.size(); i++) {
            switch (op) {
            case Operators::And:
                res = res && inputEvents[i]->getInputValue();
                break;
            case Operators::Or:
                res = res || inputEvents[i]->getInputValue();
                break;
            case Operators::Not:
                res = !inputEvents[i]->getInputValue();
            case Operators::Xor:
                res = (res && inputEvents[i]->getInputValue()) ||
                      (!res && !inputEvents[i]->getInputValue());
            default:
                break;
            }
        }

        return res;
    };

    bool isDigital() const override { return true; }

    virtual void update() override {}

    std::vector<SchemaItem> getSchema() const override {
        return {{"events", InputEvent::SchemaItem::EventArray,
                 "The list of events to use"},
                {"operator", InputEvent::SchemaItem::String,
                 "The operator to use between events"}};
    }
};

#endif