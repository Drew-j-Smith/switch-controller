// #include "pch.h" // for visual studio

#include <SFML/Window/Keyboard.hpp>

#include "pch.h"
#include "VirtualController.h"
#include "ImgProc.h"
#include "ArduinoLoader.h"

extern bool VERBOSE_OUTPUT; //TODO: should probably not use global variables this way
extern bool DISPLAY_SCREEN_CAP;
extern bool DISPLAY_IMAGE_MATCH;

int main(int argc, char **argv){

	std::string config = "config.json";

	for(int i = 1; i < argc; i++){
		if(strcmp(argv[i], "-c") == 0 && argc > i){
			config = std::string(argv[++i]);
			std::cout << "Using config \"" << config << "\"\n";
		}
		else if(strcmp(argv[i], "-v") == 0){
			VERBOSE_OUTPUT = true;
		}
		else if(strcmp(argv[i], "-x") == 0){
			DISPLAY_SCREEN_CAP = true;
		}
		else if(strcmp(argv[i], "-m") == 0){
			DISPLAY_IMAGE_MATCH = true;
		}
		else{
			std::cout << "Drew Smith's Arduino Controller     \n";
			std::cout << "usage: [-c <path>] [-v] [-x] [-h]   \n";
			std::cout << "                                    \n";
			std::cout << "-c                                  \n";
			std::cout << "    specify config location.        \n";
			std::cout << "                                    \n";
			std::cout << "-v                                  \n";
			std::cout << "    set to verbose.                 \n";
			std::cout << "                                    \n";
			std::cout << "-x                                  \n";
			std::cout << "    set to display screen capture.  \n";
			std::cout << "-m                                  \n";
			std::cout << "    set to display image match.     \n";
			return 0;
		}
	}

	ArduinoLoader l(config);
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

