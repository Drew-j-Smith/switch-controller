#ifndef DEFAULT_INPUT_EVENT_H
#define DEFAULT_INPUT_EVENT_H

/**
 * @file DefaultInputEvent.h
 * @author Drew Smith
 * @brief The DefaultInputEvent is a basic implementation of InputEvent
 * which is a digital input that always returns 0.
 * @date 2021-10-17
 * 
 */

#include "pch.h"

#include "InputEvent.h"

/**
 * @brief The DefaultInputEvent is a basic implementation of InputEvent
 * which is a digital input that always returns 0.
 * 
 */
class DefaultInputEvent : public InputEvent
{
public:
    DefaultInputEvent() {};
    int getInputValue() const override { return 0; }
    bool isDigital() const override { return true; }

    std::shared_ptr<InputEvent> makeShared(const boost::property_tree::ptree & tree, Factory<InputEvent> & factory) const override { 
        // Unused parameters
        tree;
        factory;
        
        return std::make_shared<DefaultInputEvent>(); 
    }
    std::string getTypeName() const override { return "DefaultInputEvent"; }

    void update() override {}

    std::string toString() const override { return "DefaultInputEvent"; }
    boost::property_tree::ptree toPtree() const override {
        boost::property_tree::ptree result;
        result.add_child("type", boost::property_tree::ptree("DefaultInputEvent"));
        return result;
    }
};

/**
 * @brief The ActiveInputEvent is a basic implementation of InputEvent
 * which is a digital input that always returns 1.
 * 
 */
class ActiveInputEvent : public InputEvent
{
public:
    ActiveInputEvent() {};
    int getInputValue() const override { return 1; }
    bool isDigital() const override { return true; }

    std::shared_ptr<InputEvent> makeShared(const boost::property_tree::ptree & tree, Factory<InputEvent> & factory) const override { 
        // Unused parameters
        tree;
        factory;
        
        return std::make_shared<ActiveInputEvent>(); 
    }
    std::string getTypeName() const override { return "ActiveInputEvent"; }

    void update() override {}

    std::string toString() const override { return "ActiveInputEvent"; }
    boost::property_tree::ptree toPtree() const override {
        boost::property_tree::ptree result;
        result.add_child("type", boost::property_tree::ptree("ActiveInputEvent"));
        return result;
    }
};


#endif