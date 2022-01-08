
set(CMAKE_SRC_FILES
CreateConfig.cpp
EditConfig.cpp
StartController.cpp
Decider/DeciderCollection.cpp
Decider/ImageProcessingDecider.cpp
Decider/ImageProcessingDeciderCollection.cpp
Decider/SoundDecider.cpp
Decider/SoundDeciderCollection.cpp
InputEvent/InputEventCollection.cpp
InputEvent/InputEventToggle.cpp
InputEvent/InputManager.cpp
Macro/Macro.cpp
Macro/MacroCollection.cpp
Utility/AudioSink.cpp
Utility/SerialPort.cpp
Utility/TimeString.cpp
)

add_library(SRC_LIB OBJECT ${CMAKE_SRC_FILES})
target_include_directories(SRC_LIB PUBLIC ${CMAKE_INCLUDE_DIRS})
target_precompile_headers(SRC_LIB PUBLIC pch.h)
set(CMAKE_LIBS ${CMAKE_LIBS} SRC_LIB)
set(CMAKE_LIBS ${CMAKE_LIBS} PARENT_SCOPE)

add_executable(ArduinoController ArduinoController.cpp)
target_link_libraries(ArduinoController ${CMAKE_LIBS})
