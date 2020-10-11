#ifndef ARDUINO_STRUCTS_H
#define ARDUINO_STRUCTS_H

#include <vector>
#include <string>

struct macro{
    std::string name;
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

    std::vector<std::array<char, 8>> data;

    std::vector<int> macroSuccessList;
    std::vector<int> macroFailList;
    std::vector<int> macroDefaultList;
};

struct switchButtons{
    int y;
    int b;
    int a;
    int x;
    int l;
    int r;
    int xl;
    int xr;
    int select;
    int start;
    int lClick;
    int rRlick;
    int home;
    int capture;
    int leftStickXplus;
    int leftStickXminus;
    int leftStickYplus;
    int leftStickYminus;
    int rightStickXplus;
    int rightStickXminus;
    int rightStickYplus;
    int rightStickYminus;
    int dPadUp;
    int dPadRight;
    int dPadDown;
    int dPadLeft;
    int recordMacro;
    int screenshot;
};



#endif