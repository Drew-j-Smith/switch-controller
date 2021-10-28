#ifndef PTREE_CONSTRUCTABLE_CONSTRCUTOR_H
#define PTREE_CONSTRUCTABLE_CONSTRCUTOR_H


#include "pch.h"

#include <boost/property_tree/ptree.hpp>

template<class T>
class PtreeConstructableConstructor
{
protected:
    std::map<std::string, std::shared_ptr<T>> map;
public:
    template<class classToRegister>
    void registerClass() {
        std::shared_ptr<T> obj = std::make_shared<classToRegister>();
        map.insert({obj->getTypeName(), obj});
    }

    virtual std::shared_ptr<T> generateObject(const boost::property_tree::ptree & tree) = 0;

    template<class otherClass>
    bool operator==(const PtreeConstructableConstructor<otherClass> & other) {
        if (typeid(otherClass) != typeid(T)) {
            return false;
        }

        if (map.size() != other.map.size()) {
            return false;
        }
        
        for (auto it = map.begin(); it != map.end(); ++it) {
            if (other.map.find(it->first) == other.map.end()) {
                return false;
            }
        }
        
        return true;
    }
    template<class otherClass>
    bool operator!=(const PtreeConstructableConstructor<otherClass> & other) {
        return !operator==(other);
    }
};



#endif