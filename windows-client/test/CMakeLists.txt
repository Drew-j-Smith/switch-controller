
enable_testing()

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_TEST_FILES 
ArduinoControllerTest.cpp
InputEvent/DefaultInputEventTest.cpp
InputEvent/InputEventGeneratorTest.cpp
InputEvent/InputEventTest.cpp
)


add_executable(ArduinoControllerTest ${CMAKE_TEST_FILES})
target_link_libraries(ArduinoControllerTest ${CMAKE_LIBS})


add_test(NAME DefaultInputEventTest COMMAND ArduinoControllerTest --detect_memory_leaks=0 --run_test=DefaultInputEventTest)
add_test(NAME ActiveInputEventTest COMMAND ArduinoControllerTest --detect_memory_leaks=0 --run_test=ActiveInputEventTest)
add_test(NAME InputEventGeneratorTest COMMAND ArduinoControllerTest --detect_memory_leaks=0 --run_test=InputEventGeneratorTest)
add_test(NAME InputEventTest COMMAND ArduinoControllerTest --detect_memory_leaks=0 --run_test=InputEventTest)
# ctest -C Debug --output-on-failure