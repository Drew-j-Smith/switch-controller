#ifndef DECIDER_H
#define DECIDER_H

/**
 * @file Decider.h
 * @author Drew Smith
 * @brief The Decider class is used to decide what macro
 * activates after the current one expires.
 * @date 2021-04-21
 * 
 */

#include "../pch.h"

/**
 * @brief The Decider class is used to decide what macro
 * activates after the current one expires. The name is used
 * to identify the correct decider for the macro.
 * 
 */
class Decider
{
public:
    /**
     * @brief Get the name of the object.
     * 
     * @return const std::string 
     */
    const std::string getName() const { return name; }

    /**
     * @brief Returns the index of the list to be activated
     * 
     * @return int 
     */
    virtual int nextListIndex() const = 0;
protected:
    std::string name;
};

/**
 * @brief DefaultDecider is a basic implementaion of Decider which
 * always returns zero and has the name "default"
 * 
 */
class DefaultDecider : public Decider
{
public:
    DefaultDecider() { name = "default"; }
    DefaultDecider(const std::string & name) { this->name = name; }

    int nextListIndex() const override { return 0; };
};

#endif