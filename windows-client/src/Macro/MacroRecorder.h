#ifndef MACRO_RECORDER_H
#define MACRO_RECORDER_H

#include "pch.h"

#include "Macro.h"

class MacroRecorder {
private:
    const int RECORDING_BUTTON_COOLDOWN = 1000;

    std::function<bool()> playEvent;

    ActionRecord currentRecording;
    std::shared_ptr<Macro> lastRecordedMacro;
    std::function<bool()> recordEvent;

    std::chrono::steady_clock::time_point activationTime =
        std::chrono::steady_clock::now();
    bool recording = false;

public:
    MacroRecorder(const std::function<bool()> &recordEvent,
                  const std::function<bool()> &playEvent)
        : playEvent(playEvent),
          lastRecordedMacro(std::make_shared<Macro>(Macro{{}, playEvent, {}})),
          recordEvent(recordEvent) {}

    void update(const std::array<uint8_t, 8> &data) {
        auto now = std::chrono::steady_clock::now();
        auto timeDiff = std::chrono::duration_cast<std::chrono::milliseconds>(
                            now - activationTime)
                            .count();

        if (recordEvent() && timeDiff > RECORDING_BUTTON_COOLDOWN) {
            activationTime = now;
            timeDiff = 0;
            if (recording) {
                std::string str =
                    std::to_string(now.time_since_epoch().count()) + ".hex";
                std::cout << "Saved recording to \"RecordedMacros/" << str
                          << "\"" << std::endl;

                saveActionVector("RecordedMacros/" + str, currentRecording);
                *lastRecordedMacro = Macro(currentRecording, playEvent, {});
                currentRecording = {};
            }
            recording = !recording;
        }
        if (recording) {
            currentRecording.push_back({static_cast<uint64_t>(timeDiff), data});
        }
    }

    const std::shared_ptr<Macro> getLastRecordedMacro() const {
        return lastRecordedMacro;
    }
};

#endif