import os

os.system(".\\windows-client\\build\\test\\Debug\\ArduinoControllerTest.exe --list_content --detect_memory_leaks=0 2> windows-client/build/test/Debug/py-output")

with open("windows-client/build/test/Debug/py-output") as f:
    [print(f"add_test(NAME {test} COMMAND ArduinoControllerTest --detect_memory_leaks=0 --run_test={test})") for test in \
        [line.replace("*", "").replace("\n", "") for line in f if not line.startswith(" ")]]