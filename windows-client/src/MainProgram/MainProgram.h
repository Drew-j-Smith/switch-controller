#ifndef MAIN_PROGRAM_H
#define MAIN_PROGRAM_H

#include "pch.h"

#include "Event/Event.h"
#include "Macro/Macro.h"

#include "FFmpeg/AudioFrameSink.h"
#include "FFmpeg/FFmpegRecorder.h"
#include "FFmpeg/VideoFrameSink.h"

#include <boost/asio/io_service.hpp>
#include <boost/asio/serial_port.hpp>

void StartController();
void TestVideo();
void TestAudio();

std::tuple<VideoFrameSink *, AudioFrameSink *, FFmpegRecorder,
           std::vector<std::unique_ptr<FFmpegFrameSink>>>
initializeGameCapture();

void getConfig(std::string &serialPortName,
               std::map<std::string, std::shared_ptr<Event>> &events,
               std::vector<std::shared_ptr<Macro>> &macros,
               VideoFrameSink *videoSink, AudioFrameSink *audioSink);

std::unique_ptr<boost::asio::serial_port>
initializeSerialPort(std::string serialPort, unsigned int baud,
                     boost::asio::io_service *io);
void testSerialPort(std::unique_ptr<boost::asio::serial_port> &serialPort,
                    unsigned int writeLen, const unsigned char *writeData,
                    unsigned int readLen, unsigned char *readData,
                    boost::asio::io_service *io);

#endif