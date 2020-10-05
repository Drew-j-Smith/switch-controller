/* 
 * File: Loader.h
 * Author: Drew Smith
 * Summary: loads a configs in a similar way to .ini
 * 
 */

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
protected:
    map<string, string> currentConfig;
    map<string, string> defaultConfig;
    map<string, vector<string>> catagories;

    void parseLine(istream &configStream, bool isDefault, string &currentCatagory);
    void removeDuplicateCatagories();

    void loadConfig(string configFilepath, bool isDefault);
    void loadConfig(istream &configStream, bool isDefault);
    void addDefaultElement(string catagory, string element, string content);
public:
    Loader();
    Loader(string configFilepath);
    Loader(string configFilepath, string defaultsFilepath);

    void addElement(string catagory, string element, string content);
    string getElement(string catagory, string element);

    map<string, vector<string>> getCatagories();

    string toString();
};

/*
 * Deafult Constuctor
 */
Loader::Loader(){
    catagories = {};
    currentConfig = {};
    defaultConfig = {};
}

/*
 * Loads a config from a file with no defaults set
 * 
 * @param configFilepath The path to file with the config
 */
Loader::Loader(string configFilepath){
    catagories = {};
    loadConfig(configFilepath, false);
    defaultConfig = {};
}

/* 
 * Loads a config and a defualt config from files
 * 
 * @param configFilepath The path to file with the main config
 * @param defaultsFilepath The path to file with the default config
 */
Loader::Loader(string configFilepath, string defaultsFilepath){
    catagories = {};
    loadConfig(defaultsFilepath, true);
    loadConfig(configFilepath, false);
}

/*
 * Loads a config from a file
 * 
 * @see loadConfig
 * 
 * @param configFilepath The path to file with the config
 * @param isDefault specifies if the config should be loaded as the main or default config
 */
void Loader::loadConfig(string configFilepath, bool isDefault){
    ifstream configStream;
	configStream.open(configFilepath, ios::in);

    if (configStream) {
        loadConfig(configStream, isDefault);
    }
    else{
        cerr << "Could not open file " << configFilepath << '\n';
    }

    configStream.close();
}

/*
 * Loads a config from a stream. Overwrites defaultConfig or currentConfig depending on isDefault
 * 
 * @see parseLine
 * @see removeDuplicateCatagories
 * 
 * @param configStream The stream with the config
 * @param isDefault specifies if the config should be loaded as the main or default config
 */
void Loader::loadConfig(istream &configStream, bool isDefault){
    if (isDefault)
        defaultConfig = {};
    else
        currentConfig = {};

	string currentCatagory = "";

    while (!configStream.eof()){
        parseLine(configStream, isDefault, currentCatagory);
    }

    removeDuplicateCatagories();
}


/* 
 * Helper method to loadConfig. Gets the next line of configStream and either sets currentCatagory, adds an element (which may add to catagories) or does nothing
 * 
 * @see loadConfig
 * 
 * @param configStream The stream with the config
 * @param isDefault Specifies if the config should be loaded as the main or default config
 * @param currentCatagory the Catagory that the next element would be stored in
 */
