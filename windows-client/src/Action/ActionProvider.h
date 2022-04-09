#ifndef ACTION_PROVIDER_H
#define ACTION_PROVIDER_H

#include "pch.h"

#include "Action.h"
#include "ActionSequence.h"
#include "Event/Event.h"

class ActionProviderChunk {
    ActionProviderChunk(
        const ActionSequence &a, const std::shared_ptr<Event> &e,
        const std::vector<std::shared_ptr<ActionProviderChunk>> &v){};
    // TODO
};

class ActionProvider {
private:
    std::shared_ptr<ActionProviderChunk> baseChunk;
    std::shared_ptr<ActionProviderChunk> currChunk;

    bool active;
    std::chrono::steady_clock::time_point activationTime;

public:
    ActionProvider(const std::shared_ptr<ActionProviderChunk> &chunk)
        : baseChunk(chunk), currChunk(chunk){};
    void start() {
        active = true;
        activationTime = std::chrono::steady_clock::now();
    };
    Action getAction() {
        // TODO
        return {};
    };
    bool isActive() const { return active; };
    void reset() {
        currChunk = baseChunk;
        active = false;
    };
    // TODO
};

#endif