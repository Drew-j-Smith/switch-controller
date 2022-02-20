#ifndef MACRO_RECORDER_H
#define MACRO_RECORDER_H

#include "pch.h"

#include "InputEvent/InputEvent.h"
#include "Macro.h"
#include <boost/date_time/posix_time/posix_time.hpp>

class MacroRecorder {
private:
    const int RECORDING_BUTTON_COOLDOWN = 1000;

    std::shared_ptr<Macro> currentRecordingMacro = std::make_shared<Macro>();
    std::shared_ptr<Macro> lastRecordedMacro = std::make_shared<Macro>();
    std::shared_ptr<InputEvent> record;

    std::chrono::steady_clock::time_point activationTime =
        std::chrono::steady_clock::now();
    bool recording = false;

public:
    MacroRecorder(const boost::property_tree::ptree &tree,
                  InputEventFactory &factory) {
        lastRecordedMacro->setInputEvent(
            factory.create(tree.find("playLastRecorded")->second));
        record = factory.create(tree.find("record")->second);
    }

    void update(const std::array<uint8_t, 8> &data) {
        auto now = std::chrono::steady_clock::now();

        if (record->getInputValue() &&
            std::chrono::duration_cast<std::chrono::milliseconds>(
                now - activationTime)
                    .count() > RECORDING_BUTTON_COOLDOWN) {
            activationTime = now;
            if (recording) {
                auto ptime = boost::date_time::second_clock<
                    boost::posix_time::ptime>::local_time();
                std::string str =
                    boost::posix_time::to_iso_string(ptime) + ".hex";
                std::cout << "Saved recording to \"RecordedMacros/" << str
                          << "\"" << std::endl;

                currentRecordingMacro->saveData("RecordedMacros/" + str);
                auto macroData = currentRecordingMacro->getData();
                lastRecordedMacro->setData(macroData);
                currentRecordingMacro->setData({});
            }
            recording = !recording;
        }
        if (recording) {
            currentRecordingMacro->appendData(
                {(uint64_t)
                     std::chrono::duration_cast<std::chrono::milliseconds>(
                         now - activationTime)
                         .count(),
                 data});
        }
    }

    const std::shared_ptr<Macro> getLastRecordedMacro() const {
        return lastRecordedMacro;
    }
};

#endif