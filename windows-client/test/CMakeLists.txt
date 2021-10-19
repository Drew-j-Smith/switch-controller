
enable_testing()

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_TEST_FILES 
ArduinoControllerTest.cpp
InputEvent/DefaultInputEventTest.cpp
InputEvent/InputEventGeneratorTest.cpp
)


add_executable(ArduinoControllerTest ${CMAKE_TEST_FILES})
target_link_libraries(ArduinoControllerTest ${CMAKE_LIBS})


add_test(NAME TestInputEventGenerator COMMAND ArduinoControllerTest --detect_memory_leaks=0 --run_test=InputEventGeneratorTest)
add_test(NAME TestDefaultInputEventTest COMMAND ArduinoControllerTest --detect_memory_leaks=0 --run_test=DefaultInputEventTest)
add_test(NAME TestActiveInputEventTest COMMAND ArduinoControllerTest --detect_memory_leaks=0 --run_test=ActiveInputEventTest)
# ctest -C Debug --output-on-failure