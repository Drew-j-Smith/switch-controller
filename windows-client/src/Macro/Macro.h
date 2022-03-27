#ifndef MACRO_H
#define MACRO_H

#include "pch.h"

#include "Decider/Decider.h"
#include "InputEvent/ConcreteClasses/ConstantInputEvent.h"
#include "InputEvent/InputEvent.h"

class Macro {
public:
    struct MacroData {
        uint64_t time;
        std::array<uint8_t, 8> data;
    };

    enum InputMergeMode { blockInput, macroPriority, inputPriority };

    Macro(){};
    Macro(const std::string &dataFilename,
          std::shared_ptr<InputEvent> inputEvent,
          std::shared_ptr<Decider> decider, InputMergeMode mode)
        : inputEvent(inputEvent), decider(decider), mode(mode) {
        loadData(dataFilename);
    };

    void setNextMacroLists(const std::vector<std::vector<std::weak_ptr<Macro>>>
                               &newNextMacroLists) {
        this->nextMacroLists = newNextMacroLists;
    }

private:
    std::vector<MacroData> data;
    std::shared_ptr<InputEvent> inputEvent =
        std::make_shared<ConstantInputEvent>();
    std::shared_ptr<Decider> decider = std::make_shared<DefaultDecider>();
    InputMergeMode mode = inputPriority;

    std::vector<std::vector<std::weak_ptr<Macro>>> nextMacroLists;

public:
    const std::shared_ptr<InputEvent> getInputEvent() const {
        return inputEvent;
    }
    void setInputEvent(const std::shared_ptr<InputEvent> newInputEvent) {
        this->inputEvent = newInputEvent;
    }
    std::shared_ptr<Macro> getNextMacro();

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
};

#endif
