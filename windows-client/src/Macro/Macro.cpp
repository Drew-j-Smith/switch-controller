#include "Macro.h"

Macro::Macro(const std::string & name, const NByteVector<sizeof(unsigned long long) + 7> & data, const std::shared_ptr<InputEvent> & inputEvent,
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
    bool storedAsHex = tree.get("stored as hex", false);
    data.load(filename, storedAsHex);

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

void Macro::getDataframe(const unsigned long long time, unsigned char outData[8]) const{
    if (this->data.size() == 0)
        return;
    int low = 0;
    int high = (int)this->data.size();
    while(high - low > 1){
        int mid = (low + high) / 2;
        if(getTime(mid) > time)
            high = mid;
        else
            low = mid;
    }
    unsigned char macroData[8];
    macroData[0] = 85;
    memcpy(macroData + 1, this->data[low].data() + sizeof(unsigned long long), 7);

    if (mode == macroPriority)
        mergeData(macroData, outData);
    else if (mode == inputPriority)
        mergeData(outData, macroData);

    if (mode == blockInput || mode == macroPriority)
        memcpy(outData, macroData, 8);
}

void Macro::appendData(const unsigned long long time, const unsigned char outData[8]){
    if (this->data.size() == 0 || memcmp(this->data.back().data() + sizeof(unsigned long long), outData + 1, 7) != 0) {
        std::array<unsigned char, sizeof(unsigned long long) + 7> dataframe;
        memcpy(dataframe.data(), &time, sizeof(unsigned long long));
        memcpy(dataframe.data() + sizeof(unsigned long long), outData + 1, 7);
        this->data.push_back(dataframe);
    }
}

//helper function to Macro::getNextMacro()
std::shared_ptr<Macro> Macro::cycleVector(int macroIndex) {
    if (nextMacroLists.size() == 0)
        return nullptr;
    if(nextMacroLists[macroIndex].size() == 0)
        return nullptr;
    if(nextMacroLists[macroIndex].size() != 1)
        std::rotate(nextMacroLists[macroIndex].begin(), nextMacroLists[macroIndex].begin() + 1, nextMacroLists[macroIndex].end());
    return nextMacroLists[macroIndex].back().lock();
}

std::shared_ptr<Macro> Macro::getNextMacro() {
    return cycleVector(decider->nextListIndex());
}

void Macro::mergeData(unsigned char priortyData[8], const unsigned char dataToMerge[8]){
    priortyData[1] |= dataToMerge[1];
    priortyData[2] |= dataToMerge[2];
    for (int i = 0; i < 4; i++)
        if (priortyData[i + 3] == 128)
            priortyData[i + 3] = dataToMerge[i + 3];
    if (priortyData[7] == 8)
        priortyData[7] = dataToMerge[7];
}