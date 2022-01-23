
#include "SerialPort.h"

#include <boost/asio/io_service.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>

std::unique_ptr<boost::asio::serial_port>
initializeSerialPort(std::string serialPort, unsigned int baud) {
    try {
        std::cout << "Intializing serial communication.\n";
        boost::asio::io_service io;
        auto port = std::make_unique<boost::asio::serial_port>(io);
        port->open(serialPort);
        port->set_option(boost::asio::serial_port_base::baud_rate(baud));
        std::cout << "Serial communication intialized.\n";
        return port;
    } catch (const std::exception &e) {
        std::cerr << "Fatal error opening serial port.\n";
        std::cerr << "\tError: \"" << e.what() << "\"\n";
        std::cerr << "\tDoes the port exist?" << std::endl;
        throw;
    }
}

void testSerialPort(std::unique_ptr<boost::asio::serial_port> &serialPort,
                    unsigned int writeLen, const unsigned char *writeData,
                    unsigned int readLen, unsigned char *readData) {
    // TODO there is definitly something wrong with this block of code but the
    // bug is inconsistant

    std::cout << "Testing connection.\n";
    std::atomic<bool> finished;
    finished.store(false);
    std::thread t([&]() {
        boost::asio::write(*serialPort,
                           boost::asio::buffer(writeData, writeLen));
        boost::asio::read(*serialPort, boost::asio::buffer(readData, readLen));
        finished.store(true);
    });

    auto start = std::chrono::steady_clock::now();
    while (!finished.load()) {
        auto end = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
                .count() > 1000) {
            std::cerr << "Unable to establish serial connection.\n";
            throw std::runtime_error("Unable to establish serial connection.");
        }
    }
    t.join();
    std::cout << "Serial communication established.\n";
}
