
#include <SFML/Window/Joystick.hpp>
#include <SFML/Window/Keyboard.hpp>

#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>

#include "pch.h"

#include "MainProgram.h"

#include "Decider/DeciderCollection.h"
#include "Event/Utility/InputCollection.h"
#include "Macro/MacroCollection.h"
#include "Utility/SerialPort.h"

#include "FFmpeg/AudioFrameSink.h"
#include "FFmpeg/FFmpegRecorder.h"
#include "FFmpeg/VideoFrameSink.h"

void initializeGameCapture(std::shared_ptr<FFmpegRecorder> &recorder,
                           std::shared_ptr<VideoFrameSink> &videoSink,
                           std::shared_ptr<AudioFrameSink> &audioSink);

void getConfig(std::string &serialPortName,
               std::map<std::string, std::shared_ptr<Event>> &events,
               std::vector<std::shared_ptr<Event>> &createdEvents,
               std::vector<std::shared_ptr<Decider>> &deciders,
               std::vector<std::shared_ptr<Macro>> &macros,
               std::shared_ptr<VideoFrameSink> videoSink,
               std::shared_ptr<AudioFrameSink> audioSink);

void StartController() {
    std::cout << "Initializing...\n";
    std::cout << "Loading config files.\n";

    // Updating joysticks to see if they are connected
    sf::Joystick::update();

    std::shared_ptr<FFmpegRecorder> recorder;
    std::shared_ptr<VideoFrameSink> videoSink;
    std::shared_ptr<AudioFrameSink> audioSink;
    initializeGameCapture(recorder, videoSink, audioSink);

    std::string serialPortName;
    std::map<std::string, std::shared_ptr<Event>> events;
    std::vector<std::shared_ptr<Event>> createdEvents;
    std::vector<std::shared_ptr<Decider>> deciders;
    std::vector<std::shared_ptr<Macro>> macros;
    getConfig(serialPortName, events, createdEvents, deciders, macros,
              videoSink, audioSink);

    InputCollection inputCollection(events, createdEvents);

    DeciderCollection deciderCollection(deciders);
    MacroCollection macroCollection(macros);

    macroCollection.pushBackMacro(
        inputCollection.getRecorder()->getLastRecordedMacro());

    std::cout << "Config loaded.\n";

    std::unique_ptr<boost::asio::serial_port> port;
    boost::asio::io_service io;

    // sending a nuetral signal
    std::array<uint8_t, 8> send = {85, 0, 0, 128, 128, 128, 128, 8};
    unsigned char recieve[1];

    port = initializeSerialPort(serialPortName, 57600, &io);
    testSerialPort(port, 8, send.data(), 1, recieve, &io);

    while (true) {
        sf::Joystick::update();

        inputCollection.update();
        deciderCollection.update();

        // code used to time an iteration
        // auto begin = std::chrono::steady_clock::now();

        send = inputCollection.getData();

        macroCollection.activateMacros();
        if (macroCollection.isMacroActive() &&
            inputCollection.getStopEventValue()) {
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