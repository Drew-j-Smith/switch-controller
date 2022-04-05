
#include "pch.h"

#include "Decider/Decider.h"
#include "Event/Event.h"
#include "Macro/Macro.h"

#include "Event/ConcreteClasses/EventCollection.h"
#include "Event/ConcreteClasses/EventToggle.h"
#include "Event/ConcreteClasses/SfJoystickEvent.h"
#include "Event/ConcreteClasses/SfKeyboardEvent.h"

#include "Decider/ImageProcessingDecider.h"
#include "Decider/SoundDecider.h"

#include "FFmpeg/AudioFrameSink.h"
#include "FFmpeg/FFmpegRecorder.h"
#include "FFmpeg/VideoFrameSink.h"

using std::make_shared;
using std::shared_ptr;
using std::string;

#define AC_ADD_EVENT_BUTTON(button, name)                                      \
    {                                                                          \
        shared_ptr<Event> joystickTemp =                                       \
            make_shared<SfJoystickEvent>(0, button);                           \
        shared_ptr<Event> collectionTemp = make_shared<EventCollection>(       \
            std::vector<shared_ptr<Event>>{invertedToggle, joystickTemp},      \
            EventCollection::And);                                             \
        createdEvents.push_back(joystickTemp);                                 \
        createdEvents.push_back(collectionTemp);                               \
        eventMap.insert({name, collectionTemp});                               \
    }

#define AC_ADD_EVENT_STICK(stick, name)                                        \
    {                                                                          \
        shared_ptr<Event> joystickTemp =                                       \
            make_shared<SfJoystickEvent>(0, (sf::Joystick::Axis)stick);        \
        createdEvents.push_back(joystickTemp);                                 \
        eventMap.insert({name, joystickTemp});                                 \
    }

void initializeGameCapture(shared_ptr<FFmpegRecorder> &recorder,
                           shared_ptr<VideoFrameSink> &videoSink,
                           shared_ptr<AudioFrameSink> &audioSink) {
    string inputFormat = "dshow";
    string deviceName =
        "video=Game Capture HD60 S:audio=Game Capture HD60 S Audio";
    std::map<string, string> ffmpegOptions = {{"pixel_format", "bgr24"}};

    std::vector<shared_ptr<FFmpegFrameSink>> sinks;
    videoSink = std::make_shared<VideoFrameSink>();
    sinks.push_back(videoSink);
    audioSink = std::make_shared<AudioFrameSink>(
        AV_CH_LAYOUT_MONO, AV_SAMPLE_FMT_S16, 48000, true, 48000);
    sinks.push_back(audioSink);

    av_log_set_level(AV_LOG_QUIET);

    recorder = make_shared<FFmpegRecorder>(inputFormat, deviceName,
                                           ffmpegOptions, sinks);
    recorder->start();
}

void getConfig(std::string &serialPortName,
               std::map<std::string, std::shared_ptr<Event>> &eventMap,
               std::vector<std::shared_ptr<Event>> &createdEvents,
               std::vector<std::shared_ptr<Decider>> &deciders,
               std::vector<std::shared_ptr<Macro>> &macros,
               shared_ptr<VideoFrameSink> videoSink,
               shared_ptr<AudioFrameSink> audioSink) {
    serialPortName = "COM3";

    // Events
    eventMap = {};
    createdEvents = {};

    // The following layout has only been test for the nintendo switch pro
    // controller on windows

    // capture button
    shared_ptr<Event> toggle = make_shared<SfJoystickEvent>(0, 13);
    createdEvents.push_back(toggle);

    shared_ptr<Event> invertedToggle = std::make_shared<EventCollection>(
        std::vector<std::shared_ptr<Event>>{toggle}, EventCollection::Not);
    createdEvents.push_back(invertedToggle);

    AC_ADD_EVENT_BUTTON(1, "a");
    AC_ADD_EVENT_BUTTON(0, "b");
    AC_ADD_EVENT_BUTTON(3, "x");
    AC_ADD_EVENT_BUTTON(2, "y");
    AC_ADD_EVENT_BUTTON(4, "l");
    AC_ADD_EVENT_BUTTON(5, "r");
    AC_ADD_EVENT_BUTTON(6, "xl");
    AC_ADD_EVENT_BUTTON(7, "xr");
    AC_ADD_EVENT_BUTTON(8, "select");
    AC_ADD_EVENT_BUTTON(9, "start");
    AC_ADD_EVENT_BUTTON(10, "lClick");
    AC_ADD_EVENT_BUTTON(11, "rClick");
    AC_ADD_EVENT_BUTTON(12, "home");

    AC_ADD_EVENT_STICK(0, "leftStickX");
    AC_ADD_EVENT_STICK(1, "leftStickY");
    AC_ADD_EVENT_STICK(4, "rightStickX");
    AC_ADD_EVENT_STICK(5, "rightStickY");
    AC_ADD_EVENT_STICK(6, "dpadX");
    AC_ADD_EVENT_STICK(7, "dpadY");

    // These events use the opposite of the toggle
    invertedToggle = toggle;

    AC_ADD_EVENT_BUTTON(1, "record");
    AC_ADD_EVENT_BUTTON(0, "playLastRecorded");
    AC_ADD_EVENT_BUTTON(3, "stopMacros");

    // TODO
    // AC_ADD_EVENT_BUTTON(2, "turboButtonToggle");

    // Deciders
    auto haanitDecider = make_shared<ImageProcessingDecider>(
        cv::imread("data/haanit.png"), cv::imread("data/haanit mask.png"), 3,
        .97, 0, 0, 500, 500, videoSink);
    auto animalCrossingDecider =
        make_shared<SoundDecider>("data/test3.wav", .5, audioSink);
    deciders = {haanitDecider, animalCrossingDecider};

    // TODO
    // Macros
    // clang-format off
    // auto macro1Event = make_shared<SfKeyboardEvent>(sf::Keyboard::B);
    // auto macro1 =
    //     make_shared<Macro>("data/test6.txt", macro1Event, animalCrossingDecider,
    //                        Macro::inputPriority);
    // auto macro2 =
    //     make_shared<Macro>("data/test5.txt", make_shared<ConstantEvent>(),
    //                        make_shared<DefaultDecider>(), Macro::inputPriority);

    // macro1->setNextMacroLists(
    //     std::vector<std::vector<std::weak_ptr<Macro>>>{{macro1}, {macro2}});

    // macros = {macro1, macro2};
    // clang-format on
    macros = {};
}