
enable_testing()

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_TEST_FILES 
ArduinoControllerTest.cpp
InputEvent/ConstantInputEventTest.cpp
InputEvent/InputEventTemplateTest.cpp
)


add_executable(ArduinoControllerTest ${CMAKE_TEST_FILES})
target_link_libraries(ArduinoControllerTest ${CMAKE_LIBS})


add_test(NAME ConstantInputEventTest COMMAND ArduinoControllerTest --detect_memory_leaks=0 --run_test=ConstantInputEventTest)
add_test(NAME InputEventTemplateTest COMMAND ArduinoControllerTest --detect_memory_leaks=0 --run_test=InputEventTemplateTest)
