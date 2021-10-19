cmake_minimum_required(VERSION 3.18)
project(ArduinoController)

# needed for boost json parser to not give a warning
add_definitions(-DBOOST_BIND_GLOBAL_PLACEHOLDERS)

# defining the windows version
add_definitions(-D_WIN32_WINNT=0x0A00)

find_package(OpenCV REQUIRED)
find_package(SFML 2 COMPONENTS window REQUIRED)
find_package(Boost COMPONENTS system thread unit_test_framework REQUIRED)
find_package(FFTW3f CONFIG REQUIRED)
find_path(AUDIOFILE_INCLUDE_DIRS "AudioFile.h")

set(CMAKE_LIBS ${OpenCV_LIBS} sfml-window ${Boost_LIBRARIES} FFTW3::fftw3f)

include_directories(
${CMAKE_CURRENT_SOURCE_DIR}/src
${CMAKE_CURRENT_SOURCE_DIR}/test
${OpenCV_INCLUDE_DIRS}
${SFML_INCLUDE_DIRS}
${Boost_INCLUDE_DIRS}
${FFTW3_INCLUDE_DIRS}
${AUDIOFILE_INCLUDE_DIRS}
)

set(CMAKE_SRC_FILES
src/CreateConfig.cpp
src/EditConfig.cpp
src/StartController.cpp
src/Decider/DeciderCollection.cpp
src/Decider/ImageProcessingDecider.cpp
src/Decider/ImageProcessingDeciderCollection.cpp
src/Decider/SoundDecider.cpp
src/Decider/SoundDeciderCollection.cpp
src/InputEvent/InputEventCollection.cpp
src/InputEvent/InputEventToggle.cpp
src/InputEvent/InputManager.cpp
src/Macro/Macro.cpp
src/Macro/MacroCollection.cpp
src/Utility/AudioSink.cpp
src/Utility/TimeString.cpp
)

set(CMAKE_TEST_FILES 
test/ArduinoControllerTest.cpp
test/InputEvent/InputEventGeneratorTest.cpp
)

add_library(SRC_LIB OBJECT ${CMAKE_SRC_FILES})
target_precompile_headers(SRC_LIB PUBLIC src/pch.h)
list(APPEND CMAKE_LIBS SRC_LIB)

add_executable(ArduinoController src/ArduinoController.cpp)
target_link_libraries(ArduinoController ${CMAKE_LIBS})

add_executable(ArduinoControllerTest ${CMAKE_TEST_FILES})
target_link_libraries(ArduinoControllerTest ${CMAKE_LIBS})

enable_testing()
add_test(NAME ArduinoControllerTest COMMAND ArduinoControllerTest --detect_memory_leaks=0)
# ctest -C Debug --output-on-failure