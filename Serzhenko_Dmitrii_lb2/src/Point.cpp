#include "Point.h"

Point::Point(int x, int y) : x(x), y(y) {}

int Point::getX() const { return x; }
int Point::getY() const { return y; }

void Point::setX(int x) { this->x = x; }
void Point::setY(int y) { this->y = y; }

bool Point::operator==(const Point& other) const {
    return x == other.x && y == other.y;
}

bool Point::operator!=(const Point& other) const {
    return !(*this == other);
}

Point Point::operator+(const Point& other) const {
    return Point(x + other.x, y + other.y);
}

int Point::distanceTo(const Point& other) const {
    return std::abs(x - other.x) + std::abs(y - other.y);
}