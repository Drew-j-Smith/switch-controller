#ifndef CHAR_STREAM_H
#define CHAR_STREAM_H

#include "pch.h"

template<size_t frameSize>
class CharStream : public std::vector<std::array<unsigned char, frameSize>>{
public:
    void load(std::string filename);
    void loadFromHex(std::string filename);

    void save(std::string filename);
    void saveToHex(std::string filename);
};



#endif