#include "Macro.h"

#include <boost/endian/conversion.hpp>

std::array<uint8_t, 8>
Macro::getDataframe(uint64_t time,
                    const std::array<uint8_t, 8> &dataToMerge) const {
    MacroData searchData = {time, {}};
    auto res = std::upper_bound(data.begin(), data.end(), searchData,
                                [](const MacroData &a, const MacroData &b) {
                                    return a.time < b.time;
                                }) -
               1;
    // subtract one to round down instead of up

    if (mode == macroPriority)
        return mergeData(res->data, dataToMerge);
    else if (mode == inputPriority)
        return mergeData(dataToMerge, res->data);
    else // mode == blockInput
        return res->data;
}

void Macro::appendData(const MacroData &inData) {
    if (data.size() == 0 || data.back().data != inData.data) {
        data.push_back(inData);
    }
}

std::shared_ptr<Macro> Macro::getNextMacro() {
    int macroIndex = decider->value(); // TODO
    if (nextMacroLists.size() == 0)
        return nullptr;
    if (nextMacroLists[macroIndex].size() == 0) // TODO index check
        return nullptr;
    if (nextMacroLists[macroIndex].size() != 1)
        std::rotate(nextMacroLists[macroIndex].begin(),
                    nextMacroLists[macroIndex].begin() + 1,
                    nextMacroLists[macroIndex].end());
    return nextMacroLists[macroIndex].back().lock();
}

std::array<uint8_t, 8>
Macro::mergeData(const std::array<uint8_t, 8> &priortyData,
                 const std::array<uint8_t, 8> &dataToMerge) {
    std::array<uint8_t, 8> res;
    res[0] = priortyData[0];
    res[1] = priortyData[1] | dataToMerge[1];
    res[2] = priortyData[2] | dataToMerge[2];
    for (int i = 0; i < 4; i++) {
        if (priortyData[i + 3] == 128) {
            res[i + 3] = dataToMerge[i + 3];
        } else {
            res[i + 3] = priortyData[i + 3];
        }
    }
    if (priortyData[7] == 8) {
        res[7] = dataToMerge[7];
    } else {
        res[7] = priortyData[7];
    }
    return res;
}

void Macro::saveData(std::string filename) const {
    std::ofstream outfile(filename, std::ios::binary);
    if (!outfile.is_open()) {
        std::cerr << "Could not open file " + filename + "\n";
        return;
    }
    for (auto macroData : data) {
        boost::endian::native_to_little_inplace(macroData.time);
        outfile.write((char *)&macroData, sizeof(MacroData));
    }
}
void Macro::loadData(std::string filename) {
    data.clear();
    std::ifstream infile(filename, std::ios::binary);
    if (!infile.is_open()) {
        std::cerr << "Could not open file " + filename + "\n";
        return;
    }
    while (!infile.eof()) {
        MacroData macroData;
        infile.read((char *)&macroData, sizeof(MacroData));
        boost::endian::little_to_native_inplace(macroData.time);
        data.push_back(macroData);
    }
}