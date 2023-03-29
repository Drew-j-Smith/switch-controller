
add_executable(ArduinoController MainProgram/ArduinoController.cpp)

target_include_directories(ArduinoController PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_precompile_headers(ArduinoController PUBLIC pch.h)

target_link_libraries(ArduinoController ${CMAKE_LIBS})

add_custom_command(TARGET ArduinoController POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_SOURCE_DIR}/data"
        $<TARGET_FILE_DIR:ArduinoController>/data)
