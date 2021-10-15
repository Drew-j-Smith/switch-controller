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

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${OpenCV_INCLUDE_DIRS} ${SFML_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${FFTW3_INCLUDE_DIRS})

add_executable(ArduinoController ArduinoController.cpp Macro/Macro.cpp Macro/MacroCollection.cpp
Decider/ImageProcessingDecider.cpp Decider/ImageProcessingDeciderCollection.cpp Decider/DeciderCollection.cpp 
Decider/SoundDecider.cpp Decider/SoundDeciderCollection.cpp Utility/AudioSink.cpp
InputEvent/InputManager.cpp InputEvent/InputEventCollection.cpp InputEvent/InputEventToggle.cpp Utility/TimeString.cpp
EditConfig.cpp CreateConfig.cpp StartController.cpp)
target_link_libraries(ArduinoController ${OpenCV_LIBS} sfml-window ${Boost_LIBRARIES} FFTW3::fftw3f)

target_precompile_headers(ArduinoController PUBLIC pch.h)