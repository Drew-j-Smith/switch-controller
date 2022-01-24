#include "pch.h"

#include "FFmpeg/AudioFrameSink.h"
#include "FFmpeg/FFmpegRecorder.h"
#include "FFmpeg/VideoFrameSink.h"
#include "Utility/SerialPort.h"
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include "InputEvent/InputEvent.h"

void CreateConfig(std::string &configFilename);
void EditConfig(std::string &configFilename);
void StartController(std::string &configFilename);

const std::string options =
    R"(Select one of the following options:
    1. Set config file
    2. Create config file
    3. Edit config File
    4. Run
    5. Test serial communication
    6. Test FFmpeg video capture
    7. Test FFmpeg audio capture
    8. Exit
)";

int main([[maybe_unused]] int argc, [[maybe_unused]] const char **argv) {

    boost::property_tree::ptree a1;
    boost::property_tree::ptree b1;
    boost::property_tree::ptree c1;
    a1.add("type", "ConstantInputEvent");
    a1.add("isDigital", true);
    a1.add("inputValue", 0);
    b1.add("type", "ConstantInputEvent");
    b1.add("isDigital", true);
    b1.add("inputValue", 1);
    c1.add("type", "InputEventInverter");
    c1.add_child("event", a1);

    InputEventFactory factory;
    std::shared_ptr<InputEvent> a = factory.create(a1);
    std::shared_ptr<InputEvent> b = factory.create(b1);
    std::shared_ptr<InputEvent> c = factory.create(c1);
    std::cout << a->getTypeName() << ' ' << b->getTypeName() << ' '
              << c->getTypeName() << '\n';

    int option = 0;
    std::string configFilename = "data/config.json";

    while (option != 8) {
        std::cout << options;
        std::cout << "The current config file is: \"" << configFilename
                  << "\"\n";
        std::cin >> option;
        switch (option) {
        case 1:
            std::cout << "Enter the config filename: ";
            std::getline(std::cin, configFilename);
            std::getline(std::cin, configFilename);
            break;
        case 2:
            CreateConfig(configFilename);
            break;
        case 3:
            EditConfig(configFilename);
            break;
        case 4: {
            try {
                StartController(configFilename);
            } catch (std::exception e) {
                std::cerr << e.what() << '\n';
            }
        } break;
        case 5: {
            try {
                boost::property_tree::ptree tree;
                boost::property_tree::read_json(configFilename, tree);
                auto port = initializeSerialPort(
                    tree.get<std::string>("serial port"), 57600);

                // sending a nuetral signal
                unsigned char send[8] = {85, 0, 0, 128, 128, 128, 128, 8};
                unsigned char recieve[1];

                testSerialPort(port, 8, send, 1, recieve);
            } catch (std::exception &e) {
                std::cerr << "Failure connecting via serial port.\n";
                std::cerr << e.what() << '\n';
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
            std::cin.get();
            running.store(false);
            t.join();
        } break;
        case 7: {
            std::string tempStr;
            std::getline(std::cin, tempStr);

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
            std::vector<uint8_t> data;
            audioSink->getData(data);

            std::cout << "Enter the output filename:\n";
            std::string audioFilename;
            std::getline(std::cin, audioFilename);
            std::getline(std::cin, audioFilename);

            std::ofstream outfile(audioFilename,
                                  std::ios::out | std::ios::binary);
            outfile.write((const char *)data.data(), data.size());
        } break;
        case 8:
            break;
        default:
            std::cout << "Unkown option: " << option << "\n";
        }
    }

    return 0;
}