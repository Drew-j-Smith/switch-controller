#ifndef DECIDER_COLLECTION_BASE_H
#define DECIDER_COLLECTION_BASE_H

#include "pch.h"

#include  "Decider.h"

class DeciderCollectionBase
{
public:
    virtual std::map<std::string, std::shared_ptr<Decider>> generateMap() const = 0;
};

#endif