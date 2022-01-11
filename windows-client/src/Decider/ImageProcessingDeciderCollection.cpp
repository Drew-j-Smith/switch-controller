#include "ImageProcessingDeciderCollection.h"

static void matchImage(std::shared_ptr<ImageProcessingDecider> decider, const cv::Mat & screenshot){
    decider->update(screenshot);
}

void ImageProcessingDeciderCollection::startImageProcessingThread() {
    imageProcessing.store(true);
    imageProcessingThread = std::thread([&](){
        std::vector<std::future<void>> futures;
        this->videoFrameSink->waitForInit();
        uint8_t* data = new uint8_t[this->videoFrameSink->getDataSize()];
        cv::Mat screenshot = cv::Mat(this->videoFrameSink->getHeight(), this->videoFrameSink->getWidth(), CV_8UC3, data);
        long long lastFrame = 0;
        
        while (imageProcessing.load()) {
            lastFrame = this->videoFrameSink->getNextData(data, lastFrame);
            for (auto & decider : deciders) {
                futures.push_back(std::async(std::launch::async, matchImage, decider, screenshot));
            }
            futures.clear();
        }

        delete[] data;
    });
}

ImageProcessingDeciderCollection::ImageProcessingDeciderCollection(const std::vector<std::shared_ptr<ImageProcessingDecider>> & deciders, const std::shared_ptr<VideoFrameSink> & videoFrameSink) {
    this->deciders = deciders;
    this->videoFrameSink = videoFrameSink;
    startImageProcessingThread(); 
}

ImageProcessingDeciderCollection::ImageProcessingDeciderCollection(const boost::property_tree::ptree & tree) {
    auto imageTree = tree.find("image deciders");
    if (imageTree == tree.not_found()) {
        std::cerr << "The image tree was loaded but not found in the config.\n";
        return;
    }

    std::string inputFormat = "dshow";
    std::string deviceName = "video=Game Capture HD60 S";
    std::map<std::string, std::string> options = {{"pixel_format", "bgr24"}};
    std::vector<std::shared_ptr<FFmpegFrameSink>> sinks;
    this->videoFrameSink = std::make_shared<VideoFrameSink>();
    sinks.push_back(this->videoFrameSink);

    av_log_set_level(AV_LOG_QUIET);

    this->ffmpegRecorder = std::make_shared<FFmpegRecorder>(inputFormat, deviceName, options, sinks);
    this->ffmpegRecorder->start();

    for (auto decider : imageTree->second) {
        if (decider.second.get("type", "") == "screenshot settings") {
            // TODO
        }
        if (decider.second.get("type", "") == "image processing") {
            deciders.push_back(std::make_shared<ImageProcessingDecider>(decider.second));
        }
    }

    startImageProcessingThread();
}

ImageProcessingDeciderCollection::~ImageProcessingDeciderCollection() {
    imageProcessing.store(false);
    if (imageProcessingThread.joinable())
        imageProcessingThread.join();
}

std::map<std::string, std::shared_ptr<Decider>> ImageProcessingDeciderCollection::generateMap() const {
    std::map<std::string, std::shared_ptr<Decider>> map;
    for (auto decider : deciders) {
        map.insert({decider->getName(), decider});
    }
    return map;
}