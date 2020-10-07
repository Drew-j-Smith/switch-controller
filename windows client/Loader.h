#ifndef LOADER_H
#define LOADER_H

#include <SFML/Graphics.hpp>

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map> 
#include <fstream>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>


const int bufferSize = 1000; 
const std::string keywords[10] = {
	"controls",
	"general",
	"macros",
	"pictures",
	"",
	"",
	"",
	"",
	"",
	""
};

const std::string controlKeywords[29] = {
	"y=",
	"b=",
	"a=",
	"x=",
	"l=",
	"r=",
	"xl=",
	"xr=",
	"select=",
	"start=",
	"l-click=",
	"r-click=",
	"home=",
	"capture=",
	"left-stick-x+=",
	"left-stick-x-=",
	"left-stick-y-=",
	"left-stick-y+=",
	"right-stick-x+=",
	"right-stick-x-=",
	"right-stick-y-=",
	"right-stick-y+=",
	"dpad-up=",
	"dpad-right=",
	"dpad-down=",
	"dpad-left=",
	"start-macro-recording=",
	"stop-macro-recording=",
	"desktop-screenshot="
};

const std::string generalKeywords[10] = {
	"com-port=",
	"window-name=",
	"window-width=",
	"window-height=",
	"",
	"",
	"",
	"",
	"",
	"",
};

const std::string macroKeywords[10] = {
	"macro-folder=",
	"macro-recording-filename=",
	"macro{",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
};

const std::string pictureKeywords[10] = {
	"picture-folder=",
	"picture-recording-filename=",
	"picture-filename=",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
};

const std::string individualMacroKeywords[20] = {
	"filename=",
	"button=",
	"enable-imgProc=",
	"template=",
	"mask=",
	"match-method=",
	"match-threshold=",
	"min-x=",
	"min-y=",
	"max-x=",
	"max-y=",
	"next-macro-success=",
	"next-macro-fail=",
	"next-macro-default=",
	"macro-to-import-template-setting=",
	"search-min-x=",
	"search-min-y=",
	"search-max-x=",
	"search-max-y=",
	""
};



class Loader
{
public:
	Loader();
	Loader(std::string configFilename);

	void loadConfig(std::string filename);

	int getMacroIndex(std::string name);
	std::vector<std::array<char, 8>> getMacro(int index);
	int getNumMacros();

	int getPictureIndex(std::string name);
	cv::Mat getPicture(int index);
	int getNumPictures();


	sf::Keyboard::Key getKeyCode(int index);
	std::string getCOM_Port();
	std::string getLocGameWindowName();
	int getLocWindowWidth();
	int getLocWindowHeight();

	std::string getMacroFolder();
	std::string getMacroRecordingFilename();
	sf::Keyboard::Key getMacroKeyCode(int index);

	std::string getPictureFolder();
	std::string getPictureRecordingFilename();


	bool getEnableMacroImgProc(int index);
	int getMacroTemplates(int index);
	int getMacroMasks(int index);
	int getMacroMatchMethods(int index);
	double getMatchThresholds(int index);
	cv::Point getMinMacroMatchPoints(int index);
	cv::Point getMaxMacroMatchPoints(int index);
	std::string getNextMacroImgMatch(int index);
	std::string getNextMacroNoImgMatch(int index);
	std::string getNextMacroNoImgProc(int index);

	std::string advanceNextMacroImgMatch(int index);
	std::string advanceNextMacroNoImgMatch(int index);
	std::string advanceNextMacroNoImgProc(int index);

	int getSharedMacroTemplates(int index);

	cv::Point getMinMacroSearchPoints(int index);
	cv::Point getMaxMacroSearchPoints(int index);

private:
	void loadMacro(std::string fileLoc, std::string filename);
	void loadPicture(std::string fileLoc, std::string filename);
	void loadNextMacro(std::vector<std::vector<std::string>> &nextMacroList, std::string stringLine, std::ifstream &infile);

	std::unordered_map<std::string, int> macroIndexMap;
	std::vector<std::vector<std::array<char, 8>>> macros;
	int numMacros;

	std::unordered_map<std::string, int> pictureIndexMap;
	std::vector<cv::Mat> pictures;
	int numPictures;

	sf::Keyboard::Key keyCodes[29];
	std::string COM_Port;
	std::string locGameWindowName;
	int locWindowWidth;
	int locWindowHeight;

	std::string macroFolder;
	std::string macroRecordingFilename;
	std::vector<sf::Keyboard::Key> macroKeyCodes;

	std::string pictureFolder;
	std::string pictureRecordingFilename;

