#ifndef VIRTUAL_CONTROLLER_H
#define VIRTUAL_CONTROLLER_H

#include <SFML/Graphics.hpp>

#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <iomanip>
#include "SerialPort.hpp"
#include "Loader.h"
#include "ImgProc.h"
#include <atomic>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>


const bool verbose = false;
const bool displayNextMacro = true;
const int minDelayMS = 16;
const bool enableKeyboardInput = true;
const int recoverCycles = 5;

template<typename T>
struct MobileAtomic
{
	std::atomic<T> atomic;

	MobileAtomic() : atomic(T()) {}

	explicit MobileAtomic(T const& v) : atomic(v) {}
	explicit MobileAtomic(std::atomic<T> const& a) : atomic(a.load()) {}

	MobileAtomic(MobileAtomic const&other) : atomic(other.atomic.load()) {}

	MobileAtomic& operator=(MobileAtomic const &other)
	{
		atomic.store(other.atomic.load());
		return *this;
	}
};
typedef MobileAtomic<bool> AtomicBool;




class VirtualController
{
public:
	VirtualController();
	VirtualController(Loader &l, ImgProc &i);
	
	bool isConnected();
	void update();
	void setNuetral();

	char* getData();
	void setData(char data, int byte);

	int activateMacro(int index);
	void stopMacros();
	bool isMcrActv();

	void updateImgMatch(std::vector<bool> newData);
private:
	void getDataFromKeyboard();
	void getDatafromMacro();
	void recordMacro();

	SerialPort *arduino;
	sf::Clock clockSinceLastUpdate;
	bool firstCycle = true;

	Loader loader;

	//std::vector<std::array<char, 8>> currentMacro;
	int currentMacro;
	int currentMarcoLine;
	bool isMacroActive;

	bool isMacroRecordingActive;
	std::ofstream outfile;

	ImgProc imgProc;

	std::vector<AtomicBool> imgMatch;

	int desyncCounter;

	char data[8];
	char inputBuffer[8];
	// byte 0 is a check byte set to 85

	// byte 1 bit 1: y
	// byte 1 bit 2: b
	// byte 1 bit 3: a
	// byte 1 bit 4: x
	// byte 1 bit 5: l
	// byte 1 bit 6: r
	// byte 1 bit 7: xl
	// byte 1 bit 8: xr

	// byte 2 bit 1: select
	// byte 2 bit 2: start
	// byte 2 bit 3: l click
	// byte 2 bit 4: r click
	// byte 2 bit 5: home
	// byte 2 bit 6: capture

	// byte 3 is left stick x
	// byte 4 is left stick y
	// byte 5 is right stick x
	// byte 6 is right stick y

	// byte 7 is HAT(d-pad)
	// 0 to 8 
	// up = 0 
	// counts clockwise 
	// 8 is nuetral
};

VirtualController::VirtualController() {

}

VirtualController::VirtualController(Loader &l, ImgProc &i){

	loader = l;
	imgProc = i;
	arduino = new SerialPort(loader.getCOM_Port().c_str());
	
	if (verbose)
		std::cout << "COM port connected: " << arduino->isConnected() << std::endl;


	clockSinceLastUpdate = sf::Clock();

	isMacroActive = false;
	isMacroRecordingActive = false;
	desyncCounter = 0;

	imgMatch.resize(loader.getNumMacros());

	setNuetral();
}

