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
    MacroRecorder(const MacroRecorder &other) = delete;
    MacroRecorder(MacroRecorder &&other) = default;
    MacroRecorder &operator=(MacroRecorder &&other) = default;

    void update(const std::array<uint8_t, 8> &data) {
        auto now = std::chrono::steady_clock::now();
        auto timeDiff = static_cast<uint64_t>(
            std::chrono::duration_cast<std::chrono::milliseconds>(
                now - activationTime)
                .count());

        if (recordEvent() && timeDiff > RECORDING_BUTTON_COOLDOWN) {
            activationTime = now;
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
            currentRecording.push_back({timeDiff, data});
        }
    }

    const std::shared_ptr<Macro> getLastRecordedMacro() const {
        return lastRecordedMacro;
    }
};

#endif