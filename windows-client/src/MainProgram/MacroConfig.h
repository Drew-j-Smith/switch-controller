
#pragma once

#include "pch.h"

#include "Event/ConcreteClasses/ImageEvent.h"
#include "Macro/Macro.h"
#include "Macro/MacroCollection.h"

MacroCollection
getMacroConfig(const std::shared_ptr<cv::VideoCapture> &videoCapture,
               std::vector<std::shared_ptr<Macro>> macros) {
    // Deciders
    cv::Rect rectCrop(0, 0, 500, 500);
    // TODO fix
    auto haanitDecider = std::make_shared<ImageEvent>(
        cv::imread("data/haanit.png"), cv::imread("data/haanit mask.png"), 3,
        .97, rectCrop, videoCapture);

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
    return {macros};
}
