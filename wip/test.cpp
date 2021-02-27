
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Joystick.hpp>

#include "pch.h"

#include "CharStream.h"
#include "ArduinoMacro.h"

int main(){
    CharStream<1> a;
    std::array<unsigned char, 1> b = {'a'};
    a.push_back(b);
    //std::cout << a[0][0] << std::endl;

    a.save("test.txt", true);

    CharStream<16> c;
    c.load("test2.txt");
    //std::cout << c.size() << std::endl;
    //std::cout << std::hex << std::setfill('0');

    c.save("test3.txt");

    c.save("test4.txt", false);

    c.load("test4.txt", false);
    //c.print(std::cout);

    Macro d;
    unsigned char e[] = {1, 0, 3, 4, 5, 6, 7, 8};
    d.appendData(0, e);
    e[1] = 4;
    d.appendData(4, e);
    e[1] = 8;
    d.appendData(8, e);
    e[1] = 12;
    d.appendData((0ULL)-1, e);

    d.getDataframe(5, e);
    std::cout << d.getTime(0) << std:: endl;
    std::cout << d.getTime(1) << std:: endl;
    std::cout << d.getTime(2) << std:: endl;
    std::cout << d.getTime(3) << std:: endl;
    std::cout << d.lastTime() << std:: endl;
    for (int i = 0; i < 8; i++){
        std::cout << (int)e[i] << " ";
    }
    std::cout << std::endl;

    d.saveData("test5.txt");

    d = Macro();
    d.loadData("test5.txt");
    d.getData().print(std::cout);
    


    return 0;
}