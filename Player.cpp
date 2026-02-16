#include "Player.h"
#include <algorithm>
#include <iostream>

Player::Player(int health, int damage, int mana, Point position)
    : health(health), maxHealth(health), damage(damage), score(0), 
      mana(mana), maxMana(mana), position(position), hand(2) {}

int Player::getHealth() const { 
    return health; 
}

int Player::getDamage() const { 
    return damage; 
}

int Player::getScore() const { 
    return score; 
}

int Player::getMana() const { 
    return mana; 
}

Point Player::getPosition() const { 
    return position; 
}

void Player::takeDamage(int damage) {
    health = std::max(0, health - damage);
}

void Player::increaseScore(int points) {
    score += points;
}

void Player::setPosition(const Point& newPosition) {
    position = newPosition;
}

bool Player::isAlive() const {
    return health > 0;
}

Hand& Player::getHand() {
    return hand;
}

const Hand& Player::getHand() const {
    return hand;
}

void Player::setMana(int newMana) {
    mana = std::max(0, std::min(maxMana, newMana));
}

void Player::restoreMana(int amount) {
    mana = std::min(maxMana, mana + amount);
}

void Player::heal(int amount) {
    health = std::min(maxHealth, health + amount);
}

void Player::increaseDamage(int amount) {
    damage += amount;
}