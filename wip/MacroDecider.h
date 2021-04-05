#ifndef MACRO_DECIDER_H
#define MACRO_DECIDER_H

#include "pch.h"

class MacroDecider
{
public:
    const std::string & getName() { return name; }
    virtual int nextMacroListIndex() const = 0;
protected:
    std::string name;
};

class MacroDefaultDecider : public MacroDecider
{
public:
    MacroDefaultDecider() { name = ""; }
    MacroDefaultDecider(const std::string & name) { this->name = name; }

    int nextMacroListIndex() const override { return 0; };
};

#endif