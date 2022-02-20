#ifndef SERIAL_ERROR_H
#define SERIAL_ERROR_H

#include "ErrorBase.h"
#include "pch.h"

class SerialErrorBase {};
typedef ErrorBase<SerialErrorBase, std::runtime_error> SerialError;

#endif