#include "TimeString.h"

#include <time.h>

std::string getTimeStr(){
    //getting the time as a string
    auto t = std::time(nullptr);
    if (t == (std::time_t)(-1)) {
        std::cerr << "std::time threw an execption\n";
        throw std::exception("std::time threw an execption");
    }
    tm time;
    auto error = localtime_s(&time, &t);
    if (error != 0) {
        std::cerr << "localtime_s threw the error: " << error << '\n';
        throw std::exception("localtime_s threw the error: " + error);
    }
    std::ostringstream oss;
    oss << std::put_time(&time, "%m-%d-%Y %H-%M-%S");
    return oss.str();
}