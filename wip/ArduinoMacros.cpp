#include "ArduinoMacros.h"

std::vector<std::future<void>> Macros::matchImages(const cv::Mat screenshot){
    std::vector<std::future<void>> futures;

    for(auto m : macros){
        if(m->getImageProcesssingStatus() == Macro::ImageProcessingStatus::enabled){
            futures.push_back(std::async(std::launch::async, [&](){
                m->matchImage(screenshot);
            }));
        }
    }
    return futures;
}

void Macros::getData(const unsigned long long time, unsigned char data[8]){
    if(activeMacro){
        activeMacro->getDataframe(time, data);

        if (activeMacro->lastTime() < time){
            activeMacro = activeMacro->getNextMacro();
            timeSinceMacroActivation = std::chrono::steady_clock::now();
        }
    }
}

void Macros::activateMacros(){
    if(!activeMacro){
        for(auto m : macros){
            if(sf::Keyboard::isKeyPressed((sf::Keyboard::Key)m->getMacroInfo().keyboardButton)){
                activeMacro = m;
                timeSinceMacroActivation = std::chrono::steady_clock::now();
                break;
            }
        }
    }
}

