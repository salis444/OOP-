#ifndef HAND_H
#define HAND_H

#include "ISpell.h"
#include <vector>
#include <memory>
#include <random>

class Hand {
public:
    explicit Hand(int capacity = 2);
    
    bool addSpell(std::unique_ptr<ISpell> spell);
    bool removeSpell(int index);
    
    const ISpell* getSpell(int index) const;
    ISpell* getSpell(int index);
    
    bool useSpell(int index, const Point& casterPosition, const Point& targetPosition);
    
    bool isFull() const;
    bool isEmpty() const;
    int getSize() const;
    int getCapacity() const;
    
    void drawRandomSpell(const std::vector<std::unique_ptr<ISpell>>& spellDeck);
    
    void display() const;

private:
    std::vector<std::unique_ptr<ISpell>> spells;
    int capacity;
    mutable std::mt19937 randomGenerator;
    
    bool checkInvariant() const;
};

#endif