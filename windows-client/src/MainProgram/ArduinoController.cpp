#include "Utility/SerialPort.h"

#include "pch.h"

#include "FFmpeg/AudioFrameSink.h"
#include "FFmpeg/FFmpegRecorder.h"
#include "FFmpeg/VideoFrameSink.h"
#include "InputEvent/InputEvent.h"
#include "MainProgram.h"
#include "SFML/SFMLRenderer.h"

#include <boost/log/trivial.hpp>
#include <opencv2/core/utils/logger.hpp>

const std::string options =
    R"(Select one of the following options:
    1. X
    2. X
    3. X
    4. Run
    5. Test serial communication
    6. Test FFmpeg video capture
    7. Test FFmpeg audio capture
    8. Exit
)";

int main() {

    // SFMLRenderer renderer;
    // renderer.start();

    cv::utils::logging::setLogLevel(
        cv::utils::logging::LogLevel::LOG_LEVEL_SILENT);

    int option = 0;

    while (option != 8) {
        std::cout << options;
        std::cin >> option;
        std::string tempstr;
        std::getline(std::cin, tempstr);
        switch (option) {
        case 4: {
            try {
                StartController();
            } catch (std::exception &e) {
                BOOST_LOG_TRIVIAL(error)
                    << "Uncaught exception in StartController: " +
                           std::string(e.what()) + "\n";
            }
        } break;
        case 5: {
            std::cout << "Enter the port name:\n";
            std::string portName;
            std::getline(std::cin, portName);
            try {
                boost::asio::io_service io;
                auto port = initializeSerialPort(portName, 57600, &io);

                // sending a nuetral signal
                unsigned char send[8] = {85, 0, 0, 128, 128, 128, 128, 8};
                unsigned char recieve[1];

                testSerialPort(port, 8, send, 1, recieve, &io);
            } catch (std::exception &e) {
                BOOST_LOG_TRIVIAL(error)
                    << "Failure connecting via serial port.\n" +
                           std::string(e.what()) + "\n";
            }
        } break;
        case 6: {
            std::string inputFormat = "dshow";
            std::string deviceName = "video=Game Capture HD60 S";
            std::map<std::string, std::string> ffmpegOptions = {
                {"pixel_format", "bgr24"}};
            std::vector<std::shared_ptr<FFmpegFrameSink>> sinks;
            std::shared_ptr<VideoFrameSink> videoSink =
                std::make_shared<VideoFrameSink>();
            sinks.push_back(videoSink);

            av_log_set_level(AV_LOG_QUIET);

            FFmpegRecorder recorder(inputFormat, deviceName, ffmpegOptions,
                                    sinks);
            recorder.start();

            videoSink->waitForInit();
            std::vector<uint8_t> *data;
            long long lastFrame = videoSink->getData(data);

            std::atomic<bool> running = true;
            std::string title = std::to_string(
                std::chrono::steady_clock::now().time_since_epoch().count());

            std::thread t([&]() {
                while (running.load()) {
                    videoSink->returnPointer(data);
                    lastFrame = videoSink->getNextData(data, lastFrame);
                    cv::Mat mat =
                        cv::Mat(videoSink->getHeight(), videoSink->getWidth(),
                                CV_8UC3, data->data());
                    cv::imshow(title, mat);
                    cv::waitKey(1);
                }
            });

            std::cout << "Press enter to stop\n";
            std::cin.get();
            running.store(false);
            t.join();
        } break;
        case 7: {
            std::string inputFormat;
            std::string deviceName;
            std::string recordTimeStr;
            char loopRecord;
            int64_t bufferSize = 0;

            // dshow
            // audio=Game Capture HD60 S Audio
            // ffplay -f s16le -ar 48k -ac 1 test.pcm

            std::cout << "Enter the input format (empty to load a file):\n";
            std::getline(std::cin, inputFormat);

            std::cout << "Enter the device/filename:\n";
            std::getline(std::cin, deviceName);

            std::cout << "Enter the recording time in seconds (empty to read "
                         "an entire file)\n";
            std::getline(std::cin, recordTimeStr);

            std::cout << "Use loop recording (y/n):\n";
            std::cin >> loopRecord;

            if (loopRecord == 'y') {
                std::cout << "Enter the buffer size in seconds:\n";
                std::cin >> bufferSize;
            }

            std::map<std::string, std::string> ffmpegOptions = {};
            std::vector<std::shared_ptr<FFmpegFrameSink>> sinks;
            std::shared_ptr<AudioFrameSink> audioSink =
                std::make_shared<AudioFrameSink>(
                    AV_CH_LAYOUT_MONO, AV_SAMPLE_FMT_S16, 48000,
                    loopRecord == 'y', 48000 * bufferSize);
            sinks.push_back(audioSink);

            FFmpegRecorder recorder(inputFormat, deviceName, ffmpegOptions,
                                    sinks);
            recorder.start();

            audioSink->waitForInit();
            if (recordTimeStr.length() > 0) {
                std::this_thread::sleep_for(
                    std::chrono::seconds(std::stoi(recordTimeStr)));
            } else {
                recorder.join();
            }

            recorder.stop();
            std::vector<uint8_t> *data;
            audioSink->getData(data);

            std::cout << "Enter the output filename:\n";
            std::string audioFilename;
            std::getline(std::cin, audioFilename);
            std::getline(std::cin, audioFilename);

            std::ofstream outfile(audioFilename,
                                  std::ios::out | std::ios::binary);
            outfile.write((const char *)data->data(), data->size());
        } break;
        case 8:
            break;
        default:
            std::cout << "Unkown option: " << option << "\n";
        }
    }

    return 0;
}