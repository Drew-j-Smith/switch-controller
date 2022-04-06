#ifndef MAIN_PROGRAM_H
#define MAIN_PROGRAM_H

#include "pch.h"

#include "Event/Event.h"
#include "Macro/Macro.h"

#include "FFmpeg/AudioFrameSink.h"
#include "FFmpeg/FFmpegRecorder.h"
#include "FFmpeg/VideoFrameSink.h"

void StartController();
void TestVideo();
void TestAudio();
void initializeGameCapture(std::shared_ptr<FFmpegRecorder> &recorder,
                           std::shared_ptr<VideoFrameSink> &videoSink,
                           std::shared_ptr<AudioFrameSink> &audioSink);

void getConfig(std::string &serialPortName,
               std::map<std::string, std::shared_ptr<Event>> &events,
               std::vector<std::shared_ptr<Event>> &createdEvents,
               std::vector<std::shared_ptr<Macro>> &macros,
               std::shared_ptr<VideoFrameSink> videoSink,
               std::shared_ptr<AudioFrameSink> audioSink);

#endif