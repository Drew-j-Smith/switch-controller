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
    
    void setNextMacroLists(const boost::property_tree::ptree & tree, const std::map<std::string, std::shared_ptr<Macro>> & macroList);
    void setNextMacroLists(const std::vector<std::vector<std::weak_ptr<Macro>>> & nextMacroLists) { this->nextMacroLists = nextMacroLists; }
private:
    std::string name;
    CharStream<15> data;
    std::shared_ptr<InputEvent> inputEvent;
    std::shared_ptr<MacroDecider> decider;

    std::vector<std::vector<std::weak_ptr<Macro>>> nextMacroLists;

    std::shared_ptr<Macro> cycleVector(int macroIndex);
public:
    const std::string & getName() const { return name; }
    const std::shared_ptr<InputEvent> getInputEvent() const { return inputEvent; }
    const std::shared_ptr<MacroDecider> getMacroDecider() const { return decider; }
    std::shared_ptr<Macro> getNextMacro();

    const CharStream<15>* getData() const { return &data; }
    void setData(CharStream<15>* data) { this->data = *data; }
    void appendData(const unsigned long long, const unsigned char[8]);
    void getDataframe(const unsigned long long, unsigned char[8]) const;
    unsigned long long getTime(int index) const { return *(unsigned long long*)data[index].data(); }
    unsigned long long lastTime()         const { return *(unsigned long long*)data.back().data(); }
    
};


#endif