	std::vector<bool> enableMacroImgProc;
	std::vector<int> macroTemplates;
	std::vector<int> macroMasks;
	std::vector<int> macroMatchMethods;
	std::vector<double> matchThresholds;
	std::vector<cv::Point> minMacroMatchPoints;
	std::vector<cv::Point> maxMacroMatchPoints;
	std::vector<std::vector<std::string>> nextMacroImgMatch;
	std::vector<std::vector<std::string>> nextMacroNoImgMatch;
	std::vector<std::vector<std::string>> nextMacroNoImgProc;
	std::vector<int> sharedMacroTemplates;

	std::vector<cv::Point> minMacroSearchPoints;
	std::vector<cv::Point> maxMacroSearchPoints;

};

Loader::Loader() {
	numMacros = 0;
	numPictures = 0;
}

Loader::Loader(std::string configFilename) {
	numMacros = 0;
	numPictures = 0;
	loadConfig(configFilename);
}

void Loader::loadConfig(std::string filename) {
	std::ifstream infile;
	char inputBuffer[bufferSize];
	std::string lineString;
	int key = -1;

	infile.open(filename, std::ios::in);

	if (infile) {
		while (!infile.eof()) {
			infile.getline(inputBuffer, bufferSize);
			lineString = std::string(inputBuffer);

			if (inputBuffer[0] == '#') {}
			else if (inputBuffer[0] == '[') {
				

				int i = 1;
				for (i; inputBuffer[i] != ']' && inputBuffer[i] != '\0'; i++);
				
				
				lineString = lineString.substr(1, i - 1);

				for (i = 0; i < 10; i++) {
					if (lineString == keywords[i]) {
						key = i;
						break;
					}
				}
			}
			else if (key == 0) {

				for (int i = 0; i < 29; i++) {
					if (lineString.substr(0, controlKeywords[i].size()).compare(controlKeywords[i]) == 0 && controlKeywords[i].size() > 0) {
						std::string substring = lineString.substr(controlKeywords[i].size());
						transform(substring.begin(), substring.end(), substring.begin(), ::tolower);
						if (substring.length() == 0)
							keyCodes[i] = static_cast<sf::Keyboard::Key>(-1);
						else if (substring[0] >= 'a' && substring[0] <= 'z')
							keyCodes[i] = static_cast<sf::Keyboard::Key>(substring[0] - 'a');
						else
							keyCodes[i] = static_cast<sf::Keyboard::Key>(std::stoi(substring));
					}
				}

			}

			else if (key == 1) {

				for (int i = 0; i < 10; i++) {
					if (lineString.substr(0, generalKeywords[i].size()).compare(generalKeywords[i]) == 0 && generalKeywords[i].size() > 0) {
						std::string substring = lineString.substr(generalKeywords[i].size());
						
						if (i == 0)
							COM_Port = substring;
						else if (i == 1)
							locGameWindowName = substring;
						else if (i == 2)
							locWindowWidth = std::stoi(substring);
						else if (i == 3)
							locWindowHeight = std::stoi(substring);

					}
				}

			}

			else if (key == 2) {

				for (int i = 0; i < 10; i++) {
					if (lineString.substr(0, macroKeywords[i].size()).compare(macroKeywords[i]) == 0 && macroKeywords[i].size() > 0) {
						std::string substring = lineString.substr(macroKeywords[i].size());
						
						if (i == 0)
							macroFolder = substring;
						else if (i == 1)
							macroRecordingFilename = substring;
						else if (i == 2){

							minMacroMatchPoints.push_back(cv::Point(0, 0));
							maxMacroMatchPoints.push_back(cv::Point(locWindowWidth, locWindowHeight));
							minMacroSearchPoints.push_back(cv::Point(0, 0));
							maxMacroSearchPoints.push_back(cv::Point(locWindowWidth, locWindowHeight));

							do {
								infile.getline(inputBuffer, bufferSize);
								lineString = std::string(inputBuffer);


								for (int j = 0; j < 20; j++) {
									if (lineString.substr(0, individualMacroKeywords[j].size()).compare(individualMacroKeywords[j]) == 0 && individualMacroKeywords[j].size() > 0) {
										std::string substring = lineString.substr(individualMacroKeywords[j].size());

										if (j == 0)
											loadMacro(macroFolder, substring);
										else if (j == 1) {
											if (substring.length() == 0)
												macroKeyCodes.push_back(static_cast<sf::Keyboard::Key>(-1));
											else if (substring[0] >= 'a' && substring[0] <= 'z')
												macroKeyCodes.push_back(static_cast<sf::Keyboard::Key>(substring[0] - 'a'));
											else
												macroKeyCodes.push_back(static_cast<sf::Keyboard::Key>(std::stoi(substring)));
										}
										if (j == 2) {
											if (substring.length() == 0)
												enableMacroImgProc.push_back(false);
											else
												enableMacroImgProc.push_back(std::stoi(substring));
										}
										if (j == 3) {
											if (substring.length() == 0)
												macroTemplates.push_back(-1);
											else
												macroTemplates.push_back(getPictureIndex(substring));
										}
										if (j == 4) {
											if (substring.length() == 0)
												macroMasks.push_back(-1);
											else
												macroMasks.push_back(getPictureIndex(substring));
										}
										if (j == 5) {
											if (substring.length() == 0)
												macroMatchMethods.push_back(3);
											else
												macroMatchMethods.push_back(std::stoi(substring));
										}
										if (j == 6) {
											if (substring.length() == 0)
												matchThresholds.push_back(-1);
											else
												matchThresholds.push_back(std::stod(substring));
										}
										if (j == 7) {
											if (substring.length() != 0)
												minMacroMatchPoints[minMacroMatchPoints.size() - 1].x = std::stoi(substring);
										}
										if (j == 8)
											if (substring.length() != 0)
												minMacroMatchPoints[minMacroMatchPoints.size() - 1].y = std::stoi(substring);
										if (j == 9)
											if (substring.length() != 0)
												maxMacroMatchPoints[maxMacroMatchPoints.size() - 1].x = std::stoi(substring);
										if (j == 10)
											if (substring.length() != 0)
												maxMacroMatchPoints[maxMacroMatchPoints.size() - 1].y = std::stoi(substring);
										if (j == 11) {
											if (substring.length() != 0)
												loadNextMacro(nextMacroImgMatch, substring, infile);
											else
												loadNextMacro(nextMacroImgMatch, "", infile);
										}
										if (j == 12) {
											if (substring.length() != 0)
												loadNextMacro(nextMacroNoImgMatch, substring, infile);
											else
												loadNextMacro(nextMacroNoImgMatch, "", infile);
										}
										if (j == 13) {
											if (substring.length() != 0)
												loadNextMacro(nextMacroNoImgProc, substring, infile);
											else
												loadNextMacro(nextMacroNoImgProc, "", infile);
										}
										if (j == 14) {
											if (substring.length() == 0)
												sharedMacroTemplates.push_back(-1);
											else
												sharedMacroTemplates.push_back(macroIndexMap[substring]);
										}
										if (j == 15) {
											if (substring.length() != 0)
												minMacroSearchPoints[minMacroSearchPoints.size() - 1].x = std::stoi(substring);
										}
										if (j == 16)
											if (substring.length() != 0)
												minMacroSearchPoints[minMacroSearchPoints.size() - 1].y = std::stoi(substring);
										if (j == 17)
											if (substring.length() != 0)
												maxMacroSearchPoints[maxMacroSearchPoints.size() - 1].x = std::stoi(substring);
										if (j == 18)
											if (substring.length() != 0)
												maxMacroSearchPoints[maxMacroSearchPoints.size() - 1].y = std::stoi(substring);

									}
								}


							} while (lineString.find('}') == -1);
							break;
						}

					}
				}

			}
			else if (key == 3) {

				for (int i = 0; i < 10; i++) {
					if (lineString.substr(0, pictureKeywords[i].size()).compare(pictureKeywords[i]) == 0 && pictureKeywords[i].size() > 0) {
						std::string substring = lineString.substr(pictureKeywords[i].size());

						if (i == 0)
							pictureFolder = substring;
						else if (i == 1)
							pictureRecordingFilename = substring;
						else if (i == 2)
							loadPicture(pictureFolder, substring);

					}
				}
			}
		}
	}
	else
	{
		std::cerr << "Error opening at: " + filename + "\n";
	}
	infile.close();
}

