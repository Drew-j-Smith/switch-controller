#ifndef DECIDER_COLLECTION_H
#define DECIDER_COLLECTION_H

#include "pch.h"

#include "Decider.h"

class DeciderCollection {
private:
    std::vector<std::shared_ptr<Decider>> deciders;
    std::vector<std::future<void>> updateList;

public:
    DeciderCollection(const std::vector<std::shared_ptr<Decider>> &deciders)
        : deciders(deciders) {
        updateList.resize(deciders.size());
    };

    void update() {
        for (unsigned int i = 0; i < deciders.size(); i++) {
            if (updateList[i].wait_for(std::chrono::nanoseconds::zero()) ==
                    std::future_status::ready &&
                deciders[i]->shouldUpdate()) {
                updateList[i] = std::async(std::launch::async,
                                           [&]() { deciders[i]->update(); });
            }
        }
    }
};

#endif