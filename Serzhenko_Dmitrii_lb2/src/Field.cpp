#include "Field.h"
#include <iostream>
#include <stdexcept>

Field::Field(int width, int height) : width(width), height(height), 
                                      randomGenerator(std::random_device{}()) {
    if (width < 10 || height < 10 || width > 25 || height > 25) {
        throw std::invalid_argument("Field size must be between 10x10 and 25x25");
    }
    
    grid.resize(height);
    for (int i = 0; i < height; i++) {
        grid[i].resize(width, Cell(Cell::Type::EMPTY));
    }
}

Field::Field(const Field& other) : width(other.width), height(other.height), 
                                   grid(other.grid), playerPosition(other.playerPosition),
                                   randomGenerator(std::random_device{}()) {}

Field::Field(Field&& other) noexcept : width(other.width), height(other.height),
                                       grid(std::move(other.grid)), 
                                       playerPosition(std::move(other.playerPosition)),
                                       randomGenerator(std::move(other.randomGenerator)) {
    other.width = 0;
    other.height = 0;
}

Field& Field::operator=(const Field& other) {
    if (this != &other) {
        width = other.width;
        height = other.height;
        grid = other.grid;
        playerPosition = other.playerPosition;
    }
    return *this;
}

Field& Field::operator=(Field&& other) noexcept {
    if (this != &other) {
        width = other.width;
        height = other.height;
        grid = std::move(other.grid);
        playerPosition = std::move(other.playerPosition);
        randomGenerator = std::move(other.randomGenerator);
        
        other.width = 0;
        other.height = 0;
    }
    return *this;
}

int Field::getWidth() const { return width; }
int Field::getHeight() const { return height; }

bool Field::isWithinBounds(const Point& position) const {
    return position.getX() >= 0 && position.getX() < width &&
           position.getY() >= 0 && position.getY() < height;
}

void Field::setCellType(const Point& position, Cell::Type type) {
    if (isWithinBounds(position)) {
        grid[position.getY()][position.getX()].setType(type);
    }
}

Cell::Type Field::getCellType(const Point& position) const {
    if (isWithinBounds(position)) {
        return grid[position.getY()][position.getX()].getType();
    }
    throw std::out_of_range("Position out of bounds");
}

void Field::setPlayerPosition(const Point& position) {
    if (isWithinBounds(position)) {
        playerPosition = position;
    }
}

Point Field::getPlayerPosition() const {
    return playerPosition;
}

Point Field::getRandomEmptyCell() const {
    std::vector<Point> emptyCells;
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (grid[y][x].getType() == Cell::Type::EMPTY) {
                emptyCells.push_back(Point(x, y));
            }
        }
    }
    
    if (emptyCells.empty()) {
        return Point(0, 0);
    }
    
    std::uniform_int_distribution<> dist(0, emptyCells.size() - 1);
    return emptyCells[dist(randomGenerator)];
}

void Field::draw() const {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            Cell::Type type = grid[y][x].getType();
            if (type == Cell::Type::EMPTY) {
                std::cout << ". ";
            } else if (type == Cell::Type::PLAYER) {
                std::cout << "P ";
            } else if (type == Cell::Type::ENEMY) {
                std::cout << "E ";
            }
        }
        std::cout << std::endl;
    }
}