#ifndef WINDOWS_SCREENSHOT_UTILITY_H
#define WINDOWS_SCREENSHOT_UTILITY_H

#include "pch.h"

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include "ScreenshotUtility.h"

#include <windows.h>

class WindowsScreenshotUtility : public ScreenshotUtility {
private:
    int windowWidth; 
    int windowHeight; 
    HDC hwindowDC;
    HWND hwnd;
    cv::Mat tempMat;
    
public:
    WindowsScreenshotUtility(int windowWidth, int windowHeight, const std::string & windowName) {
        this->windowWidth = windowWidth;
        this->windowHeight = windowHeight;
        hwnd = FindWindowA(NULL, windowName.c_str());
        hwindowDC = GetDC(hwnd);

        tempMat.create(windowHeight, windowWidth, CV_8UC4);
    }
    ~WindowsScreenshotUtility() {
        ReleaseDC(hwnd, hwindowDC);
    }

    void screenshot(cv::Mat& m) override {
        HDC hwindowCompatibleDC = CreateCompatibleDC(hwindowDC);
        SetStretchBltMode(hwindowCompatibleDC, COLORONCOLOR);

        HBITMAP hbwindow = CreateCompatibleBitmap(hwindowDC, windowWidth, windowHeight);

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

        SelectObject(hwindowCompatibleDC, hbwindow);
        StretchBlt(hwindowCompatibleDC, 0, 0, windowWidth, windowHeight, hwindowDC, 0, 0, windowWidth, windowHeight, SRCCOPY);
        GetDIBits(hwindowDC, hbwindow, 0, windowHeight, tempMat.data, (BITMAPINFO *)&bi, DIB_RGB_COLORS);

        DeleteObject(hbwindow);
        DeleteDC(hwindowCompatibleDC);

        cv::cvtColor(tempMat, m, cv::COLOR_BGRA2BGR);
    }
};



#endif