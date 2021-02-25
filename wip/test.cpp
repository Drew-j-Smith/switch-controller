
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Joystick.hpp>

#include "pch.h"

#include "CharStream.h"

int main(){
    CharStream<1> a;
    std::array<unsigned char, 1> b = {'a'};
    a.push_back(b);
    std::cout << a[0][0] << std::endl;



    return 0;
}