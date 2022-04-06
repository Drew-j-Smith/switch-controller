#ifndef SOUND_EVENT_H
#define SOUND_EVENT_H

#include "pch.h"

#include "Event/Event.h"

#include "FFmpeg/AudioFrameSink.h"

#include <fftw3.h>

class SoundEvent : public Event {
private:
    std::vector<float> matchAudio;

    std::shared_ptr<AudioFrameSink> audioFrameSink;

    // fftw
    mutable std::vector<float> fftwIn;
    mutable std::vector<std::complex<float>> fftwOut;
    fftwf_plan fftwPlan;
    int fftwSize;

    std::vector<float> matchFrequencies;
    double matchThreshold;
    std::atomic<double> matchValue;

    std::vector<float> findFrequencies(const std::vector<float> &samples) const;

public:
    SoundEvent(const std::string &filename, double matchThreshold,
               std::shared_ptr<AudioFrameSink> audioFrameSink);

    ~SoundEvent() override { fftwf_destroy_plan(fftwPlan); }

    void update() override {}

    uint8_t value() const override;
};

#endif