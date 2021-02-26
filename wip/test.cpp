
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Joystick.hpp>

#include "pch.h"

#include "CharStream.h"
#include "ArduinoMacro.h"

int main(){
    CharStream<1> a;
    std::array<unsigned char, 1> b = {'a'};
    a.push_back(b);
    std::cout << a[0][0] << std::endl;

    a.save("test.txt", true);

    CharStream<16> c;
    c.load("test2.txt");
    std::cout << c.size() << std::endl;
    std::cout << std::hex << std::setfill('0');
    // for (int i = 0; i < c.size(); i++) {
    //     for (int j = 0; j < 16; j++) {
    //         std::cout << std::setw(2) << (int)c.at(i).at(j);
    //     }
    //     std::cout << std::endl;
    // }
    c.save("test3.txt");

    c.save("test4.txt", false);

    c.load("test4.txt", false);
    for (int i = 0; i < c.size(); i++) {
        for (int j = 0; j < 16; j++) {
            std::cout << std::setw(2) << (int)c.at(i).at(j);
        }
        std::cout << std::endl;
    }


    return 0;
}