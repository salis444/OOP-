#include "Enemy.h"
#include <algorithm>
#include <cstdlib>
#include <ctime>

Enemy::Enemy(int health, int damage, Point position) 
    : health(health), damage(damage), position(position) {}

// Реализация методов ITarget
void Enemy::takeDamage(int damage) {
    health = std::max(0, health - damage);
}

bool Enemy::isAlive() const {
    return health > 0;
}

Point Enemy::getPosition() const { 
    return position; 
}

int Enemy::getHealth() const { 
    return health; 
}

int Enemy::getDamage() const { 
    return damage; 
}

void Enemy::setPosition(const Point& newPosition) {
    position = newPosition;
}

Point Enemy::chooseMoveDirection(const Point& currentPosition, const Point& playerPosition) const {
    int dx = playerPosition.getX() - currentPosition.getX();
    int dy = playerPosition.getY() - currentPosition.getY();
    
    if (std::abs(dx) > std::abs(dy)) {
        if (dx > 0) return Point(1, 0);
        else return Point(-1, 0);
    } else {
        if (dy > 0) return Point(0, 1);
        else return Point(0, -1);
    }
}