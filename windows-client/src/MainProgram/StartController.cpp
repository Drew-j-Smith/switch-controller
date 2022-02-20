
#include <SFML/Window/Joystick.hpp>
#include <SFML/Window/Keyboard.hpp>

#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>
#include <boost/log/trivial.hpp>
#include <boost/stacktrace/stacktrace.hpp>

#include "pch.h"

#include "MainProgram.h"

#include "Decider/DeciderCollection.h"
#include "InputEvent/ConcreteClasses/InputEventToggle.h"
#include "InputEvent/Utility/InputManager.h"
#include "Macro/MacroCollection.h"
#include "Macro/MacroRecorder.h"
#include "Utility/SerialPort.h"

void StartController(std::string &configFilename) {
    std::cout << "Initializing...\n";
    std::cout << "Loading config files.\n";

    // Updating joysticks to see if they are connected
    sf::Joystick::update();

    boost::property_tree::ptree tree;
    try {
        boost::property_tree::read_json(configFilename, tree);
    } catch (boost::property_tree::json_parser::json_parser_error &e) {
        BOOST_LOG_TRIVIAL(error) << "Error parsing config file\n" +
                                        std::string(e.what()) + "\n" +
                                        boost::stacktrace::to_string(
                                            boost::stacktrace::stacktrace());
        return;
    }

    InputEventFactory factory({});
    InputManager inputManager(tree.find("controls")->second, factory);

    auto deciders =
        std::make_shared<DeciderCollection>(tree.find("deciders")->second);
    MacroCollection macroCollection(tree.find("macros")->second, deciders,
                                    factory);

    MacroRecorder recorder(tree.find("controls")->second, factory);
    macroCollection.pushBackMacro(recorder.getLastRecordedMacro());

    std::shared_ptr<InputEvent> stopMacrosEvent = factory.create(
        tree.find("controls")->second.find("stopMacros")->second);

    std::cout << "Config files loaded.\n";

    std::unique_ptr<boost::asio::serial_port> port;
    boost::asio::io_service io;

    // sending a nuetral signal
    std::array<uint8_t, 8> send = {85, 0, 0, 128, 128, 128, 128, 8};
    unsigned char recieve[1];

    try {
        port = initializeSerialPort(tree.get<std::string>("serial port"), 57600,
                                    &io);

        testSerialPort(port, 8, send.data(), 1, recieve, &io);
    } catch (std::exception &e) {
        BOOST_LOG_TRIVIAL(error) << "Failure connecting via serial port.\n" +
                                        std::string(e.what()) + "\n";
        return;
    }

    while (true) {
        sf::Joystick::update();
        InputEventToggle::updateAll();

        // code used to time an iteration
        // auto begin = std::chrono::steady_clock::now();

        send = inputManager.getData();
        recorder.update(send);

        macroCollection.activateMacros();
        if (macroCollection.isMacroActive() &&
            stopMacrosEvent->getInputValue()) {
            macroCollection.deactivateMacros();
        }
        if (macroCollection.isMacroActive()) {
            send = macroCollection.getData(send);
        }

        boost::asio::write(*port, boost::asio::buffer(send, 8));
        boost::asio::read(*port, boost::asio::buffer(recieve, 1));

        // code used to time an iteration
        // auto end = std::chrono::steady_clock::now();
        // std::cout <<
        // std::chrono::duration_cast<std::chrono::milliseconds>(end -
        // begin).count() << std::endl;
    }
}