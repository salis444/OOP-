#ifndef POINT_H
#define POINT_H

#include <cmath>

class Point {
public:
    Point(int x = 0, int y = 0);
    
    int getX() const;
    int getY() const;
    
    void setX(int x);
    void setY(int y);
    
    bool operator==(const Point& other) const;
    bool operator!=(const Point& other) const;
    
    Point operator+(const Point& other) const;
    
    int distanceTo(const Point& other) const;

private:
    int x;
    int y;
};

#endif