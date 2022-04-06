#include "MainProgram.h"

#include "pch.h"

void TestVideo() {
    std::string inputFormat = "dshow";
    std::string deviceName = "video=Game Capture HD60 S";
    std::map<std::string, std::string> ffmpegOptions = {
        {"pixel_format", "bgr24"}};
    std::vector<std::shared_ptr<FFmpegFrameSink>> sinks;
    std::shared_ptr<VideoFrameSink> videoSink =
        std::make_shared<VideoFrameSink>();
    sinks.push_back(videoSink);

    av_log_set_level(AV_LOG_QUIET);

    FFmpegRecorder recorder(inputFormat, deviceName, ffmpegOptions, sinks);
    recorder.start();

    videoSink->waitForInit();
    std::vector<uint8_t> data;
    long long lastFrame = videoSink->getData(data);
    cv::Mat mat = cv::Mat(videoSink->getHeight(), videoSink->getWidth(),
                          CV_8UC3, data.data());

    std::atomic<bool> running = true;
    std::string title = std::to_string(
        std::chrono::steady_clock::now().time_since_epoch().count());

    std::thread t([&]() {
        while (running.load()) {
            lastFrame = videoSink->getNextData(data, lastFrame);
            cv::imshow(title, mat);
            cv::waitKey(1);
        }
    });

    std::cout << "Press enter to stop\n";
    std::cin.get();
    running.store(false);
    t.join();
}