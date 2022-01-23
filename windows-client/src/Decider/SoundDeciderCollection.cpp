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

    std::string inputFormat = "dshow";
    std::string deviceName = "audio=Game Capture HD60 S Audio";
    std::map<std::string, std::string> options = {};
    std::vector<std::shared_ptr<FFmpegFrameSink>> sinks;
    this->audioSink = std::make_shared<AudioFrameSink>(AV_CH_LAYOUT_MONO, AV_SAMPLE_FMT_S16, 48000, true, 48000);
    sinks.push_back(this->audioSink);

    av_log_set_level(AV_LOG_QUIET);

    this->ffmpegRecorder = std::make_shared<FFmpegRecorder>(inputFormat, deviceName, options, sinks);
    this->ffmpegRecorder->start();

    audioSink->waitForInit();

    updateThread = std::thread([&](){
        std::vector<uint8_t> data;
        std::vector<std::future<void>> futures;
        long long lastFrame = audioSink->getData(data);
        std::vector<float> soundData((float*)data.data(), (float*)(data.data() + data.size()));
        while (continueUpdating.load()) {
            lastFrame = audioSink->getNextData(data, lastFrame);
            for (auto decider : deciders) {
                futures.push_back(std::async(std::launch::async, matchSound, decider, soundData));
            }
            futures.clear();
        }
    });
}

SoundDeciderCollection::~SoundDeciderCollection() {
    continueUpdating.store(false);
    if (updateThread.joinable()) updateThread.join();
}

std::map<std::string, std::shared_ptr<Decider>> SoundDeciderCollection::generateMap() const {
    std::map<std::string, std::shared_ptr<Decider>> map;
    for (auto decider : deciders) {
        map.insert({decider->getName(), decider});
    }
    return map;
}