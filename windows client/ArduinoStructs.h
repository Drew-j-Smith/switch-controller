#ifndef ARDUINO_STRUCTS_H
#define ARDUINO_STRUCTS_H


struct Macro{
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

    std::shared_ptr<unsigned char[]> data;
    int macroLength;

    std::vector<int> macroSuccessList;
    std::vector<int> macroFailList;
    std::vector<int> macroDefaultList;
};

struct SwitchButtons{
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
    int dpadUp;
    int dpadRight;
    int dpadDown;
    int dpadLeft;
    int recordMacro;
    int screenshot;
};



#endif