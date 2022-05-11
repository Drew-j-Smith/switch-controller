
file(GLOB_RECURSE CMAKE_SRC_FILES CONFIGURE_DEPENDS "*.cpp")

add_executable(ArduinoController ${CMAKE_SRC_FILES})

target_include_directories(ArduinoController SYSTEM PUBLIC ${CMAKE_INCLUDE_DIRS})
target_precompile_headers(ArduinoController PUBLIC pch.h)

if(MSVC)
  target_compile_options(ArduinoController PUBLIC /W4 /WX)
else()
  target_compile_options(ArduinoController PUBLIC -Wall -Wextra -Wpedantic -Werror)
endif()

target_link_libraries(ArduinoController ${CMAKE_LIBS})

add_custom_command(TARGET ArduinoController POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_SOURCE_DIR}/data"
        $<TARGET_FILE_DIR:ArduinoController>/data)
