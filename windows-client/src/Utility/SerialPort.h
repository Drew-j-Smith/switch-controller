#include "pch.h"

#include <boost/asio/serial_port.hpp>


std::unique_ptr<boost::asio::serial_port> initializeSerialPort(std::string serialPort, unsigned int baud);
void testSerialPort(std::unique_ptr<boost::asio::serial_port>& serialPort, unsigned int writeLen, const unsigned char* writeData, unsigned int readLen, unsigned char* readData);