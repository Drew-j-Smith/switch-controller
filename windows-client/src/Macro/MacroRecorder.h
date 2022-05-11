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
        lastRecordedMacro->activateEvent = playEvent; // TODO
        record = recordEvent;
    }

    void update(const std::array<uint8_t, 8> &data) {
        auto now = std::chrono::steady_clock::now();

        if (record->value() &&
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

                // TODO
                saveActionVector("RecordedMacros/" + str,
                                 currentRecordingMacro->actionVector);
                lastRecordedMacro->actionVector =
                    currentRecordingMacro->actionVector;
                currentRecordingMacro->actionVector = {};
            }
            recording = !recording;
        }
        if (recording) {
            // TODO
            currentRecordingMacro->actionVector.push_back(
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