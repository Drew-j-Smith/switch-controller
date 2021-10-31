#ifndef PTREE_CONSTRUCTABLE_CONSTRCUTOR_H
#define PTREE_CONSTRUCTABLE_CONSTRCUTOR_H


#include "pch.h"

#include <boost/property_tree/ptree.hpp>

template<class T>
class PtreeConstructableConstructor
{
protected:
    std::map<std::string, std::shared_ptr<T>> map;

    template<class classToRegister>
    void registerClass() {
        std::shared_ptr<T> obj = std::make_shared<classToRegister>();
        map.insert({obj->getTypeName(), obj});
    }

public:   
    virtual std::shared_ptr<T> generateObject(const boost::property_tree::ptree & tree) = 0;
};



#endif