#include "pch.h"

#include "Utility/SerialInterface.h"

void CreateConfig(std::string& configFilename);
void EditConfig(std::string& configFilename);
void StartController(std::string& configFilename);

void TestSerialCom() {
    std::cout << "Intializing serial communication.\n";
    SerialInterface s("COM4", 57600, 8, 1);
    std::cout << "Serial communication intialized, testing connection... (hangs if connection cannot be established)\n";

    // sending a nuetral signal
    unsigned char send[8] = {85, 0, 0, 128, 128, 128, 128, 8};
    unsigned char recieve[1];

    s.sendData(send, recieve);
    std::cout << "Serial communication established.\n";
}

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
    std::string configFilename = "config.json";

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
                TestSerialCom();
            case 6:
                break;
            default:
                std::cout << "Unkown option: " << option << "\n";
        }
    }

    return 0;
}