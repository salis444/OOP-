#ifndef CELL_H
#define CELL_H

class Cell {
public:
    enum class Type { EMPTY, PLAYER, ENEMY };

    Cell(Type type = Type::EMPTY);
    Cell(const Cell& other);
    Cell(Cell&& other) noexcept;
    Cell& operator=(const Cell& other);
    Cell& operator=(Cell&& other) noexcept;
    
    Type getType() const;
    void setType(Type type);

private:
    Type type;
};

#endif