#ifndef MACRO_DECIDER_H
#define MACRO_DECIDER_H

/**
 * @file MacroDecider.h
 * @author Drew Smith
 * @brief The MacroDecider class is used to decide what macro
 * activates after the current one expires.
 * @date 2021-04-21
 * 
 */

#include "pch.h"

/**
 * @brief The MacroDecider class is used to decide what macro
 * activates after the current one expires. The name is used
 * to identify the correct decider for the macro.
 * 
 */
class MacroDecider
{
public:
    /**
     * @brief Get the name of the object.
     * 
     * @return const std::string 
     */
    const std::string getName() const { return name; }

    /**
     * @brief Returns the index of the macro list to be activated next
     * 
     * @return int 
     */
    virtual int nextMacroListIndex() const = 0;
protected:
    std::string name;
};

/**
 * @brief MacroDefaultDecider is a basic implementaion of MacroDecider which
 * always returns zero and has the name "default"
 * 
 */
class MacroDefaultDecider : public MacroDecider
{
public:
    MacroDefaultDecider() { name = "default"; }
    MacroDefaultDecider(const std::string & name) { this->name = name; }

    int nextMacroListIndex() const override { return 0; };
};

#endif