
set(CMAKE_SRC_FILES
CreateConfig.cpp
EditConfig.cpp
StartController.cpp
Decider/DeciderCollection.cpp
Decider/ImageProcessingDecider.cpp
Decider/ImageProcessingDeciderCollection.cpp
Decider/SoundDecider.cpp
Decider/SoundDeciderCollection.cpp
FFmpeg/FFmpegDecoder.cpp
FFmpeg/FFmpegRecorder.cpp
InputEvent/InputEventCollection.cpp
InputEvent/InputEventToggle.cpp
InputEvent/InputManager.cpp
Macro/Macro.cpp
Macro/MacroCollection.cpp
Utility/SerialPort.cpp
)

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

add_executable(ArduinoController ArduinoController.cpp)
target_link_libraries(ArduinoController ${CMAKE_LIBS})
