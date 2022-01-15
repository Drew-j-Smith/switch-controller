#include "pch.h"

#include "FFmpeg/FFmpegRecorder.h"
#include "FFmpeg/AudioFrameSink.h"
#include "FFmpeg/VideoFrameSink.h"
#include <AudioFile.h>
#include "Utility/SerialPort.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

void CreateConfig(std::string& configFilename);
void EditConfig(std::string& configFilename);
void StartController(std::string& configFilename);

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

int main(int argc, const char** argv)
{
    int option = 0;
    std::string configFilename = "data/config.json";

    while (option != 8) {
        std::cout << options;
        std::cout << "The current config file is: \"" << configFilename << "\"\n";
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
            case 4:
                StartController(configFilename);
                break;
            case 5:
                {
                    try {
                        boost::property_tree::ptree tree;
                        boost::property_tree::read_json(configFilename, tree);
                        auto port = initializeSerialPort(tree.get<std::string>("serial port"), 57600);
                        
                        // sending a nuetral signal
                        unsigned char send[8] = {85, 0, 0, 128, 128, 128, 128, 8};
                        unsigned char recieve[1];

                        testSerialPort(port, 8, send, 1, recieve);
                    }
                    catch (std::exception& e) {
                        std::cerr << "Failure connecting via serial port.\n";
                    }
                }
                break;
            case 6:
                {
                    std::string inputFormat = "dshow";
                    std::string deviceName = "video=Game Capture HD60 S";
                    std::map<std::string, std::string> options = {{"pixel_format", "bgr24"}};
                    std::vector<std::shared_ptr<FFmpegFrameSink>> sinks;
                    std::shared_ptr<VideoFrameSink> videoSink = std::make_shared<VideoFrameSink>();
                    sinks.push_back(videoSink);

                    av_log_set_level(AV_LOG_QUIET);

                    FFmpegRecorder recorder(inputFormat, deviceName, options, sinks);
                    recorder.start();

                    videoSink->waitForInit();
                    uint8_t* data = new uint8_t[videoSink->getDataSize()];
                    cv::Mat mat = cv::Mat(videoSink->getHeight(), videoSink->getWidth(), CV_8UC3, data);
                    long long lastFrame = 0;
                    while (true) {
                        lastFrame = videoSink->getNextData(data, lastFrame);
                        cv::imshow("test", mat);
                        cv::waitKey(1);
                    }
                }
                break;
            case 7:
                {
                    std::string inputFormat = "dshow";
                    std::string deviceName = "audio=Game Capture HD60 S Audio";
                    std::map<std::string, std::string> options = {};
                    std::vector<std::shared_ptr<FFmpegFrameSink>> sinks;
                    std::shared_ptr<AudioFrameSink> audioSink = std::make_shared<AudioFrameSink>();
                    sinks.push_back(audioSink);

                    // av_log_set_level(AV_LOG_QUIET);

                    FFmpegRecorder recorder(inputFormat, deviceName, options, sinks);
                    recorder.start();

                    audioSink->waitForInit();
                    std::this_thread::sleep_for(std::chrono::seconds(5));
                    
                    recorder.stop();
                    int size = audioSink->getDataSize() / sizeof(float);
                    uint8_t* data = new uint8_t[audioSink->getDataSize()];
                    audioSink->getData(data);

                    float* floatData = (float*)data;
                    std::vector<std::vector<float>> audioData;
                    audioData.push_back({});
                    audioData[0].resize(size);
                    
                    for (int i = 0; i < size; i++) {
                        audioData[0][i] = floatData[i];
                    }

                    AudioFile<float> audiofile = AudioFile<float>();
                    audiofile.setAudioBuffer(audioData);
                    audiofile.setSampleRate(48000);
                    audiofile.save("test.wav");
                }
                break;
            case 8:
                break;
            default:
                std::cout << "Unkown option: " << option << "\n";
        }
    }

    return 0;
}