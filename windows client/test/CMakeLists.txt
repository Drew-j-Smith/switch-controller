cmake_minimum_required(VERSION 3.18)
project(ArduinoControllerTest)

# needed for boost json parser to not give a warning
add_definitions(-DBOOST_BIND_GLOBAL_PLACEHOLDERS)

# defining the windows version
add_definitions(-D_WIN32_WINNT=0x0A00)

find_package(OpenCV REQUIRED)
find_package(SFML 2 COMPONENTS window REQUIRED)
find_package(Boost COMPONENTS system thread unit_test_framework REQUIRED)
find_package(FFTW3f CONFIG REQUIRED)
find_path(AUDIOFILE_INCLUDE_DIRS "AudioFile.h")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${OpenCV_INCLUDE_DIRS} ${SFML_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${FFTW3_INCLUDE_DIRS} ${AUDIOFILE_INCLUDE_DIRS})

file(GLOB_RECURSE CMAKE_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/../src/*.cpp)
file(GLOB_RECURSE CMAKE_TEST_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
list(FILTER CMAKE_SRC_FILES EXCLUDE REGEX "ArduinoController.cpp")
list(FILTER CMAKE_TEST_FILES EXCLUDE REGEX "CMakeCXXCompilerId.cpp")
add_executable(ArduinoControllerTest ${CMAKE_SRC_FILES} ${CMAKE_TEST_FILES})
target_link_libraries(ArduinoControllerTest ${OpenCV_LIBS} sfml-window ${Boost_LIBRARIES} FFTW3::fftw3f)

target_precompile_headers(ArduinoControllerTest PUBLIC ../src/pch.h)