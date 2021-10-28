#ifndef PTREE_CONSTRUCTABLE_H
#define PTREE_CONSTRUCTABLE_H

#include "pch.h"

#include <boost/property_tree/ptree.hpp>

template<class T>
class PtreeConstructable
{
public:
    virtual std::shared_ptr<T> makeShared(const boost::property_tree::ptree & tree, const std::map<std::string, std::shared_ptr<T>> & objectMap) const = 0;

    virtual std::string toString() const = 0;
    virtual boost::property_tree::ptree toPtree() const = 0;
    virtual bool operator==(const T& other) const { return typeid(*this) == typeid(other); }
    virtual bool operator!=(const T& other) const { return !operator==(other); }
};




#endif