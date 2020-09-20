#include <iostream>
#include "loader.h"
#include <sstream>
#include "ArduinoLoader.h"


int main(){
    /* Loader l = Loader();
    l.addElement("1", "1", "a");
    l.addElement("1", "2", "b");
    l.addElement("1", "3", "c");
    l.addElement("2", "1", "a");
    l.addElement("2", "2", "b");
    l.addElement("2", "3", "c");

    cout << l.getElement("1", "1") << std::endl;
    cout << l.getElement("1", "2") << std::endl;
    cout << l.getElement("1", "3") << std::endl;
    cout << l.getElement("2", "1") << std::endl;
    cout << l.getElement("2", "2") << std::endl;
    cout << l.getElement("2", "3") << std::endl;

    cout << std::endl;

    cout << l.toString(); */

    // Loader l2 = Loader("config.txt");
    // cout << l2.toString();

    /* Loader l3 = Loader();
    stringstream ss = stringstream(R"(
[1]
1=a
    )");
    l3.loadConfig(ss, true);
    cout << l3.toString(); */

    //Loader l4 = Loader("test2.txt", "test1.txt");
    // cout << l4.getElement("1", "1") << std::endl;
    // cout << l4.getElement("1", "2") << std::endl;
    // cout << l4.getElement("1", "3") << std::endl;
    // cout << l4.getElement("2", "1") << std::endl;
    // cout << l4.getElement("2", "2") << std::endl;
    // cout << l4.getElement("2", "3") << std::endl;
    // cout << l4.getElement("2", "4") << std::endl;
    // cout << l4.getElement("2", "5") << std::endl;
    // cout << l4.getElement("2", "6") << std::endl;
    // cout << l4.getElement("3", "1") << std::endl;
    // cout << l4.getElement("3", "2") << std::endl;
    // cout << l4.getElement("3", "3") << std::endl;

    //cout << l4.toString();
    
    ArduinoLoader l5 = ArduinoLoader();
    l5.loadConfigFile("config.txt");
    cout << l5.toString();

    l5 = ArduinoLoader("config.txt");
    cout << l5.toString();

    return 0;
}