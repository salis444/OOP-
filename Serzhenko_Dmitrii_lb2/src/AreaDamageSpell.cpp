#include "AreaDamageSpell.h"
#include <iostream>

std::vector<std::shared_ptr<ITarget>> AreaDamageSpell::availableTargets;

AreaDamageSpell::AreaDamageSpell(std::string name, int damage, int manaCost, int castRange, int areaSize)
    : name(std::move(name)), damage(damage), manaCost(manaCost), castRange(castRange), areaSize(areaSize) {}

std::string AreaDamageSpell::getName() const {
    return name;
}

int AreaDamageSpell::getManaCost() const {
    return manaCost;
}

int AreaDamageSpell::getCastRange() const {
    return castRange;
}

int AreaDamageSpell::getDamage() const {
    return damage;
}

int AreaDamageSpell::getAreaSize() const {
    return areaSize;
}

bool AreaDamageSpell::canCast(const Point& casterPosition, const Point& targetPosition) const {
    return isWithinRange(casterPosition, targetPosition);
}

void AreaDamageSpell::setAvailableTargets(const std::vector<std::shared_ptr<ITarget>>& targets) {
    availableTargets = targets;
}

bool AreaDamageSpell::isInArea(const Point& point, const Point& center) const {
    int dx = std::abs(point.getX() - center.getX());
    int dy = std::abs(point.getY() - center.getY());
    return dx < areaSize && dy < areaSize;
}

std::vector<std::shared_ptr<ITarget>> AreaDamageSpell::findTargetsInArea(const Point& center) const {
    std::vector<std::shared_ptr<ITarget>> targetsInArea;
    
    for (const auto& target : availableTargets) {
        if (target->isAlive() && isInArea(target->getPosition(), center)) {
            targetsInArea.push_back(target);
        }
    }
    
    return targetsInArea;
}

void AreaDamageSpell::cast(const Point& casterPosition, const Point& targetPosition) {
    
    std::cout << "Ледяной вихрь охватывает область " 
              << areaSize << "x" << areaSize << " клеток вокруг вас!" << std::endl;
    
    auto targets = findTargetsInArea(casterPosition);
    
    if (targets.empty()) {
        std::cout << "В области нет врагов. Заклинание использовано." << std::endl;
        return;
    }
    
    std::cout << "Поражено врагов: " << targets.size() << std::endl;
    
    for (const auto& target : targets) {
        std::cout << "Наносит " << damage << " урона врагу!" << std::endl;
        
        target->takeDamage(damage);
        
        if (!target->isAlive()) {
            std::cout << "Враг уничтожен!" << std::endl;
        }
    }
}

std::unique_ptr<ISpell> AreaDamageSpell::clone() const {
    return std::make_unique<AreaDamageSpell>(*this);
}