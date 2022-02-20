#ifndef DECIDER_COLLECTION_H
#define DECIDER_COLLECTION_H

#include "pch.h"

#include "DeciderCollectionBase.h"

class DeciderCollection : public DeciderCollectionBase {
private:
    std::vector<std::shared_ptr<DeciderCollectionBase>> deciderCollections;

public:
    DeciderCollection(const boost::property_tree::ptree &tree);

    std::map<std::string, std::shared_ptr<Decider>>
    generateMap() const override;
};

#endif