void VirtualController::update() {
	//std::cout << "thread 1:" << imgProc.getImgMatch(2) << '\n';
	if (verbose)
		std::cout << "Time since last update: " << clockSinceLastUpdate.getElapsedTime().asMilliseconds() << "ms\n";

	if (clockSinceLastUpdate.getElapsedTime().asMilliseconds() > minDelayMS)
		std::cerr << "Warning, runnning behind " << clockSinceLastUpdate.getElapsedTime().asMilliseconds() - minDelayMS << "ms\n";

	while (clockSinceLastUpdate.getElapsedTime().asMilliseconds() < minDelayMS);

	if (verbose)
		std::cout << "Time waited: " << clockSinceLastUpdate.getElapsedTime().asMilliseconds() << "ms\n";

	clockSinceLastUpdate.restart();

	if (!isMacroActive && !isMacroRecordingActive) {
		if (sf::Keyboard::isKeyPressed(loader.getKeyCode(26))) {
			outfile.open(loader.getMacroFolder() + loader.getMacroRecordingFilename(), std::ios::out);
			isMacroRecordingActive = true;
		}
		else {
			for (int i = 0; i < loader.getNumMacros(); i++) {
				if (sf::Keyboard::isKeyPressed(loader.getMacroKeyCode(i))) {
					activateMacro(i);
				}
			}
		}
	}

	if (sf::Keyboard::isKeyPressed(loader.getKeyCode(27))) {
		outfile.close();
		isMacroRecordingActive = false;
	}

	if (sf::Keyboard::isKeyPressed(loader.getKeyCode(28))) {
		cv::Mat img;
		imgProc.screenshot(img);
		imgProc.saveImg(img, loader.getPictureFolder() + loader.getPictureRecordingFilename());
	}


	if (isMacroActive && !isMacroRecordingActive) {
		getDatafromMacro();
		if (verbose)
			std::cout << "Playing macro\n";
	}
	else if (enableKeyboardInput) {
		getDataFromKeyboard();
		if (verbose)
			std::cout << "Taking keyboard input\n";
	}

	if (isMacroRecordingActive) {
		recordMacro();
		if (verbose)
			std::cout << "Recording\n";
	}
	

	


	if (arduino->isConnected()) {
 
		if (!arduino->writeSerialPort(data, 8))
			std::cerr << "Data was not written\n";

		if (desyncCounter >=  recoverCycles) {
			std::cerr << "Attempting to compensate\n";
			desyncCounter = 0;
			if (arduino->bytesAvailible() > 8)
				if (!arduino->readSerialPort(inputBuffer, arduino->bytesAvailible() - 8))
					std::cerr << "Error occured reading data\n";
			if (!arduino->readSerialPort(inputBuffer, arduino->bytesAvailible()))
				std::cerr << "Error occured reading data\n";
		}
		else if (!arduino->readSerialPort(inputBuffer, 8) && !firstCycle)
			std::cerr << "Error occured reading data\n";
		

		if (arduino->bytesAvailible() != 8 && !firstCycle) {
			std::cerr << "Warning, desync may have occured, bytes remaining: " << arduino->bytesAvailible() - 8 << "\n";
			desyncCounter++;
		}
		else
			desyncCounter = 0;

		if (verbose) {
			std::cout << "Bytes availible: " << arduino->bytesAvailible() << "\n\nbytes sent and recieved\n";
			for (int i = 0; i < 8; i++) {
				std::cout << int(data[i]) << " " << int(inputBuffer[i]) << "\n";
			}
			std::cout << "\n\n";
		}

		firstCycle = false;
	}
}

bool VirtualController::isConnected() {
	return arduino->isConnected();
};

void VirtualController::setNuetral() {
	data[0] = 85;
	data[1] = -128;
	data[2] = -128;
	data[3] = 0;
	data[4] = 0;
	data[5] = 0;
	data[6] = 0;
	data[7] = 8;
};

char* VirtualController::getData() {
	return data;
}

void VirtualController::setData(char data, int byte) {
	this->data[byte] = data;
}

