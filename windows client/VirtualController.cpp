#include "VirtualController.h"
#include "pch.h"

#define AC_VERBOSE_OUTPUT 0
#define AC_OUTPUT_MACRO_ACTIVATION 1
#define AC_MIN_DELAY_MS 16
#define AC_ENABLE_KAYBOARD_INPUT 1

VirtualController::VirtualController(std::vector<cv::Mat> pictures, std::vector<Macro> macros, 
SwitchButtons switchButtons, std::string serialPort, std::string macroFolder)
	: pictures(pictures), macros(macros), switchButtons(switchButtons), timeSinceUpdate(std::chrono::steady_clock::now()), 
	macroFolder(macroFolder), macrosActive(false), isMacroRecordingActive(false), macroRecordButtonLastUpdate(false)
{

	try{
		boost::asio::io_service io;
		port = std::shared_ptr<boost::asio::serial_port>(new boost::asio::serial_port(io));
		port->open(serialPort);
		port->set_option(boost::asio::serial_port_base::baud_rate(9600));
		#if AC_VERBOSE_OUTPUT == 1
			std::cout << "Serial port connected" << std::endl;
		#endif
	}
	catch(const std::exception& e){
		std::cerr << e.what() << '\n';
		std::cerr << "Fatal error opening serial port" << std::endl;
		exit(-1);
	}

	imgMatch.resize(macros.size());
	for(unsigned int i = 0; i < imgMatch.size(); i++){
		imgMatch[i] = std::unique_ptr<std::atomic<bool>>(new std::atomic<bool>(false));
	}
}

void VirtualController::update() {
	auto now = std::chrono::steady_clock::now();

	#if AC_VERBOSE_OUTPUT == 1
		std::cout << "\nTime since last update: " << std::chrono::duration_cast<std::chrono::milliseconds>(now - timeSinceUpdate).count() << "ms\n";
	#endif

	if (std::chrono::duration_cast<std::chrono::milliseconds>(now - timeSinceUpdate).count() > AC_MIN_DELAY_MS)
		std::cerr << "Warning, running behind " << std::chrono::duration_cast<std::chrono::milliseconds>(now - timeSinceUpdate).count() - AC_MIN_DELAY_MS << "ms\n";

	while (std::chrono::duration_cast<std::chrono::milliseconds>(now - timeSinceUpdate).count() < AC_MIN_DELAY_MS)
		now = std::chrono::steady_clock::now();

	#if AC_VERBOSE_OUTPUT == 1
		std::cout << "Time waited: " << std::chrono::duration_cast<std::chrono::milliseconds>(now - timeSinceUpdate).count() << "ms\n";
	#endif

	timeSinceUpdate = std::chrono::steady_clock::now();


	bool macroRecordButton = sf::Keyboard::isKeyPressed((sf::Keyboard::Key)switchButtons.recordMacro);

	if (!macrosActive && !isMacroRecordingActive) {
		if (macroRecordButton && !macroRecordButtonLastUpdate) {
			std::time_t ctime = std::time(nullptr);
			char timeString[30];
			struct tm timeinfo;
			if(!localtime_s(&timeinfo, &ctime)){
				if (std::strftime(timeString, sizeof(timeString), "%Y-%m-%d %H-%M-%S", &timeinfo)) {
					outfile.open(macroFolder + std::string(timeString) + ".txt", std::ios::out);
				}
				else{
					std::cerr << "Could not get the time\n";
					outfile.open(macroFolder + "default_macro_name.txt", std::ios::out);
				}
			}
			else{
				std::cerr << "Could not get the time\n";
				outfile.open(macroFolder + "default_macro_name.txt", std::ios::out);
			}
			
			isMacroRecordingActive = true;
			macroRecordButtonLastUpdate = true;
		}
		else {
			for (unsigned int i = 0; i < macros.size(); i++) {
				if (sf::Keyboard::isKeyPressed((sf::Keyboard::Key)macros[i].button)) {
					activateMacro(i);
				}
			}
		}
	}

	if (macroRecordButton && !macroRecordButtonLastUpdate && outfile) {
		outfile.close();
		isMacroRecordingActive = false;
	}
	macroRecordButtonLastUpdate = macroRecordButton;



	if (macrosActive && !isMacroRecordingActive) {
		getDatafromMacro(data.data());
		#if AC_VERBOSE_OUTPUT == 1
			std::cout << "Playing macro\n";
		#endif
	}
	else {
		#if AC_ENABLE_KAYBOARD_INPUT == 1
			getDataFromKeyboard(data.data());
			#if AC_VERBOSE_OUTPUT == 1
				std::cout << "Taking keyboard input\n";
			#endif
		#else
			setNuetral(data.data());
		#endif
	}

	if (isMacroRecordingActive) {
		recordMacro(data.data());
		#if AC_VERBOSE_OUTPUT == 1
			std::cout << "Recording\n";
		#endif
	}

	if(readThread.joinable())
		readThread.join();

	readThread = std::thread([&](){
		char recievedData[8];
		boost::asio::read(*port, boost::asio::buffer(recievedData, 8));

		#if AC_VERBOSE_OUTPUT == 1
			std::cout << "bytes recieved\n";
			for (int i = 0; i < 8; i++) {
				std::cout << std::setw(5) << (int)recievedData[i];
			}
			std::cout << "\n";
		#endif
	});

	boost::asio::write(*port, boost::asio::buffer(data.data(), 8));

	#if AC_VERBOSE_OUTPUT == 1
		std::cout << "bytes sent\n";
		for (int i = 0; i < 8; i++) {
			std::cout << std::setw(5) << (int)data[i];
		}
		std::cout << "\n";
	#endif


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
	memcpy(data, &macros[currentMacro].data[currentMarcoLine * 8], sizeof(char) * 8);
	data[0] = 85;
	currentMarcoLine++;
	if (currentMarcoLine == macros[currentMacro].macroLength / 8) {
		macrosActive = false;
		if (macros[currentMacro].enableImgProc) {
			
			if (imgMatch[currentMacro]->load()) {
				if (macros[currentMacro].macroSuccessList.size() == 0)
					return;
				#if AC_OUTPUT_MACRO_ACTIVATION == 1
					std::cout << "Activating macro: " << macros[macros[currentMacro].macroSuccessList[0]].name << "\n";
				#endif
				activateMacro(cycleMacros(macros[currentMacro].macroSuccessList));
			}
			else {
				if (macros[currentMacro].macroFailList.size() == 0)
					return;
				#if AC_OUTPUT_MACRO_ACTIVATION == 1
					std::cout << "Activating macro: " << macros[macros[currentMacro].macroFailList[0]].name << "\n";
				#endif
				activateMacro(cycleMacros(macros[currentMacro].macroFailList));
			}
		}
		else {
			if (macros[currentMacro].macroDefaultList.size() == 0)
				return;
			#if AC_OUTPUT_MACRO_ACTIVATION == 1
				std::cout << "Activating macro: " << macros[macros[currentMacro].macroDefaultList[0]].name << "\n";
			#endif
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

int VirtualController::cycleMacros(std::vector<int>& macroList){
	int temp = macroList[0];
	for (unsigned int i = 0; i < macroList.size() - 1; i++) {
		macroList[i] = macroList[i + 1];
	}
	macroList[macroList.size() - 1] = temp;
	return temp;
}