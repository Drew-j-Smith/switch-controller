#ifndef FACTORY_OBJECT_H
#define FACTORY_OBJECT_H

#include "pch.h"

#include <boost/property_tree/ptree.hpp>

#include "Factory.h"

template <class T> class FactoryObject {
  public:
    virtual std::shared_ptr<T>
    makeShared(const boost::property_tree::ptree &tree,
               Factory<T> &factory) const = 0;

    virtual std::string getTypeName() const = 0;
    virtual std::string toString() const = 0;
    virtual boost::property_tree::ptree toPtree() const = 0;
    virtual bool operator==(const T &other) const {
        return typeid(*this) == typeid(other);
    }
    virtual bool operator!=(const T &other) const { return !operator==(other); }
};

#endif