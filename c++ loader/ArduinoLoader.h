#ifndef ARDUINO_LOADER_H
#define ARDUINO_LOADER_H

#include <string>
#include <vector>
#include <map>
#include <sstream>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using namespace std;

//cmake -DCMAKE_TOOLCHAIN_FILE=C:/dev/vcpkg/scripts/buildsystems/vcpkg.cmake .
//todo button to start/stop recording
//todo save as current date
//todo add file errors


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

class ArduinoLoader{
private:

    vector<cv::Mat> pictures;
    vector<macro> macros;

    boost::property_tree::ptree config;


    void reloadPictures(vector<cv::Mat> &pictures, map<string, int> &pictureIndicices);
    void reloadMacros(vector<macro> &macros, map<string, int> &macroIndicices, map<string, int> pictureIndicices);


    std::vector<std::array<char, 8>> loadMacro(string filepath);
    cv::Mat loadPicture(string filepath);
public:
    ArduinoLoader();
    ArduinoLoader(string filepath);

    void loadConfig(string filepath);

    void reloadPicturesAndMacros();

    string toString();
};

ArduinoLoader::ArduinoLoader(){
    config = boost::property_tree::ptree();
}

ArduinoLoader::ArduinoLoader(string filepath){
    boost::property_tree::read_json(filepath, config);
}

void ArduinoLoader::loadConfig(string filepath){
    boost::property_tree::read_json(filepath, config);

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

    boost::property_tree::ptree macroPtree = config.find("macros")->second;

    for(boost::property_tree::ptree::iterator it = macroPtree.begin(); it != macroPtree.end(); it++){
        macroIndicices.insert({it->first, macros.size()});
        macro currentMacro;
        currentMacro.name = it->first;
        macros.push_back(currentMacro);
    }

    for(int i = 0; i < macros.size(); i++){
        boost::property_tree::ptree currentMacro = macroPtree.find(macros[i].name)->second;

        macros[i].enableImgProc = currentMacro.get("enable image match", false);
        macros[i].matchMethod = currentMacro.get("shared settings.match method", 3);
        macros[i].searchMinX = currentMacro.get("search min x", 0);
        macros[i].searchMinY = currentMacro.get("search min y", 0);
        macros[i].searchMaxX = currentMacro.get("search max x", config.get("general.window width", 0));
        macros[i].searchMaxY = currentMacro.get("search max y", config.get("general.window height", 0));
        macros[i].matchThreshold = currentMacro.get("shared settings.match threshold", 0);
        macros[i].minX = currentMacro.get("shared settings.min x", 0);
        macros[i].minY = currentMacro.get("shared settings.min y", 0);
        macros[i].maxX = currentMacro.get("shared settings.max x", config.get("general.window width", 0));
        macros[i].maxY = currentMacro.get("shared settings.max y", config.get("general.window height", 0));

        macros[i].data = loadMacro(config.get("general.macro folder", "") + currentMacro.get("filename", ""));

        string button = currentMacro.get("button", "-1");
        if(button.at(0) >= 'a' && button.at(0) <= 'z'){
            macros[i].button = button.at(0) - 'a';
        }
        else if(button.at(0) >= 'A' && button.at(0) <= 'Z'){
            macros[i].button = button.at(0) - 'A';
        }
        else{
            macros[i].button = stoi(button);
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


        boost::property_tree::ptree nextMacroList = currentMacro.find("next macro success")->second;
        for(boost::property_tree::ptree::iterator it = nextMacroList.begin(); it != nextMacroList.end(); it++){
            if(macroIndicices.find(it->first) != macroIndicices.end())
                macros[i].macroSuccessList.push_back(macroIndicices.at(it->first));
        }

        nextMacroList = currentMacro.find("next macro fail")->second;
        for(boost::property_tree::ptree::iterator it = nextMacroList.begin(); it != nextMacroList.end(); it++){
            if(macroIndicices.find(it->first) != macroIndicices.end())
                macros[i].macroFailList.push_back(macroIndicices.at(it->first));
        }

        nextMacroList = currentMacro.find("next macro default")->second;
        for(boost::property_tree::ptree::iterator it = nextMacroList.begin(); it != nextMacroList.end(); it++){
            if(macroIndicices.find(it->first) != macroIndicices.end())
                macros[i].macroDefaultList.push_back(macroIndicices.at(it->first));
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

void ArduinoLoader::reloadPicturesAndMacros(){
    map<string, int> pictureIndicices;
    map<string, int> macroIndicices;
    reloadPictures(pictures, pictureIndicices);
    reloadMacros(macros, macroIndicices, pictureIndicices);
}

string ArduinoLoader::toString(){
    string result;
    stringstream outstream = stringstream("");

    boost::property_tree::json_parser::write_json(outstream, config);

    result += outstream.str();

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

#endif