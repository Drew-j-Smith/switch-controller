#ifndef SOUND_DECIDER_H
#define SOUND_DECIDER_H

#include "pch.h"

#include "Decider.h"

#include "FFmpeg/AudioFrameSink.h"

#include <fftw3.h>

class SoundDecider : public Decider {
private:
    std::vector<float> matchAudio;

    std::shared_ptr<AudioFrameSink> audioFrameSink;

    // fftw
    std::vector<float> fftwIn;
    std::vector<std::complex<float>> fftwOut;
    fftwf_plan fftwPlan;
    int fftwSize;

    std::vector<float> matchFrequencies;
    double matchThreshold;
    std::atomic<double> matchValue;

    std::vector<float> findFrequencies(const std::vector<float> &samples);

public:
    SoundDecider(const std::string &filename, double matchThreshold,
                 std::shared_ptr<AudioFrameSink> audioFrameSink);

    ~SoundDecider() override { fftwf_destroy_plan(fftwPlan); }

    void update() override;

    int nextListIndex() const override;
};

#endif