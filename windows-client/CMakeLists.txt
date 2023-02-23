cmake_minimum_required(VERSION 3.18)

project(ArduinoController)

set(CMAKE_CXX_STANDARD 20)

add_compile_definitions(_WIN32_WINNT=0x0A00)

find_package(OpenCV REQUIRED)
find_package(SFML 2 COMPONENTS window graphics REQUIRED)
find_package(Boost REQUIRED)
find_package(FFTW3f CONFIG REQUIRED)
find_package(FFMPEG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

set(CMAKE_LIBS 
${OpenCV_LIBS}
sfml-window
sfml-audio
sfml-graphics
${Boost_LIBRARIES}
FFTW3::fftw3f
${FFMPEG_LIBRARIES}
spdlog::spdlog
)


add_subdirectory(src)
