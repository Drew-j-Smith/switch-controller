
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
    po::options_description controls_desc("Controller options");
    controls_desc.add_options()                                     //
        ("controls.y", po::value<int>()->required())                //
        ("controls.b", po::value<int>()->required())                //
        ("controls.a", po::value<int>()->required())                //
        ("controls.x", po::value<int>()->required())                //
        ("controls.l", po::value<int>()->required())                //
        ("controls.r", po::value<int>()->required())                //
        ("controls.xl", po::value<int>()->required())               //
        ("controls.xr", po::value<int>()->required())               //
        ("controls.Select", po::value<int>()->required())           //
        ("controls.start", po::value<int>()->required())            //
        ("controls.lClick", po::value<int>()->required())           //
        ("controls.rClick", po::value<int>()->required())           //
        ("controls.home", po::value<int>()->required())             //
        ("controls.capture", po::value<int>()->required())          //
        ("controls.modifier", po::value<int>()->required())         //
        ("controls.turbo", po::value<int>()->required())            //
        ("controls.record", po::value<int>()->required())           //
        ("controls.playLastRecorded", po::value<int>()->required()) //
        ("controls.stopMacros", po::value<int>()->required())       //
        ;
    po::options_description config_file_options;
    config_file_options.add(desc).add(controls_desc);

    cv::utils::logging::setLogLevel(
        cv::utils::logging::LogLevel::LOG_LEVEL_SILENT);
    av_log_set_level(AV_LOG_QUIET);

    po::variables_map vm;
    try {
        po::store(po::parse_command_line(argc, argv, desc), vm);
    } catch (std::exception &e) {
        spdlog::error("{}", e.what());
        return 1;
    }
    std::map<std::string, std::string> ffmpeg_options{};
    try {
        auto cfg_file_parsed =
            po::parse_config_file("data/config.cfg", config_file_options, true);
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
            StartController(vm, ffmpeg_options);
        }
    } catch (std::exception &e) {
        spdlog::error("Uncaught exception: {}", e.what());
        return 1;
    }

    return 0;
}