void VirtualController::getDataFromKeyboard() {
	data[0] = 85;

	//buttons
	data[1] = 0;
	for (int i = 0; i < 7; i++) {
		if (sf::Keyboard::isKeyPressed(loader.getKeyCode(i))) {
			data[1] |= (1 << i);
		}
	}
	if (sf::Keyboard::isKeyPressed(loader.getKeyCode(7))) {
		data[1] -= 128;
	}

	data[2] = 0;
	for (int i = 0; i < 6; i++) {
		if (sf::Keyboard::isKeyPressed(loader.getKeyCode(8 + i))) {
			data[2] |= (1 << i);
		}
	}

	//control sticks
	for (int i = 0; i < 4; i++) {
		data[3 + i] = sf::Keyboard::isKeyPressed(loader.getKeyCode(14 + 2 * i)) * 127
			+ sf::Keyboard::isKeyPressed(loader.getKeyCode(15 + 2 * i)) * 128 + 128;
	}

	//dpad
	if (sf::Keyboard::isKeyPressed(loader.getKeyCode(22)) &&
		sf::Keyboard::isKeyPressed(loader.getKeyCode(24)) ||
		sf::Keyboard::isKeyPressed(loader.getKeyCode(23)) &&
		sf::Keyboard::isKeyPressed(loader.getKeyCode(25))) {
		data[7] = 8;
		return;
	}

	if (sf::Keyboard::isKeyPressed(loader.getKeyCode(22)) &&
		sf::Keyboard::isKeyPressed(loader.getKeyCode(23))) {
		data[7] = 1;
	}
	if (sf::Keyboard::isKeyPressed(loader.getKeyCode(23)) &&
		sf::Keyboard::isKeyPressed(loader.getKeyCode(24))) {
		data[7] = 3;
	}
	if (sf::Keyboard::isKeyPressed(loader.getKeyCode(23)) &&
		sf::Keyboard::isKeyPressed(loader.getKeyCode(24))) {
		data[7] = 5;
	}
	if (sf::Keyboard::isKeyPressed(loader.getKeyCode(24)) &&
		sf::Keyboard::isKeyPressed(loader.getKeyCode(25))) {
		data[7] = 7;
	}

	if (sf::Keyboard::isKeyPressed(loader.getKeyCode(22))) {
		data[7] = 0;
	}
	else if (sf::Keyboard::isKeyPressed(loader.getKeyCode(23))) {
		data[7] = 2;
	}
	else if (sf::Keyboard::isKeyPressed(loader.getKeyCode(24))) {
		data[7] = 4;
	}
	else if (sf::Keyboard::isKeyPressed(loader.getKeyCode(25))) {
		data[7] = 6;
	}
	else {
		data[7] = 8;
	}


};

void VirtualController::getDatafromMacro() {
	memcpy(data, loader.getMacro(currentMacro)[currentMarcoLine].data(), sizeof(char) * 8);
	data[0] = 85;
	currentMarcoLine++;
	if (currentMarcoLine == loader.getMacro(currentMacro).size()) {
		isMacroActive = false;
		if (loader.getEnableMacroImgProc(currentMacro)) {
			//std::cout << imgMatch[currentMacro].atomic.load() << '\n';
			if (imgMatch[currentMacro].atomic.load()) {
				if (loader.getNextMacroImgMatch(currentMacro).length() == 0)
					return;
				if (displayNextMacro)
					std::cout << "Activating macro: " << loader.getNextMacroImgMatch(currentMacro) << "\n";
				activateMacro(loader.getMacroIndex(loader.advanceNextMacroImgMatch(currentMacro)));
			}
			else {
				if (loader.getNextMacroNoImgMatch(currentMacro).length() == 0)
					return;
				if (displayNextMacro)
					std::cout << "Activating macro: " << loader.getNextMacroNoImgMatch(currentMacro) << "\n";
				activateMacro(loader.getMacroIndex(loader.advanceNextMacroNoImgMatch(currentMacro)));
			}
		}
		else {
			if (loader.getNextMacroNoImgProc(currentMacro).length() == 0)
				return;
			if (displayNextMacro)
				std::cout << "Activating macro: " << loader.getNextMacroNoImgProc(currentMacro) << "\n";
			activateMacro(loader.getMacroIndex(loader.advanceNextMacroNoImgProc(currentMacro)));
		}
	}
}

void VirtualController::recordMacro() {
	if (outfile)
	{
		for (int i = 0; i < 8; i++) {
			std::stringstream ss;
			ss << std::setw(3) << std::setfill('0') << (int)data[i] + 128;
			outfile << ss.str();
		}
		outfile << '\n';
	}
	else
	{
		std::cerr << "Error opening file.\n";
	}
};


int VirtualController::activateMacro(int index) {
	if (index < 0 || index >= loader.getNumMacros() || isMacroActive || isMacroRecordingActive)
		return 0;
	isMacroActive = true;
	currentMacro = index;
	currentMarcoLine = 0;
	return 1;
}

void VirtualController::stopMacros() {
	isMacroActive = false;
}


void VirtualController::updateImgMatch(std::vector<bool> newData) {
	for (int i = 0; i < loader.getNumMacros(); i++) {
		imgMatch[i].atomic.store(newData[i]);
	}
}

bool VirtualController::isMcrActv() {
	return isMacroActive;
}


#endif