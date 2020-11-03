#ifndef VIRTUAL_CONTROLLER_H
#define VIRTUAL_CONTROLLER_H

#include <SFML/Window/Keyboard.hpp>

#include "ArduinoStructs.h"

#include <boost/asio/io_service.hpp>
#include <boost/asio/serial_port.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>

#include <opencv2/core.hpp>


class VirtualController
{
public:
	VirtualController(std::vector<cv::Mat> pictures, std::vector<Macro> macros, SwitchButtons switchButtons, std::string serialPort, std::string macroFolder);
	
	void update();

	int activateMacro(unsigned int index);
	void stopMacros();
	bool isMacroActive();

	void getDataFromKeyboard(unsigned char* data);
	void setNuetral(unsigned char* data);

	void updateImgMatch(std::vector<bool> newData);
private:

	void getDatafromMacro(unsigned char* data);
	void recordMacro(unsigned char* data);

	int cycleMacros(std::vector<int>& macroList);

	std::shared_ptr<boost::asio::serial_port> port;

	std::vector<cv::Mat> pictures;
	std::vector<Macro> macros;
	SwitchButtons switchButtons;

	unsigned char data[8];

	std::vector<std::unique_ptr<std::atomic<bool>>> imgMatch;

	std::chrono::steady_clock::time_point timeSinceUpdate;

	std::string macroFolder;

	int currentMacro;
	int currentMarcoLine;
	bool isMacroRecordingActive;
	bool macroRecordButtonLastUpdate;
	std::ofstream outfile;

	std::thread readThread;
};


#endif
