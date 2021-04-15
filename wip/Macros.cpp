#include "Macros.h"

Macros::Macros(std::vector<std::shared_ptr<Macro>> macros, std::vector<std::shared_ptr<MacroImageProcessingDecider>>) {
    this->macros = macros;
    this->deciders = deciders;
    activeMacro.reset();
}

Macros::Macros(const boost::property_tree::ptree & tree) {
    std::map<std::string, std::shared_ptr<MacroDecider>> deciderList;
    std::map<std::string, std::shared_ptr<Macro>> macroList;
    std::vector<boost::property_tree::ptree> macroTrees;

    auto decidersTree = tree.get_child(boost::property_tree::path("deciders"));
    std::cout << "t\n";
    for (auto it = decidersTree.begin(); it != decidersTree.end(); ++it) {
        if (it->second.get("type", "") == "image processing") {
            deciders.push_back(std::make_shared<MacroImageProcessingDecider>(it->second));
            deciderList.insert({deciders.back()->getName(), deciders.back()});
        }
        else if (it->second.get("type", "") == "default") {
            auto defaultDecider = std::make_shared<MacroDefaultDecider>(it->second.get("name", ""));
            deciderList.insert({defaultDecider->getName(), defaultDecider});
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

void Macros::getData(unsigned char data[8]) {
    if(activeMacro != nullptr){
        auto now = std::chrono::steady_clock::now();
        unsigned long long time = std::chrono::duration_cast<std::chrono::milliseconds>(now - timeSinceMacroActivation).count();
        std::cout << time << std::endl;
        activeMacro->getDataframe(time, data);

        if (activeMacro->lastTime() < time){
            activeMacro = activeMacro->getNextMacro();
            timeSinceMacroActivation = std::chrono::steady_clock::now();
        }
    }
}

void Macros::activateMacros() {
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

void Macros::updateImageProcessing(const cv::Mat & screenshot) const {
    for (auto decider : deciders) {
        decider->update(screenshot);
    }
}
