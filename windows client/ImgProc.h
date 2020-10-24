#ifndef IMGPROC_H
#define IMGPROC_H


#include "ArduinoStructs.h"
#ifdef _WIN32
	#include <Windows.h>
#endif
#include <string>
#include <vector>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include "opencv2/imgproc.hpp"

using namespace std;

bool DISPLAY_SCREEN_CAP = false;
bool DISPLAY_IMAGE_MATCH = false;

class ImgProc {
public:
	ImgProc();
	ImgProc(vector<cv::Mat> pictures, vector<Macro> macros, string windowName, int windowWidth, int windowHeight);

	void saveImg(cv::Mat& m, std::string filename);
	void showImg(cv::Mat& m, std::string windowName);
	bool screenshot(cv::Mat& m);

	bool update();

	std::vector<bool> getImgMatch();

	
	void matchTemplate(cv::Mat& img, cv::Mat& templ, cv::Mat& result, int match_method, double &criticalVal, cv::Point &matchPoint, string windowName = "", cv::Mat mask = cv::Mat());
private:
	bool isMatch(double value, double threshold, int match_method, cv::Point matchPoint, int minX, int minY, int maxX, int maxY);


	#ifdef _WIN32
		HDC hwindowDC;
	#endif
	

	vector<cv::Mat> pictures;
	vector<Macro> macros;
	int windowWidth;
	int windowHeight;
	

	std::vector<bool> imgMatch;
	cv::Mat scrnsht;
	cv::Mat templ;
	cv::Mat mask;
	cv::Mat rslt;
	std::vector<double> critcalVals;
	std::vector<cv::Point> matchPoints;

	
};



ImgProc::ImgProc(){
	this->pictures = {};
	this->macros = {};
	this->windowWidth = 0;
	this->windowHeight = 0;
}

ImgProc::ImgProc(vector<cv::Mat> pictures, vector<Macro> macros, string windowName, int windowWidth, int windowHeight) {

	this->pictures = pictures;
	this->macros = macros;
	this->windowWidth = windowWidth;
	this->windowHeight = windowHeight;

	imgMatch.resize(macros.size());
	critcalVals.resize(macros.size());
	matchPoints.resize(macros.size());

	#ifdef _WIN32
		HWND window = FindWindowA(NULL, windowName.c_str());
		hwindowDC = GetDC(window);
	#endif

}


bool ImgProc::update() {
	screenshot(scrnsht);
	if(DISPLAY_SCREEN_CAP)
		showImg(scrnsht, "screenshot");

	bool update = false;
	for (unsigned int i = 0; i < macros.size(); i++) {
		
		if (!macros[i].enableImgProc || macros[i].macroTemplate != -1) {
			continue;
		}
		else {
			update = true;
			cv::Rect rectCrop = cv::Rect(macros[i].searchMinX,
				macros[i].searchMinY,
				macros[i].searchMaxX - macros[i].searchMinX,
				macros[i].searchMaxY - macros[i].searchMinY);
			cv::Mat submat = cv::Mat(scrnsht, rectCrop);

			templ = pictures[macros[i].templatePic];
			if (macros[i].maskPic == -1) {
				try{
					matchTemplate(submat, templ, rslt, macros[i].matchMethod, critcalVals[i], matchPoints[i], "picture" + to_string(i));
				}
				catch(const std::exception& e){
					std::cerr << e.what() << '\n';
					std::cerr << "Error with image match" << '\n';
					exit(-1);
				}
			}
			else {
				mask = pictures[macros[i].maskPic];
				try{
					matchTemplate(submat, templ, rslt, macros[i].matchMethod, critcalVals[i], matchPoints[i], "picture" + to_string(i), mask);
				}
				catch(const std::exception& e){
					std::cerr << e.what() << '\n';
					std::cerr << "Error with image match" << '\n';
					exit(-1);
				}
			}
			matchPoints[i].x += macros[i].searchMinX; //adjusting the match point so the coordinates match with the screenshot
			matchPoints[i].y += macros[i].searchMinY;
		}
	}
	if (!update)
		return false;
	for (unsigned int i = 0; i < macros.size(); i++) {
		if (!macros[i].enableImgProc) {
			imgMatch[i] = false;
			continue;
		}
		else if (macros[i].macroTemplate != -1) {
			imgMatch[i] = isMatch(critcalVals[macros[i].macroTemplate], macros[i].matchThreshold,
				macros[macros[i].macroTemplate].matchMethod, matchPoints[macros[i].macroTemplate], 
				macros[i].minX, macros[i].minY, macros[i].maxX, macros[i].maxY);
		}
		else {
			imgMatch[i] = isMatch(critcalVals[i], macros[i].matchThreshold,
				macros[i].matchMethod, matchPoints[i],
				macros[i].minX, macros[i].minY, macros[i].maxX, macros[i].maxY);
			// std::cout << critcalVals[i] << '\n';
		}
	}

	return true;

}



