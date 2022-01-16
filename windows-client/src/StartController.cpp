
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Joystick.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "pch.h"

#include "Utility/SerialPort.h"
#include "boost/asio/read.hpp"
#include "boost/asio/write.hpp"
#include "Macro/MacroCollection.h"
#include "InputEvent/InputManager.h"
#include "Decider/DeciderCollection.h"
#include "Macro/MacroRecorder.h"
#include "InputEvent/InputEventToggle.h"

void StartController(std::string& configFilename) {
    std::cout << "Initializing...\n";
    std::cout << "Loading config files.\n";

    sf::Joystick::update();

    boost::property_tree::ptree tree;
    try {
        boost::property_tree::read_json(configFilename, tree);
    }
    catch (boost::property_tree::json_parser::json_parser_error& e) {
        std::cerr << "Error parsing config file\n";
        return;
    }

    InputManager inputManager(tree.find("controls")->second, 15);

    auto deciders = std::make_shared<DeciderCollection>(tree.find("deciders")->second);
    MacroCollection macroCollection(tree.find("macros")->second, deciders);

    MacroRecorder recorder(tree.find("controls")->second);
    macroCollection.pushBackMacro(recorder.getLastRecordedMacro());
    
    InputEventCollection stopMacrosEvent(tree.find("controls")->second.find("stopMacros")->second);

    std::cout << "Config files loaded.\n";

    std::unique_ptr<boost::asio::serial_port> port;

    // sending a nuetral signal
    unsigned char send[8] = {85, 0, 0, 128, 128, 128, 128, 8};
    unsigned char recieve[1];

    try {
        port = initializeSerialPort(tree.get<std::string>("serial port"), 57600);

        testSerialPort(port, 8, send, 1, recieve);
    }
    catch (std::exception& e) {
        std::cerr << "Failure connecting via serial port.\n";
        return;
    }



    while (true) {
        sf::Joystick::update();
        InputEventToggle::updateAll();
        
        // auto begin = std::chrono::steady_clock::now();
        

        inputManager.getData(send);
        recorder.update(send);
        
        macroCollection.activateMacros();
        if (macroCollection.isMacroActive() && stopMacrosEvent.getInputValue()) {
            macroCollection.deactivateMacros();
        }
        if (macroCollection.isMacroActive()){
            macroCollection.getData(send);
        }

        boost::asio::write(*port, boost::asio::buffer(send, 8));
        if (!boost::asio::read(*port, boost::asio::buffer(recieve, 1))) {
            std::cerr << "Error reading from serial port";
        }
        // if ((int)(recieve[0]) != 85)
        //     std::cout << (int)(recieve[0]) << std::endl;
        

        // auto end = std::chrono::steady_clock::now();
        // std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << std::endl;
    }

}