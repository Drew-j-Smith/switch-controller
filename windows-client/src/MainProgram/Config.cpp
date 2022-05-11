
#include "MainProgram.h"

#include "Event/ConcreteClasses/EventCollection.h"
#include "Event/ConcreteClasses/EventToggle.h"
#include "Event/ConcreteClasses/ImageEvent.h"
#include "Event/ConcreteClasses/SfJoystickEvent.h"
#include "Event/ConcreteClasses/SfKeyboardEvent.h"
#include "Event/ConcreteClasses/SoundEvent.h"

using std::make_shared;
using std::shared_ptr;
using std::string;
using std::unique_ptr;

static void add_event_button(const int &button, const string &name,
                             const shared_ptr<Event> &invertedToggle,
                             std::map<string, shared_ptr<Event>> &eventMap) {
    shared_ptr<Event> joystickTemp = make_shared<SfJoystickEvent>(0, button);
    shared_ptr<Event> collectionTemp = make_shared<EventCollection>(
        std::vector<shared_ptr<Event>>{invertedToggle, joystickTemp},
        EventCollection::And);
    eventMap.insert({name, collectionTemp});
}

static void add_event_stick(const int &stick, const string &name,
                            std::map<string, shared_ptr<Event>> &eventMap) {
    shared_ptr<Event> joystickTemp =
        make_shared<SfJoystickEvent>(0, (sf::Joystick::Axis)stick);
    eventMap.insert({name, joystickTemp});
}

std::tuple<VideoFrameSink *, AudioFrameSink *, std::unique_ptr<FFmpegRecorder>,
           std::vector<std::unique_ptr<FFmpegFrameSink>>>
initializeGameCapture() {
    string inputFormat = "dshow";
    string deviceName =
        "video=Game Capture HD60 S:audio=Game Capture HD60 S Audio";
    std::map<string, string> ffmpegOptions = {{"pixel_format", "bgr24"}};

    std::vector<unique_ptr<FFmpegFrameSink>> sinks;
    sinks.push_back(std::make_unique<VideoFrameSink>());
    auto videoSink = dynamic_cast<VideoFrameSink *>(sinks[0].get());
    sinks.push_back(std::make_unique<AudioFrameSink>(
        AV_CH_LAYOUT_MONO, AV_SAMPLE_FMT_S16, 48000, true, 48000));
    auto audioSink = dynamic_cast<AudioFrameSink *>(sinks[1].get());

    av_log_set_level(AV_LOG_QUIET);

    auto recorder = std::make_unique<FFmpegRecorder>(inputFormat, deviceName,
                                                     ffmpegOptions, sinks);
    return {videoSink, audioSink, std::move(recorder), std::move(sinks)};
}

void getConfig(std::string &serialPortName,
               std::map<std::string, std::shared_ptr<Event>> &eventMap,
               std::vector<std::shared_ptr<Macro>> &macros,
               [[maybe_unused]] VideoFrameSink *videoSink,
               [[maybe_unused]] AudioFrameSink *audioSink) {
    serialPortName = "COM3";

    // Events
    eventMap = {};

    // The following layout has only been test for the nintendo switch pro
    // controller on windows

    // capture button
    shared_ptr<Event> toggle = make_shared<SfJoystickEvent>(0, 13);

    shared_ptr<Event> invertedToggle = std::make_shared<EventCollection>(
        std::vector<std::shared_ptr<Event>>{toggle}, EventCollection::Not);

    add_event_button(1, "a", invertedToggle, eventMap);
    add_event_button(0, "b", invertedToggle, eventMap);
    add_event_button(3, "x", invertedToggle, eventMap);
    add_event_button(2, "y", invertedToggle, eventMap);
    add_event_button(4, "l", invertedToggle, eventMap);
    add_event_button(5, "r", invertedToggle, eventMap);
    add_event_button(6, "xl", invertedToggle, eventMap);
    add_event_button(7, "xr", invertedToggle, eventMap);
    add_event_button(8, "select", invertedToggle, eventMap);
    add_event_button(9, "start", invertedToggle, eventMap);
    add_event_button(10, "lClick", invertedToggle, eventMap);
    add_event_button(11, "rClick", invertedToggle, eventMap);
    add_event_button(12, "home", invertedToggle, eventMap);

    add_event_stick(0, "leftStickX", eventMap);
    add_event_stick(1, "leftStickY", eventMap);
    add_event_stick(4, "rightStickX", eventMap);
    add_event_stick(5, "rightStickY", eventMap);
    add_event_stick(6, "dpadX", eventMap);
    add_event_stick(7, "dpadY", eventMap);

    add_event_button(1, "record", toggle, eventMap);
    add_event_button(0, "playLastRecorded", toggle, eventMap);
    add_event_button(3, "stopMacros", toggle, eventMap);

    // TODO
    // AC_ADD_EVENT_BUTTON(2, "turboButtonToggle");

    // Deciders
    cv::Rect rectCrop(0, 0, 500, 500);
    // TODO fix
    auto haanitDecider = make_shared<ImageEvent>(
        cv::imread("data/haanit.png"), cv::imread("data/haanit mask.png"), 3,
        .97, rectCrop, videoSink);
    auto animalCrossingDecider =
        make_shared<SoundEvent>("data/test3.wav", .5, audioSink);

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