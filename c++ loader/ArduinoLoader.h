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
button=-1
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
    string name;
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
    double matchThreshold;
    int minX;
    int minY;
    int maxX;
    int maxY;

    vector<array<char, 8>> data;

    vector<int> macroSuccessList;
    vector<int> macroFailList;
    vector<int> macroDefaultList;
};

class ArduinoLoader : public Loader{
private:
    //map<string, vector<array<char, 8>>> macros;
    //map<string, cv::Mat> pictures;

    vector<cv::Mat> pictures;
    vector<macro> macros;

    Loader::addDefaultElement;
    Loader::addElement;
    Loader::getCatagories;


    void reloadPictures(vector<cv::Mat> &pictures, map<string, int> &pictureIndicices);
    void reloadMacros(vector<macro> &macros, map<string, int> &macroIndicices, map<string, int> pictureIndicices);


    std::vector<std::array<char, 8>> loadMacro(string filepath);
    cv::Mat loadPicture(string filepath);
public:
    ArduinoLoader();
    ArduinoLoader(string filepath);

    void loadConfig(string filepath);

    void reloadPicturesAndMacros();

    string getElement(string catagories, string element);

    string toString();
};

ArduinoLoader::ArduinoLoader(){
    stringstream ss = stringstream(defaultConfigString);
    Loader::loadConfig(ss, true);
}

ArduinoLoader::ArduinoLoader(string filepath){
    stringstream ss = stringstream(defaultConfigString);
    Loader::loadConfig(ss, true);
    loadConfig(filepath);
}

