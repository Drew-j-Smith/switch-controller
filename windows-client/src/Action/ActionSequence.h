#ifndef ACTION_SEQUENCE_H
#define ACTION_SEQUENCE_H

#include "pch.h"

#include "Action.h"

#include <boost/endian/conversion.hpp>

class ActionSequence : public std::vector<ActionRecord> {
public:
    void save(const std::string &filename) const {
        std::ofstream outfile(filename, std::ios::binary);
        if (!outfile.is_open()) {
            std::cerr << "Could not open file " + filename + "\n";
            return;
        }
        for (auto action : *this) {
            boost::endian::native_to_little_inplace(action.time);
            outfile.write((char *)&action, sizeof(ActionRecord));
        }
    };
    void load(const std::string &filename) {
        clear();
        std::ifstream infile(filename, std::ios::binary);
        if (!infile.is_open()) {
            std::cerr << "Could not open file " + filename + "\n";
            return;
        }
        ActionRecord action;
        infile.read((char *)&action, sizeof(ActionRecord));
        while (infile.gcount() == sizeof(ActionRecord)) {
            boost::endian::little_to_native_inplace(action.time);
            push_back(action);
            infile.read((char *)&action, sizeof(ActionRecord));
        }
    };
    Action actionAt(uint64_t time) const {
        ActionRecord searchData = {{}, time};
        auto predicate = [](const ActionRecord &a, const ActionRecord &b) {
            return a.time < b.time;
        };
        return *(std::upper_bound(begin(), end(), searchData, predicate) - 1);
    };
};

#endif