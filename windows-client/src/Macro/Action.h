#ifndef ACTION_H
#define ACTION_H

#include "pch.h"

struct Action {
    uint64_t time;
    std::array<uint8_t, 8> data;
};

using ActionRecord = std::vector<Action>;

void saveActionVector(const std::string &filename,
                      const std::vector<Action> &record);
std::vector<Action> loadActionVector(const std::string &filename);

#endif