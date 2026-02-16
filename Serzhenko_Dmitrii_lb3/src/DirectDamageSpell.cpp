#include "DirectDamageSpell.h"
#include <iostream>

std::vector<std::shared_ptr<ITarget>> DirectDamageSpell::availableTargets;

DirectDamageSpell::DirectDamageSpell(std::string name, int damage, int manaCost, int castRange)
    : name(std::move(name)), damage(damage), manaCost(manaCost), castRange(castRange) {}

std::string DirectDamageSpell::getName() const {
    return name;
}

int DirectDamageSpell::getManaCost() const {
    return manaCost;
}

int DirectDamageSpell::getCastRange() const {
    return castRange;
}

int DirectDamageSpell::getDamage() const {
    return damage;
}

bool DirectDamageSpell::canCast(const Point& casterPosition, const Point& targetPosition) const {
    return isWithinRange(casterPosition, targetPosition);
}

void DirectDamageSpell::setAvailableTargets(const std::vector<std::shared_ptr<ITarget>>& targets) {
    availableTargets = targets;
}

std::shared_ptr<ITarget> DirectDamageSpell::findTargetAtPosition(const Point& position) const {
    for (const auto& target : availableTargets) {
        if (target->getPosition() == position && target->isAlive()) {
            return target;
        }
    }
    return nullptr;
}

void DirectDamageSpell::cast(const Point& casterPosition, const Point& targetPosition) {
    int distance = casterPosition.distanceTo(targetPosition);
    
    if (distance > castRange) {
        std::cout << "Цель находится слишком далеко!" << std::endl;
        std::cout << "Расстояние: " << distance << ", максимальный радиус: " << castRange << std::endl;
        return;
    }
    
    auto target = findTargetAtPosition(targetPosition);
    if (!target) {
        std::cout << "В выбранной позиции нет врага!" << std::endl;
        std::cout << "Заклинание не использовано." << std::endl;
        return;
    }
    
    std::cout << "Молния бьет по цели! Наносит " << damage << " урона врагу!" << std::endl;
    
    target->takeDamage(damage);
    
    if (!target->isAlive()) {
        std::cout << "Враг уничтожен!" << std::endl;
    }
}

std::unique_ptr<ISpell> DirectDamageSpell::clone() const {
    return std::make_unique<DirectDamageSpell>(*this);
}