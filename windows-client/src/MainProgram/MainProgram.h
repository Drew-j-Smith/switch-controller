#ifndef MAIN_PROGRAM_H
#define MAIN_PROGRAM_H

#include "pch.h"

#include "Event/Utility/InputCollection.h"
#include "Macro/Macro.h"
#include "Macro/MacroCollection.h"
#include "Macro/MacroRecorder.h"

#include "FFmpeg/AudioFrameSink.h"
#include "FFmpeg/FFmpegRecorder.h"
#include "FFmpeg/VideoFrameSink.h"

#include <boost/asio/io_service.hpp>
#include <boost/asio/serial_port.hpp>

void StartController();
void TestVideo();
void TestAudio();

std::tuple<VideoFrameSink *, AudioFrameSink *, std::unique_ptr<FFmpegRecorder>,
           std::vector<std::unique_ptr<FFmpegFrameSink>>>
initializeGameCapture();

std::tuple<std::string /* serialPortName */,
           std::function<bool()> /* stopMacros */, InputCollection,
           MacroRecorder, MacroCollection>
getConfig(VideoFrameSink *videoSink, AudioFrameSink *audioSink);

std::unique_ptr<boost::asio::serial_port>
initializeSerialPort(std::string serialPort, unsigned int baud,
                     boost::asio::io_service *io);
void testSerialPort(std::unique_ptr<boost::asio::serial_port> &serialPort,
                    unsigned int writeLen, const unsigned char *writeData,
                    unsigned int readLen, unsigned char *readData,
                    boost::asio::io_service *io);

#endif