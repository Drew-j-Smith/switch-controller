#include "ArduinoMacro.h"

#define AC_DISPLAY_IMAGE_MATCH 1

Macro::Macro(const std::string & name, const CharStream<15> & data, const std::shared_ptr<InputEvent> & inputEvent, const std::shared_ptr<MacroDecider> & decider){
    this->name = name;
    this->data = data;
    this->inputEvent = inputEvent;
    this->decider = decider;
}

Macro::Macro(const boost::property_tree::ptree & tree, const std::map<std::string, std::shared_ptr<MacroDecider>> & deciderList) {
    name = tree.get("name", "");
    std::string filename = tree.get("filename", "");
    bool storedAsHex = tree.get("stored as hex", false);
    data.load(filename, storedAsHex);
    inputEvent = std::make_shared<InputEventCollection>(tree.get_child(boost::property_tree::path("input")));
    auto deciderIt = deciderList.find(tree.get("decider", ""));
    if (deciderIt != deciderList.end())
        decider = deciderIt->second;
}

void Macro::setNextMacroLists(const boost::property_tree::ptree & tree, const std::map<std::string, std::shared_ptr<Macro>> & macroList){
    //TODO
}

void Macro::getDataframe(const unsigned long long time, unsigned char data[8]) const{
    int low = 0;
    int high = this->data.size();
    int mid;
    while(high - low > 1){
        mid = (low + high) / 2;
        if(getTime(mid) > time)
            high = mid;
        else
            low = mid;
    }
    data[0] = 85;
    memcpy(data + 1, this->data[low].data() + 8, 7);
}

void Macro::appendData(const unsigned long long time, const unsigned char data[8]){
    std::array<unsigned char, 15> dataframe;
    memcpy(dataframe.data(), &time, 8);
    memcpy(dataframe.data() + 8, data + 1, 7);
    this->data.push_back(dataframe);
}

//helper function to Macro::getNextMacro()
std::shared_ptr<Macro> Macro::cycleVector(std::vector<std::weak_ptr<Macro>> macroVector){
    if(macroVector.size() == 0)
        return std::shared_ptr<Macro>(nullptr);
    else if(macroVector.size() != 1)
        std::rotate(macroVector.begin(), macroVector.begin() + 1, macroVector.end());
    return macroVector.back().lock();
}

const std::shared_ptr<Macro> Macro::getNextMacro() const{
    return cycleVector(nextMacroLists[decider->nextMacroListIndex()]);
}