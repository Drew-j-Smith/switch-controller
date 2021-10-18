#ifndef SOUND_DECIDER_COLLECTION_H
#define SOUND_DECIDER_COLLECTION_H

#include "pch.h"

#include <boost/property_tree/ptree.hpp>

#include "DeciderCollectionBase.h"
#include "SoundDecider.h"
#include "Utility/AudioSink.h"

class SoundDeciderCollection : public DeciderCollectionBase {
private:
    std::vector<std::shared_ptr<SoundDecider>> deciders;
    std::thread soundThread;
    std::thread updateThread;
    std::atomic<bool> continueUpdating;
    std::shared_ptr<AudioSink> audioSink;

public:
    SoundDeciderCollection(const boost::property_tree::ptree & tree);
    ~SoundDeciderCollection();

    std::map<std::string, std::shared_ptr<Decider>> generateMap() const override;
};

#endif