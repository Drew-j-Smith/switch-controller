
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Joystick.hpp>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "pch.h"

#include "Utility/SerialInterface.h"
#include "Macro/MacroCollection.h"
#include "InputEvent/InputManager.h"
#include "Utility/WindowsScreenshotUtility.h"
#include "Decider/DeciderCollection.h"
#include "Macro/MacroRecorder.h"
#include "InputEvent/InputEventToggle.h"


int main(){ 
    SerialInterface s("COM4", 57600, 8, 1);

    boost::property_tree::ptree tree;
    boost::property_tree::read_json("config.json", tree);

    InputManager inputManager(tree.find("controls")->second, 15);

    auto deciders = std::make_shared<DeciderCollection>(tree.find("deciders")->second);
    MacroCollection macroCollection(tree.find("macros")->second, deciders);

    MacroRecorder recorder(tree.find("controls")->second);
    macroCollection.pushBackMacro(recorder.getLastRecordedMacro());
    
    InputEventCollection stopMacrosEvent(tree.find("controls")->second.find("stopMacros")->second);


    while (true) {
        sf::Joystick::update();
        InputEventToggle::updateAll();
        
        // auto begin = std::chrono::steady_clock::now();
        
        unsigned char send[8];
        unsigned char recieve[1];

        inputManager.getData(send);
        recorder.update(send);
        
        macroCollection.activateMacros();
        if (macroCollection.isMacroActive() && stopMacrosEvent.getInputValue()) {
            macroCollection.deactivateMacros();
        }
        if (macroCollection.isMacroActive()){
            macroCollection.getData(send);
        }

        s.sendData(send, recieve);
        if ((int)(recieve[0]) != 85)
            std::cout << (int)(recieve[0]) << std::endl;
        

        // auto end = std::chrono::steady_clock::now();
        // std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << std::endl;
    }

    return 0;
}