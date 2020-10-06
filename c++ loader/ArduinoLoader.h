#ifndef ARDUINO_LOADER_H
#define ARDUINO_LOADER_H

#include "Loader.h"
#include <string>
#include <vector>
#include <sstream>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

using namespace std;

#pragma region
const string defaultConfigString = R"(
[controls]
y=
b=k
a=j
x=i
l=
r=o
xl=l
xr=
select=
start=e
l-click=
r-click=
home=60
capture=
left-stick-x+=d
left-stick-x-=a
left-stick-y+=w
left-stick-y-=s
right-stick-x+=
right-stick-x-=
right-stick-y+=
right-stick-y-=
dpad-up=
dpad-right=
dpad-down=
dpad-left=
start-macro-recording=y
stop-macro-recording=u
desktop-screenshot=r
[general]
com-port=\\.\COM3
window-name=Game Capture HD
window-width=1920
window-height=1080
[pictures]
picture-folder=pictures/
picture-recording-filename=picture.png
[picture]
picture-filename=
[macros]
macro-folder=macros/
macro-recording-filename=macro.txt
[macro]
filename=
button=
enable-imgProc=0
template=
mask=
match-method=3
search-min-x=0
search-min-y=0
search-max-x=0
search-max-y=0
macro-to-import-template-setting=
match-threshold=.95
min-x=0
min-y=0
max-x=0
max-y=0
next-macro-success=
next-macro-fail=
next-macro-default=
)";
#pragma endregion

struct macro{
    int button;
    bool enableImgProc;
    int templatePic;
    int maskPic;
    int matchMethod;
    int searchMinX;
    int searchMinY;
    int searchMaxX;
    int searchMaxY;
    int macroTemplate;
    int matchthreshold;
    int minX;
    int minY;
    int maxX;
    int maxY;

    vector<int> macroSuccessList;
    vector<int> macroFailList;
    vector<int> macroDefault;
};

class ArduinoLoader : public Loader{
private:
    map<string, vector<array<char, 8>>> macros;
    map<string, cv::Mat> pictures;

    Loader::addDefaultElement;
    Loader::getElement;
    Loader::addElement;
    Loader::getCatagories;

    std::vector<std::array<char, 8>> loadMacro(string filepath);
    cv::Mat loadPicture(string filepath);
public:
    ArduinoLoader();
    ArduinoLoader(string filepath);

    void loadConfig(string filepath);

    void reloadMacros();
    void reloadPictures();

    string toString();
};

ArduinoLoader::ArduinoLoader(){
    stringstream ss = stringstream(defaultConfigString);
    Loader::loadConfig(ss, true);

    macros = {};
    pictures = {};
}

ArduinoLoader::ArduinoLoader(string filepath){
    stringstream ss = stringstream(defaultConfigString);
    Loader::loadConfig(ss, true);
    loadConfig(filepath);
}

void ArduinoLoader::loadConfig(string filepath){
    Loader::loadConfig(filepath, false);

    reloadMacros();
    reloadPictures();
}


std::vector<std::array<char, 8>> ArduinoLoader::loadMacro(string filepath){
    ifstream infile;
    string stringLine;
    std::array<char, 8> inputArray;
    std::vector<std::array<char, 8>> macro = {};

    infile.open(filepath, ios::in);

    if (infile) {
        while (!infile.eof()) {
            getline(infile, stringLine);
            if (stringLine.length() != 24)
                continue;
            for (int i = 0; i < 8; i++) {
                inputArray[i] = std::stoi(stringLine.substr(i * 3, 3)) - 128;
            }
            macro.push_back(inputArray);
        }
    }
    else
    {
        std::cerr << "Could not open file " + filepath << "\n";
    }
    infile.close();

    return macro;
}

cv::Mat ArduinoLoader::loadPicture(string filepath) {
    cv::Mat image;
    image = cv::imread(filepath, cv::IMREAD_COLOR); // Read the file
    if (image.empty()) // Check for invalid input
    {
        std::cerr << "Could not open file " << filepath << "\n";
    }
    return image;
}

void ArduinoLoader::reloadMacros(){
    for(map<string, vector<string>>::iterator it = catagories.begin(); it != catagories.end(); ++it){
        int endChar = it->first.length() - 1;
        while (endChar > 0 && it->first.at(endChar) >= '0' && it->first.at(endChar) <= '9'){
            endChar--;
        }
        if (it->first.substr(0, endChar + 1) != "macro" || it->first == "macro")
            continue;

        macros.insert({it->first, loadMacro(getElement("macros", "macro-folder") + getElement(it->first, "filename"))});
    }
}

void ArduinoLoader::reloadPictures(){
    for(map<string, vector<string>>::iterator it = catagories.begin(); it != catagories.end(); ++it){
        int endChar = it->first.length() - 1;
        while (endChar > 0 && it->first.at(endChar) >= '0' && it->first.at(endChar) <= '9'){
            endChar--;
        }
        if (it->first.substr(0, endChar + 1) != "picture" || it->first == "picture")
            continue;
        
        pictures.insert({it->first, loadPicture(getElement("pictures", "picture-folder") + getElement(it->first, "picture-filename"))});
    }
}

string ArduinoLoader::toString(){
    string result = Loader::toString();

    result.append("\nMacros\n");
    for(map<string, vector<array<char, 8>>>::iterator it = macros.begin(); it != macros.end(); it++){
        result.append(it->first + " Lines:" + to_string(it->second.size()) + '\n');
    }

    result.append("\nPictures\n");
    for(map<string, cv::Mat>::iterator it = pictures.begin(); it != pictures.end(); it++){
        result.append(it->first + " width:" + to_string(it->second.size().width) + " height:" + to_string(it->second.size().height) + '\n');
    }

    return result;
}


#endif