#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include "bus.h"

class Board {
public:
    Board(int r, int c);

    int getRows() const;
    int getCols() const;

    // Acesso somente leitura à lista de barramentos
    //(usada para exibição e captura de instantâneos)
    const std::vector<Bus>& getBuses() const;


    // FIX: Substitui const_cast em gamecontroller.cpp


    std::vector<Bus>& getBusesMutable();


    // Mutating operations — replaces the const_cast anti-pattern
    void addBus(const Bus& bus);


    // ignoreBusIndex avoids a bus registering a collision with itself
    int  getBusLength(int capacity) const;
    bool isOccupied(int r, int c, int ignoreBusIndex = -1) const;


    // Funções de controlo das plataformas/slots de estacionamento
    bool hasFreeSlot() const;
    int  getNextFreeSlotIndex() const;
    void occupySlot(int index);
    void clearSlots();
    int  getNumSlots() const;
    void setNumSlots(int count); // CORREÇÃO OBRIGATÓRIA: Define o tamanho dinâmico das plataformas

private:
    int m_rows;
    int m_cols;
    std::vector<Bus>  m_buses;
    std::vector<bool> m_slots; // Esta é a variável que estava em falta!
};

#endif // BOARD_H