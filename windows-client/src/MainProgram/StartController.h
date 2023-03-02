#pragma once

#include <SFML/Window/Joystick.hpp>
#include <SFML/Window/Keyboard.hpp>

#include "InputConfig.h"
#include "MacroConfig.h"
#include "SerialPort.h"

#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>
#include <boost/program_options.hpp>

#include "Event/InputCollection.h"
#include "Macro/MacroCollection.h"

void StartController(boost::program_options::variables_map vm,
                     const std::map<std::string, std::string> &ffmpegOptions) {
    spdlog::info("initializing");

    // Updating joysticks to see if they are connected
    sf::Joystick::update();
    auto joystick_connected = sf::Joystick::isConnected(0);
    if (!joystick_connected) {
        spdlog::error("no joystick connected");
    }
    spdlog::info("joystick connected");

    spdlog::info("intializing ffmpeg");

    std::vector<std::unique_ptr<FFmpegFrameSink>> sinks{};
    sinks.push_back(std::make_unique<VideoFrameSink>());
    AVChannelLayout channel_layout = AV_CHANNEL_LAYOUT_MONO;
    sinks.push_back(std::make_unique<AudioFrameSink>(
        channel_layout, AV_SAMPLE_FMT_S16, 48000, true, 48000));

    std::unique_ptr<FFmpegRecorder> recorder{std::make_unique<FFmpegRecorder>(
        vm["inputFormat"].as<std::string>(), vm["deviceName"].as<std::string>(),
        ffmpegOptions, sinks)};

    spdlog::info("ffmpeg intialized");
    spdlog::info("loading input config");
    InputConfig inputConfig;
    spdlog::info("input config loaded");
    spdlog::info("loading macros");
    MacroCollection macroCollection =
        getMacroConfig(dynamic_cast<VideoFrameSink *>(sinks[0].get()),
                       dynamic_cast<AudioFrameSink *>(sinks[0].get()),
                       {inputConfig.macroRecorder.getLastRecordedMacro()});
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

        send = inputConfig.inputCollection.getData();
        inputConfig.macroRecorder.update(send);

        if (inputConfig.stopMacros()) {
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