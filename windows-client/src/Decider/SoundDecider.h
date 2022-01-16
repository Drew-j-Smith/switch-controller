#ifndef SOUND_DECIDER_H
#define SOUND_DECIDER_H

#include "pch.h"

#include <boost/property_tree/ptree.hpp>

#include "Decider.h"
#include "Utility/fftwPlan.h"

class SoundDecider : public Decider {
private:
    std::vector<float> matchAudio;
    std::shared_ptr<fftwPlan> plan;
    std::vector<float> matchFrequencies;
    double matchThreshold;
    std::atomic<double> matchValue;
public:
    SoundDecider(const boost::property_tree::ptree & tree); // dont forget to set name

    void update(std::vector<float> soundData);

    const std::vector<float>* getMatchAudio() const { return &matchAudio; }

    int nextListIndex() const override;
};

#endif