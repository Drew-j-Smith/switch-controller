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
    InputEventCollection(std::vector<std::shared_ptr<InputEvent>> inputEvents,
                         Operators op)
        : inputEvents(inputEvents), op(op){};

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
};

#endif