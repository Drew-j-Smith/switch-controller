#ifndef MACRO_H
#define MACRO_H

#include "pch.h"

#include <boost/property_tree/ptree.hpp>

#include "Decider/Decider.h"
#include "InputEvent/DefaultInputEvent.h"
#include "InputEvent/InputEventCollection.h"

class Macro {
public:
    struct MacroData {
        uint64_t time;
        std::array<uint8_t, 8> data;
    };

    enum InputMergeMode { blockInput, macroPriority, inputPriority };

    Macro(){};
    Macro(const std::string &name, const std::vector<MacroData> &data,
          const std::shared_ptr<InputEvent> &inputEvent,
          const std::shared_ptr<Decider> &decider, const InputMergeMode mode);
    Macro(const boost::property_tree::ptree &tree,
          const std::map<std::string, std::shared_ptr<Decider>> &deciderList);

    void setNextMacroLists(
        const boost::property_tree::ptree &tree,
        const std::map<std::string, std::shared_ptr<Macro>> &macroMap);
    void setNextMacroLists(const std::vector<std::vector<std::weak_ptr<Macro>>>
                               &newNextMacroLists) {
        this->nextMacroLists = newNextMacroLists;
    }

private:
    std::string name = "";
    std::vector<MacroData> data;
    std::shared_ptr<InputEvent> inputEvent =
        std::make_shared<DefaultInputEvent>();
    std::shared_ptr<Decider> decider = std::make_shared<DefaultDecider>();
    InputMergeMode mode = inputPriority;

    std::vector<std::vector<std::weak_ptr<Macro>>> nextMacroLists;

public:
    const std::string &getName() const { return name; }
    void setName(const std::string &newName) { this->name = newName; }
    const std::shared_ptr<InputEvent> getInputEvent() const {
        return inputEvent;
    }
    void setInputEvent(const std::shared_ptr<InputEvent> newInputEvent) {
        this->inputEvent = newInputEvent;
    }
    const std::shared_ptr<Decider> getMacroDecider() const { return decider; }
    void setMacroDecider(const std::shared_ptr<Decider> &newDecider) {
        this->decider = newDecider;
    }
    std::shared_ptr<Macro> getNextMacro();
    InputMergeMode getMode() { return mode; }
    void setMode(const InputMergeMode newMode) { this->mode = newMode; }

    void saveData(std::string filename) const;
    void loadData(std::string filename);
    void setData(const std::vector<MacroData> &newData) { data = newData; }
    std::vector<MacroData> getData() const { return data; }
    void appendData(const MacroData &inData);
    std::array<uint8_t, 8>
    getDataframe(uint64_t time,
                 const std::array<uint8_t, 8> &dataToMerge) const;
    uint64_t lastTime() const { return data.size() > 0 ? data.back().time : 0; }

    static std::array<uint8_t, 8>
    mergeData(const std::array<uint8_t, 8> &priortyData,
              const std::array<uint8_t, 8> &dataToMerge);
    InputMergeMode strToInputMergeMode(const std::string &str);
};

#endif
