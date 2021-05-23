#include "MacroCollection.h"

MacroCollection::MacroCollection(const std::vector<std::shared_ptr<Macro>> & macros, const std::shared_ptr<DeciderCollectionBase> & deciders) {
    this->macros = macros;
    this->deciders = deciders;
}

MacroCollection::MacroCollection(const boost::property_tree::ptree & tree, const std::shared_ptr<DeciderCollectionBase> & deciders) {
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
    if(activeMacros.size()){
        auto now = std::chrono::steady_clock::now();
        std::vector<std::shared_ptr<Macro>> toAdd;
        std::vector<std::shared_ptr<Macro>> toRemove;

        for (auto it : activeMacros) {
            unsigned long long time = std::chrono::duration_cast<std::chrono::milliseconds>(now - it.second).count();
            it.first->getDataframe(time, data);

            if (it.first->lastTime() < time){
                toAdd.push_back(it.first->getNextMacro());
                toRemove.push_back(it.first);
            }
        }

        for (auto m : toRemove)
            activeMacros.erase(m);

        for (auto m : toAdd)
            if (m)
                activeMacros.insert({m, std::chrono::steady_clock::now()});
    }
}

void MacroCollection::activateMacros() {
    for(auto m : macros){
        if(activeMacros.find(m) == activeMacros.end() && m->getInputEvent()->getInputValue()) {
            activeMacros.insert({m, std::chrono::steady_clock::now()});
        }
    }
}