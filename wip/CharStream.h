#ifndef CHAR_STREAM_H
#define CHAR_STREAM_H

#include "pch.h"

template<size_t frameSize>
class CharStream {
private:
    std::vector<std::array<unsigned char, frameSize>> data = {};
public:
    CharStream() {};

    void load(std::string filename);
    void loadFromHex(std::string filename);

    void save(std::string filename);
    void saveToHex(std::string filename);

    size_t size() const       { return data.size();   }
    void clear()              { data.clear();         }
    void pop_back()           { data.pop_back();      }
    void reserve(size_t size) { data.reserve(size)    }
    void shrink_to_fit()      { data.shrink_to_fit(); }
    void resize(size_t size)  { data.resize(size);    }

    void push_back(std::array<unsigned char, frameSize> & dataFrame) { data.push_back(dataFrame); }

    std::array<unsigned char, frameSize> & at         (size_t pos) const { return data.at(pos); }
    std::array<unsigned char, frameSize> & operator[] (size_t pos) const { return data[pos];    }
};



#endif