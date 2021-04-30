
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Joystick.hpp>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "pch.h"

#include "SerialInterface.h"
#include "MacroCollection.h"
#include "InputManager.h"
#include "WindowsScreenshotUtility.h"
#include "MacroImageProcessingDeciderCollection.h"

#include <windows.h>


int main(){ 
    SerialInterface s("COM4", 57600, 8, 1);
    unsigned char send[8];
    unsigned char recieve[1];

    boost::property_tree::ptree tree;
    boost::property_tree::read_json("test5.json", tree);

    InputManager i(tree.find("controls")->second);

    auto screenshotUtil = std::make_shared<WindowsScreenshotUtility>(1920, 1080, "Game Capture HD");
    auto deciders = std::make_shared<MacroImageProcessingDeciderCollection>(tree.find("deciders")->second, screenshotUtil);
    MacroCollection macroCollection(tree.find("macros")->second, deciders);
    macroCollection.pushBackMacro(i.getLastRecordedMacro());

    while (true) {
        sf::Joystick::update();
        
        // auto begin = std::chrono::steady_clock::now();
        
        macroCollection.activateMacros();
        if (macroCollection.isMacroActive()){
            macroCollection.getData(send);
        }
        else {
            i.getData(send);
        }

        s.sendData(send, recieve);
        if ((int)(recieve[0]) != 85)
            std::cout << (int)(recieve[0]) << std::endl;
        

        // auto end = std::chrono::steady_clock::now();
        // std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << std::endl;
    }

    return 0;
}