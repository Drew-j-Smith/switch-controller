#ifndef MACRO_RECORDER_H
#define MACRO_RECORDER_H

#include "pch.h"

#include <boost/property_tree/ptree.hpp>

#include "Macro.h"
#include "InputEventCollection.h"
#include "TimeString.h"

class MacroRecorder
{
private:
    const int RECORDING_BUTTON_COOLDOWN = 1000;

    std::shared_ptr<Macro> currentRecordingMacro = std::make_shared<Macro>();
    std::shared_ptr<Macro> lastRecordedMacro = std::make_shared<Macro>();
    std::shared_ptr<InputEvent> record;

    std::chrono::steady_clock::time_point activationTime = std::chrono::steady_clock::now();
    bool recording = false;
public:
    MacroRecorder(const boost::property_tree::ptree & tree) {
        lastRecordedMacro->setInputEvent(std::make_shared<InputEventCollection>(tree.find("playLastRecorded")->second));
        record = std::make_shared<InputEventCollection>(tree.find("record")->second);
    }

    void update(const unsigned char data[8]) {
        auto now = std::chrono::steady_clock::now();
        
        if (record->getInputValue() && std::chrono::duration_cast<std::chrono::milliseconds>(now - activationTime).count() > RECORDING_BUTTON_COOLDOWN) {
            activationTime = now;
            if (recording) {
                std::string str = getTimeStr();
                std::cout << "Saved recording to \"" << str << "\"" << std::endl;

                auto macroData = currentRecordingMacro->getData();
                macroData.save(str);
                lastRecordedMacro->setData(macroData);
                currentRecordingMacro->setData({});
            }
            recording = !recording;
        }
        if (recording) {
            currentRecordingMacro->appendData(std::chrono::duration_cast<std::chrono::milliseconds>(now - activationTime).count(), data);
        }
    }

    const std::shared_ptr<Macro> getLastRecordedMacro() const { return lastRecordedMacro; }
};


#endif