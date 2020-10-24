// #include "pch.h" // for visual studio

#include <SFML/Graphics.hpp>

#include <iostream>
#include <thread>
#include "VirtualController.h"
#include "ImgProc.h"
#include "ArduinoLoader.h"


int main(){
	ArduinoLoader l("config.json");
	ImgProc i(l.getPictures(), l.getMacros(), l.getOption("window name"), stoi(l.getOption("window width")), stoi(l.getOption("window height")));
	VirtualController v(l.getPictures(), l.getMacros(), l.getSwitchButtons(), l.getOption("serial port"), l.getOption("macro folder"));

	std::thread th1([&](){
		while (i.update()) {
			v.updateImgMatch(i.getImgMatch());
		};
	});

	std::cout << "Ready\n";

	while (true) {
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape) && v.isMacroActive()){
			v.stopMacros();
			std::cout << "Stopping macros\n";
		}
		v.update();
	}
}


