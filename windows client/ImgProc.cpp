#include "ImgProc.h"
#include "pch.h"

#define AC_DISPLAY_SCREEN_CAP 0
#define AC_DISPLAY_IMAGE_MATCH 0

ImgProc::ImgProc()
	:windowWidth(0), windowHeight(0), pictureFolder(""), screenshotButton(-1)
{
}

ImgProc::ImgProc(std::vector<cv::Mat> pictures, std::vector<Macro> macros, std::string windowName, 
int windowWidth, int windowHeight, std::string pictureFolder, int screenshotButton) 
	: pictures(pictures), macros(macros), windowWidth(windowWidth), windowHeight(windowHeight),
	pictureFolder(pictureFolder), screenshotButton(screenshotButton)
{
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
	#if AC_DISPLAY_SCREEN_CAP == 1
		showImg(scrnsht, "screenshot");
	#endif

	if (sf::Keyboard::isKeyPressed((sf::Keyboard::Key)screenshotButton)){
		std::time_t ctime = std::time(nullptr);
		char timeString[30];
		struct tm timeinfo;
		if(!localtime_s(&timeinfo, &ctime)){
			if (std::strftime(timeString, sizeof(timeString), "%Y-%m-%d %H-%M-%S", &timeinfo)) {
				saveImg(scrnsht, pictureFolder + std::string(timeString) + ".png");
			}
			else{
				std::cerr << "Could not get the time\n";
				saveImg(scrnsht, pictureFolder + "default_screenshot_name.png");
			}
		}
		else{
			std::cerr << "Could not get the time\n";
			saveImg(scrnsht, pictureFolder + "default_screenshot_name.png");
		}
	}


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
					matchTemplate(submat, templ, rslt, macros[i].matchMethod, critcalVals[i], matchPoints[i], "picture" + std::to_string(i));
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
					matchTemplate(submat, templ, rslt, macros[i].matchMethod, critcalVals[i], matchPoints[i], "picture" + std::to_string(i), mask);
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



void ImgProc::matchTemplate(cv::Mat& img, cv::Mat& templ, cv::Mat& result, int match_method, double &criticalVal, 
cv::Point &matchPoint, std::string windowName, cv::Mat mask) {
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
	#if AC_DISPLAY_IMAGE_MATCH == 1
		rectangle(img_display, matchPoint, cv::Point(matchPoint.x + templ.cols, matchPoint.y + templ.rows), cv::Scalar::all(0), 2, 8, 0);
		rectangle(result, matchPoint, cv::Point(matchPoint.x + templ.cols, matchPoint.y + templ.rows), cv::Scalar::all(0), 2, 8, 0);
		showImg(img_display, windowName + "-1");
		showImg(result, windowName + "-2");
	#endif
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