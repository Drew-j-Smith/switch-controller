cmake_minimum_required(VERSION 2.8)
project(ArduinoController)

find_package(OpenCV REQUIRED)
find_package(SFML 2 COMPONENTS graphics REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS} ${SFML_INCLUDE_DIRS})

add_executable(ArduinoController Arduino.cpp SerialPort.cpp)
target_link_libraries(ArduinoController ${OpenCV_LIBS} sfml-graphics)