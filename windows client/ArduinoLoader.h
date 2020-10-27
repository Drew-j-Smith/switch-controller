#ifndef ARDUINO_LOADER_H
#define ARDUINO_LOADER_H

#include "ArduinoStructs.h"

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>


class ArduinoLoader{
private:
    std::vector<cv::Mat> pictures;
    std::vector<Macro> macros;
    SwitchButtons switchButtons;
    boost::property_tree::ptree config;

    void reloadPictures(std::vector<cv::Mat> &pictures, std::map<std::string, int> &pictureIndicices);
    void reloadMacros(std::vector<Macro> &macros, std::map<std::string, int> &macroIndicices, std::map<std::string, int> pictureIndicices);
    SwitchButtons reloadSwitchButtons();
    std::vector<std::array<char, 8>> loadMacro(std::string filename);
    cv::Mat loadPicture(std::string filename);
    int getButton(std::string button);
public:
    ArduinoLoader();
    ArduinoLoader(std::string filename);

    void loadConfig(std::string filename);

    std::vector<cv::Mat> getPictures();
    std::vector<Macro> getMacros();
    SwitchButtons getSwitchButtons();
    std::string getOption(std::string option);


    std::string toString();
};

#endif