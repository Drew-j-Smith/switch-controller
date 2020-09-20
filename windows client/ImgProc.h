#ifndef IMGPROC_H
#define IMGPROC_H


#include "Loader.h"
#include <Windows.h>
#include <string>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include "opencv2/imgproc.hpp"



const bool displayImg = false;

class ImgProc {
public:
	ImgProc();
	ImgProc(Loader &l);

	void saveImg(cv::Mat& m, std::string filename);
	void showImg(cv::Mat& m, std::string windowName);
	bool screenshot(cv::Mat& m);

	bool update();

	std::vector<bool> getImgMatch();

	
	void matchTemplate(cv::Mat& img, cv::Mat& templ, cv::Mat& result, int match_method, double &criticalVal, cv::Point &matchPoint, cv::Mat mask = cv::Mat());
private:
	bool isMatch(double value, double threshold, int match_method, cv::Point matchPoint, cv::Point minPoint, cv::Point maxPoint);

	Loader loader;

	HDC hwindowDC;

	std::vector<bool> imgMatch;
	cv::Mat scrnsht;
	cv::Mat templ;
	cv::Mat mask;
	cv::Mat rslt;
	std::vector<double> critcalVals;
	std::vector<cv::Point> matchPoints;

	
};



ImgProc::ImgProc(){
	loader = Loader();
}

ImgProc::ImgProc(Loader &l) {
	loader = l;

	imgMatch.resize(loader.getNumMacros());
	critcalVals.resize(loader.getNumMacros());
	matchPoints.resize(loader.getNumMacros());

	HWND window = FindWindow(NULL, loader.getLocGameWindowName().c_str());
	hwindowDC = GetDC(window);

}


bool ImgProc::update() {
	screenshot(scrnsht);
	//showImg(scrnsht, "0");

	bool update = false;
	for (int i = 0; i < loader.getNumMacros(); i++) {
		
		if (!loader.getEnableMacroImgProc(i) || loader.getSharedMacroTemplates(i) != -1) {
			continue;
		}
		else {
			update = true;
			cv::Rect rectCrop = cv::Rect(loader.getMinMacroSearchPoints(i).x,
				loader.getMinMacroSearchPoints(i).y,
				loader.getMaxMacroSearchPoints(i).x - loader.getMinMacroSearchPoints(i).x,
				loader.getMaxMacroSearchPoints(i).y - loader.getMinMacroSearchPoints(i).y);
			cv::Mat submat = cv::Mat(scrnsht, rectCrop);

			templ = loader.getPicture(loader.getMacroTemplates(i));
			if (loader.getMacroMasks(i) == -1) {
				matchTemplate(submat, templ, rslt, loader.getMacroMatchMethods(i), critcalVals[i], matchPoints[i]);
			}
			else {
				mask = loader.getPicture(loader.getMacroMasks(i));
				matchTemplate(submat, templ, rslt, loader.getMacroMatchMethods(i), critcalVals[i], matchPoints[i], mask);
			}
			matchPoints[i].x += loader.getMinMacroSearchPoints(i).x;
			matchPoints[i].y += loader.getMinMacroSearchPoints(i).y;
		}
	}
	if (!update)
		return false;

	for (int i = 0; i < loader.getNumMacros(); i++) {
		if (!loader.getEnableMacroImgProc(i)) {
			imgMatch[i] = false;
			continue;
		}
		else if (loader.getSharedMacroTemplates(i) != -1) {
			imgMatch[i] = isMatch(critcalVals[loader.getSharedMacroTemplates(i)], loader.getMatchThresholds(i),
				loader.getMacroMatchMethods(loader.getSharedMacroTemplates(i)), matchPoints[loader.getSharedMacroTemplates(i)], 
				loader.getMinMacroMatchPoints(i), loader.getMaxMacroMatchPoints(i));
		}
		else {
			imgMatch[i] = isMatch(critcalVals[i], loader.getMatchThresholds(i),
				loader.getMacroMatchMethods(i), matchPoints[i],
				loader.getMinMacroMatchPoints(i), loader.getMaxMacroMatchPoints(i));
			//std::cout << critcalVals[i] << '\n';
		}
	}

	return true;

}



void ImgProc::matchTemplate(cv::Mat& img, cv::Mat& templ, cv::Mat& result, int match_method, double &criticalVal, cv::Point &matchPoint, cv::Mat mask) {
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
	if (displayImg) {
		rectangle(img_display, matchPoint, cv::Point(matchPoint.x + templ.cols, matchPoint.y + templ.rows), cv::Scalar::all(0), 2, 8, 0);
		rectangle(result, matchPoint, cv::Point(matchPoint.x + templ.cols, matchPoint.y + templ.rows), cv::Scalar::all(0), 2, 8, 0);
		showImg(img_display, "1");
		showImg(result, "2");
	}
}


bool ImgProc::isMatch(double value, double threshold, int match_method, cv::Point matchPoint, cv::Point minPoint, cv::Point maxPoint) {
	if (match_method == cv::TM_SQDIFF || match_method == cv::TM_SQDIFF_NORMED) {
		return value < threshold && 
			matchPoint.x >= minPoint.x && 
			matchPoint.y >= minPoint.y && 
			matchPoint.x <= maxPoint.x && 
			matchPoint.y <= maxPoint.y;
	}
	else {
		return threshold < value &&
		matchPoint.x >= minPoint.x &&
		matchPoint.y >= minPoint.y &&
		matchPoint.x <= maxPoint.x &&
		matchPoint.y <= maxPoint.y;
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

	HBITMAP hbwindow = CreateCompatibleBitmap(hwindowDC, loader.getLocWindowWidth(), loader.getLocWindowHeight());
	if (!hbwindow)
	{
		DeleteDC(hwindowCompatibleDC);
		return false;
	}

	BITMAPINFOHEADER bi;
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = loader.getLocWindowWidth();
	bi.biHeight = -loader.getLocWindowHeight();
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
		loader.getLocWindowWidth(), loader.getLocWindowHeight(),
		hwindowDC,
		0, 0,
		loader.getLocWindowWidth(), loader.getLocWindowHeight(),
		SRCCOPY))
	{
		DeleteObject(hbwindow);
		DeleteDC(hwindowCompatibleDC);
		return false;
	}

	tempMat.create(loader.getLocWindowHeight(), loader.getLocWindowWidth(), CV_8UC4);

	if (!GetDIBits(
		hwindowDC,
		hbwindow,
		0,
		loader.getLocWindowHeight(),
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




#endif
