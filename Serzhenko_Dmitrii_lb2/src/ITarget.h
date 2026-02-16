#ifndef ITARGET_H
#define ITARGET_H

#include "Point.h"

class ITarget {
public:
    virtual ~ITarget() = default;
    
    virtual void takeDamage(int damage) = 0;
    virtual bool isAlive() const = 0;
    virtual Point getPosition() const = 0;
    virtual int getHealth() const = 0;
};

#endif