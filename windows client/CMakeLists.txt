cmake_minimum_required(VERSION 2.8)
project(ArduinoController)

find_package(OpenCV REQUIRED)
find_package(SFML 2 COMPONENTS graphics REQUIRED)
find_package(Boost COMPONENTS system thread REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS} ${SFML_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})

add_executable(ArduinoController Arduino.cpp)
target_link_libraries(ArduinoController ${OpenCV_LIBS} sfml-graphics ${Boost_LIBRARIES})

target_precompile_headers(ArduinoController PUBLIC pch.h)