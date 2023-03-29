#pragma once

#include "pch.h"

void TestVideo(const boost::program_options::variables_map &vm) {
    cv::Mat frame;
    cv::VideoCapture cap;
    int deviceID = vm["deviceIndex"].as<int>();
    int apiID = vm["videoCaptureBackend"].as<int>();
    cap.open(deviceID, apiID);
    if (!cap.isOpened()) {
        spdlog::error("Unable to open video capture");
        return;
    }
    cap.set(cv::CAP_PROP_FRAME_WIDTH, 1920);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 1080);
    for (;;) {
        cap.grab();
        cap.retrieve(frame);
        if (frame.empty()) {
            spdlog::error("blank frame grabbed");
            break;
        }
        cv::imshow("0", frame);
        if (cv::waitKey(5) >= 0)
            break;
    }
}