void Loader::parseLine(istream &configStream, bool isDefault, string& currentCatagory){
    string lineString;
    getline(configStream, lineString);

    if(lineString.length() == 0 || lineString.at(0) == '#'){
        return;
    }
    else if(lineString.at(0) == '['){
        currentCatagory = lineString.substr(1, lineString.find(']') - 1);
    }
    else if(currentCatagory != ""){
        int equalSymbolPos = lineString.find('=');
        if (equalSymbolPos == -1)
            return;
        string currentElement = lineString.substr(0, equalSymbolPos);
        string currentContent = lineString.substr(equalSymbolPos + 1, lineString.length() - equalSymbolPos - 1);
        
        
        if (isDefault){
            addDefaultElement(currentCatagory, currentElement, currentContent);
        }
        else{
            if(defaultConfig.size() != 0){
                int endChar = currentCatagory.length() - 1;
                while (endChar > 0 && currentCatagory.at(endChar) >= '0' && currentCatagory.at(endChar) <= '9'){
                    endChar--;
                }
                if(catagories.find(currentCatagory.substr(0, endChar + 1)) == catagories.end())
                    cerr << "Warning: The catagory \"" << currentCatagory << "\" was not in the default config but was used.\n";
                if(defaultConfig.find(currentCatagory.substr(0, endChar + 1) + ":" + currentElement) == defaultConfig.end())
                    cerr << "Warning: The element \"" << currentElement << "\" in the catagory \"" <<
                    currentCatagory << "\" was not in the default config but was used.\n";
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

/* 
 * Helper method to loadConfig. Removes duplicate entries in the vectors of catagories
 * 
 * @see loadConfig
 * 
 */
void Loader::removeDuplicateCatagories(){
    for(map<string, vector<string>>::iterator it = catagories.begin(); it != catagories.end(); ++it){
        vector<string> elements = {};
        for(unsigned int i = 0; i < it->second.size(); i++){
            unsigned int j = 0;
            while (j < elements.size()){
                if (it->second[i] == elements[j])
                    break;
                j++;
            }
            if (j == elements.size())
                    elements.push_back(it->second[i]);
        }
        it->second = elements;
    }
}

/*
 * Adds an element to currentConfig in the form of {catagory + ":" + element, content}
 * 
 * @param catagory The catagory the content is added
 * @param element The element the content is added
 * @param content the value to be stored in the catagory and element
 */
void Loader::addElement(string catagory, string element, string content){
    currentConfig.insert({catagory + ":" + element, content});
}

/*
 * Adds an element to defaultConfig in the form of {catagory:element, content}
 * 
 * @param catagory The catagory the content is added
 * @param element The element the content is added
 * @param content the value to be stored in the catagory and element
 */
void Loader::addDefaultElement(string catagory, string element, string content){
    defaultConfig.insert({catagory + ":" + element, content});
}

/*
 * Attempts to get the content at {catagory:element}. Will try to retrieve from the main config and then the default config. 
 * Will attempt to retireve from deafult if content in main config has length 0.
 * 
 * @param catagory The catagory that is checked
 * @param element The element that is checked
 */
string Loader::getElement(string catagory, string element){
    if(currentConfig.find(catagory + ":" + element) != currentConfig.end() && currentConfig.at(catagory + ":" + element).length() != 0)
        return currentConfig.at(catagory + ":" + element);
    else if(defaultConfig.find(catagory + ":" + element) != defaultConfig.end())
        return defaultConfig.at(catagory + ":" + element);

    int endChar = catagory.length() - 1;
    while (endChar > 0 && catagory.at(endChar) >= '0' && catagory.at(endChar) <= '9'){
        endChar--;
    }
    if(defaultConfig.find(catagory.substr(0, endChar + 1) + ":" + element) != defaultConfig.end())
        return defaultConfig.at(catagory.substr(0, endChar + 1) + ":" + element);
    
    return "";
}

string Loader::toString(){
    string result = "Current config:\n";
    for (map<string,string>::iterator it=currentConfig.begin(); it!=currentConfig.end(); ++it)
        result.append(it->first + " => " + it->second + '\n');

    result.append("\nDefault config:\n");
    for (map<string,string>::iterator it=defaultConfig.begin(); it!=defaultConfig.end(); ++it)
        result.append(it->first + " => " + it->second + '\n');

    result.append("\nEvaluated config:\n");
    for(map<string, vector<string>>::iterator it = catagories.begin(); it != catagories.end(); ++it){
        for(unsigned int i = 0; i < it->second.size(); i++){
            result.append(it->first + ":" + it->second[i] + " => " + getElement(it->first, it->second[i]) + '\n');
        }
        result.append("\n");
    }

    result.append("\nCatagories:\n");
    for(map<string, vector<string>>::iterator it = catagories.begin(); it != catagories.end(); ++it){
        result.append(it->first + '\n');
        for(unsigned int i = 0; i < it->second.size(); i++){
            result.append(it->second.at(i) + " ");
        }
        result.append("\n\n");
    }

    return result;
}

map<string, vector<string>> Loader::getCatagories(){
    return catagories;
}

#endif