cmake_minimum_required(VERSION 3.18)

if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
    CACHE STRING "")
endif()

project(ArduinoController)

# needed for boost json parser to not give a warning
add_definitions(-DBOOST_BIND_GLOBAL_PLACEHOLDERS)

# defining the windows version
add_definitions(-D_WIN32_WINNT=0x0A00)

find_package(OpenCV REQUIRED)
find_package(SFML 2 COMPONENTS window audio REQUIRED)
find_package(Boost COMPONENTS system thread unit_test_framework REQUIRED)
find_package(FFTW3f CONFIG REQUIRED)

set(FFMPEG_INCLUDE_NAMES
libavcodec/avcodec.h
libavformat/avformat.h
libavutil/avutil.h
libavdevice/avdevice.h
libswscale/swscale.h
libswresample/swresample.h
)

set(FFMPEG_LIB_NAMES
avcodec
avformat
avutil
avdevice
swscale
swresample
)

foreach(DIR_NAME LIB_NAME IN ZIP_LISTS FFMPEG_INCLUDE_NAMES FFMPEG_LIB_NAMES)
    set(LIB "LIB-NOTFOUND")
    find_path(DIR ${DIR_NAME})
    find_library(LIB ${LIB_NAME})
    list(APPEND FFMPEG_INCLUDE_DIRS ${DIR})
    list(APPEND FFMPEG_LIBS ${LIB})
endforeach()

set(CMAKE_LIBS 
${OpenCV_LIBS}
sfml-window
sfml-audio
${Boost_LIBRARIES}
FFTW3::fftw3f
${FFMPEG_LIBS}
)

set(CMAKE_INCLUDE_DIRS
${CMAKE_CURRENT_SOURCE_DIR}/src
${OpenCV_INCLUDE_DIRS}
${SFML_INCLUDE_DIRS}
${Boost_INCLUDE_DIRS}
${FFTW3_INCLUDE_DIRS}
${AUDIOFILE_INCLUDE_DIRS}
${FFMPEG_INCLUDE_DIRS}
)


add_subdirectory(src)

enable_testing()
add_subdirectory(test)
