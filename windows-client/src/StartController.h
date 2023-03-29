#pragma once

#include <SFML/Window/Joystick.hpp>
#include <SFML/Window/Keyboard.hpp>

#include "MacroConfig.h"
#include "MacroRecorder.h"
#include "SerialPort.h"


#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>
#include <boost/program_options.hpp>

#include "Controller.h"
#include "MacroCollection.h"

void StartController(boost::program_options::variables_map vm) {
    spdlog::info("initializing");

    // Updating joysticks to see if they are connected
    sf::Joystick::update();
    auto joystick_connected = sf::Joystick::isConnected(0);
    if (!joystick_connected) {
        spdlog::error("no joystick connected");
        return;
    }
    spdlog::info("joystick connected");

    spdlog::info("intializing opencv video capture");

    auto cap = std::make_shared<cv::VideoCapture>();
    int deviceID = vm["deviceIndex"].as<int>();
    int apiID = vm["videoCaptureBackend"].as<int>();
    cap->open(deviceID, apiID);
    if (!cap->isOpened()) {
        spdlog::error("Unable to open video capture");
        return;
    }
    cap->set(cv::CAP_PROP_FRAME_WIDTH, 1920);
    cap->set(cv::CAP_PROP_FRAME_HEIGHT, 1080);

    spdlog::info("opencv video capture intialized");
    spdlog::info("loading input config");
    Controller controller(vm);
    auto modifierButton =
        static_cast<unsigned int>(vm["controls.modifier"].as<int>());
    auto stopMacros = [modifierButton,
                       stopButton = static_cast<unsigned int>(
                           vm["controls.stopMacros"].as<int>())] {
        return sf::Joystick::isButtonPressed(0, stopButton) &&
               sf::Joystick::isButtonPressed(0, modifierButton);
    };
    MacroRecorder macroRecorder{
        [modifierButton, recordBtn = static_cast<unsigned int>(
                             vm["controls.record"].as<int>())] {
            return sf::Joystick::isButtonPressed(0, recordBtn) &&
                   sf::Joystick::isButtonPressed(0, modifierButton);
        },
        [modifierButton, playBtn = static_cast<unsigned int>(
                             vm["controls.playLastRecorded"].as<int>())] {
            return sf::Joystick::isButtonPressed(0, playBtn) &&
                   sf::Joystick::isButtonPressed(0, modifierButton);
        }};
    spdlog::info("input config loaded");
    spdlog::info("loading macros");
    MacroCollection macroCollection =
        getMacroConfig(nullptr, {macroRecorder.getLastRecordedMacro()});
    spdlog::info("macros loaded");

    std::unique_ptr<boost::asio::serial_port> port;
    boost::asio::io_service io;
    port = initializeSerialPort(vm["port"].as<std::string>(), 57600, io);
    testSerialPort(port, io);

    std::array<uint8_t, 8> send;
    unsigned char recieve[1];

    spdlog::info("Ready");

    while (true) {
        sf::Joystick::update();

        // code used to time an iteration
        // auto begin = std::chrono::steady_clock::now();

        send = controller.getData();
        macroRecorder.update(send);

        if (stopMacros()) {
            macroCollection.deactivateMacros();
        }
        macroCollection.activateMacros();
        if (macroCollection.isMacroActive()) {
            constexpr auto mergeFunction =
                []([[maybe_unused]] std::array<uint8_t, 8> action1,
                   std::array<uint8_t, 8> action2) { return action2; };
            send = macroCollection.getData(send, mergeFunction);
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