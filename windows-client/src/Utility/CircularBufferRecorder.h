#ifndef AUDIO_LOOP_RECORDER_H
#define AUDIO_LOOP_RECORDER_H

#include <pch.h>

#include <SFML/Audio/SoundRecorder.hpp>


class CircularBufferRecorder : public sf::SoundRecorder
{
private:
    std::mutex mMutex;
    std::vector<sf::Int16> samples;
    long long bufferSize;
    int currSample = 0;
public:
    CircularBufferRecorder(long long bufferSize) : bufferSize(bufferSize) {
        setProcessingInterval(sf::milliseconds(20));
    }
    ~CircularBufferRecorder() override {}

    bool onStart() override { samples.resize(bufferSize); return true; };
    bool onProcessSamples(const sf::Int16* samples, std::size_t sampleCount) override {
        std::lock_guard<std::mutex> lock(mMutex);
        if (currSample + sampleCount > bufferSize) {
            std::copy(samples, samples + bufferSize - currSample, this->samples.begin() + currSample);
            std::copy(samples + bufferSize - currSample, samples + sampleCount, this->samples.begin());

            currSample -= bufferSize;
        }
        else {
            std::copy(samples, samples + sampleCount, this->samples.begin() + currSample);
        }
        currSample += sampleCount;
        return true;
    };
    void onStop() override {};

    void getSamples(std::vector<sf::Int16>& vec) {
        std::lock_guard<std::mutex> lock(mMutex);
        std::copy(samples.begin() + currSample, samples.end(), vec.begin());
        std::copy(samples.begin(), samples.begin() + currSample, vec.begin() + samples.size() - currSample);
    };
    void setBufferSize(double size) { bufferSize = size; };
    double getBufferSize() { return bufferSize; };

};





#endif