cmake_minimum_required(VERSION 3.18)
project(ArduinoController)

# needed for boost json parser to not give a warning
add_definitions(-DBOOST_BIND_GLOBAL_PLACEHOLDERS)

# defining the windows version
add_definitions(-D_WIN32_WINNT=0x0A00)

find_package(OpenCV REQUIRED)
find_package(SFML 2 COMPONENTS window REQUIRED)
find_package(Boost COMPONENTS system thread REQUIRED)
find_package(FFTW3f CONFIG REQUIRED)
find_path(AUDIOFILE_INCLUDE_DIRS "AudioFile.h")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${OpenCV_INCLUDE_DIRS} ${SFML_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${FFTW3_INCLUDE_DIRS} ${AUDIOFILE_INCLUDE_DIRS})

file(GLOB_RECURSE CMAKE_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
add_executable(ArduinoController ${CMAKE_SRC_FILES})
target_link_libraries(ArduinoController ${OpenCV_LIBS} sfml-window ${Boost_LIBRARIES} FFTW3::fftw3f)

target_precompile_headers(ArduinoController PUBLIC src/pch.h)