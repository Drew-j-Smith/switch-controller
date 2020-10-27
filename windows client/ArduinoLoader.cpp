#include "ArduinoLoader.h"
#include "pch.h"

ArduinoLoader::ArduinoLoader(){
    config = boost::property_tree::ptree();
}

ArduinoLoader::ArduinoLoader(std::string filename){
    loadConfig(filename);
}

void ArduinoLoader::loadConfig(std::string filename){
    boost::property_tree::read_json(filename, config);

    std::map<std::string, int> pictureIndicices;
    std::map<std::string, int> macroIndicices;
    reloadPictures(pictures, pictureIndicices);
    reloadMacros(macros, macroIndicices, pictureIndicices);
    switchButtons = reloadSwitchButtons();
}

#pragma region privateMethods

void ArduinoLoader::loadMacro(std::string filename, std::shared_ptr<char[]> & data, int & length){
    std::ifstream infile;
    std::string stringLine;
    char inputArray[8];
    char buffer[10000];
    int bufferPos = 0;

    infile.open(filename, std::ios::in);

    if (infile) {
        while (!infile.eof() && bufferPos * 8 < 10000) {
            getline(infile, stringLine);
            if (stringLine.length() != 24)
                continue;
            for (int i = 0; i < 8; i++) {
                inputArray[i] = std::stoi(stringLine.substr(i * 3, 3)) - 128;
            }
            memcpy(buffer + bufferPos * 8, inputArray, 8);
            bufferPos++;
        }

        if(bufferPos * 8 > 10000){
            std::cerr << "ERROR: Buffer was overrun.\n";
        }

        data = std::shared_ptr<char[]>(new char[bufferPos * 8]);
        memcpy(data.get(), buffer, bufferPos * 8);
        length = bufferPos * 8;
    }
    else
    {
        std::cerr << "Could not open file " + filename << "\n";
        length = 0;
    }
    infile.close();

}

cv::Mat ArduinoLoader::loadPicture(std::string filename) {
    cv::Mat image;
    image = cv::imread(filename, cv::IMREAD_COLOR); // Read the file
    if (image.empty()) // Check for invalid input
    {
        std::cerr << "Could not open file " << filename << "\n";
    }
    return image;
}

void ArduinoLoader::reloadMacros(std::vector<Macro> &macros, std::map<std::string, int> &macroIndicices, std::map<std::string, int> pictureIndicices){
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
        macros[i].searchMinX     = currentMacro.get("shared settings.search min x", 0);
        macros[i].searchMinY     = currentMacro.get("shared settings.search min y", 0);
        macros[i].searchMaxX     = currentMacro.get("shared settings.search max x", config.get("general.window width", 0));
        macros[i].searchMaxY     = currentMacro.get("shared settings.search max y", config.get("general.window height", 0));
        macros[i].matchThreshold = currentMacro.get("match threshold", 0.0);
        macros[i].minX           = currentMacro.get("min x", 0);
        macros[i].minY           = currentMacro.get("min y", 0);
        macros[i].maxX           = currentMacro.get("max x", config.get("general.window width", 0));
        macros[i].maxY           = currentMacro.get("max y", config.get("general.window height", 0));

        loadMacro(config.get("general.macro folder", "") + currentMacro.get("filename", ""), macros[i].data, macros[i].macroLength);

        std::string button = currentMacro.get("button", "-1");
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

        std::string templatePic = currentMacro.get("shared settings.template image", "");
        if(pictureIndicices.find(templatePic) != pictureIndicices.end())
            macros[i].templatePic = pictureIndicices.at(templatePic);
        else
            macros[i].templatePic = -1;
        
        std::string maskPic = currentMacro.get("shared settings.mask image", "");
        if(pictureIndicices.find(maskPic) != pictureIndicices.end())
            macros[i].maskPic = pictureIndicices.at(maskPic);
        else
            macros[i].maskPic = -1;


        std::string macroTemplate = currentMacro.get("share settings from macro", "");
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
                if(macroIndicices.find(it->second.get("","")) != macroIndicices.end()){
                    macros[i].macroSuccessList.push_back(macroIndicices.at(it->second.get("","")));
                }
            }
        }

        macros[i].macroFailList = {};
        if(currentMacro.find("next macro fail") != currentMacro.not_found()){
            nextMacroList = currentMacro.find("next macro fail")->second;
            for(boost::property_tree::ptree::iterator it = nextMacroList.begin(); it != nextMacroList.end(); it++){
                if(macroIndicices.find(it->second.get("","")) != macroIndicices.end()){
                    macros[i].macroFailList.push_back(macroIndicices.at(it->second.get("","")));
                }
            }
        }

        macros[i].macroDefaultList = {};
        if(currentMacro.find("next macro default") != currentMacro.not_found()){
            nextMacroList = currentMacro.find("next macro default")->second;
            for(boost::property_tree::ptree::iterator it = nextMacroList.begin(); it != nextMacroList.end(); it++){
                if(macroIndicices.find(it->second.get("","")) != macroIndicices.end()){
                    macros[i].macroDefaultList.push_back(macroIndicices.at(it->second.get("","")));
                }
            }
        }
        
    }

}

