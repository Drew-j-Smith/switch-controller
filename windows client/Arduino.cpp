#include "pch.h"
#include <SFML/Graphics.hpp>

#include <iostream>
#include <thread>
#include "VirtualController.h"
#include "Loader.h"
#include "ImgProc.h"

Loader l;
ImgProc i;
VirtualController v;

void updateThread() {
	while (i.update()) {
		v.updateImgMatch(i.getImgMatch());
	};
}

int main()
{
	l = Loader("config.txt");
	i = ImgProc(l);
	v = VirtualController(l, i);

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


