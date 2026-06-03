#include "bus.h"

Bus::Bus(QString color, int capacity, Direction dir, int row, int col)
    : m_color(color),
    m_capacity(capacity),
    m_direction(dir),
    m_row(row),
    m_col(col),
    m_currentPassengers(0)
{}

QString Bus::getColor() const { return m_color; }
int Bus::getCapacity() const { return m_capacity; }
Direction Bus::getDirection() const { return m_direction; }
int Bus::getRow() const { return m_row; }
int Bus::getCol() const { return m_col; }

void Bus::setPosition(int newRow, int newCol) {
    m_row = newRow;
    m_col = newCol;
}
