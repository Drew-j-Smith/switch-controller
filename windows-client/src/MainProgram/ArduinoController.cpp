#include "MainProgram.h"

#include "SFML/SFMLRenderer.h"

#include <opencv2/core/utils/logger.hpp>

const std::string options =
    R"(Select one of the following options:
    1. Run
    2. Test serial communication
    3. Test FFmpeg video capture
    4. Test FFmpeg audio capture
    5. Exit
)";

int main() {

    // SFMLRenderer renderer;
    // renderer.start();

    cv::utils::logging::setLogLevel(
        cv::utils::logging::LogLevel::LOG_LEVEL_SILENT);

    int option = 0;

    while (option != 5) {
        std::cout << options;
        std::cin >> option;
        std::string tempstr;
        std::getline(std::cin, tempstr);
        switch (option) {
        case 1: {
            try {
                StartController();
            } catch (std::exception &e) {
                std::cerr << "Uncaught exception in StartController: " +
                                 std::string(e.what()) + "\n";
            }
        } break;
        case 2: {
            std::cout << "Enter the port name:\n";
            std::string portName;
            std::getline(std::cin, portName);
            try {
                boost::asio::io_service io;
                auto port = initializeSerialPort(portName, 57600, &io);

                // sending a nuetral signal
                unsigned char send[8] = {85, 0, 0, 128, 128, 128, 128, 8};
                unsigned char recieve[1];

                testSerialPort(port, 8, send, 1, recieve, &io);
            } catch (std::exception &e) {
                std::cerr << "Failure connecting via serial port.\n" +
                                 std::string(e.what()) + "\n";
            }
        } break;
        case 3: {
            TestVideo();
        } break;
        case 4: {
            TestAudio();
        } break;
        case 5:
            break;
        default:
            std::cout << "Unkown option: " << option << "\n";
        }
    }

    return 0;
}