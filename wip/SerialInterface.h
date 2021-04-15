#ifndef SERIAL_INTERFACE_H
#define SERIAL_INTERFACE_H

#include "pch.h"

#include <boost/asio/io_service.hpp>
#include <boost/asio/serial_port.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>

template<size_t writeFrameSize, size_t readFrameSize>
class SerialInterface
{
private:
    std::mutex m_mutex;
    std::unique_ptr<boost::asio::serial_port> port;
    std::thread readThread;
    unsigned char readBytes[readFrameSize];
public:
    SerialInterface(std::string serialPort, unsigned long long baud) {
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
        memset(readBytes, 0, sizeof(readBytes));
    }

    void sendData(const unsigned char * data) {
        if (readThread.joinable())
            readThread.join();
        
        readThread = std::thread([&](){
            unsigned char recevedData[readFrameSize];
            boost::asio::read(*port, boost::asio::buffer(recevedData, readFrameSize));
            std::lock_guard<std::mutex> guard(m_mutex);
            memcpy(readBytes, recevedData, readFrameSize);
        });

        boost::asio::write(*port, boost::asio::buffer(data, writeFrameSize));
    }

    void getData(unsigned char* data) {
        std::lock_guard<std::mutex> guard(m_mutex);
        memcpy(data, readBytes, readFrameSize);
    }

};

#endif