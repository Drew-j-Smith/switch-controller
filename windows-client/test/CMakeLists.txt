
enable_testing()

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE CMAKE_TEST_FILES CONFIGURE_DEPENDS "*.cpp")
set(TEST_EXECUTABLE_NAME ArduinoControllerTest)

add_executable(${TEST_EXECUTABLE_NAME} ${CMAKE_TEST_FILES})
target_link_libraries(${TEST_EXECUTABLE_NAME} ${CMAKE_LIBS})

# this will create a test case for every BOOST_AUTO_TEST_CASE in a BOOST_AUTO_TEST_SUITE
# this structure is required for this to work
foreach(TEST_FILE ${CMAKE_TEST_FILES})
    file(READ "${TEST_FILE}" TEST_FILE_CONTENTS)
    
    string(REGEX MATCHALL "BOOST_AUTO_TEST_CASE\\([ \t]*[^\\)]+" 
        FOUND_TESTS ${TEST_FILE_CONTENTS})
    string(REGEX MATCH "BOOST_AUTO_TEST_SUITE\\([ \t]*[^\\)]+"
        TEST_SUITE_TEMP ${TEST_FILE_CONTENTS})
    if (TEST_SUITE_TEMP)
        string(REGEX REPLACE ".*\\([ \t]*([^\\) \t]+)" "\\1" 
            TEST_SUITE ${TEST_SUITE_TEMP})
        foreach(TEST ${FOUND_TESTS})
            string(REGEX REPLACE ".*\\([ \t]*([^\\) \t]+)" "\\1" 
                TEST_NAME ${TEST})
            add_test(NAME "${TEST_EXECUTABLE_NAME}.${TEST_NAME}" 
                        COMMAND ${TEST_EXECUTABLE_NAME}
                        --run_test=${TEST_SUITE}/${TEST_NAME} --detect_memory_leaks=0)
        endforeach()
    endif()
endforeach()
