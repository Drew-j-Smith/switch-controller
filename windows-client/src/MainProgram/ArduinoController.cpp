
#include "SerialPort.h"
#include "StartController.h"
#include "TestAudio.h"
#include "TestVideo.h"

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

    cv::utils::logging::setLogLevel(
        cv::utils::logging::LogLevel::LOG_LEVEL_SILENT);

    int option = 0;

    while (option != 5) {
        std::cout << options;
        std::cin >> option;
        std::string tempstr;
        std::getline(std::cin, tempstr);
        try {
            switch (option) {
            case 1: {
                StartController();

            } break;
            case 2: {
                std::cout << "Enter the port name:\n";
                std::string portName;
                std::getline(std::cin, portName);
                boost::asio::io_service io;
                auto port = initializeSerialPort(portName, 57600, io);
                testSerialPort(port, io);
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
                spdlog::warn("Unkown option: {}", option);
            }
        } catch (std::exception &e) {
            spdlog::error("Uncaught exception: {}", e.what());
        }
    }

    return 0;
}