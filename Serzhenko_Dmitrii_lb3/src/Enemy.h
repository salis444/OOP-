#ifndef ENEMY_H
#define ENEMY_H

#include "Point.h"
#include "ITarget.h"
#include <cstdlib>

class Enemy : public ITarget {
public:
    Enemy(int health = 50, int damage = 5, Point position = Point(0, 0));
    
    // Методы ITarget
    void takeDamage(int damage) override;
    bool isAlive() const override;
    Point getPosition() const override;
    int getHealth() const override;
    
    // Оригинальные методы
    int getDamage() const;
    Point chooseMoveDirection(const Point& currentPosition, const Point& playerPosition) const;
    
    // Сеттер позиции
    void setPosition(const Point& position);

private:
    int health;
    int damage;
    Point position;
};

#endif