void Loader::loadMacro(std::string fileLoc, std::string filename) {
	std::ifstream infile;
	char inputBuffer[bufferSize];
	std::array<char, 8> inputArray;
	std::vector<std::array<char, 8>> macro;

	infile.open(fileLoc + filename, std::ios::in);
	
	if (infile) {
		while (!infile.eof()) {
			infile.getline(inputBuffer, bufferSize);
			if (std::string(inputBuffer).length() != 24)
				continue;
			for (int i = 0; i < 8; i++) {
				inputArray[i] = std::stoi(std::string(inputBuffer).substr(i * 3, 3)) - 128;
			}
			macro.push_back(inputArray);
		}

		macros.push_back(macro);
		macroIndexMap[filename] = int(macros.size() - 1);
		numMacros++;
	}
	else
	{
		std::cerr << "Error opening at: " + fileLoc + filename << "\n";
	}
	infile.close();
}

void Loader::loadPicture(std::string fileLoc, std::string filename) {
	cv::Mat image;
	image = cv::imread(fileLoc + filename, cv::IMREAD_COLOR); // Read the file
	if (image.empty()) // Check for invalid input
	{
		std::cerr << "Could not open or find the image at: " + fileLoc + filename << "\n";
		return;
	}
	pictures.push_back(image);
	pictureIndexMap[filename] = int(pictures.size() - 1);
	numPictures++;
}

