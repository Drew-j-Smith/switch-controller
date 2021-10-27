
#include "pch.h"

#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Audio/SoundRecorder.hpp>
#include "AudioFile.h"

#include "Utility/CircularBufferRecorder.h"
#include "Utility/TimeString.h"

const std::string options =
R"(Select one of the following options:
    1. isAvailable
    2. getAvailableDevices
    3. getDeviceName
    4. setDevice
    5. getSampleRate
    6. Run 1 time
    7. Run 5 times
    8. Exit
)";

int main() {
    int ans = 0;
    CircularBufferRecorder recorder(44410);

    while (ans < 8) {
        std::cout << options;
        std::cout << "Enter option: ";
        std::cin >> ans;
        switch (ans)
        {
        case 1:
            std::cout << "Is availible: " << recorder.isAvailable() << std::endl;
            break;
        case 5:
            std::cout << "Sample rate: " << recorder.getSampleRate() << std::endl;
            break;
        case 3:
            std::cout << "Device Name: \"" << recorder.getDevice() << "\"\n";
            break;
        case 2:
            {
                std::cout << "Devices availible\n";
                for (auto& s : recorder.getAvailableDevices()) {
                    std::cout << "Device: \"" << s << "\"\n";
                }
                break;
            }
        case 4:
            {
                std::cout << "Enter the device name: ";
                std::string device;
                std::getline(std::cin, device);
                std::getline(std::cin, device);
                if (recorder.setDevice(device)) {
                    std::cout << "Device name switched\n";
                }
                else {
                    std::cout << "Device name not switched\n";
                }
                break;
            }

        case 6:
            {
                std::cout << "Initalizing...\n";
                recorder.start();
                std::this_thread::sleep_for(std::chrono::milliseconds(2000));
                std::cout << "Recording...\n";
                std::vector<sf::Int16> samples(recorder.getBufferSize());
                recorder.getSamples(samples);
                sf::SoundBuffer b;
                b.loadFromSamples(&samples[0], samples.size(), recorder.getChannelCount(), recorder.getSampleRate());
                b.saveToFile(getTimeStr() + ".wav");
                std::cout << "Cleaning up...\n";
                recorder.stop();
                break;
            }
        case 7:
            {
                std::cout << "Initalizing...\n";
                recorder.start();
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                std::cout << "Recording...\n";
                std::vector<sf::Int16> samples(recorder.getBufferSize());
                for (int i = 0; i < 5; i++) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(200));
                    recorder.getSamples(samples);
                    sf::SoundBuffer b;
                    b.loadFromSamples(&samples[0], samples.size(), recorder.getChannelCount(), recorder.getSampleRate());
                    b.saveToFile(getTimeStr() + "-" + std::to_string(i) + ".wav");
                }
                std::cout << "Cleaning up...\n";
                recorder.stop();
                break;
            }
        }
    }
}