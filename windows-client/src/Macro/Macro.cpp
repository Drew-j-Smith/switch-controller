#include "Macro.h"

#include <boost/endian/conversion.hpp>

Macro::Macro(const std::string & name, const std::vector<MacroData>& data, const std::shared_ptr<InputEvent> & inputEvent,
    const std::shared_ptr<Decider> & decider, const InputMergeMode mode){
    this->name = name;
    this->data = data;
    this->inputEvent = inputEvent;
    this->decider = decider;
    this->mode = mode;
}

Macro::Macro(const boost::property_tree::ptree & tree, const std::map<std::string, std::shared_ptr<Decider>> & deciderList) {
    name = tree.get("name", "");
    std::string filename = tree.get("filename", "");
    loadData(filename);

    auto inputIt = tree.find("input");
    if (inputIt != tree.not_found())
        inputEvent = std::make_shared<InputEventCollection>(inputIt->second);

    auto deciderIt = deciderList.find(tree.get("decider", ""));
    if (deciderIt != deciderList.end())
        decider = deciderIt->second;

    mode = strToInputMergeMode(tree.get("input merge mode", ""));
}

Macro::InputMergeMode Macro::strToInputMergeMode(const std::string & str) {
    if (str == "block input")
        return blockInput;
    if (str == "input priority")
        return inputPriority;
    if (str == "macro priority")
        return macroPriority;
    
    std::cerr << "Unrecognized input merge mode \"" << str << "\" in macro \"" << name << "\"\n";
    return inputPriority;
}

void Macro::setNextMacroLists(const boost::property_tree::ptree & tree, const std::map<std::string, std::shared_ptr<Macro>> & macroMap){
    boost::property_tree::ptree nextMacroTree;
    for (auto macroTree : tree) {
        if (macroTree.second.get("name", "") == name) {
            nextMacroTree = macroTree.second.find("next macro list")->second;
        }
    }
    for (auto macroListTree : nextMacroTree) {
        std::vector<std::weak_ptr<Macro>> macroList;
        for (auto macroTree : macroListTree.second) {
            macroList.push_back(macroMap.at(macroTree.second.get("", "")));
        }
        nextMacroLists.push_back(macroList);
    }
}

std::array<uint8_t, 8> Macro::getDataframe(uint64_t time, const std::array<uint8_t, 8>& dataToMerge) const {
    MacroData searchData = {time, {}};
    auto res = std::lower_bound(data.begin(), data.end(), searchData, 
        [](const MacroData& a, const MacroData& b) {
            return a.time < b.time;
         }
    );

    if (mode == macroPriority)
        return mergeData(res->data, dataToMerge);
    else if (mode == inputPriority)
        return mergeData(dataToMerge, res->data);
    else // mode == blockInput
        return res->data;
}

void Macro::appendData(const MacroData& inData) {
    if (data.size() == 0 || data.back().data != inData.data) {
        data.push_back(inData);
    }
}

std::shared_ptr<Macro> Macro::getNextMacro() {
    int macroIndex = decider->nextListIndex();
    if (nextMacroLists.size() == 0)
        return nullptr;
    if(nextMacroLists[macroIndex].size() == 0)
        return nullptr;
    if(nextMacroLists[macroIndex].size() != 1)
        std::rotate(nextMacroLists[macroIndex].begin(), nextMacroLists[macroIndex].begin() + 1, nextMacroLists[macroIndex].end());
    return nextMacroLists[macroIndex].back().lock();
}

std::array<uint8_t, 8> Macro::mergeData(const std::array<uint8_t, 8>& priortyData, const std::array<uint8_t, 8>& dataToMerge){
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
    std::ofstream outfile(filename, std::ios::out | std::ios::binary);
    for (auto macroData : data) {
        boost::endian::native_to_little_inplace(macroData.time);
        outfile.write((char*)&macroData, sizeof(MacroData));
    }
}
void Macro::loadData(std::string filename) {
    data.clear();
    std::ifstream infile(filename, std::ios::in | std::ios::binary);
    while (!infile.eof()) {
        MacroData macroData;
        infile.read((char*)&macroData, sizeof(MacroData));
        boost::endian::little_to_native_inplace(macroData.time);
        data.push_back(macroData);
    }
}