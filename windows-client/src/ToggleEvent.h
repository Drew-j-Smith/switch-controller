#pragma once

#include "pch.h"

using namespace std::chrono_literals;

template <typename Callable> class TurboEvent {
private:
    Callable callable;
    bool active{};
    std::chrono::steady_clock::time_point start{};
    std::chrono::milliseconds delay;

public:
    constexpr TurboEvent(Callable callable,
                         const std::chrono::milliseconds &delay)
        : callable(callable), delay(delay) {}

    constexpr auto operator()() {
        auto val = callable();
        if (!val) {
            active = false;
            return decltype(val){};
        }
        auto now = std::chrono::steady_clock::now();
        if (!active) {
            active = true;
            start = now;
        }
        auto diff = now - start;
        return diff / delay % 2 ? decltype(val){} : val;
    }
};
