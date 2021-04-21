#include "MacroCollection.h"

MacroCollection::MacroCollection(const std::vector<std::shared_ptr<Macro>> & macros, const std::shared_ptr<MacroImageProcessingDeciderCollection> & deciders) {
    this->macros = macros;
    this->deciders = deciders;
}

MacroCollection::MacroCollection(const boost::property_tree::ptree & tree, const std::shared_ptr<MacroImageProcessingDeciderCollection> & deciders) {
    this->deciders = deciders;
    std::map<std::string, std::shared_ptr<Macro>> macroMap;
    auto deciderMap = deciders->generateMap();

    for (auto macro : tree) {
        macros.push_back(std::make_shared<Macro>(macro.second, deciderMap));
        macroMap.insert({macros.back()->getName(), macros.back()});
    }
    for (auto macro : macros) {
        macro->setNextMacroLists(tree, macroMap);
    }
}

void MacroCollection::getData(unsigned char data[8]) {
    if(activeMacro != nullptr){
        auto now = std::chrono::steady_clock::now();
        unsigned long long time = std::chrono::duration_cast<std::chrono::milliseconds>(now - timeSinceMacroActivation).count();

        activeMacro->getDataframe(time, data);

        if (activeMacro->lastTime() < time){
            activeMacro = activeMacro->getNextMacro();
            timeSinceMacroActivation = std::chrono::steady_clock::now();
        }
    }
}

void MacroCollection::activateMacros() {
    if(activeMacro == nullptr){
        for(auto m : macros){
            if(m->getInputEvent()->getInputValue()){
                activeMacro = m;
                timeSinceMacroActivation = std::chrono::steady_clock::now();
                break;
            }
        }
    }
}