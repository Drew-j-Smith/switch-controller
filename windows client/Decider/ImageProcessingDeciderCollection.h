#ifndef IMAGE_PROCESSING_DECIDER_COLLECTION_H
#define IMAGE_PROCESSING_DECIDER_COLLECTION_H

#include "../pch.h"

#include <boost/property_tree/ptree.hpp>

#include "ImageProcessingDecider.h"
#include "../Utility/ScreenshotUtility.h"


class ImageProcessingDeciderCollection
{
private:
    std::vector<std::shared_ptr<ImageProcessingDecider>> deciders;
    std::shared_ptr<ScreenshotUtility> screenshotUtility;
    std::thread imageProcessingThread;
    std::atomic<bool> imageProcessing;

    static void matchImage(std::shared_ptr<ImageProcessingDecider> decider, const cv::Mat & screenshot){
        decider->update(screenshot);
    }

    void startImageProcessingThread() {
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

    void stopImageProcessingThread() {
        imageProcessing.store(false);
        if (imageProcessingThread.joinable())
            imageProcessingThread.join();
    }

public:
    ImageProcessingDeciderCollection(const std::vector<std::shared_ptr<ImageProcessingDecider>> & deciders, const std::shared_ptr<ScreenshotUtility> & screenshotUtility) {
        this->deciders = deciders;
        this->screenshotUtility = screenshotUtility;
        startImageProcessingThread(); 
    }
    ImageProcessingDeciderCollection(const boost::property_tree::ptree & tree, const std::shared_ptr<ScreenshotUtility> & screenshotUtility) {
        this->screenshotUtility = screenshotUtility;
        for (auto decider : tree) {
            if (decider.second.get("type", "") == "image processing") {
                deciders.push_back(std::make_shared<ImageProcessingDecider>(decider.second));
            }
        }
        startImageProcessingThread();
    }
    ~ImageProcessingDeciderCollection() {
        stopImageProcessingThread();
    }

    void setDeciders(const std::vector<std::shared_ptr<ImageProcessingDecider>> & deciders) { this->deciders = deciders; }
    void setScreenshotUtility(const std::shared_ptr<ScreenshotUtility> & screenshotUtility) { this->screenshotUtility = screenshotUtility; }
    const std::vector<std::shared_ptr<ImageProcessingDecider>> getDeciders() const { return deciders; }
    const std::shared_ptr<ScreenshotUtility> getScreenshotUtility() { return screenshotUtility; }

    const std::map<std::string, std::shared_ptr<Decider>> generateMap() {
        std::map<std::string, std::shared_ptr<Decider>> map;
        for (auto decider : deciders) {
            map.insert({decider->getName(), decider});
        }
        return map;
    }
};

#endif