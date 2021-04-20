#include "MacroCollection.h"

MacroCollection::MacroCollection(std::vector<std::shared_ptr<Macro>> macros, std::vector<std::shared_ptr<MacroImageProcessingDecider>>) {
    this->macros = macros;
    this->deciders = deciders;
    activeMacro.reset();
}

MacroCollection::MacroCollection(const boost::property_tree::ptree & tree) {
    std::map<std::string, std::shared_ptr<MacroDecider>> deciderList;
    std::map<std::string, std::shared_ptr<Macro>> macroList;
    std::vector<boost::property_tree::ptree> macroTrees;

    auto decidersTree = tree.get_child(boost::property_tree::path("deciders"));

    for (auto it = decidersTree.begin(); it != decidersTree.end(); ++it) {
        if (it->second.get("type", "") == "image processing") {
            deciders.push_back(std::make_shared<MacroImageProcessingDecider>(it->second));
            deciderList.insert({deciders.back()->getName(), deciders.back()});
        }
    }

    auto macrosTree = tree.get_child(boost::property_tree::path("macros"));
    for (auto it = macrosTree.begin(); it != macrosTree.end(); ++it) {
        macros.push_back(std::make_shared<Macro>(it->second, deciderList));
        macroList.insert({macros.back()->getName(), macros.back()});
        macroTrees.push_back(it->second);
    }
    for (int i = 0; i < macros.size(); i++) {
        macros[i]->setNextMacroLists(macroTrees[i], macroList);
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

void MacroCollection::updateImageProcessing(const cv::Mat & screenshot) const {
    for (auto decider : deciders) {
        decider->update(screenshot);
    }
}
