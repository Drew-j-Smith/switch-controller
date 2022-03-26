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

#include "pch.h"

/**
 * @brief The Decider class is used to decide what macro
 * activates after the current one expires. The name is used
 * to identify the correct decider for the macro.
 *
 */
class Decider {
public:
    virtual ~Decider() {}

    /**
     * @brief Returns the index of the list to be activated
     *
     * @return int
     */
    virtual int nextListIndex() const = 0;

    void enable() { useCount++; }

    void disable() { useCount--; }

    bool shouldUpdate() { return useCount > 0; }

    virtual void update() = 0;

private:
    int useCount;
};

/**
 * @brief DefaultDecider is a basic implementaion of Decider which
 * always returns zero and has the name "default"
 *
 */
class DefaultDecider : public Decider {
public:
    DefaultDecider() {}

    int nextListIndex() const override { return 0; }
    void update() override {}
};

#endif