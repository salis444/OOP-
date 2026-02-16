#include "Hand.h"
#include "DirectDamageSpell.h"
#include "AreaDamageSpell.h"
#include <iostream>
#include <random>

Hand::Hand(int capacity) 
    : capacity(capacity <= 0 ? 2 : capacity), 
      randomGenerator(std::random_device{}()) {
    spells.reserve(capacity);
}

bool Hand::checkInvariant() const {
    return spells.size() <= static_cast<size_t>(capacity);
}

bool Hand::addSpell(std::unique_ptr<ISpell> spell) {
    if (isFull()) {
        std::cout << "Рука полна! Невозможно добавить новое заклинание." << std::endl;
        return false;
    }
    
    spells.push_back(std::move(spell));
    return checkInvariant();
}

bool Hand::removeSpell(int index) {
    if (index < 0 || index >= static_cast<int>(spells.size())) {
        return false;
    }
    
    spells.erase(spells.begin() + index);
    return true;
}

const ISpell* Hand::getSpell(int index) const {
    if (index < 0 || index >= static_cast<int>(spells.size())) {
        return nullptr;
    }
    return spells[index].get();
}

ISpell* Hand::getSpell(int index) {
    if (index < 0 || index >= static_cast<int>(spells.size())) {
        return nullptr;
    }
    return spells[index].get();
}

bool Hand::useSpell(int index, const Point& casterPosition, const Point& targetPosition) {
    if (index < 0 || index >= static_cast<int>(spells.size())) {
        std::cout << "Неверный индекс заклинания!" << std::endl;
        return false;
    }
    
    std::cout << ">>> АКТИВИРОВАНО: " << spells[index]->getName() << " <<<" << std::endl;
    spells[index]->cast(casterPosition, targetPosition);
    
    std::cout << "Заклинание использовано." << std::endl;
    return true;
}

bool Hand::isFull() const {
    return spells.size() >= static_cast<size_t>(capacity);
}

bool Hand::isEmpty() const {
    return spells.empty();
}

int Hand::getSize() const {
    return static_cast<int>(spells.size());
}

int Hand::getCapacity() const {
    return capacity;
}

void Hand::drawRandomSpell(const std::vector<std::unique_ptr<ISpell>>& spellDeck) {
    if (spellDeck.empty()) {
        std::cout << "Колода заклинаний пуста!" << std::endl;
        return;
    }
    
    if (isFull()) {
        std::cout << "Невозможно взять заклинание: рука полна!" << std::endl;
        return;
    }
    
    std::uniform_int_distribution<> dist(0, spellDeck.size() - 1);
    int randomIndex = dist(randomGenerator);
    
    if (addSpell(spellDeck[randomIndex]->clone())) {
        std::cout << "Вы получили: " << spellDeck[randomIndex]->getName() << std::endl;
    }
}

void Hand::display() const {
    if (isEmpty()) {
        std::cout << "Рука пуста." << std::endl;
        return;
    }
    
    std::cout << "Заклинания в руке (" << spells.size() << "/" << capacity << "):" << std::endl;
    for (size_t i = 0; i < spells.size(); ++i) {
        std::cout << i + 1 << ". " << spells[i]->getName() 
                  << " (Радиус: " << spells[i]->getCastRange() 
                  << ", Мана: " << spells[i]->getManaCost() << ")";
        
        if (auto directSpell = dynamic_cast<const DirectDamageSpell*>(spells[i].get())) {
            std::cout << " [Прямой урон: " << directSpell->getDamage() << "]";
        } else if (auto areaSpell = dynamic_cast<const AreaDamageSpell*>(spells[i].get())) {
            std::cout << " [Урон по площади: " << areaSpell->getDamage() 
                      << ", Область: " << areaSpell->getAreaSize() << "x" 
                      << areaSpell->getAreaSize() << "]";
        }
        std::cout << std::endl;
    }
}