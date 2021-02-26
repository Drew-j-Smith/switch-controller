#ifndef CHAR_STREAM_H
#define CHAR_STREAM_H

#include "pch.h"



template<size_t frameSize>
class CharStream : public std::vector<std::array<unsigned char, frameSize>>{
private:
    int hexToInt(char t){
        if(isdigit(t))
            return t - '0';
        else
            return tolower(t) - 'a' + 10;
    }
public:
    void load(const std::string & filename, const bool isHex = true){
        clear();
        reserve(10000);
        if (isHex){
            std::ifstream infile(filename, std::ios::in | std::ios::binary);
            if (infile) {
                std::array<unsigned char, frameSize> buffer;
                char currentChar;
                while (!infile.eof()) {
                    int i;
                    for (i = 0; i < frameSize * 2; i++) {
                        infile >> currentChar;
                        if (!isxdigit(currentChar))
                            break;
                        if (i % 2 == 0)
                            buffer[i/2] = hexToInt(currentChar) * 16;
                        else
                            buffer[i/2] += hexToInt(currentChar);
                    }
                    if (i != frameSize * 2){
                        std::cerr << "Non-hex character in file \"" << filename << "\"\n";
                        break;
                    }
                    if (infile.good())
                        push_back(buffer);
                }
            }
            else {
                std::cerr << "Error opening file \"" << filename << "\"\n";
            }
            infile.close();
        }
        else {
            std::ifstream infile(filename, std::ios::in | std::ios::binary);
            if (infile) {
                std::array<unsigned char, frameSize> buffer;
                while (!infile.eof()) {
                    infile.read((char*)buffer.data(), frameSize);
                    if (infile.good())
                        push_back(buffer);
                }
            }
            else {
                std::cerr << "Error opening file \"" << filename << "\"\n";
            }
            infile.close();
        }
        shrink_to_fit();
    }

    void save(const std::string & filename, const bool asHex = true){
        std::ofstream outfile;
        if (asHex)
            outfile.open(filename, std::ios::out);
        else
            outfile.open(filename, std::ios::out | std::ios::binary);
        if (outfile) {
            print(outfile, asHex);
        }
        else {
            std::cerr << "Error writing to file \"" << filename << "\"\n";
        }
        outfile.close();
    }

    void print(std::ostream & out, const bool asHex = true){
        if (asHex){
            out << std::hex << std::setfill('0');
            for (int i = 0; i < size(); i++) {
                for (int j = 0; j < frameSize; j++) {
                    out << std::setw(2) << (int)at(i).at(j);
                }
                out << std::endl;
            }
        }
        else {
            for (int i = 0; i < size(); i++) {
                out.write((char*)at(i).data(), frameSize);
            }
        }
    }
};



#endif