void ImgProc::matchTemplate(cv::Mat& img, cv::Mat& templ, cv::Mat& result, int match_method, double &criticalVal, cv::Point &matchPoint, string windowName, cv::Mat mask) {
	cv::Mat img_display;
	img.copyTo(img_display);
	int result_cols = img.cols - templ.cols + 1;
	int result_rows = img.rows - templ.rows + 1;
	result.create(result_rows, result_cols, CV_32FC1);
	if ((cv::TM_SQDIFF == match_method || match_method == cv::TM_CCORR_NORMED) && mask.cols > 0 && mask.rows > 0) {
		cv::matchTemplate(img, templ, result, match_method, mask);
	}
	else {
		cv::matchTemplate(img, templ, result, match_method);
	}
	//normalize(result, result, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());
	double minVal; double maxVal;
	cv::Point minLoc; cv::Point maxLoc;
	minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat());
	if (match_method == cv::TM_SQDIFF || match_method == cv::TM_SQDIFF_NORMED) {
		matchPoint = minLoc;
		criticalVal = minVal;
	}
	else {
		matchPoint = maxLoc;
		criticalVal = maxVal;
	}
	if (DISPLAY_IMAGE_MATCH) {
		rectangle(img_display, matchPoint, cv::Point(matchPoint.x + templ.cols, matchPoint.y + templ.rows), cv::Scalar::all(0), 2, 8, 0);
		rectangle(result, matchPoint, cv::Point(matchPoint.x + templ.cols, matchPoint.y + templ.rows), cv::Scalar::all(0), 2, 8, 0);
		showImg(img_display, windowName + "-1");
		showImg(result, windowName + "-2");
	}
}


bool ImgProc::isMatch(double value, double threshold, int match_method, cv::Point matchPoint, int minX, int minY, int maxX, int maxY) {
	if (match_method == cv::TM_SQDIFF || match_method == cv::TM_SQDIFF_NORMED) {
		return value < threshold && 
			matchPoint.x >= minX && 
			matchPoint.y >= minY && 
			matchPoint.x <= maxX && 
			matchPoint.y <= maxY;
	}
	else {
		return threshold < value &&
		matchPoint.x >= minX &&
		matchPoint.y >= minY &&
		matchPoint.x <= maxX &&
		matchPoint.y <= maxY;
	}
}


void ImgProc::saveImg(cv::Mat& m, std::string filename) {
	cv::imwrite(filename, m);
}

void ImgProc::showImg(cv::Mat& m, std::string windowName) {
	cv::namedWindow(windowName, cv::WINDOW_AUTOSIZE);
	cv::imshow(windowName, m);
	cv::waitKey(1);
}

std::vector<bool> ImgProc::getImgMatch() {
	return imgMatch;
}



bool ImgProc::screenshot(cv::Mat& m){
	
	#ifdef _WIN32
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
		
	#else
		m = cv::Mat();
		return false;
	#endif

}




#endif