void ArduinoLoader::reloadPictures(std::vector<cv::Mat> &pictures, std::map<std::string, int> &pictureIndicices){
    pictures = {};
    pictureIndicices = {};

    boost::property_tree::ptree picturePtree = config.find("pictures")->second;

    for(boost::property_tree::ptree::iterator it = picturePtree.begin(); it != picturePtree.end(); it++){
        pictureIndicices.insert({it->first, pictures.size()});
        pictures.push_back(loadPicture(config.get("general.picture folder","") + it->second.get("picture filename", "")));
    }
}

SwitchButtons ArduinoLoader::reloadSwitchButtons(){
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
    switchButtons.lClick           = getButton("lClick");
    switchButtons.rRlick           = getButton("rRlick");
    switchButtons.home             = getButton("home");
    switchButtons.capture          = getButton("capture");
    switchButtons.leftStickXplus   = getButton("leftStickXplus");
    switchButtons.leftStickXminus  = getButton("leftStickXminus");
    switchButtons.leftStickYplus   = getButton("leftStickYplus");
    switchButtons.leftStickYminus  = getButton("leftStickYminus");
    switchButtons.rightStickXplus  = getButton("rightStickXplus");
    switchButtons.rightStickXminus = getButton("rightStickXminus");
    switchButtons.rightStickYplus  = getButton("rightStickYplus");
    switchButtons.rightStickYminus = getButton("rightStickYminus");
    switchButtons.dpadUp           = getButton("dpadUp");
    switchButtons.dpadRight        = getButton("dpadRight");
    switchButtons.dpadDown         = getButton("dpadDown");
    switchButtons.dpadLeft         = getButton("dpadLeft");
    switchButtons.recordMacro      = getButton("recordMacro");
    switchButtons.screenshot       = getButton("screenshot");
    return switchButtons;
}

