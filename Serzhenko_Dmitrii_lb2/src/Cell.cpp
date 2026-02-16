#include "Cell.h"

Cell::Cell(Type type) : type(type) {}

Cell::Cell(const Cell& other) : type(other.type) {}

Cell::Cell(Cell&& other) noexcept : type(other.type) {
    other.type = Type::EMPTY;
}

Cell& Cell::operator=(const Cell& other) {
    if (this != &other) {
        type = other.type;
    }
    return *this;
}

Cell& Cell::operator=(Cell&& other) noexcept {
    if (this != &other) {
        type = other.type;
        other.type = Type::EMPTY;
    }
    return *this;
}

Cell::Type Cell::getType() const {
    return type;
}

void Cell::setType(Type type) {
    this->type = type;
}