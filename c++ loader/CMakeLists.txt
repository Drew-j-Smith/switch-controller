cmake_minimum_required(VERSION 2.8)
project(loaderTest)

find_package(OpenCV REQUIRED)
find_package(SFML 2 COMPONENTS graphics REQUIRED)
find_package(Boost COMPONENTS system thread REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS} ${SFML_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})

add_executable(loaderTest main.cpp)
target_link_libraries(loaderTest ${OpenCV_LIBS} ${Boost_LIBRARIES} sfml-graphics)