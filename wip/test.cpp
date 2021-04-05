
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Joystick.hpp>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include <windows.h>

#include <boost/property_tree/json_parser.hpp>

#include "pch.h"

#include "CharStream.h"
#include "ArduinoMacro.h"
#include "ArduinoMacros.h"
#include "MacroDecider.h"
#include "MacroImageProcessingDecider.h"
#include "SfKeyboardInputEvent.h"
#include "SfJoystickInputEvent.h"
#include "InputEventCollection.h"

bool screenshot(cv::Mat& m, int windowWidth, int windowHeight, HDC & hwindowDC);

int main(){
    // CharStream<1> a;
    // std::array<unsigned char, 1> b = {'a'};
    // a.push_back(b);
    // //std::cout << a[0][0] << std::endl;

    // a.save("test.txt", true);

    // CharStream<16> c;
    // c.load("test2.txt");
    // //std::cout << c.size() << std::endl;
    // //std::cout << std::hex << std::setfill('0');

    // c.save("test3.txt");

    // c.save("test4.txt", false);

    // c.load("test4.txt", false);
    // //c.print(std::cout);

    // Macro d;
    // unsigned char e[] = {1, 0, 3, 4, 5, 6, 7, 8};
    // d.appendData(0, e);
    // e[1] = 4;
    // d.appendData(4, e);
    // e[1] = 8;
    // d.appendData(8, e);
    // e[1] = 12;
    // d.appendData((0ULL)-1, e);

    // d.getDataframe(5, e);
    // std::cout << d.getTime(0) << std:: endl;
    // std::cout << d.getTime(1) << std:: endl;
    // std::cout << d.getTime(2) << std:: endl;
    // std::cout << d.getTime(3) << std:: endl;
    // std::cout << d.lastTime() << std:: endl;
    // for (int i = 0; i < 8; i++){
    //     std::cout << (int)e[i] << " ";
    // }
    // std::cout << std::endl;

    // d.getData()->save("test5.txt");

    // Macro f;
    // CharStream<15> g;
    // g.load("test5.txt");
    // f.setData(&g);
    // f.getData()->print(std::cout);

    // MacroDecider a;
    // std::cout << a.getName() << std::endl;
    // std::cout << a.nextMacroListIndex() << std::endl;

    // MacroDecider b("test");
    // std::cout << b.getName() << std::endl;
    // std::cout << b.nextMacroListIndex() << std::endl;
    
    cv::Mat temp = cv::imread("haanit.png");
    cv::Mat mask = cv::imread("haanit mask.png");

    //MacroImageProcessingDecider c("t", {temp, mask, cv::TM_CCORR_NORMED, .97, 0, 0, 1920, 1080});

    //MacroDecider* d = &c;
    MacroImageProcessingDecider::ImageProcessingInfo imageProcessingInfo = {temp, mask, cv::TM_CCORR_NORMED, .97, 0, 0, 500, 500};
    std::shared_ptr<MacroImageProcessingDecider> macroImageProcessingDecider = std::make_shared<MacroImageProcessingDecider>("t", imageProcessingInfo);
    std::shared_ptr<SfKeyboardInputEvent> sfKeyboardInputEvent = std::make_shared<SfKeyboardInputEvent>(sf::Keyboard::T);
    std::shared_ptr<SfJoystickDigitalInputEvent> sfJoystickInputEvent = std::make_shared<SfJoystickDigitalInputEvent>(0, 0);
    std::shared_ptr<SfJoystickAnalogInputEvent> sfJoystickInputEvent2 = std::make_shared<SfJoystickAnalogInputEvent>(0, sf::Joystick::X);
    std::vector<std::shared_ptr<InputEvent>> inputEventVector = {sfKeyboardInputEvent, sfJoystickInputEvent};
    std::shared_ptr<InputEventCollection> inputEventCollection = std::make_shared<InputEventCollection>(inputEventVector);
    boost::property_tree::ptree tree;
    boost::property_tree::read_json("test.json", tree);
    std::shared_ptr<InputEventCollection> inputEventCollection2 = std::make_shared<InputEventCollection>(tree);
    
    HWND window = FindWindowA(NULL, "Game Capture HD");
    HDC hwindowDC = GetDC(window);

    cv::Mat img;

    std::shared_ptr<Macro> f = std::make_shared<Macro>("1", CharStream<15>(), sfKeyboardInputEvent, macroImageProcessingDecider);
    std::vector<std::vector<std::weak_ptr<Macro>>> nextMacroLists = {{f}, {f}};
    f->setNextMacroLists(nextMacroLists);

    boost::property_tree::ptree tree2;
    boost::property_tree::read_json("test2.json", tree2);
    std::map<std::string, std::shared_ptr<MacroDecider>> deciderMap;
    deciderMap.insert({"t", macroImageProcessingDecider});
    auto g = std::make_shared<Macro>(tree2, deciderMap);

    g->getData()->save("test3.txt");
    std::cout << g->getName() << std::endl;

    std::thread thd([&](){
        while (true) {
            sf::Joystick::update();
            // std::cout << c.nextMacroListIndex() << std::endl;
            // std::cout << f->getInputEvent()->getInputValue() << std::endl;
            // std::cout << sfJoystickInputEvent->getInputValue() << std::endl;
            // std::cout << sfJoystickInputEvent2->getInputValue() << std::endl;
            // std::cout << inputEventCollection->getInputValue() << std::endl;
            // std::cout << inputEventCollection2->getInputValue() << std::endl;

            std::cout << "input" << g->getInputEvent()->getInputValue() << std::endl;
            std::cout << "decider" << g->getMacroDecider()->nextMacroListIndex() << std::endl;

            // f->getNextMacro();
            std::this_thread::sleep_for (std::chrono::milliseconds(500));
        }
    });

    while (true) {
        //auto begin = std::chrono::steady_clock::now();
        screenshot(img, 1920, 1080, hwindowDC);

        macroImageProcessingDecider->update(img);
        // auto end = std::chrono::steady_clock::now();
        //std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << std::endl;

        // cv::namedWindow("test", cv::WINDOW_AUTOSIZE);
        // cv::imshow("test", img);
        // cv::waitKey(1);
    }

    return 0;
}

