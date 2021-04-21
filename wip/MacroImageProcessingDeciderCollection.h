#ifndef MACRO_IMAGE_PROCESSING_DECIDER_COLLECTION
#define MACRO_IMAGE_PROCESSING_DECIDER_COLLECTION

#include "pch.h"

#include <boost/property_tree/ptree.hpp>

#include "MacroImageProcessingDecider.h"
#include "ScreenshotUtility.h"


class MacroImageProcessingDeciderCollection
{
private:
    std::vector<std::shared_ptr<MacroImageProcessingDecider>> deciders;
    std::shared_ptr<ScreenshotUtility> screenshotUtility;
    std::thread imageProcessingThread;
    std::atomic<bool> imageProcessing;

    static void matchImage(std::shared_ptr<MacroImageProcessingDecider> decider, const cv::Mat & screenshot){
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
    MacroImageProcessingDeciderCollection(const std::vector<std::shared_ptr<MacroImageProcessingDecider>> & deciders, const std::shared_ptr<ScreenshotUtility> & screenshotUtility) {
        this->deciders = deciders;
        this->screenshotUtility = screenshotUtility;
        startImageProcessingThread(); 
    }
    MacroImageProcessingDeciderCollection(const boost::property_tree::ptree & tree, const std::shared_ptr<ScreenshotUtility> & screenshotUtility) {
        this->screenshotUtility = screenshotUtility;
        for (auto decider : tree) {
            if (decider.second.get("type", "") == "image processing") {
                deciders.push_back(std::make_shared<MacroImageProcessingDecider>(decider.second));
            }
        }
        startImageProcessingThread();
    }
    ~MacroImageProcessingDeciderCollection() {
        stopImageProcessingThread();
    }

    void setDeciders(const std::vector<std::shared_ptr<MacroImageProcessingDecider>> & deciders) { this->deciders = deciders; }
    void setScreenshotUtility(const std::shared_ptr<ScreenshotUtility> & screenshotUtility) { this->screenshotUtility = screenshotUtility; }
    const std::vector<std::shared_ptr<MacroImageProcessingDecider>> getDeciders() const { return deciders; }
    const std::shared_ptr<ScreenshotUtility> getScreenshotUtility() { return screenshotUtility; }

    const std::map<std::string, std::shared_ptr<MacroDecider>> generateMap() {
        std::map<std::string, std::shared_ptr<MacroDecider>> map;
        for (auto decider : deciders) {
            map.insert({decider->getName(), decider});
        }
        return map;
    }
};

#endif