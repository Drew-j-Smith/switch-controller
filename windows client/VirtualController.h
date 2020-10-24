#ifndef VIRTUAL_CONTROLLER_H
#define VIRTUAL_CONTROLLER_H

#include <SFML/Graphics.hpp>

#include <boost/asio/io_service.hpp>
#include <boost/asio/serial_port.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>

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


const bool VERBOSE_OUTPUT = false;
const bool OUTPUT_MACRO_ACTIVATION = true;
const int MIN_DELAY_MS = 16;
const bool ENABLE_KAYBOARD_INPUT = true;


class VirtualController
{
public:
	VirtualController(vector<cv::Mat> pictures, vector<Macro> macros, SwitchButtons switchButtons, string serialPort, string macroFolder);
	
	void update();

	int activateMacro(unsigned int index);
	void stopMacros();
	bool isMacroActive();

	void getDataFromKeyboard(char* data);
	void setNuetral(char* data);

	void updateImgMatch(std::vector<bool> newData);
private:
	void getDatafromMacro(char* data);
	void recordMacro(char* data);

	int cycleMacros(vector<int>& macroList);

	std::shared_ptr<boost::asio::serial_port> port;

	vector<cv::Mat> pictures;
	vector<Macro> macros;
	SwitchButtons switchButtons;

	char data[8];

	std::vector<std::unique_ptr<std::atomic<bool>>> imgMatch;

	std::chrono::steady_clock::time_point timeSinceUpdate;

	string macroFolder;

	int currentMacro;
	int currentMarcoLine;
	bool macrosActive;
	bool isMacroRecordingActive;
	std::ofstream outfile;

	thread readThread;
	thread writeThread;
};


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
	

	
	if (VERBOSE_OUTPUT)
		std::cout << "Serial port connected" << std::endl;


	timeSinceUpdate = std::chrono::steady_clock::now();

	macrosActive = false;
	isMacroRecordingActive = false;

	imgMatch.resize(macros.size());

	for(unsigned int i = 0; i < imgMatch.size(); i++){
		imgMatch[i] = std::unique_ptr<std::atomic<bool>>(new std::atomic<bool>(false));
	}
}

void VirtualController::update() {
	auto now = std::chrono::steady_clock::now();

	if (VERBOSE_OUTPUT)
		std::cout << "\nTime since last update: " << std::chrono::duration_cast<std::chrono::milliseconds>(now - timeSinceUpdate).count() << "ms\n";

	if (std::chrono::duration_cast<std::chrono::milliseconds>(now - timeSinceUpdate).count() > MIN_DELAY_MS)
		std::cerr << "Warning, running behind " << std::chrono::duration_cast<std::chrono::milliseconds>(now - timeSinceUpdate).count() - MIN_DELAY_MS << "ms\n";

	while (std::chrono::duration_cast<std::chrono::milliseconds>(now - timeSinceUpdate).count() < MIN_DELAY_MS)
		now = std::chrono::steady_clock::now();;

	if (VERBOSE_OUTPUT)
		std::cout << "Time waited: " << std::chrono::duration_cast<std::chrono::milliseconds>(now - timeSinceUpdate).count() << "ms\n";

	timeSinceUpdate = std::chrono::steady_clock::now();

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
		if (VERBOSE_OUTPUT)
			std::cout << "Playing macro\n";
	}
	else if (ENABLE_KAYBOARD_INPUT) {
		getDataFromKeyboard(data);
		if (VERBOSE_OUTPUT)
			std::cout << "Taking keyboard input\n";
	}
	else{
		setNuetral(data);
	}

	if (isMacroRecordingActive) {
		recordMacro(data);
		if (VERBOSE_OUTPUT)
			std::cout << "Recording\n";
	}

	if(readThread.joinable())
		readThread.join();

	if(writeThread.joinable())
		writeThread.join();

	readThread = thread([&](){
		char recievedData[8];
		boost::asio::read(*port, boost::asio::buffer(recievedData, 8));

		if (VERBOSE_OUTPUT) {
			std::cout << "bytes recieved\n";
			for (int i = 0; i < 8; i++) {
				std::cout << setw(5) << int(recievedData[i]);
			}
			std::cout << "\n";
		}
	});

	writeThread = thread([&](){
		char datacpy[8];
		memcpy(datacpy, data, 8 * sizeof(char));
		boost::asio::write(*port, boost::asio::buffer(datacpy, 8));

		if (VERBOSE_OUTPUT) {
			std::cout << "bytes sent\n";
			for (int i = 0; i < 8; i++) {
				std::cout << setw(5) << int(datacpy[i]);
			}
			std::cout << "\n";
		}
	});


}

void VirtualController::setNuetral(char* data) {
	data[0] = 85;
	data[1] = -128;
	data[2] = -128;
	data[3] = 0;
	data[4] = 0;
	data[5] = 0;
	data[6] = 0;
	data[7] = 8;
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
				if (OUTPUT_MACRO_ACTIVATION)
					std::cout << "Activating macro: " << macros[macros[currentMacro].macroSuccessList[0]].name << "\n";
				activateMacro(cycleMacros(macros[currentMacro].macroSuccessList));
			}
			else {
				if (macros[currentMacro].macroFailList.size() == 0)
					return;
				if (OUTPUT_MACRO_ACTIVATION)
					std::cout << "Activating macro: " << macros[macros[currentMacro].macroFailList[0]].name << "\n";
				activateMacro(cycleMacros(macros[currentMacro].macroFailList));
			}
		}
		else {
			if (macros[currentMacro].macroDefaultList.size() == 0)
				return;
			if (OUTPUT_MACRO_ACTIVATION)
				std::cout << "Activating macro: " << macros[macros[currentMacro].macroDefaultList[0]].name << "\n";
			activateMacro(cycleMacros(macros[currentMacro].macroDefaultList));
		}
	}
}

void VirtualController::recordMacro(char* data) {
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
