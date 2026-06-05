#include "bus.h"

//Construtor da classe Bus
Bus::Bus(QString color, int capacity, Direction dir, int row, int col)
    : m_color(color),
    m_capacity(capacity),
    m_direction(dir),
    m_row(row),
    m_col(col),
    m_currentPassengers(0) // Todos os autocarros começam vazios
{}

// Retorna a cor do autocarro
QString Bus::getColor() const { return m_color; }

// Retorna a capacidade máxima do autocarro
int Bus::getCapacity() const { return m_capacity; }

// Retorna a direção permitida para o movimento (Enum: Horizontal ou Vertical)
Direction Bus::getDirection() const { return m_direction; }

// Retorna a coordenada atual da Linha na grelha
int Bus::getRow() const { return m_row; }

// Retorna a coordenada atual da Coluna na grelha
int Bus::getCol() const { return m_col; }


//Método crucial para quando o autocarro se move em direção à plataforma de embarque.
void Bus::setPosition(int newRow, int newCol) {
    m_row = newRow;
    m_col = newCol;
}

