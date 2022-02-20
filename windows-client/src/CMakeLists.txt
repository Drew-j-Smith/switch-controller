
file(GLOB_RECURSE CMAKE_SRC_FILES CONFIGURE_DEPENDS "*.cpp")
list(FILTER CMAKE_SRC_FILES EXCLUDE REGEX "ArduinoController.cpp")

add_library(SRC_LIB OBJECT ${CMAKE_SRC_FILES})
target_include_directories(SRC_LIB SYSTEM PUBLIC ${CMAKE_INCLUDE_DIRS})
target_precompile_headers(SRC_LIB PUBLIC pch.h)
set(CMAKE_LIBS ${CMAKE_LIBS} SRC_LIB)
set(CMAKE_LIBS ${CMAKE_LIBS} PARENT_SCOPE)

if(MSVC)
  target_compile_options(SRC_LIB PUBLIC /W4 /WX)
else()
  target_compile_options(SRC_LIB PUBLIC -Wall -Wextra -Wpedantic -Werror)
endif()

add_executable(ArduinoController MainProgram/ArduinoController.cpp)
target_link_libraries(ArduinoController ${CMAKE_LIBS})

add_custom_command(TARGET ArduinoController POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_SOURCE_DIR}/data"
        $<TARGET_FILE_DIR:ArduinoController>/data)
