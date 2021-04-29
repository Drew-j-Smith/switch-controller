#ifndef MACRO_H
#define MACRO_H

#include "pch.h"

#include <boost/property_tree/ptree.hpp>

#include "CharStream.h"
#include "MacroDecider.h"
#include "InputEventCollection.h"

class Macro {
public:
    Macro() {};
    Macro(const std::string & name, const CharStream<15> & data, const std::shared_ptr<InputEvent> & inputEvent, const std::shared_ptr<MacroDecider> & decider);
    Macro(const boost::property_tree::ptree & tree, const std::map<std::string, std::shared_ptr<MacroDecider>> & deciderList);
    
    void setNextMacroLists(const boost::property_tree::ptree & tree, const std::map<std::string, std::shared_ptr<Macro>> & macroMap);
    void setNextMacroLists(const std::vector<std::vector<std::weak_ptr<Macro>>> & nextMacroLists) { this->nextMacroLists = nextMacroLists; }
private:
    std::string name = "";
    CharStream<15> data = {};
    std::shared_ptr<InputEvent> inputEvent = std::make_shared<DefaultInputEvent>();
    std::shared_ptr<MacroDecider> decider = std::make_shared<MacroDefaultDecider>();

    std::vector<std::vector<std::weak_ptr<Macro>>> nextMacroLists;

    std::shared_ptr<Macro> cycleVector(int macroIndex);
public:
    const std::string & getName() const { return name; }
    void setName(const std::string & name) { this->name = name; }
    const std::shared_ptr<InputEvent> getInputEvent() const { return inputEvent; }
    void setInputEvent(const std::shared_ptr<InputEvent> inputEvent) { this->inputEvent = inputEvent; }
    const std::shared_ptr<MacroDecider> getMacroDecider() const { return decider; }
    void setMacroDecider(const std::shared_ptr<MacroDecider> & decider) { this->decider = decider; }
    std::shared_ptr<Macro> getNextMacro();

    const CharStream<15> getData() const { return data; }
    void setData(const CharStream<15> data) { this->data = data; }
    void appendData(const unsigned long long, const unsigned char[8]);
    void getDataframe(const unsigned long long, unsigned char[8]) const;
    unsigned long long getTime(int index) const { return data.size() > 0 ? *(unsigned long long*)data[index].data() : 0; }
    unsigned long long lastTime()         const { return data.size() > 0 ? *(unsigned long long*)data.back().data() : 0; }
    
};


#endif
