#include "SoundDeciderCollection.h"

static void matchSound(std::shared_ptr<SoundDecider> decider, const std::vector<float> & soundData) {
    decider->update(soundData);
}

SoundDeciderCollection::SoundDeciderCollection(const boost::property_tree::ptree & tree) {
    continueUpdating.store(true);

    auto soundTree = tree.find("sound deciders");
    if (soundTree == tree.not_found()) {
        std::cerr << "The sound tree was load but not found in config.\n";
        return;
    }



    for (auto decider : soundTree->second) {
        if (decider.second.get("type", "") == "sound processing") {
            deciders.push_back(std::make_shared<SoundDecider>(decider.second));
        }
    }

    double bufferTime = 1.0;
    AudioSink::AudioInfo audioInfo = {0, 0, 0};
    audioSink = std::make_shared<AudioSink>(bufferTime, audioInfo);

    soundThread = std::thread([&](){
        audioSink->RecordAudioStreamLoopback();
    });

    updateThread = std::thread([&](){
        std::this_thread::sleep_for(std::chrono::milliseconds((long long)(1000 * bufferTime + 500)));
        std::vector<float> soundData;
        std::vector<std::future<void>> futures;
        while (continueUpdating.load()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            soundData = audioSink->getData()[0];
            for (auto decider : deciders) {
                futures.push_back(std::async(std::launch::async, matchSound, decider, soundData));
            }
            futures.clear();
        }
    });
}

SoundDeciderCollection::~SoundDeciderCollection() {
    audioSink->setFinished(true);
    continueUpdating.store(false);
    if (soundThread.joinable()) soundThread.join();
    if (updateThread.joinable()) updateThread.join();
}

std::map<std::string, std::shared_ptr<Decider>> SoundDeciderCollection::generateMap() const {
    std::map<std::string, std::shared_ptr<Decider>> map;
    for (auto decider : deciders) {
        map.insert({decider->getName(), decider});
    }
    return map;
};