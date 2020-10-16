#ifndef VIRTUAL_CONTROLLER_H
#define VIRTUAL_CONTROLLER_H

#include <SFML/Graphics.hpp>

#include <boost/asio.hpp>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include "ArduinoStructs.h"
#include "ImgProc.h"

#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <iomanip>
#include <atomic>
#include <memory>
#include <chrono>
#include <ctime>

#include <thread>


using namespace std;


const bool verbose = false;
const bool displayNextMacro = true;
const int minDelayMS = 16;
const bool enableKeyboardInput = true;



class VirtualController
{
public:
	VirtualController();
	VirtualController(vector<cv::Mat> pictures, vector<Macro> macros, SwitchButtons switchButtons, string serialPort, string macroFolder);
	
	void update();
	void setNuetral();

	char* getData();
	void setData(char data, int byte);

	int activateMacro(unsigned int index);
	void stopMacros();
	bool isMacroActive();

	void updateImgMatch(std::vector<bool> newData);
private:
	void getDataFromKeyboard(char* data);
	void getDatafromMacro(char* data);
	void recordMacro();

	int cycleMacros(vector<int>& macroList);

	std::shared_ptr<boost::asio::serial_port> port;

	sf::Clock clockSinceLastUpdate;

	vector<cv::Mat> pictures;
	vector<Macro> macros;
	SwitchButtons switchButtons;

	string macroFolder;

	int currentMacro;
	int currentMarcoLine;
	bool macrosActive;

	bool isMacroRecordingActive;
	std::ofstream outfile;

	thread thread1;
	thread thread2;

	static void write(std::shared_ptr<boost::asio::serial_port> port, char* data){
		char* datacpy[8];
		memcpy(datacpy, data, 8 * sizeof(char));
		boost::asio::write(*port, boost::asio::buffer(datacpy, 8));
	}

	static void read(std::shared_ptr<boost::asio::serial_port> port, char* data){
		boost::asio::read(*port, boost::asio::buffer(data, 8));
	}

