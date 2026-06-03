#include "board.h"

Board::Board(int r, int c) 
    : m_rows(r), 
    m_cols(c) {
    m_slots.resize(6, false); // Valor padrão inicial
}

int Board::getRows() const { return m_rows; }
int Board::getCols() const { return m_cols; }

const std::vector<Bus>& Board::getBuses() const { return m_buses; }

// Necessário para o GameController conseguir alterar a posição (-10, -10) e mover os autocarros
std::vector<Bus>& Board::getBusesMutable() { return m_buses; }

void Board::addBus(const Bus& bus) {
    m_buses.push_back(bus);
}

// Remapeamento do tamanho físico usando estritamente a fórmula (Capacidade / 2)
int Board::getBusLength(int capacity) const {
    return capacity / 2;
}

bool Board::hasFreeSlot() const {
    for (bool occupied : m_slots)
        if (!occupied) return true;
    return false;
}

int Board::getNextFreeSlotIndex() const {
    for (size_t i = 0; i < m_slots.size(); ++i)
        if (!m_slots[i]) return static_cast<int>(i);
    return -1;
}

void Board::occupySlot(int index) {
    if (index >= 0 && index < static_cast<int>(m_slots.size()))
        m_slots[index] = true;
}

void Board::clearSlots() {
    std::fill(m_slots.begin(), m_slots.end(), false);
}

int Board::getNumSlots() const {
    return static_cast<int>(m_slots.size());
}

// Altera o tamanho do vetor de slots dinamicamente por nível
void Board::setNumSlots(int count) {
    if (count >= 4 && count <= 8) {
        m_slots.resize(count, false);
    }
}

// Verifica a ocupação correta baseada no comprimento dinâmico
bool Board::isOccupied(int r, int c, int ignoreBusIndex) const {
    for (size_t i = 0; i < m_buses.size(); ++i) {
        if (static_cast<int>(i) == ignoreBusIndex) continue; // Ignora o próprio autocarro

        const auto& bus = m_buses[i];
        int len = getBusLength(bus.getCapacity());

        for (int j = 0; j < len; ++j) {
            int br = bus.getRow();
            int bc = bus.getCol();

            // Left and Right buses occupy cells horizontally from their origin
            if (bus.getDirection() == Direction::Left ||
                bus.getDirection() == Direction::Right) {
                bc += j;
            } else {
                br += j;
            }

            if (br == r && bc == c) return true;
        }
    }
    return false;
}