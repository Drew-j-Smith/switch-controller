#ifndef ARDUINO_LOADER_H
#define ARDUINO_LOADER_H

#include "ArduinoStructs.h"

#include <string>
#include <vector>
#include <map>
#include <sstream>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using namespace std;

//todo save as current date
//todo fix indicies of macro

class ArduinoLoader{
private:
    vector<cv::Mat> pictures;
    vector<Macro> macros;
    SwitchButtons switchButtons;
    boost::property_tree::ptree config;

    void reloadPictures(vector<cv::Mat> &pictures, map<string, int> &pictureIndicices);
    void reloadMacros(vector<Macro> &macros, map<string, int> &macroIndicices, map<string, int> pictureIndicices);
    SwitchButtons getSwitchButtons();
    std::vector<std::array<char, 8>> loadMacro(string filename);
    cv::Mat loadPicture(string filename);
    int getButton(string button);
public:
    ArduinoLoader();
    ArduinoLoader(string filename);

    void loadConfig(string filename);

    string toString();
};

ArduinoLoader::ArduinoLoader(){
    config = boost::property_tree::ptree();
}

ArduinoLoader::ArduinoLoader(string filename){
    loadConfig(filename);
}

void ArduinoLoader::loadConfig(string filename){
    boost::property_tree::read_json(filename, config);

    map<string, int> pictureIndicices;
    map<string, int> macroIndicices;
    reloadPictures(pictures, pictureIndicices);
    reloadMacros(macros, macroIndicices, pictureIndicices);
    switchButtons = getSwitchButtons();
}


std::vector<std::array<char, 8>> ArduinoLoader::loadMacro(string filename){
    ifstream infile;
    string stringLine;
    std::array<char, 8> inputArray;
    std::vector<std::array<char, 8>> macro = {};

    infile.open(filename, ios::in);

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
        std::cerr << "Could not open file " + filename << "\n";
    }
    infile.close();

    return macro;
}

cv::Mat ArduinoLoader::loadPicture(string filename) {
    cv::Mat image;
    image = cv::imread(filename, cv::IMREAD_COLOR); // Read the file
    if (image.empty()) // Check for invalid input
    {
        std::cerr << "Could not open file " << filename << "\n";
    }
    return image;
}

void ArduinoLoader::reloadMacros(vector<Macro> &macros, map<string, int> &macroIndicices, map<string, int> pictureIndicices){
    macros = {};
    macroIndicices = {};

    boost::property_tree::ptree macroPtree = config.find("macros")->second;

    for(boost::property_tree::ptree::iterator it = macroPtree.begin(); it != macroPtree.end(); it++){
        macroIndicices.insert({it->first, macros.size()});
        Macro currentMacro;
        currentMacro.name = it->first;
        macros.push_back(currentMacro);
    }

    for(unsigned int i = 0; i < macros.size(); i++){
        boost::property_tree::ptree currentMacro = macroPtree.find(macros[i].name)->second;

        macros[i].enableImgProc  = currentMacro.get("enable image match", false);
        macros[i].matchMethod    = currentMacro.get("shared settings.match method", 3);
        macros[i].searchMinX     = currentMacro.get("search min x", 0);
        macros[i].searchMinY     = currentMacro.get("search min y", 0);
        macros[i].searchMaxX     = currentMacro.get("search max x", config.get("general.window width", 0));
        macros[i].searchMaxY     = currentMacro.get("search max y", config.get("general.window height", 0));
        macros[i].matchThreshold = currentMacro.get("shared settings.match threshold", 0.0);
        macros[i].minX           = currentMacro.get("shared settings.min x", 0);
        macros[i].minY           = currentMacro.get("shared settings.min y", 0);
        macros[i].maxX           = currentMacro.get("shared settings.max x", config.get("general.window width", 0));
        macros[i].maxY           = currentMacro.get("shared settings.max y", config.get("general.window height", 0));

        macros[i].data = loadMacro(config.get("general.macro folder", "") + currentMacro.get("filename", ""));

        string button = currentMacro.get("button", "-1");
        if(button.at(0) >= 'a' && button.at(0) <= 'z'){
            macros[i].button = button.at(0) - 'a';
        }
        else if(button.at(0) >= 'A' && button.at(0) <= 'Z'){
            macros[i].button = button.at(0) - 'A';
        }
        else{
            try{
                macros[i].button = stoi(button);
            }
            catch(const std::exception& e){
                std::cerr << e.what() << '\n';
                std::cerr << "Error loading key for " << macros[i].name << std::endl;
                macros[i].button = -1;
            }
        }

        string templatePic = currentMacro.get("shared settings.template picture", "");
        if(pictureIndicices.find(templatePic) != pictureIndicices.end())
            macros[i].templatePic = pictureIndicices.at(templatePic);
        else
            macros[i].templatePic = -1;
        
        string maskPic = currentMacro.get("shared settings.mask picture", "");
        if(pictureIndicices.find(maskPic) != pictureIndicices.end())
            macros[i].maskPic = pictureIndicices.at(maskPic);
        else
            macros[i].maskPic = -1;


        string macroTemplate = currentMacro.get("share settings from macro", "");
        if(macroIndicices.find(macroTemplate) != macroIndicices.end()){
            macros[i].macroTemplate = macroIndicices.at(macroTemplate);
        }
        else{
            macros[i].macroTemplate = -1;
        }


        boost::property_tree::ptree nextMacroList;
        macros[i].macroSuccessList = {};
        if(currentMacro.find("next macro success") != currentMacro.not_found()){
            nextMacroList = currentMacro.find("next macro success")->second;
            for(boost::property_tree::ptree::iterator it = nextMacroList.begin(); it != nextMacroList.end(); it++){
                if(macroIndicices.find(it->first) != macroIndicices.end()){
                    macros[i].macroSuccessList.push_back(macroIndicices.at(it->first));
                }
            }
        }

        macros[i].macroFailList = {};
        if(currentMacro.find("next macro fail") != currentMacro.not_found()){
            nextMacroList = currentMacro.find("next macro fail")->second;
            for(boost::property_tree::ptree::iterator it = nextMacroList.begin(); it != nextMacroList.end(); it++){
                if(macroIndicices.find(it->first) != macroIndicices.end()){
                    macros[i].macroFailList.push_back(macroIndicices.at(it->first));
                }
            }
        }

        macros[i].macroDefaultList = {};
        if(currentMacro.find("next macro default") != currentMacro.not_found()){
            nextMacroList = currentMacro.find("next macro default")->second;
            for(boost::property_tree::ptree::iterator it = nextMacroList.begin(); it != nextMacroList.end(); it++){
                if(macroIndicices.find(it->first) != macroIndicices.end()){
                    macros[i].macroDefaultList.push_back(macroIndicices.at(it->first));
                }
            }
        }
        
    }

}