	std::vector<std::unique_ptr<std::atomic<bool>>> imgMatch;

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

VirtualController::VirtualController(vector<cv::Mat> pictures, vector<Macro> macros, SwitchButtons switchButtons, string serialPort, string macroFolder){

	this->pictures = pictures;
	this->macros = macros;
	this->switchButtons = switchButtons;
	this->macroFolder = macroFolder;

	try{
		boost::asio::io_service io;
		port = std::shared_ptr<boost::asio::serial_port>(new boost::asio::serial_port(io));
		port->open(serialPort);
		port->set_option(boost::asio::serial_port_base::baud_rate(9600));
	}
	catch(const std::exception& e){
		std::cerr << e.what() << '\n';
		std::cerr << "Fatal error opening serial port" << std::endl;
		exit(-1);
	}
	

	
	if (verbose)
		std::cout << "Serial port connected" << std::endl;


	clockSinceLastUpdate = sf::Clock();

	macrosActive = false;
	isMacroRecordingActive = false;

	imgMatch.resize(macros.size());

	for(unsigned int i = 0; i < imgMatch.size(); i++){
		imgMatch[i] = std::unique_ptr<std::atomic<bool>>(new std::atomic<bool>(false));
	}

	setNuetral();
}

void VirtualController::update() {
	
	if (verbose)
		std::cout << "Time since last update: " << clockSinceLastUpdate.getElapsedTime().asMilliseconds() << "ms\n";

	if (clockSinceLastUpdate.getElapsedTime().asMilliseconds() > minDelayMS)
		std::cerr << "Warning, running behind " << clockSinceLastUpdate.getElapsedTime().asMilliseconds() - minDelayMS << "ms\n";

	while (clockSinceLastUpdate.getElapsedTime().asMilliseconds() < minDelayMS);

	if (verbose)
		std::cout << "Time waited: " << clockSinceLastUpdate.getElapsedTime().asMilliseconds() << "ms\n";

	clockSinceLastUpdate.restart();

	if (!macrosActive && !isMacroRecordingActive) {
		if (sf::Keyboard::isKeyPressed((sf::Keyboard::Key)switchButtons.recordMacro)) {
			outfile.open(/*todo loader.getMacroFolder() +  loader.getMacroRecordingFilename()*/"temp", std::ios::out);
			isMacroRecordingActive = true;
		}
		else {
			for (unsigned int i = 0; i < macros.size(); i++) {
				if (sf::Keyboard::isKeyPressed((sf::Keyboard::Key)macros[i].button)) {
					activateMacro(i);
				}
			}
		}
	}

	if (sf::Keyboard::isKeyPressed((sf::Keyboard::Key)switchButtons.recordMacro)) {
		outfile.close();
		isMacroRecordingActive = false;
	}

	// if (sf::Keyboard::isKeyPressed(loader.getKeyCode(28))) { todo
	// 	cv::Mat img;
	// 	imgProc.screenshot(img);
	// 	imgProc.saveImg(img, loader.getPictureFolder() + loader.getPictureRecordingFilename());
	// }

	if (macrosActive && !isMacroRecordingActive) {
		getDatafromMacro(data);
		if (verbose)
			std::cout << "Playing macro\n";
	}
	else if (enableKeyboardInput) {
		getDataFromKeyboard(data);
		if (verbose)
			std::cout << "Taking keyboard input\n";
	}
	else{
		setNuetral();
	}

	if (isMacroRecordingActive) {
		recordMacro();
		if (verbose)
			std::cout << "Recording\n";
	}
	

	if(thread1.joinable())
		thread1.join();

	if(thread2.joinable())
		thread2.join();

	thread1 = thread(VirtualController::read, port, inputBuffer);

	thread2 = thread(VirtualController::write, port, data);


	
	

	if (verbose) {
		std::cout << "\n\nbytes sent and recieved\n";
		for (int i = 0; i < 8; i++) {
			std::cout << int(data[i]) << " " << int(inputBuffer[i]) << "\n";
		}
		std::cout << "\n\n";
	}


}

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

void VirtualController::getDataFromKeyboard(char* data) {
	data[0] = 85;

	data[1] = 0;
	data[2] = 0;
	if(sf::Keyboard::isKeyPressed((sf::Keyboard::Key)switchButtons.y      ))data[1] |= (1 << 0);
	if(sf::Keyboard::isKeyPressed((sf::Keyboard::Key)switchButtons.b      ))data[1] |= (1 << 1);
	if(sf::Keyboard::isKeyPressed((sf::Keyboard::Key)switchButtons.a      ))data[1] |= (1 << 2);
	if(sf::Keyboard::isKeyPressed((sf::Keyboard::Key)switchButtons.x      ))data[1] |= (1 << 3);
	if(sf::Keyboard::isKeyPressed((sf::Keyboard::Key)switchButtons.l      ))data[1] |= (1 << 4);
	if(sf::Keyboard::isKeyPressed((sf::Keyboard::Key)switchButtons.r      ))data[1] |= (1 << 5);
	if(sf::Keyboard::isKeyPressed((sf::Keyboard::Key)switchButtons.xl     ))data[1] |= (1 << 6);
	if(sf::Keyboard::isKeyPressed((sf::Keyboard::Key)switchButtons.xr     ))data[2] -= 128;
	if(sf::Keyboard::isKeyPressed((sf::Keyboard::Key)switchButtons.select ))data[2] |= (1 << 0);
	if(sf::Keyboard::isKeyPressed((sf::Keyboard::Key)switchButtons.start  ))data[2] |= (1 << 1);
	if(sf::Keyboard::isKeyPressed((sf::Keyboard::Key)switchButtons.lClick ))data[2] |= (1 << 2);
	if(sf::Keyboard::isKeyPressed((sf::Keyboard::Key)switchButtons.rRlick ))data[2] |= (1 << 3);
	if(sf::Keyboard::isKeyPressed((sf::Keyboard::Key)switchButtons.home   ))data[2] |= (1 << 4);
	if(sf::Keyboard::isKeyPressed((sf::Keyboard::Key)switchButtons.capture))data[2] |= (1 << 5);

	//control sticks
	data[3] = sf::Keyboard::isKeyPressed((sf::Keyboard::Key)switchButtons.leftStickXplus  ) * 127 + 
			  sf::Keyboard::isKeyPressed((sf::Keyboard::Key)switchButtons.leftStickXminus ) * 128 + 128;
	data[4] = sf::Keyboard::isKeyPressed((sf::Keyboard::Key)switchButtons.leftStickYminus ) * 127 + 
	          sf::Keyboard::isKeyPressed((sf::Keyboard::Key)switchButtons.leftStickYplus  ) * 128 + 128;
	data[5] = sf::Keyboard::isKeyPressed((sf::Keyboard::Key)switchButtons.rightStickXplus ) * 127 + 
			  sf::Keyboard::isKeyPressed((sf::Keyboard::Key)switchButtons.rightStickXminus) * 128 + 128;
	data[6] = sf::Keyboard::isKeyPressed((sf::Keyboard::Key)switchButtons.rightStickYminus) * 127 + 
			  sf::Keyboard::isKeyPressed((sf::Keyboard::Key)switchButtons.rightStickYplus ) * 128 + 128;

	data[7] = 8;
	if (sf::Keyboard::isKeyPressed((sf::Keyboard::Key)switchButtons.dpadUp) && 
		!sf::Keyboard::isKeyPressed((sf::Keyboard::Key)switchButtons.dpadDown)){
		data[7] = 0; //up
	}
	if (sf::Keyboard::isKeyPressed((sf::Keyboard::Key)switchButtons.dpadDown) &&
		!sf::Keyboard::isKeyPressed((sf::Keyboard::Key)switchButtons.dpadUp)){
		data[7] = 4; //down
	}
	if (sf::Keyboard::isKeyPressed((sf::Keyboard::Key)switchButtons.dpadRight) &&
		!sf::Keyboard::isKeyPressed((sf::Keyboard::Key)switchButtons.dpadLeft)){
		if(data[7] == 0) data[7] = 1; //up right
		if(data[7] == 8) data[7] = 2; //right
		if(data[7] == 4) data[7] = 3; //down right
	}
	if (sf::Keyboard::isKeyPressed((sf::Keyboard::Key)switchButtons.dpadLeft) &&
		!sf::Keyboard::isKeyPressed((sf::Keyboard::Key)switchButtons.dpadRight)){
		if(data[7] == 4) data[7] = 5; //down left
		if(data[7] == 8) data[7] = 6; //left
		if(data[7] == 0) data[7] = 7; //up left
	}

};

void VirtualController::getDatafromMacro(char* data) {
	memcpy(data, &macros[currentMacro].data[currentMarcoLine], sizeof(char) * 8);
	data[0] = 85;
	currentMarcoLine++;
	if (currentMarcoLine == macros[currentMacro].data.size()) {
		macrosActive = false;
		if (macros[currentMacro].enableImgProc) {
			
			if (imgMatch[currentMacro]->load()) {
				if (macros[currentMacro].macroSuccessList.size() == 0)
					return;
				if (displayNextMacro)
					std::cout << "Activating macro: " << macros[macros[currentMacro].macroSuccessList[0]].name << "\n";
				activateMacro(cycleMacros(macros[currentMacro].macroSuccessList));
			}
			else {
				if (macros[currentMacro].macroFailList.size() == 0)
					return;
				if (displayNextMacro)
					std::cout << "Activating macro: " << macros[macros[currentMacro].macroFailList[0]].name << "\n";
				activateMacro(cycleMacros(macros[currentMacro].macroFailList));
			}
		}
		else {
			if (macros[currentMacro].macroDefaultList.size() == 0)
				return;
			if (displayNextMacro)
				std::cout << "Activating macro: " << macros[macros[currentMacro].macroDefaultList[0]].name << "\n";
			activateMacro(cycleMacros(macros[currentMacro].macroDefaultList));
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


int VirtualController::activateMacro(unsigned int index) {
	if (index < 0 || index >= macros.size() || macrosActive || isMacroRecordingActive)
		return 0;
	macrosActive = true;
	currentMacro = index;
	currentMarcoLine = 0;
	return 1;
}

void VirtualController::stopMacros() {
	macrosActive = false;
}


void VirtualController::updateImgMatch(std::vector<bool> newData) {
	for (unsigned int i = 0; i < macros.size(); i++) {
		imgMatch[i]->store(newData[i]);
	}
}

bool VirtualController::isMacroActive() {
	return macrosActive;
}

int VirtualController::cycleMacros(vector<int>& macroList){
	int temp = macroList[0];
	for (unsigned int i = 0; i < macroList.size() - 1; i++) {
		macroList[i] = macroList[i + 1];
	}
	macroList[macroList.size() - 1] = temp;
	return temp;
}

#endif