int Loader::getMacroIndex(std::string name) {
	return macroIndexMap[name];
}

std::vector<std::array<char, 8>> Loader::getMacro(int index) {
	return macros[index];
}

int Loader::getPictureIndex(std::string name) {
	return pictureIndexMap[name];
}

cv::Mat Loader::getPicture(int index) {
	return pictures[index];
}

sf::Keyboard::Key Loader::getKeyCode(int index) {
	return keyCodes[index];
}

std::string Loader::getCOM_Port() {
	return COM_Port;
}

std::string Loader::getLocGameWindowName() {
	return locGameWindowName;
}

int Loader::getLocWindowWidth() {
	return locWindowWidth;
}

int Loader::getLocWindowHeight() {
	return locWindowHeight;
}

int Loader::getNumMacros() {
	return numMacros;
}

int Loader::getNumPictures() {
	return numPictures;
}

std::string Loader::getMacroRecordingFilename() {
	return macroRecordingFilename;
}

std::string Loader::getMacroFolder() {
	return macroFolder;
}

sf::Keyboard::Key Loader::getMacroKeyCode(int index) {
	return macroKeyCodes[index];
}

std::string Loader::getPictureFolder() {
	return pictureFolder;
};

std::string Loader::getPictureRecordingFilename() {
	return pictureRecordingFilename;
}

bool Loader::getEnableMacroImgProc(int index) {
	return enableMacroImgProc[index];
}

int Loader::getMacroTemplates(int index) {
	return macroTemplates[index];
}

int Loader::getMacroMasks(int index) {
	return macroMasks[index];
}

int Loader::getMacroMatchMethods(int index) {
	return macroMatchMethods[index];
}

double Loader::getMatchThresholds(int index) {
	return matchThresholds[index];
};

cv::Point Loader::getMinMacroMatchPoints(int index) {
	return minMacroMatchPoints[index];
}

cv::Point Loader::getMaxMacroMatchPoints(int index) {
	return maxMacroMatchPoints[index];
}

std::string Loader::advanceNextMacroImgMatch(int index) {
	std::string temp = nextMacroImgMatch[index][0];
	for (int i = 0; i < nextMacroImgMatch[index].size() - 1;  i++) {
		nextMacroImgMatch[index][i] = nextMacroImgMatch[index][i + 1];
	}
	nextMacroImgMatch[index][nextMacroImgMatch[index].size() - 1] = temp;
	return temp;
}

std::string Loader::advanceNextMacroNoImgMatch(int index) {
	std::string temp = nextMacroNoImgMatch[index][0];
	for (int i = 0; i < nextMacroNoImgMatch[index].size() - 1; i++) {
		nextMacroNoImgMatch[index][i] = nextMacroNoImgMatch[index][i + 1];
	}
	nextMacroNoImgMatch[index][nextMacroNoImgMatch[index].size() - 1] = temp;
	return temp;
}

std::string Loader::advanceNextMacroNoImgProc(int index) {
	std::string temp = nextMacroNoImgProc[index][0];
	for (int i = 0; i < nextMacroNoImgProc[index].size() - 1; i++) {
		nextMacroNoImgProc[index][i] = nextMacroNoImgProc[index][i + 1];
	}
	nextMacroNoImgProc[index][nextMacroNoImgProc[index].size() - 1] = temp;
	return temp;
}

std::string Loader::getNextMacroImgMatch(int index) {
	return nextMacroImgMatch[index][0];
}

std::string Loader::getNextMacroNoImgMatch(int index) {
	return nextMacroNoImgMatch[index][0];
}

std::string Loader::getNextMacroNoImgProc(int index) {
	return nextMacroNoImgProc[index][0];
}

void Loader::loadNextMacro(std::vector<std::vector<std::string>> &nextMacroList, std::string stringLine, std::ifstream &infile) {
	std::vector<std::string> temp;

	if (stringLine.compare("switch") != 0) {
		temp.push_back(stringLine);
	}
	else {
		char inputBuffer[bufferSize];
		std::string lineStr;
		
		while (true) {
			infile.getline(inputBuffer, bufferSize);
			lineStr = std::string(inputBuffer);

			if (lineStr.compare("end-switch") == 0)
				break;
			
			temp.push_back(lineStr);
		}
	}

	nextMacroList.push_back(temp);
}

int Loader::getSharedMacroTemplates(int index) {
	return sharedMacroTemplates[index];
}

cv::Point Loader::getMinMacroSearchPoints(int index) {
	return minMacroSearchPoints[index];
}
cv::Point Loader::getMaxMacroSearchPoints(int index) {
	return maxMacroSearchPoints[index];
}







#endif