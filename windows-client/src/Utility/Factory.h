#ifndef FACTORY_H
#define FACTORY_H


#include "pch.h"

#include <boost/property_tree/ptree.hpp>

#include "PtreeConstructable.h"

template<class T>
class Factory
{
private:
    std::map<std::string, std::shared_ptr<PtreeConstructable<T>>> map;

public:
    Factory(std::map<std::string, std::shared_ptr<PtreeConstructable<T>>> classNameToObjectMap) { map = classNameToObjectMap; }

    std::shared_ptr<PtreeConstructable<T>> generateObject(const boost::property_tree::ptree & tree) {
        // TODO throw meaningful errors
        std::string type = tree.get<std::string>("ClassName");
        return (map.at(type))->makeShared(tree, map);
    }
};



#endif