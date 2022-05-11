#include "Action.h"

#include <boost/endian/conversion.hpp>

void saveActionVector(const std::string &filename,
                      const std::vector<Action> &record) {
    std::ofstream outfile(filename, std::ios::binary);
    if (!outfile.is_open()) {
        std::cerr << "Could not open file " + filename + "\n";
        return;
    }
    for (auto action : record) {
        boost::endian::native_to_little_inplace(action.time);
        outfile.write((char *)&action, sizeof(action));
    }
}
std::vector<Action> loadActionVector(const std::string &filename) {
    std::vector<Action> res;
    std::ifstream infile(filename, std::ios::binary);
    if (!infile.is_open()) {
        std::cerr << "Could not open file " + filename + "\n";
        return {};
    }
    while (!infile.eof()) {
        Action action;
        infile.read((char *)&action, sizeof(action));
        boost::endian::little_to_native_inplace(action.time);
        res.push_back(action);
    }
    return res;
}