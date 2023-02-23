
#include "MainProgram.h"

#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>
#include <boost/bind/bind.hpp>

std::unique_ptr<boost::asio::serial_port>
initializeSerialPort(std::string serialPort, unsigned int baud,
                     boost::asio::io_service *io) {
    try {
        std::cout << "Intializing serial communication.\n";
        auto port = std::make_unique<boost::asio::serial_port>(*io);
        port->open(serialPort);
        port->set_option(boost::asio::serial_port_base::baud_rate(baud));
        std::cout << "Serial communication intialized.\n";
        return port;
    } catch (const std::exception &e) {
        throw std::runtime_error(
            "Fatal error opening serial port.\n\tError: \"" +
            std::string(e.what()) + "\"\n\tDoes the port exist?\n");
    }
}

static void asyncHandler(const boost::system::error_code &error,
                         std::size_t bytes_transferred, int *res) {
    if (error) {
        std::cerr << error.what() << '\n';
        *res = -1;
    }
    *res += static_cast<int>(bytes_transferred);
};

void testSerialPort(std::unique_ptr<boost::asio::serial_port> &serialPort,
                    unsigned int writeLen, const unsigned char *writeData,
                    unsigned int readLen, unsigned char *readData,
                    boost::asio::io_service *io) {
    std::cout << "Testing connection.\n";

    int writeRes = 0;
    int readRes = 0;
    boost::asio::async_write(*serialPort,
                             boost::asio::buffer(writeData, writeLen),
                             boost::bind(&asyncHandler, boost::placeholders::_1,
                                         boost::placeholders::_2, &writeRes));
    boost::asio::async_read(*serialPort, boost::asio::buffer(readData, readLen),
                            boost::bind(&asyncHandler, boost::placeholders::_1,
                                        boost::placeholders::_2, &readRes));

    io->run_for(std::chrono::milliseconds(1000));
    serialPort->cancel();
    if (writeRes <= 0 || readRes <= 0) {
        throw std::runtime_error("Unable to establish serial connection.\n");
    }
    std::cout << "Serial communication established.\n";
}
