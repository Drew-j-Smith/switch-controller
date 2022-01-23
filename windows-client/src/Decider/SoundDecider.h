#ifndef SOUND_DECIDER_H
#define SOUND_DECIDER_H

#include "pch.h"

#include <boost/property_tree/ptree.hpp>

#include "Decider.h"

extern "C" {
    #include <fftw3.h>
}

class SoundDecider : public Decider {
private:
    std::vector<float> matchAudio;
    
    // fftw
    std::vector<float> fftwIn;
    std::vector<std::complex<float>> fftwOut;
    fftwf_plan fftwPlan;
    int fftwSize;

    std::vector<float> matchFrequencies;
    double matchThreshold;
    std::atomic<double> matchValue;

    std::vector<float> findFrequencies(const std::vector<float> & samples);

public:
    SoundDecider(const boost::property_tree::ptree & tree); // dont forget to set name

    ~SoundDecider() override {
        fftwf_destroy_plan(fftwPlan);
    }

    void update(std::vector<float> soundData);

    const std::vector<float>* getMatchAudio() const { return &matchAudio; }

    int nextListIndex() const override;
};

#endif