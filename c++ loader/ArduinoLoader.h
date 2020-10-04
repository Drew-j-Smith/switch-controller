#ifndef ARDUINO_LOADER_H
#define ARDUINO_LOADER_H

#include "Loader.h"
#include <string>
#include <vector>
#include <sstream>

using namespace std;

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

class ArduinoLoader : public Loader{
private:
    std::vector<std::vector<std::array<char, 8>>> macros;
public:
    ArduinoLoader();
    ArduinoLoader(string filepath);

    void loadConfigFile(string filepath);
};

ArduinoLoader::ArduinoLoader(){
    stringstream ss = stringstream(defaultConfigString);
    loadConfig(ss, true);
}

ArduinoLoader::ArduinoLoader(string filepath){
    stringstream ss = stringstream(defaultConfigString);
    loadConfig(ss, true);
    loadConfigFile(filepath);
}

void ArduinoLoader::loadConfigFile(string filepath){
    loadConfigFromFile(filepath, false);
}






#endif