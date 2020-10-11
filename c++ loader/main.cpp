#include <SFML/Graphics.hpp>

#include <iostream>
#include "ArduinoLoader.h"

using namespace std;

int main(){
    ArduinoLoader l5 = ArduinoLoader();
    l5.loadConfig("config.json");
    cout << l5.toString();

    return 0;
}