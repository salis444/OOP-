#ifndef FIELD_H
#define FIELD_H

#include <vector>
#include <random>
#include "Cell.h"
#include "Point.h"

class Field {
public:
    Field(int width, int height);
    Field(const Field& other);
    Field(Field&& other) noexcept;
    Field& operator=(const Field& other);
    Field& operator=(Field&& other) noexcept;
    ~Field() = default;
    
    int getWidth() const;
    int getHeight() const;
    
    bool isWithinBounds(const Point& position) const;
    
    void setCellType(const Point& position, Cell::Type type);
    Cell::Type getCellType(const Point& position) const;
    
    void setPlayerPosition(const Point& position);
    Point getPlayerPosition() const;
    
    void draw() const;
    
    Point getRandomEmptyCell() const;

private:
    int width;
    int height;
    std::vector<std::vector<Cell>> grid;
    Point playerPosition;
    mutable std::mt19937 randomGenerator;
};

#endif