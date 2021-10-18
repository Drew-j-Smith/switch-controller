#ifndef MACRO_H
#define MACRO_H

#include "pch.h"

#include <boost/property_tree/ptree.hpp>

#include "NByteVector.h"
#include "Decider/Decider.h"
#include "InputEvent/DefaultInputEvent.h"
#include "InputEvent/InputEventCollection.h"

class Macro {
public:
    enum InputMergeMode {blockInput, macroPriority, inputPriority};

    Macro() {};
    Macro(const std::string & name, const NByteVector<sizeof(unsigned long long) + 7> & data, const std::shared_ptr<InputEvent> & inputEvent,
        const std::shared_ptr<Decider> & decider, const InputMergeMode mode);
    Macro(const boost::property_tree::ptree & tree, const std::map<std::string, std::shared_ptr<Decider>> & deciderList);
    
    void setNextMacroLists(const boost::property_tree::ptree & tree, const std::map<std::string, std::shared_ptr<Macro>> & macroMap);
    void setNextMacroLists(const std::vector<std::vector<std::weak_ptr<Macro>>> & nextMacroLists) { this->nextMacroLists = nextMacroLists; }
private:
    std::string name = "";
    NByteVector<sizeof(unsigned long long) + 7> data = {};
    std::shared_ptr<InputEvent> inputEvent = std::make_shared<DefaultInputEvent>();
    std::shared_ptr<Decider> decider = std::make_shared<DefaultDecider>();
    InputMergeMode mode = inputPriority;

    std::vector<std::vector<std::weak_ptr<Macro>>> nextMacroLists;

    std::shared_ptr<Macro> cycleVector(int macroIndex);
public:
    const std::string & getName() const { return name; }
    void setName(const std::string & name) { this->name = name; }
    const std::shared_ptr<InputEvent> getInputEvent() const { return inputEvent; }
    void setInputEvent(const std::shared_ptr<InputEvent> inputEvent) { this->inputEvent = inputEvent; }
    const std::shared_ptr<Decider> getMacroDecider() const { return decider; }
    void setMacroDecider(const std::shared_ptr<Decider> & decider) { this->decider = decider; }
    std::shared_ptr<Macro> getNextMacro();
    InputMergeMode getMode() { return mode; }
    void setMode(const InputMergeMode mode) { this->mode = mode; }

    const NByteVector<sizeof(unsigned long long) + 7> getData() const { return data; }
    void setData(const NByteVector<sizeof(unsigned long long) + 7> data) { this->data = data; }
    void appendData(const unsigned long long, const unsigned char[8]);
    void getDataframe(const unsigned long long, unsigned char[8]) const;
    unsigned long long getTime(int index) const { return data.size() > 0 ? *(unsigned long long*)data[index].data() : 0; }
    unsigned long long lastTime()         const { return data.size() > 0 ? *(unsigned long long*)data.back().data() : 0; }

    static void mergeData(unsigned char priortyData[8], const unsigned char dataToMerge[8]);
    InputMergeMode strToInputMergeMode(const std::string & str);
};


#endif
