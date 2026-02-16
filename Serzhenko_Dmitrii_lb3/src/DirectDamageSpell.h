#ifndef DIRECTDAMAGESPELL_H
#define DIRECTDAMAGESPELL_H

#include "ISpell.h"
#include <vector>
#include <memory>

class DirectDamageSpell : public ISpell {
public:
    DirectDamageSpell(std::string name, int damage, int manaCost, int castRange);
    
    std::string getName() const override;
    int getManaCost() const override;
    int getCastRange() const override;
    bool canCast(const Point& casterPosition, const Point& targetPosition) const override;
    void cast(const Point& casterPosition, const Point& targetPosition) override;
    std::unique_ptr<ISpell> clone() const override;
    
    int getDamage() const;
    
    static void setAvailableTargets(const std::vector<std::shared_ptr<ITarget>>& targets);

private:
    std::string name;
    int damage;
    int manaCost;
    int castRange;
    
    static std::vector<std::shared_ptr<ITarget>> availableTargets;
    
    std::shared_ptr<ITarget> findTargetAtPosition(const Point& position) const;
};

#endif