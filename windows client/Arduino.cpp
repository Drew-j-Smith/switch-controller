// #include "pch.h" // for visual studio

#include <SFML/Graphics.hpp>

#include <iostream>
#include <thread>
#include "VirtualController.h"
#include "ImgProc.h"
#include "ArduinoLoader.h"

ImgProc i;
VirtualController v;

void updateThread() {
	while (i.update()) {
		v.updateImgMatch(i.getImgMatch());
	};
}

int main()
{
	ArduinoLoader l("config.json");
	i = ImgProc(l.getPictures(), l.getMacros(), l.getOption("window name"), stoi(l.getOption("window width")), stoi(l.getOption("window height")));
	v = VirtualController(l.getPictures(), l.getMacros(), l.getSwitchButtons(), l.getOption("serial port"));

	std::thread th1(updateThread);

	std::cout << "Ready\n";

	while (true) {
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape) && v.isMcrActv()){
			v.stopMacros();
			std::cout << "Stopping macros\n";
		}
		v.update();
	}
}


