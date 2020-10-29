// #include "pch.h" // for visual studio

#include <SFML/Window/Keyboard.hpp>

#include "pch.h"
#include "VirtualController.h"
#include "ImgProc.h"
#include "ArduinoLoader.h"


int main(int argc, char **argv){

	std::string config = "config.json";

	if(argc > 1){
		config = std::string(argv[2]);
		std::cout << "Using config \"" << config << "\"\n";
	}
		

	ArduinoLoader l(config);
	ImgProc i(l.getPictures(), l.getMacros(), l.getOption("window name"), stoi(l.getOption("window width")), 
		stoi(l.getOption("window height")), l.getOption("picture folder"), l.getSwitchButtons().screenshot);
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


