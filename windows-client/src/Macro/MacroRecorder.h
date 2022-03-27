#ifndef MACRO_RECORDER_H
#define MACRO_RECORDER_H

#include "pch.h"

#include "Event/Event.h"
#include "Macro.h"

class MacroRecorder {
private:
    const int RECORDING_BUTTON_COOLDOWN = 1000;

    std::shared_ptr<Macro> currentRecordingMacro = std::make_shared<Macro>();
    std::shared_ptr<Macro> lastRecordedMacro = std::make_shared<Macro>();
    std::shared_ptr<Event> record;

    std::chrono::steady_clock::time_point activationTime =
        std::chrono::steady_clock::now();
    bool recording = false;

public:
    MacroRecorder(std::shared_ptr<Event> recordEvent,
                  std::shared_ptr<Event> playEvent) {
        lastRecordedMacro->setActivateEvent(playEvent);
        record = recordEvent;
    }

    void update(const std::array<uint8_t, 8> &data) {
        auto now = std::chrono::steady_clock::now();

        if (record->getEventValue() &&
            std::chrono::duration_cast<std::chrono::milliseconds>(
                now - activationTime)
                    .count() > RECORDING_BUTTON_COOLDOWN) {
            activationTime = now;
            if (recording) {
                auto time = std::chrono::steady_clock::now();
                std::string str =
                    std::to_string(time.time_since_epoch().count()) + ".hex";
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