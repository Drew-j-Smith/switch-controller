#ifndef IMGPROC_H
#define IMGPROC_H


#include "ArduinoStructs.h"
#ifdef _WIN32
	#include <Windows.h>
#endif

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include "opencv2/imgproc.hpp"


class ImgProc {
public:
	ImgProc();
	ImgProc(std::vector<cv::Mat> pictures, std::vector<Macro> macros, std::string windowName, int windowWidth, int windowHeight);

	void saveImg(cv::Mat& m, std::string filename);
	void showImg(cv::Mat& m, std::string windowName);
	bool screenshot(cv::Mat& m);

	bool update();

	std::vector<bool> getImgMatch();

	
	void matchTemplate(cv::Mat& img, cv::Mat& templ, cv::Mat& result, int match_method, double &criticalVal, cv::Point &matchPoint, std::string windowName = "", cv::Mat mask = cv::Mat());
private:
	bool isMatch(double value, double threshold, int match_method, cv::Point matchPoint, int minX, int minY, int maxX, int maxY);


	#ifdef _WIN32
		HDC hwindowDC;
	#endif
	

	std::vector<cv::Mat> pictures;
	std::vector<Macro> macros;
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



#endif
