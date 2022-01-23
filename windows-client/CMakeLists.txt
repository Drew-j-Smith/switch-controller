cmake_minimum_required(VERSION 3.18)

if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
    CACHE STRING "")
endif()

project(ArduinoController)

set(CMAKE_CXX_STANDARD 17)

# set(CMAKE_CXX_CLANG_TIDY "clang-tidy;-checks=*")

# needed for boost json parser to not give a warning
add_definitions(-DBOOST_BIND_GLOBAL_PLACEHOLDERS)

# defining the windows version
add_definitions(-D_WIN32_WINNT=0x0A00)

find_package(OpenCV REQUIRED)
find_package(SFML 2 COMPONENTS window REQUIRED)
find_package(Boost COMPONENTS system thread unit_test_framework REQUIRED)
find_package(FFTW3f CONFIG REQUIRED)
find_package(FFMPEG REQUIRED)

set(CMAKE_LIBS 
${OpenCV_LIBS}
sfml-window
sfml-audio
${Boost_LIBRARIES}
FFTW3::fftw3f
${FFMPEG_LIBRARIES}
)

set(CMAKE_INCLUDE_DIRS
${CMAKE_CURRENT_SOURCE_DIR}/src
${OpenCV_INCLUDE_DIRS}
${SFML_INCLUDE_DIRS}
${Boost_INCLUDE_DIRS}
${FFTW3_INCLUDE_DIRS}
${AUDIOFILE_INCLUDE_DIRS}
${FFMPEG_INCLUDE_DIRS}
${FFMPEG_LIBRARY_DIRS}
)


add_subdirectory(src)

enable_testing()
add_subdirectory(test)
