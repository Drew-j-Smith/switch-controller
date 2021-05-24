#include "DeciderCollection.h"

#include "ImageProcessingDeciderCollection.h"
#include "SoundDeciderCollection.h"

DeciderCollection::DeciderCollection(const boost::property_tree::ptree & tree) {
    deciderCollections.push_back(std::make_shared<ImageProcessingDeciderCollection>(tree));
    deciderCollections.push_back(std::make_shared<SoundDeciderCollection>(tree));
}

std::map<std::string, std::shared_ptr<Decider>> DeciderCollection::generateMap() const {
    std::map<std::string, std::shared_ptr<Decider>> map;
    for (auto collection : deciderCollections) {
        auto tempMap = collection->generateMap();
        map.insert(tempMap.begin(), tempMap.end());
    }
    return map;
}