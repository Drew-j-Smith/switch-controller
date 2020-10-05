cmake_minimum_required(VERSION 2.8)
project(loaderTest)
find_package(OpenCV REQUIRED)
find_package(SFML 2 COMPONENTS graphics REQUIRED)   # any 2.x version
include_directories(${OpenCV_INCLUDE_DIRS})
add_executable(loaderTest main.cpp)
target_link_libraries(loaderTest ${OpenCV_LIBS} sfml-graphics)