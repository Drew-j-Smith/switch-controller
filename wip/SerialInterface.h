#ifndef SERIAL_INTERFACE_H
#define SERIAL_INTERFACE_H

#include "pch.h"

#include <boost/asio/io_service.hpp>
#include <boost/asio/serial_port.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>

class SerialInterface
{
private:
    std::unique_ptr<boost::asio::serial_port> port;
    size_t writeFrameSize;
    size_t readFrameSize;
public:
    SerialInterface(std::string serialPort, unsigned long long baud, size_t writeFrameSize, size_t readFrameSize) {
        try{
            boost::asio::io_service io;
            port = std::make_unique<boost::asio::serial_port>(io);
            port->open(serialPort);
            port->set_option(boost::asio::serial_port_base::baud_rate(baud));
        }
        catch(const std::exception& e){
            std::cerr << e.what() << '\n';
            std::cerr << "Fatal error opening serial port" << std::endl;
            exit(-1);
        }
        this->writeFrameSize = writeFrameSize;
        this->readFrameSize = readFrameSize;
    }

    void sendData(const unsigned char * dataToWrite, unsigned char * dataToRead) {
        boost::asio::write(*port, boost::asio::buffer(dataToWrite, writeFrameSize));
        boost::asio::read(*port, boost::asio::buffer(dataToRead, readFrameSize)); 
    }

};

#endif