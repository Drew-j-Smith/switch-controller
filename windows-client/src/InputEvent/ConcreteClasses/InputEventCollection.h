#ifndef INPUT_EVENT_COLLECTION_H
#define INPUT_EVENT_COLLECTION_H

#include "pch.h"

#include "InputEvent/InputEvent.h"

#include <boost/log/trivial.hpp>
#include <boost/stacktrace.hpp>

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
        try {
            boost::property_tree::ptree childTree = tree.get_child("events");
            for (auto event : childTree) {
                inputEvents.push_back(factory.create(event.second));
            }
            std::string opStr = tree.get<std::string>("operator");
            if (opStr == "and") {
                op = Operators::And;
            } else if (opStr == "or") {
                op = Operators::Or;
            } else if (opStr == "not") {
                op = Operators::Not;
            } else /* if (opStr == "xor") */ {
                op = Operators::Xor;
            }
        } catch (std::exception &e) {
            std::stringstream ss;
            ss << e.what();
            ss << "\nUnable to parse input event collection from ptree:\n";
            boost::property_tree::write_json(ss, tree);
            ss << boost::stacktrace::stacktrace();
            BOOST_LOG_TRIVIAL(error) << ss.str();
            inputEvents.clear();
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

    bool operator==(const InputEvent &other) const override {
        if (typeid(*this) != typeid(other))
            return false;
        auto localOther = dynamic_cast<const InputEventCollection &>(other);
        if (op != localOther.op ||
            inputEvents.size() != localOther.inputEvents.size()) {
            return false;
        }
        for (int i = 0; i < inputEvents.size(); i++) {
            if (!(*inputEvents[i] == *localOther.inputEvents[i])) {
                return false;
            }
        }
        return true;
    }
};

#endif