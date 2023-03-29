cmake_minimum_required(VERSION 3.18)

project(ArduinoController)

set(CMAKE_CXX_STANDARD 20)

add_compile_definitions(_WIN32_WINNT=0x0A00)

find_package(OpenCV REQUIRED)
find_package(SFML 2 COMPONENTS window graphics REQUIRED)
find_package(Boost REQUIRED COMPONENTS program_options)
find_package(spdlog CONFIG REQUIRED)

add_executable(ArduinoController src/ArduinoController.cpp)

target_include_directories(ArduinoController PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_precompile_headers(ArduinoController PUBLIC src/pch.h)

target_link_libraries(ArduinoController ${OpenCV_LIBS} sfml-window sfml-system ${Boost_LIBRARIES} spdlog::spdlog_header_only)

add_custom_command(TARGET ArduinoController POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_SOURCE_DIR}/data"
        $<TARGET_FILE_DIR:ArduinoController>/data)
