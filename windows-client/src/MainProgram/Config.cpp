
#include "MainProgram.h"

#include <SFML/Window/Joystick.hpp>

#include "Event/ConcreteClasses/ImageEvent.h"
#include "Event/ConcreteClasses/SoundEvent.h"

using std::make_shared;
using std::shared_ptr;
using std::string;
using std::unique_ptr;

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

constexpr static auto convertSFML_Axis(float pos) {
    // SFML works on a [-100, 100] scale
    // this scales it to [0, 255] scale
    float SFML_RATIO = 255.0f / 200.0f;
    float SCALE_FACTOR = 1.4f;
    float SCALE = SCALE_FACTOR * SFML_RATIO;
    int OFFSET = 128;

    int scaled = static_cast<int>(pos * SCALE) + OFFSET;
    return static_cast<uint8_t>(std::clamp(scaled, 0, 255));
}

// capture button
constexpr static auto toggle = [] {
    return sf::Joystick::isButtonPressed(0, 13);
};

std::tuple<std::string /* serialPortName */,
           std::function<bool()> /* stopMacros */, InputCollection,
           MacroRecorder, MacroCollection>
getConfig([[maybe_unused]] VideoFrameSink *videoSink,
          [[maybe_unused]] AudioFrameSink *audioSink) {
    std::string serialPortName = "COM3";

    // The following layout has only been test for the nintendo switch pro
    // controller on windows

    std::array<std::function<bool()>, 14> buttons;
    buttons[InputCollection::buttonIndicies::b] = [] {
        return sf::Joystick::isButtonPressed(0, 0) && !toggle();
    };
    buttons[InputCollection::buttonIndicies::a] = [] {
        return sf::Joystick::isButtonPressed(0, 1) && !toggle();
    };
    buttons[InputCollection::buttonIndicies::y] = [] {
        return sf::Joystick::isButtonPressed(0, 2) && !toggle();
    };
    buttons[InputCollection::buttonIndicies::x] = [] {
        return sf::Joystick::isButtonPressed(0, 3) && !toggle();
    };
    buttons[InputCollection::buttonIndicies::l] = [] {
        return sf::Joystick::isButtonPressed(0, 4) && !toggle();
    };
    buttons[InputCollection::buttonIndicies::r] = [] {
        return sf::Joystick::isButtonPressed(0, 5) && !toggle();
    };
    buttons[InputCollection::buttonIndicies::xl] = [] {
        return sf::Joystick::isButtonPressed(0, 6) && !toggle();
    };
    buttons[InputCollection::buttonIndicies::xr] = [] {
        return sf::Joystick::isButtonPressed(0, 7) && !toggle();
    };
    buttons[InputCollection::buttonIndicies::select] = [] {
        return sf::Joystick::isButtonPressed(0, 8) && !toggle();
    };
    buttons[InputCollection::buttonIndicies::start] = [] {
        return sf::Joystick::isButtonPressed(0, 9) && !toggle();
    };
    buttons[InputCollection::buttonIndicies::lClick] = [] {
        return sf::Joystick::isButtonPressed(0, 10) && !toggle();
    };
    buttons[InputCollection::buttonIndicies::rClick] = [] {
        return sf::Joystick::isButtonPressed(0, 11) && !toggle();
    };
    buttons[InputCollection::buttonIndicies::home] = [] {
        return sf::Joystick::isButtonPressed(0, 12) && !toggle();
    };
    buttons[InputCollection::buttonIndicies::capture] = [] {
        return sf::Joystick::isButtonPressed(0, 13) && !toggle();
    };

    std::array<std::function<std::array<uint8_t, 2>()>, 3> sticks;
    sticks[InputCollection::stickIndicies::left] = [] {
        return std::array<uint8_t, 2>{
            convertSFML_Axis(
                sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::X)),
            convertSFML_Axis(
                sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::Y))};
    };
    sticks[InputCollection::stickIndicies::right] = [] {
        return std::array<uint8_t, 2>{
            convertSFML_Axis(
                sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::U)),
            convertSFML_Axis(
                sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::V))};
    };
    sticks[InputCollection::stickIndicies::hat] = [] {
        return std::array<uint8_t, 2>{
            convertSFML_Axis(
                sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::PovX)),
            convertSFML_Axis(
                sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::PovY))};
    };

    InputCollection inputCollection(buttons, sticks);

    MacroRecorder macroRecorder(
        [] { return sf::Joystick::isButtonPressed(0, 1) && toggle(); },
        [] { return sf::Joystick::isButtonPressed(0, 0) && toggle(); });

    constexpr auto stopMacros = [] {
        return sf::Joystick::isButtonPressed(0, 3) && toggle();
    };

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
    MacroCollection macroCollection({macroRecorder.getLastRecordedMacro()});

    return {serialPortName, stopMacros, std::move(inputCollection),
            std::move(macroRecorder), std::move(macroCollection)};
}