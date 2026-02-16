#ifndef ISPELL_H
#define ISPELL_H

#include "ITarget.h"
#include <string>
#include <memory>

class ISpell {
public:
    virtual ~ISpell() = default;
    
    virtual std::string getName() const = 0;
    virtual int getManaCost() const = 0;
    virtual int getCastRange() const = 0;
    virtual bool canCast(const Point& casterPosition, const Point& targetPosition) const = 0;
    virtual void cast(const Point& casterPosition, const Point& targetPosition) = 0;
    virtual std::unique_ptr<ISpell> clone() const = 0;
    
    bool isWithinRange(const Point& casterPosition, const Point& targetPosition) const {
        return casterPosition.distanceTo(targetPosition) <= getCastRange();
    }
};

#endif