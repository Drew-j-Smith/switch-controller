#ifndef ARDUINO_MACROS_H
#define ARDUINO_MACROS_H

#include "pch.h"

#include <SFML/Window/Keyboard.hpp>
#include "ArduinoMacro.h"

class Macros
{
private:
    std::vector<std::shared_ptr<Macro>> macros;
    std::shared_ptr<Macro> activeMacro;
    std::chrono::steady_clock::time_point timeSinceMacroActivation;
public:
    Macros(std::vector<std::shared_ptr<Macro>> macros) : macros(macros), activeMacro(nullptr) {}

    std::vector<std::future<void>> matchImages(const cv::Mat);
    void getData(const unsigned long long, unsigned char[8]);
    void activateMacros();
    bool isMacroActive() const { return &activeMacro; }
};


#endif