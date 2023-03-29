#pragma once

#include "pch.h"

template <typename Callable> class ToggleEvent {
private:
    Callable callable;
    bool value{};
    std::chrono::steady_clock::time_point start{};
    std::chrono::milliseconds delay;

public:
    constexpr ToggleEvent(Callable callable,
                          const std::chrono::milliseconds &delay)
        : callable(callable), delay(delay) {}

    constexpr bool operator()() {
        if (callable()) {
            auto now = std::chrono::steady_clock::now();
            if (now > start + delay) {
                start = now;
                value = !value;
            }
        }
        return value;
    }
};
