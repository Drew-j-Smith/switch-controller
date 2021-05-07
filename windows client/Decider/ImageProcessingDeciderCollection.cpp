#include "ImageProcessingDeciderCollection.h"

static void matchImage(std::shared_ptr<ImageProcessingDecider> decider, const cv::Mat & screenshot){
    decider->update(screenshot);
}

void ImageProcessingDeciderCollection::startImageProcessingThread() {
    imageProcessing.store(true);
    imageProcessingThread = std::thread([&](){
        std::vector<std::future<void>> futures;
        cv::Mat screenshot;
        while (imageProcessing.load()) {
            screenshotUtility->screenshot(screenshot);
            for (auto & decider : deciders) {
                futures.push_back(std::async(std::launch::async, matchImage, decider, screenshot));
            }
            futures.clear();
        }
    });
}

ImageProcessingDeciderCollection::ImageProcessingDeciderCollection(const std::vector<std::shared_ptr<ImageProcessingDecider>> & deciders, const std::shared_ptr<ScreenshotUtility> & screenshotUtility) {
    this->deciders = deciders;
    this->screenshotUtility = screenshotUtility;
    startImageProcessingThread(); 
}

ImageProcessingDeciderCollection::ImageProcessingDeciderCollection(const boost::property_tree::ptree & tree, const std::shared_ptr<ScreenshotUtility> & screenshotUtility) {
    this->screenshotUtility = screenshotUtility;
    for (auto decider : tree) {
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