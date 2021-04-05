#include "ArduinoMacros.h"

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
            if(m->getInputEvent()->getInputValue()){
                activeMacro = m;
                timeSinceMacroActivation = std::chrono::steady_clock::now();
                break;
            }
        }
    }
}

