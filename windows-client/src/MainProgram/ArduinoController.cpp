
#include "SerialPort.h"
#include "StartController.h"
#include "TestAudio.h"
#include "TestVideo.h"

#include <opencv2/core/utils/logger.hpp>

#include <boost/program_options.hpp>

int main(int argc, char **argv) {
    namespace po = boost::program_options;
    po::options_description desc("Allowed options");
    desc.add_options()                     //
        ("help,h", "produce help message") //
        ("testAudio,a", po::value<int>(),
         "test audio saving (time in seconds to record)") //
        ("audioLoopTime", po::value<int>()->default_value(-1),
         "the loop time in audio recording") //
        ("testVideo,v", "test video saving") //
        ("testSerial,s", "test serial port") //
        ("port,p", po::value<std::string>()->required(),
         "Port used for serial communication") //
        ("inputFormat", po::value<std::string>()->required(),
         "input format for ffmpeg") //
        ("deviceName", po::value<std::string>()->required(),
         "device name for ffmpeg");

    cv::utils::logging::setLogLevel(
        cv::utils::logging::LogLevel::LOG_LEVEL_SILENT);

    po::variables_map vm;
    try {
        po::store(po::parse_command_line(argc, argv, desc), vm);
    } catch (std::exception &e) {
        spdlog::error("{}", e.what());
        return 1;
    }
    std::map<std::string, std::string> ffmpeg_options{};
    try {
        auto cfg_file_parsed = po::parse_config_file("config.cfg", desc, true);
        po::store(cfg_file_parsed, vm);
        auto unrecognized_options = po::collect_unrecognized(
            cfg_file_parsed.options,
            po::collect_unrecognized_mode::include_positional);
        for (std::size_t i = 0; i + 1 < unrecognized_options.size(); i += 2) {
            ffmpeg_options[unrecognized_options[i]] =
                unrecognized_options[i + 1];
        }
    } catch (std::exception &e) {
        spdlog::warn("{}", e.what());
    }
    try {
        po::notify(vm);
    } catch (std::exception &e) {
        spdlog::error("{}", e.what());
        std::cout << desc << "\n";
        return 1;
    }

    if (vm.count("help")) {
        std::cout << desc << "\n";
        return 0;
    }

    try {
        if (vm.count("testSerial")) {
            boost::asio::io_service io;
            auto port =
                initializeSerialPort(vm["port"].as<std::string>(), 57600, io);
            testSerialPort(port, io);
        } else if (vm.count("testVideo")) {
            TestVideo(vm, ffmpeg_options);
        } else if (vm.count("testAudio")) {
            TestAudio(vm, ffmpeg_options);
        } else {
            StartController();
        }
    } catch (std::exception &e) {
        spdlog::error("Uncaught exception: {}", e.what());
        return 1;
    }

    return 0;
}