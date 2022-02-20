#ifndef ERROR_BASE_H
#define ERROR_BASE_H

#include "pch.h"

#include <boost/stacktrace.hpp>

template <class T, class Base> class ErrorBase : public Base {
private:
    std::string stacktrace_str() {
        std::stringstream ss;
        ss << "\nStacktrace:\n" << boost::stacktrace::stacktrace();
        return ss.str();
    }

public:
    ErrorBase(const char *message)
        : Base(std::string(message) + stacktrace_str()) {}
    ErrorBase(const std::string &message) : Base(message + stacktrace_str()) {}
    ErrorBase(const ErrorBase &other) : Base(other) {}
};

#endif