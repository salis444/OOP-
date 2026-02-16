#ifndef AREADAMAGESPELL_H
#define AREADAMAGESPELL_H

#include "ISpell.h"
#include <vector>
#include <memory>

class AreaDamageSpell : public ISpell {
public:
    AreaDamageSpell(std::string name, int damage, int manaCost, int castRange, int areaSize = 2);
    
    std::string getName() const override;
    int getManaCost() const override;
    int getCastRange() const override;
    bool canCast(const Point& casterPosition, const Point& targetPosition) const override;
    void cast(const Point& casterPosition, const Point& targetPosition) override;
    std::unique_ptr<ISpell> clone() const override;
    
    int getDamage() const;
    int getAreaSize() const;
    
    static void setAvailableTargets(const std::vector<std::shared_ptr<ITarget>>& targets);

private:
    std::string name;
    int damage;
    int manaCost;
    int castRange;
    int areaSize;
    
    static std::vector<std::shared_ptr<ITarget>> availableTargets;
    
    std::vector<std::shared_ptr<ITarget>> findTargetsInArea(const Point& center) const;
    
    bool isInArea(const Point& point, const Point& center) const;
};

#endif