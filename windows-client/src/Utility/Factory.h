#ifndef FACTORY_H
#define FACTORY_H


#include "pch.h"

#include <boost/property_tree/ptree.hpp>

template<class T>
class Factory
{
private:
    std::map<std::string, std::shared_ptr<T>> map;

public:
    Factory(std::map<std::string, std::shared_ptr<T>> classNameToObjectMap) { map = classNameToObjectMap; }

    std::shared_ptr<T> generateObject(const boost::property_tree::ptree & tree) {
        // TODO throw meaningful errors
        std::string type = tree.get<std::string>("TypeName");
        return (map.at(type))->makeShared(tree, *this);
    }
};



#endif