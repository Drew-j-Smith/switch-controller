#ifndef IMAGE_PROCESSING_DECIDER_COLLECTION_H
#define IMAGE_PROCESSING_DECIDER_COLLECTION_H

#include "pch.h"

#include <boost/property_tree/ptree.hpp>

#include "DeciderCollectionBase.h"
#include "ImageProcessingDecider.h"
#include "FFmpeg/VideoFrameSink.h"
#include "FFmpeg/FFmpegRecorder.h"


class ImageProcessingDeciderCollection : public DeciderCollectionBase
{
private:
    std::vector<std::shared_ptr<ImageProcessingDecider>> deciders;
    std::shared_ptr<FFmpegRecorder> ffmpegRecorder;
    std::shared_ptr<VideoFrameSink> videoFrameSink;
    std::thread imageProcessingThread;
    std::atomic<bool> imageProcessing;

    void startImageProcessingThread();

public:
    ImageProcessingDeciderCollection(const std::vector<std::shared_ptr<ImageProcessingDecider>> & deciders, const std::shared_ptr<VideoFrameSink> & videoFrameSink);
    ImageProcessingDeciderCollection(const boost::property_tree::ptree & tree);
    ~ImageProcessingDeciderCollection();

    void setDeciders(const std::vector<std::shared_ptr<ImageProcessingDecider>> & newDeciders) { this->deciders = newDeciders; }
    void setVideoFrameSink(const std::shared_ptr<VideoFrameSink> & newVideoFrameSink) { this->videoFrameSink = newVideoFrameSink; }
    const std::vector<std::shared_ptr<ImageProcessingDecider>> getDeciders() const { return deciders; }
    const std::shared_ptr<VideoFrameSink> getVideoFrameSink() { return videoFrameSink; }

    std::map<std::string, std::shared_ptr<Decider>> generateMap() const override;
};

#endif