bool screenshot(cv::Mat& m, int windowWidth, int windowHeight, HDC & hwindowDC){
    cv::Mat tempMat;
    HDC hwindowCompatibleDC = CreateCompatibleDC(hwindowDC);
    if (!hwindowCompatibleDC)
    {
        return false;
    }

    if (!SetStretchBltMode(hwindowCompatibleDC, COLORONCOLOR))
    {
        DeleteDC(hwindowCompatibleDC);
        return false;
    }

    HBITMAP hbwindow = CreateCompatibleBitmap(hwindowDC, windowWidth, windowHeight);
    if (!hbwindow)
    {
        DeleteDC(hwindowCompatibleDC);
        return false;
    }

    BITMAPINFOHEADER bi;
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = windowWidth;
    bi.biHeight = -windowHeight;
    bi.biPlanes = 1;
    bi.biBitCount = 32;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrUsed = 0;
    bi.biClrImportant = 0;

    if (!SelectObject(hwindowCompatibleDC, hbwindow))
    {
        DeleteObject(hbwindow);
        DeleteDC(hwindowCompatibleDC);
        return false;
    }

    if (!StretchBlt(
        hwindowCompatibleDC,
        0, 0,
        windowWidth, windowHeight,
        hwindowDC,
        0, 0,
        windowWidth, windowHeight,
        SRCCOPY))
    {
        DeleteObject(hbwindow);
        DeleteDC(hwindowCompatibleDC);
        return false;
    }

    tempMat.create(windowHeight, windowWidth, CV_8UC4);

    if (!GetDIBits(
        hwindowDC,
        hbwindow,
        0,
        windowHeight,
        tempMat.data,
        (BITMAPINFO *)&bi,
        DIB_RGB_COLORS))
    {
        tempMat.release();
        DeleteObject(hbwindow);
        DeleteDC(hwindowCompatibleDC);
        return false;
    }

    DeleteObject(hbwindow);
    DeleteDC(hwindowCompatibleDC);

    const bool success = tempMat.cols > 0 && tempMat.rows > 0;

    cv::cvtColor(tempMat, m, cv::COLOR_BGRA2BGR);
    return success;
}