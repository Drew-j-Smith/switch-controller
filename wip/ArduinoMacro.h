#ifndef ARDUINO_MACRO_H
#define ARDUINO_MACRO_H

#include "pch.h"

#include <boost/property_tree/ptree.hpp>

#include "CharStream.h"
#include "MacroDecider.h"

class Macro {
public:
    Macro() {};
    Macro(std::string name, CharStream<15> & data, int keyboardButton, std::shared_ptr<MacroDecider> decider);
    Macro(const boost::property_tree::ptree & tree, const std::map<std::string, MacroDecider> & deciderList);
    
    void setMacroLists(const boost::property_tree::ptree & tree, const std::map<std::string, std::shared_ptr<Macro>> & macroList);
    void setMacroLists(std::vector<std::vector<std::weak_ptr<Macro>>> & nextMacroLists) { this->nextMacroLists = nextMacroLists; }
private:
    std::string name;
    CharStream<15> data;
    int keyboardButton;
    std::shared_ptr<MacroDecider> decider;

    std::vector<std::vector<std::weak_ptr<Macro>>> nextMacroLists;

    static std::shared_ptr<Macro> cycleVector(std::vector<std::weak_ptr<Macro>> macroVector);
public:
    void setData(CharStream<15>* data) { this->data = *data; }
    void appendData(const unsigned long long, const unsigned char[8]);

    //thread safe methods
    const CharStream<15>* getData() const { return &data; }
    int getKeyboardButton() const { return keyboardButton; }
    void getDataframe(const unsigned long long, unsigned char[8]) const;
    std::shared_ptr<Macro> getNextMacro() const;

    unsigned long long getTime(int index) const { return *(unsigned long long*)data[index].data(); }
    unsigned long long lastTime()         const { return *(unsigned long long*)data.back().data(); }
    //end thread safe methods
    
};


#endif
