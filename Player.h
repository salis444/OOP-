#ifndef PLAYER_H
#define PLAYER_H

#include "Hand.h"
#include "Point.h"
#include <memory>

class Player {
public:
    Player(int health = 100, int damage = 10, int mana = 30, Point position = Point(0, 0));
    
    // Основные методы
    int getHealth() const;
    int getDamage() const;
    int getScore() const;
    int getMana() const;
    Point getPosition() const;
    
    void takeDamage(int damage);
    void increaseScore(int points);
    void setPosition(const Point& position);
    
    bool isAlive() const;
    
    // Методы для заклинаний
    Hand& getHand();
    const Hand& getHand() const;
    void setMana(int newMana);           
    void restoreMana(int amount);   
    
    // Дополнительные методы
    void heal(int amount);
    void increaseDamage(int amount);

private:
    int health;
    int maxHealth;
    int damage;
    int score;
    int mana;
    int maxMana;
    Point position;
    Hand hand;
};

#endif