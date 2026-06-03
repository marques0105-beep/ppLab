#ifndef BUS_H
#define BUS_H

#include <QString>

// Enumeração com as 4 direções independentes
enum class Direction { Up, Down, Left, Right };

class Bus {
public:

    // Construtor para criar um autocarro com as suas propriedades
    Bus(QString color, int capacity, Direction dir, int row, int col);


    // Funções para ler as propriedades (Getters)
    QString getColor() const;
    int getCapacity() const;
    Direction getDirection() const;
    int getRow() const;
    int getCol() const;


    // Setters e Movimento (Função para atualizar a posição quando o autocarro move)
    void setPosition(int newRow, int newCol);


    // Métodos para gestão do embarque dinâmico
    int getCurrentPassengers() const { return m_currentPassengers; }
    bool isFull() const { return m_currentPassengers >= m_capacity; }
    void addPassenger() { m_currentPassengers++; }

private:
    QString m_color;
    int m_capacity;
    Direction m_direction;
    int m_row;
    int m_col;
    int m_currentPassengers; // Contador interno de ocupação

};

#endif // BUS_H