void ArduinoLoader::reloadPictures(vector<cv::Mat> &pictures, map<string, int> &pictureIndicices){
    pictures = {};
    pictureIndicices = {};

    boost::property_tree::ptree picturePtree = config.find("pictures")->second;

    for(boost::property_tree::ptree::iterator it = picturePtree.begin(); it != picturePtree.end(); it++){
        pictureIndicices.insert({it->first, macros.size()});
        pictures.push_back(loadPicture(config.get("general.picture folder","") + it->second.get("picture filename", "")));
    }
}

SwitchButtons ArduinoLoader::getSwitchButtons(){
    SwitchButtons switchButtons;
    switchButtons.y                = getButton("y");
    switchButtons.b                = getButton("b");
    switchButtons.a                = getButton("a");
    switchButtons.x                = getButton("x");
    switchButtons.l                = getButton("l");
    switchButtons.r                = getButton("r");
    switchButtons.xl               = getButton("xl");
    switchButtons.xr               = getButton("xr");
    switchButtons.select           = getButton("select");
    switchButtons.start            = getButton("start");
    switchButtons.lClick           = getButton("l click");
    switchButtons.rRlick           = getButton("r click");
    switchButtons.home             = getButton("home");
    switchButtons.capture          = getButton("capture");
    switchButtons.leftStickXplus   = getButton("left stick x+");
    switchButtons.leftStickXminus  = getButton("left stick x-");
    switchButtons.leftStickYplus   = getButton("left stick y+");
    switchButtons.leftStickYminus  = getButton("left stick y-");
    switchButtons.rightStickXplus  = getButton("right stick x+");
    switchButtons.rightStickXminus = getButton("right stick x-");
    switchButtons.rightStickYplus  = getButton("right stick y+");
    switchButtons.rightStickYminus = getButton("right stick y-");
    switchButtons.dpadUp           = getButton("dpad up");
    switchButtons.dpadRight        = getButton("dpad right");
    switchButtons.dpadDown         = getButton("dpad down");
    switchButtons.dpadLeft         = getButton("dpad left");
    switchButtons.recordMacro      = getButton("record macro");
    switchButtons.screenshot       = getButton("screenshot");
    return switchButtons;
}

