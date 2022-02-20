#ifndef FFMPEG_ERROR_H
#define FFMPEG_ERROR_H

#include "ErrorBase.h"
#include "pch.h"

class FFmpegInitErrorBase {};
typedef ErrorBase<FFmpegInitErrorBase, std::runtime_error> FFmpegInitError;

class FFmpegRuntimeErrorBase {};
typedef ErrorBase<FFmpegRuntimeErrorBase, std::runtime_error>
    FFmpegRuntimeError;

#endif