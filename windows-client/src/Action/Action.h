#ifndef ACTION_H
#define ACTION_H

#include "pch.h"

struct Action {
    std::array<uint8_t, 8> data;
};

struct ActionRecord : public Action {
    uint64_t time;
};

#endif