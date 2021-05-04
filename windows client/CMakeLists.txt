cmake_minimum_required(VERSION 3.18)
project(ArduinoController)

find_package(OpenCV REQUIRED)
find_package(SFML 2 COMPONENTS window REQUIRED)
find_package(Boost COMPONENTS system thread REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS} ${SFML_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})

add_executable(ArduinoController ArduinoController.cpp Macro/Macro.cpp Macro/MacroCollection.cpp
Decider/ImageProcessingDecider.cpp Utility/InputManager.cpp Utility/TimeString.cpp)
target_link_libraries(ArduinoController ${OpenCV_LIBS} sfml-window ${Boost_LIBRARIES})

target_precompile_headers(ArduinoController PUBLIC pch.h)