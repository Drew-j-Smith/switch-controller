cmake_minimum_required(VERSION 2.8)
project(ArduinoController)

find_package(OpenCV REQUIRED)
find_package(SFML 2 COMPONENTS graphics REQUIRED)
find_package(Boost COMPONENTS system)

if(NOT Boost_FOUND)
    message(WARNING "Boost has not been found but error was not thrown for compatabilty with vcpkg")
endif()


include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${SFML_INCLUDE_DIRS})
include_directories(${Boost_INCLUDE_DIRS})

add_executable(ArduinoController Arduino.cpp)
target_link_libraries(ArduinoController ${OpenCV_LIBS} ${Boost_LIBRARIES} sfml-graphics -lpthread)