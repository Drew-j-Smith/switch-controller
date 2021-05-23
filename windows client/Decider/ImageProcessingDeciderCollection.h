#ifndef IMAGE_PROCESSING_DECIDER_COLLECTION_H
#define IMAGE_PROCESSING_DECIDER_COLLECTION_H

#include "pch.h"

#include <boost/property_tree/ptree.hpp>

#include "DeciderCollectionBase.h"
#include "ImageProcessingDecider.h"
#include "Utility/ScreenshotUtility.h"


class ImageProcessingDeciderCollection : public DeciderCollectionBase
{
private:
    std::vector<std::shared_ptr<ImageProcessingDecider>> deciders;
    std::shared_ptr<ScreenshotUtility> screenshotUtility;
    std::thread imageProcessingThread;
    std::atomic<bool> imageProcessing;

    void startImageProcessingThread();

public:
    ImageProcessingDeciderCollection(const std::vector<std::shared_ptr<ImageProcessingDecider>> & deciders, const std::shared_ptr<ScreenshotUtility> & screenshotUtility);
    ImageProcessingDeciderCollection(const boost::property_tree::ptree & tree);
    ~ImageProcessingDeciderCollection();

    void setDeciders(const std::vector<std::shared_ptr<ImageProcessingDecider>> & deciders) { this->deciders = deciders; }
    void setScreenshotUtility(const std::shared_ptr<ScreenshotUtility> & screenshotUtility) { this->screenshotUtility = screenshotUtility; }
    const std::vector<std::shared_ptr<ImageProcessingDecider>> getDeciders() const { return deciders; }
    const std::shared_ptr<ScreenshotUtility> getScreenshotUtility() { return screenshotUtility; }

    std::map<std::string, std::shared_ptr<Decider>> generateMap() const override;
};

#endif