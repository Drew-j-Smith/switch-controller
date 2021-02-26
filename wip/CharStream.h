#ifndef CHAR_STREAM_H
#define CHAR_STREAM_H

#include "pch.h"

template<size_t frameSize>
class CharStream : public std::vector<std::array<unsigned char, frameSize>>{
public:
    void load(const std::string & filename, bool isHex = true);

    void save(const std::string & filename, bool asHex = true);
};



#endif