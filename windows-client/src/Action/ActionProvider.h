#ifndef ACTION_PROVIDER_H
#define ACTION_PROVIDER_H

#include "pch.h"

#include "Action.h"
#include "ActionSequence.h"

class ActionProvider {
private:
    const ActionSequence sequence;
    const std::function<std::future<std::shared_ptr<ActionProvider>>()>
        nextProvider;
    bool active;
    std::chrono::steady_clock::time_point activationTime;

    uint64_t getTimeDelta() const {
        auto now = std::chrono::steady_clock::now();
        auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(
            now - activationTime);
        return diff.count();
    }

public:
    ActionProvider(
        const ActionSequence &sequence,
        const std::function<std::future<std::shared_ptr<ActionProvider>>()>
            &nextProvider)
        : sequence(sequence), nextProvider(nextProvider){};

    void start() {
        activationTime = std::chrono::steady_clock::now();
        active = true;
    }
    void stop() { active = false; }
    bool isActive() {
        if (active && getTimeDelta() > sequence.back().time) {
            active = false;
        }
        return active;
    }
    Action getAction() const { return sequence.actionAt(getTimeDelta()); }
    std::future<std::shared_ptr<ActionProvider>> getNextProvider() const {
        return nextProvider();
    }
};

#endif