void ArduinoLoader::loadConfig(string filepath){
    Loader::loadConfig(filepath, false);

    reloadPicturesAndMacros();
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

void ArduinoLoader::reloadMacros(vector<macro> &macros, map<string, int> &macroIndicices, map<string, int> pictureIndicices){
    macros = {};//todo set size beforehand
    macroIndicices = {};

    for(map<string, vector<string>>::iterator it = catagories.begin(); it != catagories.end(); ++it){
        if (removeTrailingDigit(it->first) != "macro" || it->first == "macro")
            continue;
        cout << it->first << endl;

        macro currentMacro;
        currentMacro.name = it->first;
        string button = Loader::getElement(it->first, "button");
        if(button.at(0) >= 'a' && button.at(0) <= 'z'){
            currentMacro.button = button.at(0) - 'a';
        }
        else if(button.at(0) >= 'A' && button.at(0) <= 'Z'){
            currentMacro.button = button.at(0) - 'A';
        }
        else{
            currentMacro.button = stoi(button);
        }
        currentMacro.enableImgProc = stoi(Loader::getElement(it->first, "enable-imgProc"));
        
        string templatePic = Loader::getElement(it->first, "template");
        if(pictureIndicices.find(templatePic) != pictureIndicices.end())
            currentMacro.templatePic = pictureIndicices.at(templatePic);
        else
            currentMacro.templatePic = -1;

        string maskPic = Loader::getElement(it->first, "mask");
        if(pictureIndicices.find(maskPic) != pictureIndicices.end())
            currentMacro.maskPic = pictureIndicices.at(maskPic);
        else
            currentMacro.maskPic = -1;

        currentMacro.matchMethod = stoi(Loader::getElement(it->first, "match-method"));
        currentMacro.searchMinX = stoi(Loader::getElement(it->first, "search-min-x"));
        currentMacro.searchMinY = stoi(Loader::getElement(it->first, "search-min-y"));
        currentMacro.searchMaxX = stoi(Loader::getElement(it->first, "search-max-x"));
        currentMacro.searchMaxY = stoi(Loader::getElement(it->first, "search-max-y"));
        currentMacro.matchThreshold = stof(Loader::getElement(it->first, "match-threshold"));
        currentMacro.minX = stoi(Loader::getElement(it->first, "min-x"));
        currentMacro.minY = stoi(Loader::getElement(it->first, "min-y"));
        currentMacro.maxX = stoi(Loader::getElement(it->first, "max-x"));
        currentMacro.maxY = stoi(Loader::getElement(it->first, "max-y"));

        currentMacro.data = loadMacro(getElement("macros", "macro-folder") + Loader::getElement(it->first, "filename"));

        macros.push_back(currentMacro);

        macroIndicices.insert({it->first, macros.size() - 1});
    }

    for(unsigned int i = 0; i < macros.size(); i++){
        string macroTemplate = Loader::getElement(macros[i].name, "macro-to-import-template-setting");
        if(macroIndicices.find(macroTemplate) != macroIndicices.end()){
            macros[i].macroTemplate = macroIndicices.at(macroTemplate);
        }
        else{
            macros[i].macroTemplate = -1;
        }

        string macroStr;

        macros[i].macroSuccessList = {};
        stringstream macroSuccessStream = stringstream(Loader::getElement(macros[i].name, "next-macro-success"));
        while(!macroSuccessStream.eof()){
            macroSuccessStream >> macroStr;
            if(macroIndicices.find(macroStr) != macroIndicices.end())
                macros[i].macroSuccessList.push_back(macroIndicices.at(macroStr));
        }

        macros[i].macroFailList = {};
        stringstream macroFailStream = stringstream(Loader::getElement(macros[i].name, "next-macro-fail"));
        while(!macroFailStream.eof()){
            macroFailStream >> macroStr;
            if(macroIndicices.find(macroStr) != macroIndicices.end())
                macros[i].macroFailList.push_back(macroIndicices.at(macroStr));
        }

        macros[i].macroDefaultList = {};
        stringstream macroDefaultStream = stringstream(Loader::getElement(macros[i].name, "next-macro-default"));
        while(!macroDefaultStream.eof()){
            macroDefaultStream >> macroStr;
            if(macroIndicices.find(macroStr) != macroIndicices.end())
                macros[i].macroDefaultList.push_back(macroIndicices.at(macroStr));
        }
    }
}

void ArduinoLoader::reloadPictures(vector<cv::Mat> &pictures, map<string, int> &pictureIndicices){
    pictures = {};
    pictureIndicices = {};

    for(map<string, vector<string>>::iterator it = catagories.begin(); it != catagories.end(); ++it){
        if (removeTrailingDigit(it->first) != "picture" || it->first == "picture")
            continue;
        
        pictures.push_back(loadPicture(Loader::getElement("pictures", "picture-folder") + Loader::getElement(it->first, "picture-filename")));
        pictureIndicices.insert({it->first, pictures.size() - 1});
    }
}

void ArduinoLoader::reloadPicturesAndMacros(){
    map<string, int> pictureIndicices;
    map<string, int> macroIndicices;
    reloadPictures(pictures, pictureIndicices);
    reloadMacros(macros, macroIndicices, pictureIndicices);
}

string ArduinoLoader::toString(){
    string result = Loader::toString();

    result.append("\nMacros\n");
    for(unsigned int i = 0; i < macros.size(); i++){
        result.append(macros[i].name);
        result.append(" " + to_string(macros[i].data.size()));
        result.append(" " + to_string(macros[i].button));
        result.append(" " + to_string(macros[i].enableImgProc));
        result.append(" " + to_string(macros[i].templatePic));
        result.append(" " + to_string(macros[i].maskPic));
        result.append(" " + to_string(macros[i].matchMethod));
        result.append(" " + to_string(macros[i].searchMinX));
        result.append(" " + to_string(macros[i].searchMinY));
        result.append(" " + to_string(macros[i].searchMaxX));
        result.append(" " + to_string(macros[i].searchMaxY));
        result.append(" " + to_string(macros[i].macroTemplate));
        result.append(" " + to_string(macros[i].matchThreshold));
        result.append(" " + to_string(macros[i].minX));
        result.append(" " + to_string(macros[i].minY));
        result.append(" " + to_string(macros[i].maxX));
        result.append(" " + to_string(macros[i].maxY));
        result.append("\n");
    }

    result.append("\nPictures\n");
    for(unsigned int i = 0; i < pictures.size(); i++){
        result.append(to_string(i) + " width:" + to_string(pictures[i].size().width) + " height:" + to_string(pictures[i].size().height) + '\n');
    }

    return result;
}

string ArduinoLoader::getElement(string catagory, string element){
    if(removeTrailingDigit(catagory) == "macro" || removeTrailingDigit(catagory) == "picture")
        return "";
    else
        return Loader::getElement(catagory, element);
}


#endif