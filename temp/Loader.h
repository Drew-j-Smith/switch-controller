#ifndef LOADER_H
#define LOADER_H

#include <map>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>

using namespace std;

class Loader{
private:
    map<string, string> currentConfig;
    map<string, string> defaultConfig;
    map<string, vector<string>> catagories;

    void parseLine();
public:
    Loader();
    Loader(string configFilepath);
    Loader(string configFilepath, string defaultsFilepath);
    void loadConfigFromFile(string configFilepath, bool isDefault);
    void loadConfig(istream &configStream, bool isDefault);

    void addElement(string catagory, string element, string content);
    void addDefaultElement(string catagory, string element, string content);
    string getElement(string catagory, string element);

    vector<string> getCatagories();

    string toString();
};


Loader::Loader(){
    catagories = {};
    currentConfig = {};
    defaultConfig = {};
}

Loader::Loader(string configFilepath){
    catagories = {};
    loadConfigFromFile(configFilepath, false);
    defaultConfig = {};
}

Loader::Loader(string configFilepath, string defaultsFilepath){
    catagories = {};
    loadConfigFromFile(defaultsFilepath, true);
    loadConfigFromFile(configFilepath, false);
}

void Loader::loadConfigFromFile(string configFilepath, bool isDefault){
    ifstream configStream;
	configStream.open(configFilepath, std::ios::in);

    if (configStream) {
        loadConfig(configStream, isDefault);
    }
    else{
        cerr << "Could not open file " << configFilepath << '\n';
    }

    configStream.close();
}

void Loader::loadConfig(istream &configStream, bool isDefault){
    if (isDefault)
        defaultConfig = {};
    else
        currentConfig = {};

	string lineString, currentCatagory = "";

    while (!configStream.eof()){
        getline(configStream, lineString);

        if(lineString.length() == 0 || lineString.at(0) == '#'){
            continue;
        }
        else if(lineString.at(0) == '['){
            currentCatagory = lineString.substr(1, lineString.find(']') - 1);
        }
        else if(currentCatagory != ""){
            int equalSymbolPos = lineString.find('=');
            if (equalSymbolPos == -1)
                continue;
            string currentElement = lineString.substr(0, equalSymbolPos);
            string currentContent = lineString.substr(equalSymbolPos + 1, lineString.length() - equalSymbolPos - 1);
            
            
            if (isDefault){
                addDefaultElement(currentCatagory, currentElement, currentContent);
            }
            else{
                if(defaultConfig.size() != 0){
                    if(catagories.find(currentCatagory) == catagories.end())
                        cerr << "The catagory " << currentCatagory << " was not in the default config but was used.\n";
                    else if(defaultConfig.find(currentCatagory + ":" + currentElement) == defaultConfig.end())
                        cerr << "The element " << currentElement << " was not in the default config but was used.\n";
                }

                addElement(currentCatagory, lineString.substr(0, equalSymbolPos),
                lineString.substr(equalSymbolPos + 1, lineString.length() - equalSymbolPos - 1));
            }

            if(catagories.find(currentCatagory) == catagories.end()){
                catagories.insert({currentCatagory, {currentElement}});
            }
            else{
                catagories.at(currentCatagory).push_back(currentElement);
            }
        }
    }

    for(std::map<string, vector<string>>::iterator it = catagories.begin(); it != catagories.end(); ++it){
        std::vector<string>::iterator elementIt;
        elementIt = std::unique(it->second.begin(), it->second.end());
        it->second.resize(std::distance(it->second.begin(),elementIt));
    }
}


void Loader::addElement(string catagory, string element, string content){
    currentConfig.insert({catagory + ":" + element, content});
}

void Loader::addDefaultElement(string catagory, string element, string content){
    defaultConfig.insert({catagory + ":" + element, content});
}

string Loader::getElement(string catagory, string element){
    if(currentConfig.find(catagory + ":" + element) != currentConfig.end())
        return currentConfig.at(catagory + ":" + element);
    else if(defaultConfig.find(catagory + ":" + element) != defaultConfig.end())
        return defaultConfig.at(catagory + ":" + element);
    return "";
}

string Loader::toString(){
    string result = "";
    for (std::map<string,string>::iterator it=currentConfig.begin(); it!=currentConfig.end(); ++it)
        result += it->first + " => " + it->second + '\n';

    result += '\n';

    for (std::map<string,string>::iterator it=defaultConfig.begin(); it!=defaultConfig.end(); ++it)
        result += it->first + " => " + it->second + '\n';

    result += '\n';

    for(std::map<string, vector<string>>::iterator it = catagories.begin(); it != catagories.end(); ++it){
        result += it->first + '\n';
        for(int i = 0; i < it->second.size(); i++){
            result += it->second.at(i) + " ";
        }
        result += '\n';
    }

    return result;
}


#endif