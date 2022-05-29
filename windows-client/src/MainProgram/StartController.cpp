
#include <SFML/Window/Joystick.hpp>
#include <SFML/Window/Keyboard.hpp>

#include "MainProgram.h"

#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>

#include "Event/Utility/InputCollection.h"
#include "Macro/MacroCollection.h"

void StartController() {
    std::cout << "Initializing...\n";
    std::cout << "Loading config files.\n";

    // Updating joysticks to see if they are connected
    sf::Joystick::update();

    auto [videoSink, audioSink, recorder, sinks] = initializeGameCapture();

    auto [serialPortName, stopMacros, inputCollection, macroRecorder,
          macroCollection] = getConfig(videoSink, audioSink);

    std::cout << "Config loaded.\n";

    std::unique_ptr<boost::asio::serial_port> port;
    boost::asio::io_service io;

    // sending a nuetral signal
    std::array<uint8_t, 8> send = {85, 0, 0, 128, 128, 128, 128, 8};
    unsigned char recieve[1];

    port = initializeSerialPort(serialPortName, 57600, &io);
    testSerialPort(port, 8, send.data(), 1, recieve, &io);

    std::cout << "Ready.\n";

    while (true) {
        sf::Joystick::update();

        // code used to time an iteration
        // auto begin = std::chrono::steady_clock::now();

        send = inputCollection.getData();
        macroRecorder.update(send);

        if (stopMacros()) {
            macroCollection.deactivateMacros();
        }
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