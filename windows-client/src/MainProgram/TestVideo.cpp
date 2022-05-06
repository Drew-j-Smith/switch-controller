#include "MainProgram.h"

#include "pch.h"

void TestVideo() {
    std::string inputFormat = "dshow";
    std::string deviceName = "video=Game Capture HD60 S (2)";
    std::map<std::string, std::string> ffmpegOptions = {
        {"pixel_format", "bgr24"}};
    std::vector<std::unique_ptr<FFmpegFrameSink>> sinks;
    auto tempSink = std::make_unique<VideoFrameSink>();
    auto videoSink = tempSink.get();
    sinks.push_back(std::move(tempSink));

    av_log_set_level(AV_LOG_QUIET);

    FFmpegRecorder recorder(inputFormat, deviceName, ffmpegOptions,
                            std::move(sinks));
    recorder.start();

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