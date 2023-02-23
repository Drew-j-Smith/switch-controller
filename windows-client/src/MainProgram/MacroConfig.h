
#pragma once

#include "pch.h"

#include "GameCaptureConfig.h"

#include "Event/ConcreteClasses/ImageEvent.h"
#include "Event/ConcreteClasses/SoundEvent.h"
#include "Macro/Macro.h"
#include "Macro/MacroCollection.h"

#include "FFmpeg/AudioFrameSink.h"
#include "FFmpeg/FFmpegRecorder.h"
#include "FFmpeg/VideoFrameSink.h"

MacroCollection getMacroConfig(InputConfig &inputConfig) {
    // Deciders
    cv::Rect rectCrop(0, 0, 500, 500);
    // TODO fix
    auto haanitDecider = std::make_shared<ImageEvent>(
        cv::imread("data/haanit.png"), cv::imread("data/haanit mask.png"), 3,
        .97, rectCrop,
        dynamic_cast<VideoFrameSink *>(
            inputConfig.gameCaptureConfig.sinks[0].get()));
    auto animalCrossingDecider = std::make_shared<SoundEvent>(
        "data/test3.wav", .5,
        dynamic_cast<AudioFrameSink *>(
            inputConfig.gameCaptureConfig.sinks[0].get()));

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
    return {{inputConfig.macroRecorder.getLastRecordedMacro()}};
}