int ArduinoLoader::getButton(string button){
    string buttonCode = config.get("controls." + button, "-1");
    if(buttonCode.at(0) >= 'a' && buttonCode.at(0) <= 'z'){
        return buttonCode.at(0) - 'a';
    }
    else if(buttonCode.at(0) >= 'A' && buttonCode.at(0) <= 'Z'){
        return buttonCode.at(0) - 'A';
    }
    else{
        try{
            return stoi(buttonCode);
        }
        catch(const std::exception& e){
            std::cerr << e.what() << '\n';
            std::cerr << "Error loading key for " << button << std::endl;
            return -1;
        }
        
        
    }
}

string ArduinoLoader::toString(){
    string result;
    stringstream outstream = stringstream("");

    boost::property_tree::json_parser::write_json(outstream, config);

    result += outstream.str();

    result.append("\nMacros\n");
    for(unsigned int i = 0; i < macros.size(); i++){
        result.append(macros[i].name);
        result.append("\nmacro length:   " + to_string(macros[i].data.size()));
        result.append("\nbutton:         " + to_string(macros[i].button));
        result.append("\nenableImgProc:  " + to_string(macros[i].enableImgProc));
        result.append("\ntemplatePic:    " + to_string(macros[i].templatePic));
        result.append("\nmaskPic:        " + to_string(macros[i].maskPic));
        result.append("\nmatchMethod:    " + to_string(macros[i].matchMethod));
        result.append("\nsearchMinX:     " + to_string(macros[i].searchMinX));
        result.append("\nsearchMinY:     " + to_string(macros[i].searchMinY));
        result.append("\nsearchMaxX:     " + to_string(macros[i].searchMaxX));
        result.append("\nsearchMaxY:     " + to_string(macros[i].searchMaxY));
        result.append("\nmacroTemplate:  " + to_string(macros[i].macroTemplate));
        result.append("\nmatchThreshold: " + to_string(macros[i].matchThreshold));
        result.append("\nminX:           " + to_string(macros[i].minX));
        result.append("\nminY:           " + to_string(macros[i].minY));
        result.append("\nmaxX:           " + to_string(macros[i].maxX));
        result.append("\nmaxY:           " + to_string(macros[i].maxY));
        result.append("\n");
    }

    result.append("\nControls");
    result.append("\ny:                " + to_string(switchButtons.y));
    result.append("\nb:                " + to_string(switchButtons.b));
    result.append("\na:                " + to_string(switchButtons.a));
    result.append("\nx:                " + to_string(switchButtons.x));
    result.append("\nl:                " + to_string(switchButtons.l));
    result.append("\nr:                " + to_string(switchButtons.r));
    result.append("\nxl:               " + to_string(switchButtons.xl));
    result.append("\nxr:               " + to_string(switchButtons.xr));
    result.append("\nselect:           " + to_string(switchButtons.select));
    result.append("\nstart:            " + to_string(switchButtons.start));
    result.append("\nlClick:           " + to_string(switchButtons.lClick));
    result.append("\nrRlick:           " + to_string(switchButtons.rRlick));
    result.append("\nhome:             " + to_string(switchButtons.home));
    result.append("\ncapture:          " + to_string(switchButtons.capture));
    result.append("\nleftStickXplus:   " + to_string(switchButtons.leftStickXplus));
    result.append("\nleftStickXminus:  " + to_string(switchButtons.leftStickXminus));
    result.append("\nleftStickYplus:   " + to_string(switchButtons.leftStickYplus));
    result.append("\nleftStickYminus:  " + to_string(switchButtons.leftStickYminus));
    result.append("\nrightStickXplus:  " + to_string(switchButtons.rightStickXplus));
    result.append("\nrightStickXminus: " + to_string(switchButtons.rightStickXminus));
    result.append("\nrightStickYplus:  " + to_string(switchButtons.rightStickYplus));
    result.append("\nrightStickYminus: " + to_string(switchButtons.rightStickYminus));
    result.append("\ndpadUp:           " + to_string(switchButtons.dpadUp));
    result.append("\ndpadRight:        " + to_string(switchButtons.dpadRight));
    result.append("\ndpadDown:         " + to_string(switchButtons.dpadDown));
    result.append("\ndpadLeft:         " + to_string(switchButtons.dpadLeft));
    result.append("\nrecordMacro:      " + to_string(switchButtons.recordMacro));
    result.append("\nscreenshot:       " + to_string(switchButtons.screenshot));
    result.append("\n");


    result.append("\nPictures\n");
    for(unsigned int i = 0; i < pictures.size(); i++){
        result.append("index:" + to_string(i) + " width:" + to_string(pictures[i].size().width) + " height:" + to_string(pictures[i].size().height) + '\n');
    }

    return result;
}

#endif