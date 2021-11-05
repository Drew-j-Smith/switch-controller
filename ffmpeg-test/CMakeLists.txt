cmake_minimum_required(VERSION 3.17)

if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
    CACHE STRING "")
endif()

project(decode_encode)

find_package(OpenCV REQUIRED)

set(FFMPEG_INCLUDE_NAMES
libavcodec/avcodec.h
libavformat/avformat.h
libavutil/avutil.h
libavdevice/avdevice.h
libswscale/swscale.h
)

set(FFMPEG_LIB_NAMES
avcodec
avformat
avutil
avdevice
swscale
)

foreach(DIR_NAME LIB_NAME IN ZIP_LISTS FFMPEG_INCLUDE_NAMES FFMPEG_LIB_NAMES)
    set(LIB "LIB-NOTFOUND")
    find_path(DIR ${DIR_NAME})
    find_library(LIB ${LIB_NAME})
    list(APPEND FFMPEG_INCLUDE_DIRS ${DIR})
    list(APPEND FFMPEG_LIBS ${LIB})
endforeach()

add_executable(decode_encode test1.cpp)
target_include_directories(decode_encode PRIVATE ${FFMPEG_INCLUDE_DIRS} ${OpenCV_INCLUDE_DIRS} )
target_link_libraries(decode_encode PRIVATE ${FFMPEG_LIBS} ${OpenCV_LIBS})

add_executable(ffmpeg-test test2.cpp FfmpegRecorder.cpp FfmpegDecoder.cpp)
target_include_directories(ffmpeg-test PRIVATE ${FFMPEG_INCLUDE_DIRS} ${OpenCV_INCLUDE_DIRS} )
target_link_libraries(ffmpeg-test PRIVATE ${FFMPEG_LIBS} ${OpenCV_LIBS})