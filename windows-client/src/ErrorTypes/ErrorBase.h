#ifndef ERROR_BASE_H
#define ERROR_BASE_H

#include "pch.h"

#include <boost/stacktrace.hpp>

template <class T, class Base> class ErrorBase : public Base {
public:
    ErrorBase(const char *message)
        : Base(std::string(message) + "\n" +
               boost::stacktrace::to_string(boost::stacktrace::stacktrace())) {}
    ErrorBase(const std::string &message)
        : Base(message + "\n" +
               boost::stacktrace::to_string(boost::stacktrace::stacktrace())) {}
    ErrorBase(const ErrorBase &other) : Base(other) {}
};

#endif