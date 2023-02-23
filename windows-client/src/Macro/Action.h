#pragma once

#include "pch.h"
#include <boost/endian/conversion.hpp>

struct Action {
    uint64_t time;
    std::array<uint8_t, 8> data;
};

void saveActionVector(const std::string &filename,
                      const std::vector<Action> &record) {
    std::ofstream outfile(filename, std::ios::binary);
    if (!outfile.is_open()) {
        spdlog::error("Could not open file: {}", filename);
        return;
    }
    for (auto action : record) {
        boost::endian::native_to_little_inplace(action.time);
        outfile.write(reinterpret_cast<char *>(&action), sizeof(action));
    }
}
std::vector<Action> loadActionVector(const std::string &filename) {
    std::vector<Action> res;
    std::ifstream infile(filename, std::ios::binary);
    if (!infile.is_open()) {
        spdlog::error("Could not open file: {}", filename);
        return {};
    }
    Action action;
    while (infile.readsome(reinterpret_cast<char *>(&action), sizeof(action)) ==
           sizeof(action)) {
        boost::endian::little_to_native_inplace(action.time);
        res.push_back(action);
    }
    return res;
}
