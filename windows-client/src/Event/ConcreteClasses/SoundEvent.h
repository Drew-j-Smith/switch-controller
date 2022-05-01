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
    mutable std::vector<uint8_t> audioData;

    // fftw
    mutable std::vector<float> fftwIn;
    mutable std::vector<std::complex<float>> fftwOut;
    struct fftwf_deleter {
        void operator()(fftwf_plan *f) const noexcept {
            fftwf_destroy_plan(*f);
            delete f;
        };
    };
    std::unique_ptr<fftwf_plan, fftwf_deleter> fftwPlan{new fftwf_plan(),
                                                        fftwf_deleter()};
    int fftwSize;

    std::vector<float> matchFrequencies;
    double matchThreshold;
    std::atomic<double> matchValue;

    std::vector<float> findFrequencies(const std::vector<float> &samples) const;

public:
    SoundEvent(const std::string &filename, double matchThreshold,
               std::shared_ptr<AudioFrameSink> audioFrameSink);

    uint8_t value() const override;
};

#endif