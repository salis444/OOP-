#ifndef PLAYER_H
#define PLAYER_H

#include "Hand.h"
#include "Point.h"
#include <memory>

class Player {
public:
    Player(int health = 100, int damage = 10, int mana = 30, Point position = Point(0, 0));
    
    int getHealth() const;
    int getMaxHealth() const { return maxHealth; }
    int getDamage() const;
    int getScore() const;
    int getMana() const;
    int getMaxMana() const { return maxMana; }
    Point getPosition() const;
    
    void takeDamage(int damage);
    void increaseScore(int points);
    void setPosition(const Point& position);
    
    void setHealth(int h) { health = h; }
    void setMaxHealth(int mh) { maxHealth = mh; }
    void setDamage(int d) { damage = d; }
    void setScore(int s) { score = s; }
    void setMana(int m) { mana = m; }
    void setMaxMana(int mm) { maxMana = mm; }
    
    bool isAlive() const;
    
    Hand& getHand();
    const Hand& getHand() const;
    void restoreMana(int amount);
    
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