int ArduinoLoader::getButton(std::string button){
    std::string buttonCode = config.get("controls." + button, "-1");
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

#pragma endregion privateMethods

std::string ArduinoLoader::toString(){
    std::string result;
    std::stringstream outstream = std::stringstream("");

    boost::property_tree::json_parser::write_json(outstream, config);

    result += outstream.str();

    result.append("\nMacros\n");
    for(unsigned int i = 0; i < macros.size(); i++){
        result.append("\n" + macros[i].name);
        result.append("\nmacro length:   " + std::to_string(macros[i].macroLength));
        result.append("\nbutton:         " + std::to_string(macros[i].button));
        result.append("\nenableImgProc:  " + std::to_string(macros[i].enableImgProc));
        result.append("\ntemplatePic:    " + std::to_string(macros[i].templatePic));
        result.append("\nmaskPic:        " + std::to_string(macros[i].maskPic));
        result.append("\nmatchMethod:    " + std::to_string(macros[i].matchMethod));
        result.append("\nsearchMinX:     " + std::to_string(macros[i].searchMinX));
        result.append("\nsearchMinY:     " + std::to_string(macros[i].searchMinY));
        result.append("\nsearchMaxX:     " + std::to_string(macros[i].searchMaxX));
        result.append("\nsearchMaxY:     " + std::to_string(macros[i].searchMaxY));
        result.append("\nmacroTemplate:  " + std::to_string(macros[i].macroTemplate));
        result.append("\nmatchThreshold: " + std::to_string(macros[i].matchThreshold));
        result.append("\nminX:           " + std::to_string(macros[i].minX));
        result.append("\nminY:           " + std::to_string(macros[i].minY));
        result.append("\nmaxX:           " + std::to_string(macros[i].maxX));
        result.append("\nmaxY:           " + std::to_string(macros[i].maxY));
        result.append("\n");
    }

    result.append("\nControls");
    result.append("\ny:                " + std::to_string(switchButtons.y));
    result.append("\nb:                " + std::to_string(switchButtons.b));
    result.append("\na:                " + std::to_string(switchButtons.a));
    result.append("\nx:                " + std::to_string(switchButtons.x));
    result.append("\nl:                " + std::to_string(switchButtons.l));
    result.append("\nr:                " + std::to_string(switchButtons.r));
    result.append("\nxl:               " + std::to_string(switchButtons.xl));
    result.append("\nxr:               " + std::to_string(switchButtons.xr));
    result.append("\nselect:           " + std::to_string(switchButtons.select));
    result.append("\nstart:            " + std::to_string(switchButtons.start));
    result.append("\nlClick:           " + std::to_string(switchButtons.lClick));
    result.append("\nrRlick:           " + std::to_string(switchButtons.rRlick));
    result.append("\nhome:             " + std::to_string(switchButtons.home));
    result.append("\ncapture:          " + std::to_string(switchButtons.capture));
    result.append("\nleftStickXplus:   " + std::to_string(switchButtons.leftStickXplus));
    result.append("\nleftStickXminus:  " + std::to_string(switchButtons.leftStickXminus));
    result.append("\nleftStickYplus:   " + std::to_string(switchButtons.leftStickYplus));
    result.append("\nleftStickYminus:  " + std::to_string(switchButtons.leftStickYminus));
    result.append("\nrightStickXplus:  " + std::to_string(switchButtons.rightStickXplus));
    result.append("\nrightStickXminus: " + std::to_string(switchButtons.rightStickXminus));
    result.append("\nrightStickYplus:  " + std::to_string(switchButtons.rightStickYplus));
    result.append("\nrightStickYminus: " + std::to_string(switchButtons.rightStickYminus));
    result.append("\ndpadUp:           " + std::to_string(switchButtons.dpadUp));
    result.append("\ndpadRight:        " + std::to_string(switchButtons.dpadRight));
    result.append("\ndpadDown:         " + std::to_string(switchButtons.dpadDown));
    result.append("\ndpadLeft:         " + std::to_string(switchButtons.dpadLeft));
    result.append("\nrecordMacro:      " + std::to_string(switchButtons.recordMacro));
    result.append("\nscreenshot:       " + std::to_string(switchButtons.screenshot));
    result.append("\n");


    result.append("\nPictures\n");
    for(unsigned int i = 0; i < pictures.size(); i++){
        result.append("index:" + std::to_string(i) + " width:" + std::to_string(pictures[i].size().width) + " height:" + std::to_string(pictures[i].size().height) + '\n');
    }

    return result;
}

std::vector<cv::Mat> ArduinoLoader::getPictures(){
    return pictures;
}

std::vector<Macro> ArduinoLoader::getMacros(){
    return macros;
}

SwitchButtons ArduinoLoader::getSwitchButtons(){
    return switchButtons;
}

std::string ArduinoLoader::getOption(std::string option){
    return config.get("general." + option, "");
}