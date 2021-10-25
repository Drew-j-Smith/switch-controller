#include "pch.h"

#include "Utility/SerialPort.h"

void CreateConfig(std::string& configFilename);
void EditConfig(std::string& configFilename);
void StartController(std::string& configFilename);

const std::string options =
R"(Select one of the following options:
    1. Set config file
    2. Create config file
    3. Edit config File
    4. Run
    5. Test serial communication
    6. Exit
)";

int main(int argc, const char** argv)
{
    int option = 0;
    std::string configFilename = "data/config.json";

    while (option != 6) {
        std::cout << options;
        std::cout << "The current config file is: \"" << configFilename << "\"\n";
        std::cin >> option;
        switch (option) {
            case 1:
                std::cout << "Enter the config filename: ";
                std::getline(std::cin, configFilename);
                std::getline(std::cin, configFilename);
                break;
            case 2:
                CreateConfig(configFilename);
                break;
            case 3:
                EditConfig(configFilename);
                break;
            case 4:
                StartController(configFilename);
                break;
            case 5:
                {
                    try {
                        auto port = initializeSerialPort("COM4", 57600);
                        // sending a nuetral signal
                        unsigned char send[8] = {85, 0, 0, 128, 128, 128, 128, 8};
                        unsigned char recieve[1];

                        testSerialPort(port, 8, send, 1, recieve);
                    }
                    catch (std::exception& e) {
                        std::cerr << "Failure connecting via serial port.\n";
                    }
                }
            case 6:
                break;
            default:
                std::cout << "Unkown option: " << option << "\n";
        }
    }

    return 0;
}