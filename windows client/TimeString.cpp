#include "TimeString.h"

std::string getTimeStr(){
    //getting the time as a string
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%m-%d-%Y %H-%M-%S");
    return oss.str();
}